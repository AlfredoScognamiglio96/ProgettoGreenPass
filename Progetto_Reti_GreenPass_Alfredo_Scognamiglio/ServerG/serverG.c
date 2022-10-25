#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "wrapperSysCall.h"

#define PORTA_1 8082										//Porta per comunicare con ClientS e ClientT
#define PORTA_2 8081										//Porta per comunicare con ServerV
#define CLIENT_IP "127.0.0.1"
#define SA struct sockaddr
#define MAX_BUFFER 1024

//THREADS 
void *main_thread(void *client){
   
   //int id_x;
   int *client_thread = (int *)client;
   char buffer[MAX_BUFFER];
   
   int serverV_fd;
   struct sockaddr_in servaddr_2;											//Struttura client

	//Creazione socket
	serverV_fd = socket(AF_INET,SOCK_STREAM,0);							// AF_INET indica il dominio della socket (iPV4)
        																// SOCK_STREAM indica il tipo della socket (Canale affidabile e bidirezionale)
 		  																// PROTOCOL si intende il protocollo utilizzato nella socket (0 di default)	  																
	//Verifico creazione socket
	if(serverV_fd == -1){
	printf("Creazione Socket Client fallita!\n");
	exit(0);
	}
	else
	printf("------------------------------------\nCreazione Socket Client riuscita!\n");

	//Assegnazione campi Struct client
	servaddr_2.sin_family = AF_INET;										// Inizializzo la famiglia AF_INET cioè iPV4
	servaddr_2.sin_addr.s_addr = inet_addr(CLIENT_IP);						// inizializzo indirizzo (localhost)
	servaddr_2.sin_port = htons(PORTA_2);									// Inizializzo il numero di porta(per collegarmi col CentroVaccinale)	
	
	//Effettuo la connect	
	if(connect(serverV_fd,(SA*)&servaddr_2,sizeof(servaddr_2)) != 0) {
	printf("Connessione con il ServerV fallita!\n");
	exit(0);
	}
	else
	printf("Connessione con il ServerV riuscita!\n------------------------------------\n");

   strcpy(buffer,"ServerG");
   FullWrite(serverV_fd,buffer,sizeof(buffer));						
   
   FullRead(*client_thread,buffer,sizeof(buffer));					//Ricevo buffer di chi contatta il ServerG tra ServerV e ClientS e ClientT
   
   //ClientS
   if(strcmp(buffer,"ClientS") == 0){
   		
   		int clientS = *client_thread;
   		
   		printf("ServerG contattato dal ClientS\n");
   		
   		strcpy(buffer,"ClientS");
   		FullWrite(serverV_fd,buffer,sizeof(buffer));								//Invio buffer al ServerV
   		
   		FullRead(clientS,buffer,sizeof(buffer));									//Ricevo codice dal ClientS
   		printf("Il codice ricevuto dal ClientS e' '%s'\nInvio richiesta controllo al ServerV\n",buffer);
   		
   		printf("------------------------------------\n");
   		
   		FullWrite(serverV_fd,buffer,sizeof(buffer));								//Invio codice al ServerV	 		
   		
   		FullRead(serverV_fd,buffer,sizeof(buffer));									//Ricevo risposta esito controllo dal ServerV
   		
   		printf("Invio risposta esito controllo al ClientS\n");
   		FullWrite(clientS,buffer,sizeof(buffer));									//Invio la risposta esito controllo al ClientS
   		
   }
   
   //ClientT
   if(strcmp(buffer,"ClientT") == 0){
   		
   		int clientT = *client_thread;
   		int validazione;															
   		
   		printf("ServerG contattato dal ClientT\n");
   		
   		strcpy(buffer,"ClientT");
   		FullWrite(serverV_fd,buffer,sizeof(buffer));
   		
   		FullRead(clientT,buffer,sizeof(buffer));									//Lettura tessera sanitaria
   		printf("Codice Tessera Sanitaria : %s\n",buffer);
   		
   		FullWrite(serverV_fd,buffer,sizeof(buffer));								//Scrittura tessera
   		
   		
   		FullRead(clientT,(char*)&validazione,sizeof(validazione));					//Lettura modifica
   		
   		
   		FullWrite(serverV_fd,buffer,sizeof(buffer));								//Scrittura modifica
   		
   
   		FullRead(serverV_fd,buffer,sizeof(buffer));									//Lettura modifica
   		
   		printf("Invio risposta al ClientT\n");
   		FullWrite(clientT,buffer,sizeof(buffer));									//Invio esito ClientT
   }
	   
   /* Il thread  termina */
   pthread_exit(client_thread);
}
    

void close_server(){
	exit(0);
}    
    
int main(){

	int sockfd;
	int enable = 1;
	pthread_t *thread_i;
	
	struct sockaddr_in servaddr;
	int len = sizeof(servaddr);

	//Creazione Socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);											// AF_INET indica il dominio della socket (iPV4
																					// SOCK_STREAM indica il tipo della socket (Canale affidabile e bidirezionale)
																					// PROTOCOL si intende il protocollo utilizzato nella socket (0 di default
	if(sockfd == -1){																
		printf("Creazione Socket ServerG fallita!\n");
		exit(0);
	}else
		printf("------------------------------------\nCreazione Socket ServerG riuscita!\n");

	//Assegnazione campi Struct servaddr
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORTA_1);
	
	signal(SIGTSTP,close_server);													//richiamo funzione per stoppare il server in caso di evenienza
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));					//funzione per il riuso dell indirizzo locale
	

	//Bind per andare ad assegnare indirizzo IP al socket
	if((bind(sockfd,(SA*)&servaddr,sizeof(servaddr))) != 0){
		printf("Socket Bind ServerG fallita!\n");
		exit(0);
	}else
		printf("Socket Bind ServerG riuscita!\n");
		
		//Listen per mettere in ascolto il ServerV
	if(listen(sockfd,1024) != 0){
		printf("Listen ServerG fallita!\n");
		exit(0);
	}else
		printf("Listen ServerG riuscita, in ascolto..\n");
		
	
	while(1){

      /* Accetta una nuova connessione */
      int *client_socket_i = (int *)malloc(sizeof(int));
      //Accept per accettare nuove connessioni da parte del client
	  *client_socket_i = accept(sockfd,(SA*)&servaddr, &len);	
      int client_socket = *client_socket_i;
      free(client_socket_i);
    
      /* Creo un nuovo thread per ogni connessione accettata */
      thread_i = (pthread_t *)malloc(sizeof(pthread_t));
         
      /* creo e lancio un thread per gestire la connessione da parte del TYPECLIENT/SERVER */
      pthread_create(thread_i, NULL,(void *)main_thread, &client_socket);
      
	}
     
}




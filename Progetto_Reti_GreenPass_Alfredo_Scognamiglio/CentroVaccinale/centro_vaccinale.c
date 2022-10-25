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

#define PORTA_1 8080								//Porta per comunicare con il Client
#define PORTA_2 8081								//Porta per comunicare con il ServerV
#define CLIENT_IP "127.0.0.1"
#define SA struct sockaddr
#define MAX_BUFFER 1024


//THREADS 
void *main_thread(void *client){
   
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

   strcpy(buffer,"CentroVaccinale");
   FullWrite(serverV_fd,buffer,sizeof(buffer));					//Comunica al serverV che lo sta contattando il CentroVaccinale
   
   FullRead(*client_thread,buffer,sizeof(buffer));				//Ricevo codice dal client
   printf("Codice tessera sanitaria '%s' ricevuta dal client\n",buffer);
   
   FullWrite(serverV_fd,buffer,sizeof(buffer));					//Invio codice al ServerV

   strcpy(buffer,"6 mesi");
   FullWrite(serverV_fd,buffer,sizeof(buffer));					//Invio mese al ServerV
   printf("Mese validita' codice di '%s'\n",buffer);
   
   printf("Invio informazioni al ServerV!\n");
	   
   /* Il thread  termina */
   pthread_exit(client_thread);
}
    

void close_server(){
	exit(0);
}    

//CentroVaccinale
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
		printf("Creazione Socket CentroVaccinale fallita!\n");
		exit(0);
	}else
		printf("------------------------------------\nCreazione Socket CentroVaccinale riuscita!\n");

	//Assegnazione campi Struct servaddr
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORTA_1);
	
	signal(SIGTSTP,close_server);													//richiamo funzione per stoppare il server in caso di evenienza
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));					//funzione per il riuso dell indirizzo locale
	

	//Bind per andare ad assegnare indirizzo IP al socket
	if((bind(sockfd,(SA*)&servaddr,sizeof(servaddr))) != 0){
		printf("Socket Bind CentroVaccinale fallita!\n");
		exit(0);
	}else
		printf("Socket Bind CentroVaccinale riuscita!\n");
		
		//Listen per mettere in ascolto il ServerV
	if(listen(sockfd,1024) != 0){
		printf("Listen CentroVaccinale fallita!\n");
		exit(0);
	}else
		printf("Listen CentroVaccinale riuscita, in ascolto..\n");
		
	
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




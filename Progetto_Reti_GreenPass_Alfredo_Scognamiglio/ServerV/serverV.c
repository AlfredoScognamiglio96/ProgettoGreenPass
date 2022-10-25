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

#define PORTA 8081										//Porta per comunicare con il C.Vaccinale
#define SA struct sockaddr
#define MAX_CODICE_T_S 8
#define MAX_BUFFER 1024

struct utente{
	char codice_tessera_sanitaria[MAX_CODICE_T_S];
	int mese_validita_codice;
	int flag_validita;
}server[20];

//Variabili Globali
int indice=0;

//THREADS 
void *main_thread(void *client){
   
   int *client_thread = (int *)client;
   char buffer[MAX_BUFFER];
   
   FullRead(*client_thread,buffer,sizeof(buffer));			//Ricevo buffer di chi mi contatta tra C.Vaccinale e ServerG
 
    //CentroVaccinale
   if(strcmp(buffer,"CentroVaccinale") == 0){
   		
   		printf("ServerV contattato dal CentroVaccinale\n");
   		int centro_vaccinale = *client_thread;
   
   		FullRead(centro_vaccinale,buffer,sizeof(buffer));						//Ricevo Codice dal C.Vaccinale
   		printf("Codice tessera sanitaria '%s'\n",buffer);
   		
		strcpy(server[indice].codice_tessera_sanitaria,buffer);

   		FullRead(centro_vaccinale,buffer,sizeof(buffer));						//Ricevo Mese dal C.Vaccinale
   		printf("Mese validita' codice tessera '%s'\n",buffer);
   		
   		printf("------------------------------------\n");
   		
   		server[indice].mese_validita_codice = 1;
   		
   		server[indice].flag_validita = 1;
   		
   		indice++;
   }
   
   //ServerG
   if(strcmp(buffer,"ServerG") == 0){
   
   		printf("ServerV contattato dal ServerG\n");
   		int serverG = *client_thread;
	   	FullRead(serverG,buffer,sizeof(buffer));					//Riceve Tipologia 
	   
	   if(strcmp(buffer,"ClientS") == 0){
	   		printf("Il Server G richiede il controllo validità della tessera sanitaria con codice ");
	   		FullRead(serverG,buffer,sizeof(buffer));				//Ricevo Codice dal ServerG 
	   		printf("'%s'\n",buffer);
	   		
	   		for(int i=0 ; i < 20; i++){
	   				
	   				if((strcmp(buffer,server[i].codice_tessera_sanitaria) == 0) && (server[i].mese_validita_codice == 1)){

	   				strcpy(buffer,"Esito positivo");									
	   				goto stop;
	   				}
	   				else
	   				{	
						strcpy(buffer,"Esito negativo");
	   				}
	   		}
	   		stop: FullWrite(serverG,buffer,sizeof(buffer));
	   		printf("Invio risposta controllo al ServerG\n------------------------------------\n");
	  }
	   
	   if(strcmp(buffer,"ClientT") == 0){
	   	
	   		int validazione;
	   		int successo_ricerca=0;									
	   			
	   		FullRead(serverG,buffer,sizeof(buffer));				//Lettura tessera
	  		
	  		validazione = atoi(buffer);
	   		
	   		FullRead(serverG,buffer,sizeof(buffer));				//Lettura modifica		
	   		
	   for(int i=0 ; i < 20; i++){
	   		
	   		
	   		if((strcmp(buffer,server[i].codice_tessera_sanitaria) == 0) && (server[i].mese_validita_codice == 1) && (server[i].flag_validita == 1)){
	   		
	   		successo_ricerca = 1;									//Flag per la ricerca riuscita o meno della tessera
	   		server[i].flag_validita = validazione;	
	   		printf("La tessera con codice '%s' non e' piu' valida\n------------------------------------\n",server[i].codice_tessera_sanitaria);
	   		strcpy(buffer,"Invalidazione Tessera");												//Risposta controllo tessera
	   		}
	   		else if((strcmp(buffer,server[i].codice_tessera_sanitaria) == 0) && (server[i].mese_validita_codice == 1) && (server[i].flag_validita == 0)){
	   		
	   			successo_ricerca = 1;
	   			server[i].flag_validita = validazione;
	   			printf("La tessera con codice '%s' viene ripristinata\n------------------------------------\n",server[i].codice_tessera_sanitaria);
	   			strcpy(buffer,"Ripristino Tessera");											//Già in quarantena
	   		}
	   }	
	   
	   if(successo_ricerca == 0){
	   			printf("La tessera con codice '%s' non e' stata trovata!\n------------------------------------\n",buffer);
	   			strcpy(buffer,"Tessera non trovata");
	   }
	   
	   		FullWrite(serverG,buffer,sizeof(buffer));						//Risposta esito (guarito o quarantena)
	   }
   
  
   /* Il thread  termina */
   pthread_exit(client_thread);
   }
} 

void close_server(){
	exit(0);
}  

//ServerV
int main(){

	int sockfd;
	int enable = 1;
	pthread_t *thread_i;
	
	struct sockaddr_in servaddr;
	int len = sizeof(servaddr);
	
	//Creazione socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);											// AF_INET indica il dominio della socket (iPV4)
        																			// SOCK_STREAM indica il tipo della socket (Canale affidabile e bidirezionale)
 		  																			// PROTOCOL si intende il protocollo utilizzato nella socket (0 di default)
	//Verifico creazione socket
	if(sockfd == -1){
	printf("Creazione Socket ServerV fallita!\n");
	exit(0);
	}
	else
	printf("------------------------------------\nCreazione Socket ServerV riuscita!\n");
	
	//Assegnazione campi Struct servaddr
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORTA);
	
	signal(SIGTSTP,close_server);													//richiamo funzione per stoppare il server in caso di evenienza
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));					//funzione per il riuso dell indirizzo locale
	

	//Bind per andare ad assegnare indirizzo IP al socket
	if((bind(sockfd,(SA*)&servaddr,sizeof(servaddr))) != 0){
		printf("Socket Bind ServerV fallita!\n");
		exit(0);
	}else
		printf("Socket Bind ServerV riuscita!\n");
		
		//Listen per mettere in ascolto il ServerV
	if(listen(sockfd,1024) != 0){
		printf("Listen ServerV fallita!\n");
		exit(0);
	}else
		printf("Listen ServerV riuscita, in ascolto..\n------------------------------------\n");
		
	
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



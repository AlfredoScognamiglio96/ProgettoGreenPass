#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include "wrapperSysCall.h"

#define PORTA 8082										//Porta per comunicare con il ServerG
#define CLIENT_IP "127.0.0.1"
#define SA struct sockaddr
#define MAX_CODICE_T_S 8
#define MAX_BUFFER 1024

//Variabili Globali											
char codice_tessera_sanitaria[MAX_CODICE_T_S];									//Buffer nel quale andare a copiare il codice_tessera_sanitaria

//CLIENT MAIN
int main(){

	int sockfd;
	char buffer[MAX_BUFFER];
	
	struct sockaddr_in client;											//Struttura client
	
	//Creazione socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);								// AF_INET indica il dominio della socket (iPV4)
        																// SOCK_STREAM indica il tipo della socket (Canale affidabile e bidirezionale)
 		  																// PROTOCOL si intende il protocollo utilizzato nella socket (0 di default)
	//Verifico creazione socket
	if(sockfd == -1){
	printf("Creazione Socket Client fallita!\n");
	exit(0);
	}
	else
	printf("------------------------------------\nCreazione Socket Client riuscita!\n");
	
	//Assegnazione campi Struct client
	client.sin_family = AF_INET;										// Inizializzo la famiglia AF_INET cioè iPV4
	client.sin_addr.s_addr = inet_addr(CLIENT_IP);						// inizializzo indirizzo (localhost)
	client.sin_port = htons(PORTA);										// Inizializzo il numero di porta(per collegarmi col CentroVaccinale)
	
	//Effettuo la connect	
	if(connect(sockfd,(SA*)&client,sizeof(client)) != 0) {
	printf("Connessione con il ServerG fallita!\n-----------------------------------\n");
	exit(0);
	}
	else
	printf("Connessione con il ServerG riuscita!\n------------------------------------\n");
	
	strcpy(buffer,"ClientS");
	FullWrite(sockfd,buffer,sizeof(buffer));
	
	printf("Inserisci codice tessera sanitaria:");
	scanf("%s",buffer);
	
	FullWrite(sockfd,buffer,sizeof(buffer));							//Invio codice al ServerG
	
	FullRead(sockfd,buffer,sizeof(buffer));								//Ricevo dal ServerG risposta controllo
	printf("Risposta in arrivo dal ServerG -> %s\n",buffer);
	
	//Chiusura Socket
	close(sockfd);
	
}



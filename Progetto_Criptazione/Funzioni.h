#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>

#define BACKLOG 10
#define MAXLINE 256
#define MAX_CLIENTS 10
#define MAXWORDS 256
typedef struct{

    char buffer[MAXLINE];
    int criptazioneScelta;


}CollezioneDati;

typedef struct{

    int clientSocket;
    int nroFile;
    CollezioneDati collezionedati[];

} ClientData;



    /*
    Funzione per la ricezione dei buffer provenienti dal server centrale.Questi buffer
    verrano salvati all'interno di file creati sul momento che avranno lo stesso nome dato
    dall'utente con un id che li rende univoci. Restituisce 0 se l'operazione è andata male e 1 se è andata bene

    */

    int riceviDati(int sock_fd, char *buffer, size_t buffer_size,char *term,CollezioneDati *collezione) 
    {
        int m = 0;

        printf("\n \n \n Inizio RICEZIONE \n \n \n");
        while (1) {
            
            if (strcmp(buffer,term) == 0) 
            {
                printf("Mi è stato inviato un messaggio di uscita dal server \n");
                return 1;
                break;
            }
            
                printf("\n \n \n Stampo l'iteratore %d \n \n \n", m);

            ssize_t nwrite_size = recv(sock_fd, buffer, buffer_size, 0);
            if(nwrite_size<=0)
            {
                return 0;
            }
            sleep(2);
            
            printf("Buffer ricevuto correttamente %s \n", buffer);
            if(strcmp(buffer,term) != 0)
            {
                strcat(collezione[m].buffer,buffer);
                printf("Dati Ricevuti correttamente nella collezione %s\n",collezione[m].buffer);
            }
            m++;
            
            
            
        }
    }


    /*
    Questa funzione permette di inviare dati ad un qualsiasi server.
    Prende come parametri la socket collegata al server in questione, Una struttura dati ClientData da cui prendersi il Client corrente e il buffer corrente e i relativi indici.

    */
    void handleRequest(int secondarySocket,ClientData *clientData,int s,int q,int i)
    {
        printf("Buffer ricevuto all'iterazione %d  . Contenuto %s \n",q,clientData[i].collezionedati[q].buffer);                
        send(secondarySocket, clientData[i].collezionedati[q].buffer, sizeof(clientData[i].collezionedati[q].buffer), 0);
        //sleep(10);
        recv(secondarySocket, clientData[i].collezionedati[q].buffer,sizeof(clientData[i].collezionedati[q].buffer), 0);
        printf("Ricevo: %s \n",clientData[i].collezionedati[q].buffer);
        

    }


/*
    In questo file c'è il server che si occupa di invertire il contenuto dei file che gli vengono inviati dal server centrale
*/


#include "Funzioni.h"

#define PORT 1024
#define BUFFER_SIZE 256

void * reverseString(void *arg) {
     char *str = (char *)arg;
    int start = 0;
    int end = strlen(str) - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
void Thread(char *buffer) {
    pthread_t thread;
    
    // Creazione del thread per invertire la stringa
    pthread_create(&thread, NULL, reverseString,(void *)buffer);
    pthread_join(thread, NULL);
}

int main() {
    int secondarySocket;
    struct sockaddr_in serverAddr;
    char buffer[MAXLINE];

    // Creazione del socket secondario
    secondarySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (secondarySocket == -1) {
        perror("Errore nella creazione del socket secondario");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server secondario
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Connessione al server principale
    if (connect(secondarySocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Errore nella connessione al server principale");
        close(secondarySocket);
        exit(EXIT_FAILURE);
    }
    
    // Ciclo di comunicazione con il server principale
    while (1) {
        // Ricezione del messaggio dal server principale
        recv(secondarySocket, buffer,sizeof(buffer), 0);
        printf("\nMessaggio ricevuto dal server principale:\n\"%s\"\n", buffer);

        // Inversione del messaggio
        Thread(buffer);
        
    //si mostra la stringa modificata
    printf("\nMessaggio modificato:\n\"%s\"\n", buffer);
    
        // Invio del messaggio invertito al server principale
        send(secondarySocket, buffer, strlen(buffer), 0);

        // Pulizia del buffer
        memset(buffer, 0,MAXLINE);
    }

    // Chiusura del socket secondario
    close(secondarySocket);

    return 0;
}

/*
    In questo file c'è il server che si occupa della criptazione del contenuto dei file che gli vengono inviati dal server centrale in ASCII
*/


#include "Funzioni.h"
#define MAX_LENGTH 256

#define PORT 1024
#define BUFFER_SIZE 256
char risultato[MAXLINE*5];


//Converte il testo in ASCII
void convertiInASCII(const char *testo, char result[]) {
    int i = 0;
    while (*testo) {
        i += sprintf(result + i, "%d ", *testo);
        testo++;
    }
    result[i] = '\0';  // Aggiungi un terminatore di stringa
}

void *threadFunction(void *arg) {
    char *input = (char *)arg;
    convertiInASCII(input, risultato);

    printf("Rappresentazione ASCII: %s\n", risultato);

    pthread_exit(NULL);
}

int main() 
{
     pthread_t thread;
    int secondarySocket;
    struct sockaddr_in serverAddr;
    char buffer[MAXLINE];
    char input[MAXLINE];

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
    while (1) 
    {
        // Ricezione del messaggio dal server principale
        recv(secondarySocket,input,sizeof(input), 0);

        printf("\nMessaggio ricevuto dal server principale:\n\"%s\"\n", input);

        // Inversione del messaggio
        if (pthread_create(&thread, NULL, threadFunction, (void *)input) != 0) 
        {
            fprintf(stderr, "Errore nella creazione del thread.\n");
            return 1;
        }
        pthread_join(thread, NULL);
        
        //si mostra la stringa modificata
        printf("\nMessaggio modificato:\n\"%s\"\n",risultato);
    
        // Invio del messaggio invertito al server principale
        send(secondarySocket,risultato, strlen(risultato), 0);

        // Pulizia del buffer
        memset(risultato, 0, MAXLINE);
        memset(input, 0,MAXLINE);
    }

    // Chiusura del socket secondario
    close(secondarySocket);

    return 0;
}

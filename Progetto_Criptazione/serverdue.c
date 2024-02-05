#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#define MAX_LENGTH 256

#define PORT 1024
#define BUFFER_SIZE 256
char risultato[BUFFER_SIZE*5];

void reverseString(char* str) {
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

void *rendiMaiuscoli(void *arg) {
    char *buffer=(char *)arg;
    for (int i = 0; buffer[i]; i++) {
        buffer[i] = toupper(buffer[i]);
    }
}

void Thread(char *buffer) {
    pthread_t thread;
    
    // Creazione del thread per invertire la stringa
    pthread_create(&thread, NULL, rendiMaiuscoli,(void *)buffer);
    pthread_join(thread, NULL);
}




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

int main() {
     pthread_t thread;
    int secondarySocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

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
        recv(secondarySocket,input,sizeof(input), 0);

        printf("\nMessaggio ricevuto dal server principale:\n\"%s\"\n", input);

        // Inversione del messaggio
         if (pthread_create(&thread, NULL, threadFunction, (void *)input) != 0) {
        fprintf(stderr, "Errore nella creazione del thread.\n");
        return 1;
    }
    pthread_join(thread, NULL);
        
    //si mostra la stringa modificata
    printf("\nMessaggio modificato:\n\"%s\"\n",risultato);
    
        // Invio del messaggio invertito al server principale
        send(secondarySocket,risultato, strlen(risultato), 0);

        // Pulizia del buffer
        memset(risultato, 0, BUFFER_SIZE);
        memset(input, 0, BUFFER_SIZE);
    }

    // Chiusura del socket secondario
    close(secondarySocket);

    return 0;
}

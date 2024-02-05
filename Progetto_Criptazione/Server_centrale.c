
#include "Funzioni.h"

void reverse_string(char* str);
void handleRequest(int secondarySocket, char buffer[],int choice );


int numero_file=0;

int main() {
    //Dati
    struct timeval timeout;
    socklen_t lens;
    int server_socket, client_sockets[MAX_CLIENTS],secondary1Socket,secondary2Socket;
    struct sockaddr_in server_addr,secondaryServer,secondary2Addr,client_addr;
    socklen_t client_len,addrSize;
    fd_set read_fds, active_fds;
    int h=0;
    timeout.tv_sec = 15; // Imposta il timeout a 5 secondi
    timeout.tv_usec = 0;
    char argomenti[MAXLINE];
    ClientData clientData[MAX_CLIENTS];
    char buffer[MAXLINE];
    char buffer_argv[MAXLINE];

    // Creazione del socket del server
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Inizializzazione dell'indirizzo del server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1024);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Binding del socket del server
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Impostazione del socket in modalità di ascolto
    if (listen(server_socket, BACKLOG) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 1024...\n");
    
     // Accettazione delle connessioni dai server secondari
    secondary1Socket = accept(server_socket, (struct sockaddr*)&secondaryServer, &lens);
    printf("Connessione accettata da Server Secondario 1\n");
    
    secondary2Socket = accept(server_socket, (struct sockaddr*)&secondary2Addr, &addrSize);
    printf("Connessione accettata da Server Secondario 2\n");
    
    //secondary3Socket = accept(mainSocket, (struct sockaddr*)&secondary3Addr, &addrSize);
    //printf("Connessione accettata da Server Secondario 3\n");



    FD_ZERO(&active_fds);
    FD_SET(server_socket, &active_fds);

    int max_fd = server_socket;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_sockets[i] = -1;
    }

    while (1) {
        read_fds = active_fds;

        // Utilizzo di select per gestire gli eventi su tutti i socket aperti
        if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) < 0) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // Verifica se il socket di ascolto ha attività (nuova connessione)
        if (FD_ISSET(server_socket, &read_fds)) {
            client_len = sizeof(client_addr);
            int new_client = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

            if (new_client < 0) {
                perror("Accept error");
                exit(EXIT_FAILURE);
            }

            // Aggiungi il nuovo client alla lista
            int i;
            for (i = 0; i < MAX_CLIENTS; ++i) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = new_client;
                    clientData[i].clientSocket = new_client;
                    break;
                }
            }

            // Aggiorna il massimo descrittore di file se necessario
            if (new_client > max_fd) {
                max_fd = new_client;
            }

            FD_SET(new_client, &active_fds);
            printf("New connection from client %d\n", i);
        }

        // Gestisci l'input da tutti i client
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int client_socket = client_sockets[i];
            int currentFileNumber = 0;
           
            if (client_socket != -1 && FD_ISSET(client_socket, &read_fds)) {
                printf("Gestisco il client: %d \n",i);
                
                
                //ricezione dell'array contenente i nomi dei file e il relativo criptaggio
                ssize_t nread_argv;
                nread_argv = recv(client_socket, buffer_argv,sizeof(buffer_argv),0);
                //Calcolo del numero di file contenuti
                printf("numero caratteri all'interno del buffer: %ld \n",strlen(buffer_argv));
                
                if (nread_argv < 0)
                {
                    perror("Errore nella lettura degli argomenti");
                    exit(1);
                }else
                {
                    printf("letti: %ld\n",nread_argv);
                }
                    int argc=0;
                    char words[MAXWORDS][256];
                    // Tokenizza il buffer utilizzando uno spazio come delimitatore
                    char *token = strtok(buffer_argv, " ");
                    while (token != NULL && argc < MAXWORDS) {
                        // Copia la parola nel nuovo array
                        strcpy(words[argc], token);
                        numero_file++;

                        // Passa alla prossima parola
                        token = strtok(NULL, " ");
                        argc++;
                    }
                
                printf("Array contenente i nomi di tutti i file passati:%s\n",words[0]);
                
                //Ricevi File dal Client
                int h=0;
                char word[500][500];
                /*
                
                
                 while (1)
                {
                    printf("h vale: %d",h);
                    ssize_t nread = recv(client_socket, buffer, sizeof(buffer), 0);
                    if(strcmp(buffer,"exit")==0)
                    {
                        printf("Mi è stato inviato un messaggio di uscita dal cliente %d\n",i);
                        break;
                    }
                    printf("Stampo nread %ld",nread);
                    if (nread < 0) {
                    perror("Read error");
                    close(client_socket);
                    FD_CLR(client_socket, &active_fds);
                    client_sockets[i] = -1;
                    printf("Client %d disconnected\n", i);
                    break;
                } else if (nread == 0) {
                    // Connessione chiusa dal client
                    close(client_socket);
                    FD_CLR(client_socket, &active_fds);
                    client_sockets[i] = -1;
                    printf("Client %d disconnected\n", i);
                    break;
                } else {
                    if(strcmp(buffer,"exit")!=0)
                    {
                        strcat(clientData[i].collezionedati[h].buffer, buffer);

                        
                    }
                    
        
                    
                    printf("Buffer ricevuto all'iterazione %d  . Contenuto %s \n",h,clientData[i].collezionedati[h].buffer);
                    h++;
                    
                }
                
                
                }
                
                
                
                */
               char term[5]="exit";
               riceviDati(client_socket,buffer,MAXLINE,term,clientData[i].collezionedati);
               printf("Buffer ricevuto all'iterazione %d  . Contenuto %s \n",h,clientData[i].collezionedati[h].buffer);

                
               printf("La lunghezza dell'array %d \n \n \n",numero_file);

                clientData[i].nroFile=numero_file/2;
                
                printf("sono uscito dal while senza rimanere bloccato nella read con numero di file uguale a %d \n",clientData[i].nroFile);

                //Switch case ma prima la prova del 9
                int s=1;
                int q=0;
                while(s<=clientData[i].nroFile+1)
                {
                    
                if (strcmp(words[s], "0")==0) {
                    printf("Buffer ricevuto all'iterazione %d  . Contenuto %s \n",h,clientData[i].collezionedati[q].buffer);

                    
                    printf("\nSono il processo figlio del caso %s",word[s]);
                    send(secondary1Socket, clientData[i].collezionedati[q].buffer, sizeof(buffer), 0);
                    //sleep(10);
                    recv(secondary1Socket, clientData[i].collezionedati[q].buffer,sizeof(buffer), 0);
                    printf("Ricevo: %s \n",clientData[i].collezionedati[q].buffer);
                    printf("\nMessaggio ricevuto dal server destinatario:\n\"%s\"\n", clientData[i].collezionedati[q].buffer);
                   
                }else if(strcmp(words[s], "1") == 0)
                {
                    printf("Buffer ricevuto all'iterazione %d  . Contenuto %s \n",h,clientData[i].collezionedati[q].buffer);

                    
                    printf("\nSono il processo figlio del caso %s",word[s]);
                    send(secondary2Socket, clientData[i].collezionedati[q].buffer, sizeof(buffer), 0);
                    //sleep(10);
                    recv(secondary2Socket, clientData[i].collezionedati[q].buffer,sizeof(buffer), 0);
                    printf("Ricevo: %s \n",clientData[i].collezionedati[q].buffer);
                    printf("\nMessaggio ricevuto dal server destinatario:\n\"%s\"\n", clientData[i].collezionedati[q].buffer);
                   
                }else if(strcmp(words[s], "2") == 0)
                {
                    handleRequest(client_socket,clientData[i].collezionedati[q].buffer,0);
                    printf("Server 2 %d",s);
                    fflush(stdout);

                }else if(strcmp(words[s], "3") == 0)
                {
                    printf("Server 3 %d",s);
                    fflush(stdout);
                }
                q++;
                s=s+2;
                }


                    q=0;
                    printf("\n \n \n è arrivato il momento di mandare i file al client. numero di file: %d \n \n \n ",clientData[i].nroFile);
                    fflush(stdout);
                    //Mandare i file al Client
                    while (q<clientData[i].nroFile)
                    {
                        printf("Stampo l'iteratore %d\n",q);

                        ssize_t numero=send(client_socket,clientData[i].collezionedati[q].buffer,sizeof(buffer),0);
                        printf("ho inviato un buffer al client %s\n",clientData[i].collezionedati[q].buffer);
                        fflush(stdout);
                        if(numero<0)
                        {
                            perror("errore sulla send");
                            exit(1);
                        }
                        
                        
                        q++;

                    }
                    
                    send(client_socket,"&&",sizeof("&&"),0);
                    close(client_socket);
                    FD_CLR(client_socket, &active_fds);
                    client_sockets[i] = -1;
            }
            
        }
    }

    // Chiusura del socket del server
    close(server_socket);

    return 0;
}

void reverse_string(char* str) {
    int length = strlen(str);
    int start = 0;
    int end = length - 1;

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}


void handleRequest(int secondarySocket, char buffer[],int choice ){

    
         printf("\nSono il processo figlio del caso %d", choice);
         send(secondarySocket, buffer, strlen(buffer), 0);
         recv(secondarySocket, buffer,strlen(buffer), 0);
         printf("Ricevo: %s \n",buffer);
         printf("\nMessaggio ricevuto dal server destinatario:\n\"%s\"\n", buffer);
         exit(EXIT_SUCCESS);
    

}



    /*
        Di seguito troverai il codice per quanto riguarda il client del progetto di reti di calcolatori. Questo progetto permette di avviare un file che si comporta come un client e di passare da terminale i propri file con 
        La criptazione o decriptazione che si vuole ottenere.Il Client si connette ad un server centrale che accetta la connessione e genera un processo figlio che gestisce la richiesta di quel client.
        1 febbraio 9:39
    */

    

    #include "Funzioni.h"
   

    //Funzioni utili che dovrò mettere in una libreria

    //Apre tutti i file presenti nella lista di argomenti.

    FILE *Apertura_File(FILE *file[],char *argv[],int argc,char *open,int univoco);

    //Converte il doppio puntatore argv in un array di caratteri che restituisce
    char *Conversione_Argv(char *argv[],int argc);

    //Funzione per inviare un certo numero di file al server
    int Invio_File(FILE *file[],int socket);

    //Questa funzione ci servirà per ricevere i dati dal server e metterli all'interno di file nuovi

   int riceviDati(int sock_fd, char *buffer, size_t buffer_size,char *term,CollezioneDati * dati);

    //Main
    int k=0;

    int main(int argc, char *argv[])
    {

        //Dichiarazione di tutte le variabili che saranno utili piu avanti

        int sock_fd;
        struct sockaddr_in server_addr;
        int i,j;
        char buffer_input[MAXLINE];
        char buffer_argomenti[MAXLINE];
        FILE *file_client[MAXLINE];
        FILE *recv[MAXLINE];
        CollezioneDati dati[MAXLINE];

        //Creazione della socket
        if ((sock_fd=socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }

        // Inizializzazione dell'indirizzo del server a cui il client si connetterà
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(1024);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        // Connessione al server
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
        {
            perror("Connection error");
            exit(EXIT_FAILURE);
        }

        //Inizio del loop per dare all'utente la possibilità di fare cose diverse
        while(1)
        {
            printf("Connessione effettuata correttamente al server. \n Scegli una delle seguenti opzioni: \n 0. Criptazione \n 1. Esci\n");
            //Se l'utente non ha immesso nessun carattere gli verrà chiesto nuovamento di iserirlo.

            if(fgets(buffer_input, MAXLINE,stdin)==NULL)
            {
                continue;
            }else
            {
                //In base all'input dell'utente si fanno cose diverse

                switch (buffer_input[0])
                {
                    case '0':
                    //Effeutto la conversione di argv e mi salvo quest'ultima nel buffer_argomenti
                    fflush(stdout);
                    //Eseguo l'apertura dei file e li salvo nell'array di file file_client
                    char open[2]="r";
                    *file_client=Apertura_File(file_client,argv,argc,open,0);
                    //Verifico quanti file ci sono
                    
                    printf("Numero totale dei file: %d \n",k);

                    //Invio array contenente i nomi di tutti i file con la relativa criptazione

                    char buffer_argv[MAXLINE]={'\0'};
                    // Copia gli argomenti nel buffer

                    for (int i = 1; i < argc; ++i) 
                    {
                        strncat(buffer_argv, argv[i], sizeof(buffer_argv) - strlen(buffer_argv) - 1);

                        // Aggiungi uno spazio tra gli argomenti, se non è l'ultimo
                        if (i < argc - 1) 
                        {
                            strncat(buffer_argv, " ", sizeof(buffer_argv) - strlen(buffer_argv) - 1);
                        }
                            
                    }

                    printf("Il buffer degli argomenti vale: %s",buffer_argv);

                    ssize_t nwrite_size = send(sock_fd,buffer_argv,strlen(buffer_argv), 0);
                    if (nwrite_size < 0) 
                    {
                        perror("Write error");
                        break;
                    }



                    //Il client ora deve fare un'altra write e per evitare che scriva tutto insieme è necessario inserire una sleep per dare il tempo al server di passare alla prossima read
                    sleep(2);
                    
                    Invio_File(file_client,sock_fd);
                    char Buffer[MAXLINE];

                    //Ricezione dei file 
                    int m=0;
                    /*
                    
                    printf(" \n \n \n Inizio RICEZIONE \n \n \n");
                    while (m<k)
                    {
                        printf(" \n \n \n Stampo l'iteratore %d \n \n \n",m);
                       
                       ssize_t nwrite_size =recv(sock_fd,Buffer,sizeof(Buffer),0);
                       sleep(2);
                       m++;
                       printf("Buffer ricevuto correttamente %s \n",Buffer);
                       if(strcmp(Buffer,"&&")==0)
                    {
                        printf("Mi è stato inviato un messaggio di uscita dal server \n");
                        break;
                    }
                    
                    
                    
                    */
                    
                    char terminazione[3]="&&";
                    
                    //Ricezione dei testi criptati dal server centrale
                    riceviDati(sock_fd,Buffer,sizeof(Buffer),terminazione,dati);
                    printf("\n \n \nESEGUITA FUORI DA RICEVI DATI %s \n \n \n",dati[0].buffer);

                    //Andiamo a mettere questi dati in file
                    char write[3]="w";
                   *recv=Apertura_File(recv,argv,argc,write,1);

                   printf("Sono uscito dall'apertura file\n");
                   fflush(stdout);

                   for(int u=0;u<2;u++)
                   {
                    fwrite(dati[u].buffer, sizeof(char), sizeof(dati[u].buffer),recv[u]);
                    printf("Ho inseriti  idati correttamente all'interno del file\n");
                    fflush(stdout);
                   }
                    
                    printf("ho finito di fare tutto \n");
                    //Chiusura di tutti i file

                    
                    
                    //Chiusura di tutti i file
                    for(int g=0;g<2;g++)
                    {
                        fclose(file_client[g]);
                        fclose(recv[g]);
                    }
                    close(sock_fd);

                    break;
                
                    default:
                    printf("Hai inserito un numero non presente nella lista");
                    break;
                }

            }

        }

    }


    //Funzioni utilzzate nel codice che dovrò mettere in una libreria

    /*
        La funzione utilizzata per l'apertura dei file in base ad una lista di file che sono posti in modo alternato
        prende 3 argomenti,ossia l'array di file da riempire,l'array con all'interno i nomi dei file disposti in modo alternato e infine il size complessivo di quest'ultimo array.
        
    */
    FILE *Apertura_File(FILE *file[],char *argv[],int argc,char *open,int univoco)
    {
        

        //Indice per gli argomenti che parte da uno poichè il primo argomento si suppone che sia il nome del file da cui si avvia il codice
        int l=0;
        int j=1;
        //Apertura file in lettura 
        //Il while viene effettuato finché l'indice j non è arrivato all'ultimo nome del file.
        if(univoco==0)
        {
            while (j<argc-1)
            {
                //Al k-esimo passo il file k viene aperto in lettura e gli si da il j-esimo nome.        
                file[k]=fopen(argv[j],open);
                printf("k: %d    j: %d argc-1: %d\n",k,j,argc-1);
            
                fflush(stdout);
                //Se il file non è stato aperto correttamente si esce con un errore
                if(!file[k])
                    {
                        printf("Qualcosa è andato storto nell'apertura del file %d \n",k);
                        fflush(stdout);
                        exit(1);
                    }
                    //k viene in crementato di 1 mentre j di due poiché deve sempre saltare l elemento "inutile" e passare al prossimo nome
                    k++;
                    j=j+2;
                    
            }

        }else
        {
             while (j<argc-1)
            {
                //Al k-esimo passo il file k viene aperto in lettura e gli si da il j-esimo nome.
                char output[200]="output";
                strcat(output,argv[j]);        
                file[l]=fopen(output,open);
                printf("l: %d    j: %d argc-1: %d\n",l,j,argc-1);
            
                fflush(stdout);
                //Se il file non è stato aperto correttamente si esce con un errore
                if(!file[l])
                    {
                        printf("Qualcosa è andato storto nell'apertura del file %d \n",l);
                        fflush(stdout);
                        exit(1);
                    }
                    //k viene in crementato di 1 mentre j di due poiché deve sempre saltare l elemento "inutile" e passare al prossimo nome
                    l++;
                    j=j+2;
                    
            }

        }
        
            //Restituisci l'array di file
            return *file;
    }


    
    

    /*
    Questa funzione ci servirà per inviare una serie di file al server o al client
    restituisce 1 se l'operazione è andata a buon fine e 0 se è andata male. Prende
    come parametri l'array di file,iò numero di file presente in quest'ultimo e la socket attraverso cui inviare i dati
    */
    
    int Invio_File(FILE *file[],int socket)
    {
        int j=0;
        char buffer[MAXLINE];
        int h=0;
        char file_interi[MAXLINE];
        //Itera su tutti i file finché non sono finiti
        while (j<k)

        {
            file_interi[0] = '\0';
            //Al j-esimo passo viene inserito il contenuto del file j all'interno di un buffer
            while (fgets(buffer, sizeof(buffer), file[j]) != NULL) 
            {
    
                //Faccio la concatenazione ad ogni iterazione della linea corrente al file finale
                strcat(file_interi,buffer);
                //Mantengo la formattazione originale
                strcat(buffer, "\n");
                printf("Sono nel while e sto iterando strlen:%ld sizeof %ld \n",strlen(buffer),sizeof(buffer));
                            
            }
            //Prima mi metto tutte le righe nel buffer e poi successivamente lo invio nella sua interezza 
            ssize_t nwrite=send(socket,file_interi,sizeof(file_interi),0);
             sleep(2);
            printf("Stampo il buffer corrente %s \n",file_interi);
            printf("Ho finito di inviare il file %d\n",j);
            //Se c'è un errore su di un file viene segnalato
                if (nwrite < 0) 
                {
                    printf("Write error on file %d",j);
                    fflush(stdout);
                    exit(1);
                }
            //Si passa al file successivo
            j++; 
        }
        //Sto per inviare la terminazione
        printf("Sto per inviare la terminazione");
        send(socket,"exit",sizeof("exit"),0);
       
    
    }


    



    







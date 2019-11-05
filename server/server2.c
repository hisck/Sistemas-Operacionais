#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

sem_t semaforo;

void *connection_handler(void *);

int main(int argc, char *argv[]){
    int socket_description, client_socket, *new_socket, c;
    struct sockaddr_in serverAddr, clientAddr;

    sem_init(&semaforo, 0, 1);

    //criando socket
    socket_description = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_description == -1){
        printf("\n[-]Failed to create Socket\n");
    }else{
        puts("\n[+]Socket Created\n");
    }

    //prepara o sockaddr_in
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(10200);

    //Bind
    if(bind(socket_description, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        perror("\n[-]Bind Failed!\n");
        return 1;
    }else{
        puts("\n[+]Bind completed\n");
    }

    //listen
    listen(socket_description, 3);
    //aceita e espera conexões/requisições
    puts("\n[+]Esperando por requisições . . . ");
    c = sizeof(struct sockaddr_in);

    while((client_socket = accept(socket_description, (struct sockaddr *)&clientAddr, (socklen_t*)&c))){
        puts("\n\nConnection Accepted\n\n");
        pthread_t sniffer_thread;
        new_socket = malloc(1);
        *new_socket = client_socket;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_socket) < 0){
            perror("could not create thread\n");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned\n");
    }

    if (client_socket < 0){
        perror("accept failed\n");
        return 1;
    }

    return 0;
}


void *connection_handler(void *socket_desc){
    FILE *memoria;

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size, val;
    char buffer[65535];  
    char dados[2000];
    char *message , *client_message, *client_message_aux;
    pid_t pid;
    char *myfifo = "/tmp/myfifo";
    int fd0, fd1;
    int fd[2];

    mkfifo(myfifo, 0666);


    //Receive a message from client
    while( (read_size = recv(sock , buffer , 65535 , 0)) > 0 ){
        sem_wait(&semaforo);

        //Abre o arquivo de memoria compartilhada
        if(!(memoria = fopen("shared_memory2.txt", "r+"))){
            memoria = fopen("shared_memory2.txt", "w+");
        }

        printf("%s\n", buffer);

        if(!strcmp(buffer, "1")){
            sem_getvalue(&semaforo, &val);
            message = "Insira os dados: ";
            send(sock, message, strlen(message), 0);
            client_message = malloc(sizeof(char)*2000);
            client_message[0] = '\0';
            recv(sock, client_message, 2000, 0);
            //client_message_aux = strtok(client_message, "\n");
            //printf("%d\n", strlen(client_message_aux));

            //Utilizando PIPE
            if(pipe(fd)<0) {
                perror("Pipe");
            }
            if((pid = fork()) < 0){
                perror("Erro no fork!\n");
                exit(1);
            }
            //Processo Pai
            if(pid > 0){
                client_message[strlen(client_message) - 1] = '\0';
                fd0 = open(myfifo, O_WRONLY);
                write(fd0, client_message, strlen(client_message));
                close(fd0);
            }
            //Processo filho
            else{
                fd1 = open(myfifo, O_RDONLY);
                read(fd1, dados, 2000);
                int tamDados = strlen(dados);
                close(fd1);

                fseek(memoria, 0, SEEK_END);

                //puts(client_message_aux);
                fwrite(&tamDados, sizeof(int), 1, memoria);
                fwrite(dados, strlen(dados), 1, memoria);
                //fprintf(memoria, "%s" ,client_message_aux);

                printf("Carro Inserido!");

                message = "Carro registrado!";
                //fscanf(aux, %s, %s, %s, nome, marca, placa);
                free(client_message);
                send(sock, message, strlen(message), 0);
                //exit(0);
            }
        }
        else if(buffer[0] == '2'){
            char* linha = NULL;
            char mes[5000];
            char cabo[] = "Carro alugado!\n";
            char car[2];
            int i, tam;
            size_t size = 0;
            fseek(memoria, 0, SEEK_SET);

            while(getline(&linha, &size, memoria) != -1){
                strcat(mes, linha);
            }

            send(sock, mes, strlen(mes), 0);
            recv(sock, car, 2, 0);

            int j = 0;
            for(i = 0; i < atoi(car)-9; i++){
                while(mes[j] != '|'){
                    putchar(mes[j]);
                    j++;
                }
                j++;
                while(mes[j] != '|'){
                    putchar(mes[j]);
                    j++;
                }
                j++;
                while(mes[j] != '|'){
                    putchar(mes[j]);
                    j++;
                }
                j++;
                while(mes[j] != '|'){
                    putchar(mes[j]);
                    j++;
                }
                j++;
            }
            mes[j-2] = '0';
            fclose(memoria);
            memoria = fopen("shared_memory2.txt", "w+");
            fwrite(mes, sizeof(char), strlen(mes), memoria);
            fclose(memoria);
            memoria = fopen("shared_memory2.txt", "r+");

            send(sock, cabo, strlen(cabo), 0);
        }

        //Send the message back to client
        fclose(memoria);
        write(sock , client_message , strlen(client_message));

        sem_post(&semaforo);
    }

    if(read_size == 0){
        puts("Cliente desconectado");
        fflush(stdout);
    }
    else if(read_size == -1){
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
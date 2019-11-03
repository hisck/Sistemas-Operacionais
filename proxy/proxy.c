#include "../server/server.h"

sem_t semaforo;

void *connection_handler(void *);

int main(int argc, char *argv[]){
    int socket_description, client_socket, *new_socket, c;
    struct sockaddr_in serverAddr, clientAddr;

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
    serverAddr.sin_port = htons(8079);

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

        puts("Handler assigned\n");
    }

    if (client_socket < 0){
        perror("accept failed\n");
        return 1;
    }
    while( (read_size = recv(sock , escolha , 2 , 0)) > 0 ){

        printf("%s\n", escolha);

        if(!strcmp(escolha, "1")){
            serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons( 8080 );
            if (connect(new_socket , (struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0){
                perror("[-]Connection Failed!");
                exit(1);
            }
            printf("[+]Connected to Server! ");
        }else if(escolha[0] == '2'){
            serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons( 8081 );
            if (connect(new_socket , (struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0){
                perror("[-]Connection Failed!");
                exit(1);
            }
            printf("[+]Connected to Server! ");
        } 
    }

    return 0;
}

#include "server.h"

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
        int sock_server1, sock_server2, sock_server3;
        puts("\n\nConnection Accepted\n\n");
        pthread_t sniffer_thread;
        new_socket = malloc(1);
        *new_socket = client_socket;
        

        puts("Handler assigned\n");
    }

    if (client_socket < 0){
        perror("accept failed\n");
        return 1;
    }

    return 0;
}

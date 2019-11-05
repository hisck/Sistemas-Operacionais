#include "server.h"

 struct serverInfo  
 {  
      int client_fd;  
      char ip[100];  
      char port[100];  
 };  

void *runSocket(void *vargp)  
 {  
   struct serverInfo *info = (struct serverInfo *)vargp;  
   char buffer[65535];  
   int bytes =0;  
   char escolha1[2];
      printf("client:%d\n",info->client_fd);  
      fputs(info->ip,stdout);  
      //fputs(info->port,stdout);  
      //code to connect to main server via this proxy server  
      int server_fd =0;  
      struct sockaddr_in server_sd;  
      // create a socket  
      server_fd = socket(AF_INET, SOCK_STREAM, 0);  
      if(server_fd < 0)  
      {  
           printf("server socket not created\n");  
      }  
      printf("server socket created\n");       
      memset(&server_sd, 0, sizeof(server_sd));  
      // set socket variables  
      while(1){
        recv(info->client_fd , escolha1, 2 , 0);
        printf("%s\n", escolha1);
        if(!strcmp(escolha1, "1")) strcpy(info->port,"10100");
        else if(escolha1[0] == '2') strcpy(info->port,"10200");
        else if(escolha1[0] == '3') strcpy(info->port,"10300");
        printf("\nServer port %s\n", item->port);
        server_sd.sin_family = AF_INET;  
        server_sd.sin_port = htons(atoi(info->port));  
        server_sd.sin_addr.s_addr = inet_addr(info->ip);  
        //connect to main server from this proxy server  
        if((connect(server_fd, (struct sockaddr *)&server_sd, sizeof(server_sd)))<0)  
        {  
            printf("server connection not established");  
            exit(1);
        }  
        printf("server socket connected\n");  
        int i = 0;
        while(i < 2)  
        {  
            //receive data from client  
            //memset(&buffer, '\0', sizeof(buffer));  
            bytes = read(info->client_fd, buffer, sizeof(buffer));  
            if(bytes <= 0)  
            {  
                printf("error");
            }  
            else   
            {  
                    // send data to main server  
                    write(server_fd, buffer, sizeof(buffer));  
                    //printf("client fd is : %d\n",c_fd);                    
                    printf("From client :\n");                    
                    fputs(buffer,stdout);       
                    fflush(stdout);  
            }  
            //recieve response from server  
            memset(&buffer, '\0', sizeof(buffer));  
            bytes = read(server_fd, buffer, sizeof(buffer));  
            if(bytes <= 0)  
            {  
            }            
            else  
            {  
                    // send response back to client  
                    write(info->client_fd, buffer, sizeof(buffer));  
                    printf("From server :\n");                    
                    fputs(buffer,stdout);     
                    i++;       
            }  
        };     
      }  
   return NULL;  
 }  

int main(int argc, char *argv[]){
    pthread_t tid;  
    int socket_description, client_socket, *new_socket, c, read_size;
    //char escolha1[2], escolha2[2];
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
    serverAddr.sin_port = htons(10000);

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
    struct serverInfo *item = malloc(sizeof(struct serverInfo));  
    while((client_socket = accept(socket_description, (struct sockaddr *)&clientAddr, (socklen_t*)&c))){
        puts("\n\nConnection Accepted\n\n");
        item->client_fd = client_socket;  
        strcpy(item->ip,"127.0.0.1");  
        pthread_t sniffer_thread;
        new_socket = malloc(1);
        *new_socket = client_socket;
        /*recv(item->client_fd , escolha1, 2 , 0);
        printf("%s\n", escolha1);
        if(!strcmp(escolha1, "1")) strcpy(item->port,"10100");
        else if(escolha1[0] == '2') strcpy(item->port,"10200");
        else if(escolha1[0] == '3') strcpy(item->port,"10300");
        printf("\nServer port %s\n", item->port);*/
        if( pthread_create( &sniffer_thread , NULL ,  runSocket , (void*)item) < 0){
            perror("could not create thread\n");
            return 1;
        }

        puts("Handler assigned\n");
    }

    if (client_socket < 0){
        perror("accept failed\n");
        return 1;
    }

    return 0;
}

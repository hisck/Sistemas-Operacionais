#import "client.h"

int main(int argc , char *argv[]){

    int ClientSocket;
    char escolha[2], buffer[2000];
    struct sockaddr_in serverAddr;
    char mensagem[1000] , respServidor[2000], aux[2000], aux2[2000];

    //Criação socket
    ClientSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (ClientSocket == -1){
           printf("[-]Error in connection.\n");
           exit(1);
    }
    printf("[+]Client Socket created");
    memset(&serverAddr, '\0', sizeof(serverAddr));
    int PORT;
    
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( PORT );

  //Conexão
    if (connect(ClientSocket , (struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0){
        perror("[-]Connection Failed!");
        exit(1);
    }
    printf("[+]Connected to Server!");
    printf("SUPER SISTEMA DE ALUGUEL DE CARROS\n");

    //Conexão contínua
    while(1){
        printf("Escolha uma função: \n");
        printf("\t1- Registrar um carro\n");
        printf("\t2- Alugar um carro\n");
        scanf("%s" , escolha);
        system("clear");

        if( send(ClientSocket , escolha , 2 , 0) < 0){
            puts("Falha ao enviar");
            return 1;
        }

        if( recv(ClientSocket , respServidor , 2000 , 0) < 0){
            puts("Falha");
            break;
        }

        puts(respServidor);

        // Alugar um carro
        if(!(strcmp(escolha, "2"))){
            char* nome, *marca, *placa, *disp, *resp;
            char car[2];
            char cabo[16];
            resp = malloc(sizeof(char) * strlen(respServidor));
            respServidor[strlen(respServidor)-1] = '\0';
            strcpy(resp, respServidor);
            int ind = 10;
            nome = strtok(&respServidor, "|");
            while(nome){
                printf("carro %d\n", ind);
                printf("nome: %s\n", nome);
                marca = strtok(NULL, "|");
                printf("marca: %s\n", marca);
                placa = strtok(NULL, "|");
                printf("placa: %s\n", placa);
                disp = strtok(NULL, "|");
                printf("disponivel: %s\n", disp);
                ind++;
                nome = strtok(NULL, "|");
                printf("----------------------\n");
            }

            printf("Digite o carro desejado: ");
            scanf("%s", &car);

            send(ClientSocket, car, 2, 0);
            recv(ClientSocket, cabo, 16, 0);
            puts(cabo);

        // Registrar um carro
        } else {
            //aux = malloc(sizeof(char)*2000);
            //aux[0] = '\0';
            //aux2 = malloc(sizeof(char)*2000);
            //aux2[0] = '\0';

            fflush(stdin);
            printf("\nInsira o nome do carro: ");
            scanf("%s", aux);
            fflush(stdin);
            strcat(aux, "|");

            printf("\nInsira a marca do carro: ");
            fflush(stdin);
            scanf("%s", aux2);
            strcat(aux, aux2);
            strcat(aux, "|");

            printf("\nInsira a placa do carro: ");
            scanf("%s", aux2);
            strcat(aux, aux2);

            strcat(aux, "|1|\n");

            system("clear");
            printf("Carro Inserido!\n");


            printf("%s\n", aux);
            //printf("%d\n", strlen(aux));
            send(ClientSocket, aux, strlen(aux), 0);

            //free(aux);
            //free(aux2);

            recv(ClientSocket, respServidor , 2000, 0);

        }

    }

    close(ClientSocket);
    return 0;
}
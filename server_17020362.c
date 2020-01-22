#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#define PORT 8080
#define BUFFERSIZE 4096
#define ERROR (-1)
#define SERVER_BACKLOG 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void connectionFunction (int client_socket);
int exceptionHandler(const char *msg, int exp);


int main(int argc, char **argv){
    int server_socket;
    int client_socket;
    int addr_size;
    SA_IN server_addr;
    SA_IN client_addr;

    exceptionHandler("Failed while creating a socket", (server_socket = socket(AF_INET, SOCK_STREAM, 0)));
    
    memset(&server_addr, '0', sizeof(server_addr));
    // char msg[256] = "Server reached!";        
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    exceptionHandler("Failed while Bining the port", bind(server_socket,(SA*)&server_addr, sizeof(server_addr)));
    exceptionHandler("Failed while start listning" , listen(server_socket, SERVER_BACKLOG));

    while (true) {
        printf("Waiting for connections..........lol...!!!");

        addr_size = sizeof(SA_IN);
        exceptionHandler("Accept failed" , (client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size)));
        // client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size)

        printf("Connected! \n");
        // send(client_socket, msg, sizeof(msg), 0);
        // close(client_socket);
        connectionFunction(client_socket);

        close(client_socket);
    }
    return 0;
}

int exceptionHandler(const char *msg, int exp){
    if (exp == ERROR) {
        perror(msg);
        exit(1);
    }
    return exp;    
}

void connectionFunction(int client_socket){
    char buffer[BUFFERSIZE];
    size_t bytes_read;
    int msgSize = 0;
    char* actualpath;
    char path[1024];
    char requestType[16];
    while ((bytes_read = read(client_socket, buffer+msgSize, sizeof(buffer)-msgSize-1))>0){
        msgSize += bytes_read;
        if (msgSize > BUFFERSIZE-1 || buffer[msgSize-1] == '\n')
        {
            break;
        }        
    }
    // exceptionHandler("recv error", bytes_read);
    buffer[msgSize-1] = 0;
    printf("REQUEST: %s \n", buffer);
    fflush(stdout);
    
    sscanf(buffer ,"%s %s ", requestType, path);
    actualpath = (char *) malloc(strlen(path));

    strncpy(actualpath, path, strlen(path));

    actualpath++;
    printf("Path : %s\n", actualpath);

    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL)
    {
        printf("error (open): %s\n",buffer);
        // close(client_socket);
        return;
    }
    
    while ((bytes_read = fread(buffer, 1, BUFFERSIZE, fp)) > 0)
    {
        printf("Sending%zu bytes - \n",bytes_read);
        send(client_socket,buffer,bytes_read,0);
    }
    fclose(fp);
    // free(actualpath);
    printf("Closing Connection\n");

}

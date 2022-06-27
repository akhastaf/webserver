// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <string.h>

#define PORT 8080
int main(int argc, char const *argv[])
{

    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    char *hello = strdup("HTTP/1.1 307 OK\nServer: Hello\nLocation:/test\r\n\r\n");
    char *hello1 = strdup("HTTP/1.1 200 OK\nServer: Hello\nContent-Length: 100\nContent-Type: text/html\nSet-Cookie: yummy_cookie=choco\n\n<html><head><link rel=\"stylesheet\" href=\"app.css\" ></head><body><h1>Hello, world</h1></body></html>\r\n\r\n");
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        std::cout << "\n+++++++ Waiting for new connection ++++++++" << std::endl << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
       
        
        printf("%s\n",buffer );
        if (buffer[4] == '/' && buffer[5] == ' ')
            write(new_socket , hello , strlen(hello));
        else
            write(new_socket , hello1 , strlen(hello1));
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }
    return 0;
}
#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <winsock2.h>
#include <winsock.h>
#include <pthread.h>
#include <strings.h>

void error(char *m)
{
    perror(m);
}

void *handle_client(LPVOID client_socket)
{
    int newsockfd = *(int *) client_socket;
    char buffer[256];
    int n;

    memset(buffer, 0, 256);
    n = recv(newsockfd, buffer, 255, 0);

    printf("Message received: %s\n", buffer);
    
    // Convert the message to an integer, multiply it by 5
    int number = atoi(buffer);
    int result = number * 5;
    
    // Send the result back to the client
    memset(buffer, 0, 256);
    sprintf(buffer, "%d", result);
    n = send(newsockfd, buffer, strlen(buffer), 0);
    
    if (n < 0)
        error("ERROR writing back to socket");


}


int main(int argc, char *argv[])
{ 
    int sockfd, newsockfd, port, clilen, n;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    
    if (argc < 2)
        error("ERROR, no port provided\n");
    
    port = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port); //host to network
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR binding to socket");
    
    listen(sockfd,2);
    
    clilen = sizeof(cli_addr);
    
    for (int i=0; i<3;i++) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
            continue;
        }
        pthread_t thread_id;
        int Tret;
        
        // Spawn a new thread to handle the client
        Tret = pthread_create(&thread_id, NULL, handle_client, &newsockfd);

        pthread_join(thread_id, NULL);

        

    }

    closesocket(newsockfd);


    return 0;
}
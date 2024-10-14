#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void error (char *m)
{
	perror(m);
}

int checker=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void *handle_socket(int *newsockfd)
{
	char buffer[256];
	int n;
	memset(buffer,0,256);
	int sock = *(int*)newsockfd;

	n = read(sock,buffer,255);
	if (n<0)
		error("ERROR reading from socket");
	printf("Message received: %s\n",buffer);
	
	pthread_mutex_lock(&mutex);
	checker++;
	if(checker == 3) {
		pthread_cond_broadcast(&cond);
	}
	pthread_mutex_unlock(&mutex);
	
	pthread_mutex_lock(&mutex);
	while(checker <3) {
		pthread_cond_wait(&cond, &mutex);
	}
	pthread_mutex_unlock(&mutex);
	
	int num = 5 * atoi(buffer);
	sprintf(buffer, "%d", num);
	n = write(sock,buffer,255);
	if(n<0)
		error("ERROR writing back to socket");
}



int main(int argc, char *argv[])
{
	int sockfd, newsockfd, port, clilen, n;
	struct sockaddr_in serv_addr, cli_addr;

	if(argc < 2)
		error("ERROR, no port provided\n");
	port = atoi(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd <0)
		error("ERROR opening socket");

	memset((char*) &serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if(bind(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
		error("ERROR binding to socket");

	printf("Server has started!!\n");
	listen(sockfd,2);
	clilen = sizeof(cli_addr);
	
	pthread_t threads[3];
	int client_sockets[3];
	
	for(int i=0; i<3;i++)
	{
		client_sockets[i] = accept(sockfd,(struct sockaddr*)&cli_addr, &clilen);
		if(client_sockets[i] <0)
			error("ERROR on accept");
		int tret = pthread_create(&threads[i], NULL, handle_socket, &client_sockets[i]);
		
	}
	
	for(int i=0; i<3; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	close(newsockfd);

	return 0;
}
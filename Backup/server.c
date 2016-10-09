#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define QUEUE_SIZE 16

int main(int argc, char* argv[])
{
  int sockfd, newsockfd, clntaddrlen, portNo;
  struct sockaddr_in servAddr, clntAddr;
  char buffer[256];
  
  portNo = atoi(argv[1]);
  
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  clntaddrlen = sizeof(clntAddr);

  if(socket < 0)
    {
      printf("Socket Creation Failed\n");
      exit(1);
    }

  fprintf(stdout, "Socket Created\n");

  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(portNo);
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
      printf("Binding Failed\n");
      exit(1);
    }

  fprintf(stdout, "Binding Successful\n");

  if(listen(sockfd, QUEUE_SIZE) < 0)
    {
      printf("Listening Failed\n");
      exit(1);
    }

  fprintf(stdout, "I'm Listening\n");

  while(1)
    {
      newsockfd = accept(sockfd, (struct sockaddr*)&clntAddr, &clntaddrlen);
      if(newsockfd < 0)
	{
	  printf("Accepting Failed\n");
	  exit(1);
	}

      fprintf(stdout, "Accepted\n");

      if(fork() == 0)
	{
	  int n = recv(newsockfd, buffer, sizeof(buffer), 0);
	  printf("Message received from client: %s\n", buffer);
	  printf("Sending message back to client\n");
	  send(newsockfd, buffer, n, 0);
	  close(newsockfd);
	}
    }
  close(sockfd);
  return 0;
}
      

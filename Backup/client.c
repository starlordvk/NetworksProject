#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>

int main(int argc, char* argv[])
{
  int sockfd, portNo;
  struct sockaddr_in servAddr;
  char buffer[256];
  char *servName, *string;

  servName = argv[1];
  portNo = atoi(argv[2]);
  string = argv[3];
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0)
    {
      printf("Socket Creation Failed\n");
      exit(1);
    }

  fprintf(stdout, "Socket Created\n");
  
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(portNo);
  inet_pton(AF_INET, servName, &servAddr.sin_addr.s_addr);

  if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
      printf("Connection Failed\n");
      exit(1);
    }

  fprintf(stdout, "Connection Successful\n");

  send(sockfd, string, strlen(string), 0);
  int n = recv(sockfd, buffer, sizeof(buffer), 0);
  buffer[n] = '\0';
  printf("Message Received: %s\n", buffer);
  close(sockfd);
  return 0;
}
  
  

  

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char* argv[])
{
  int sockfd, portNo, status;
  struct sockaddr_in servAddr;
  char ipbuffer[256], msgbuffer[256], pending_msg[100];
  char *ipaddr, *message;

  portNo = atoi(argv[1]);
  ipaddr = argv[2];
  message  = argv[3];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0)
    {
      printf("Socket Creation Failed\n");
      exit(1);
    }

  fprintf(stdout, "Socket Created\n");

  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(portNo);
  servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
      printf("Connection Failed\n");
      exit(1);
    }

  fprintf(stdout, "Connection Successful\n");

  int count_pending_msg;
  recv(sockfd,(int*)&count_pending_msg,sizeof(int),0);
  
  fprintf(stdout, "Pending Messages are: \n");
  
  int i;
  for(i = 0; i < count_pending_msg; i++){
  int m = recv(sockfd, pending_msg, sizeof(pending_msg), 0);
  pending_msg[m] = '\0';
  printf("%s\n", pending_msg);}

  int n = send(sockfd, ipaddr, strlen(ipaddr), 0);
  recv(sockfd, ipbuffer, sizeof(ipbuffer), 0);
  ipbuffer[n] = '\0';
  printf("IP Address received: %s\n", ipbuffer);


  int n1 = send(sockfd, message, strlen(message), 0);
  recv(sockfd, msgbuffer, sizeof(msgbuffer), 0);
  msgbuffer[n1] = '\0';
  printf("Message received: %s\n", msgbuffer);

  close(sockfd);
  return 0;
}

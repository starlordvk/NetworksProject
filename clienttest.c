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
  char buffer[256], portNoBuffer[256], ipAddrBuffer[256];
  char *portno, *ipaddr;
  
  portNo = atoi(argv[1]);
  portno = argv[2];
  ipaddr = argv[3];
  //char *servName, *string;

  //servName = argv[1];
  //portNo = atoi(argv[2]);
  //string = argv[3];
  
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

  //send(sockfd, string, strlen(string), 0);
  
  //Sending the Port Number of Receiving client to Server
  int n = send(sockfd, portno, strlen(portno), 0);
  recv(sockfd, portNoBuffer, sizeof(portNoBuffer), 0);
  portNoBuffer[n] = '\0';
  printf("Port Number Received: %s\n", portNoBuffer);
  //Sending the Ip Address of Receving client to Server
  int n1 = send(sockfd, ipaddr, strlen(ipaddr), 0);
  recv(sockfd, ipAddrBuffer, sizeof(ipAddrBuffer), 0);
  ipAddrBuffer[n1] = '\0';
  printf("IP Address Received: %s\n", ipAddrBuffer);
  close(sockfd);
  return 0;
} 

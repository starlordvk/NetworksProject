	#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<sqlite3.h>

#define QUEUE_SIZE 16

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char* argv[])
{
  
   sqlite3 *db;
   char *errmsg = 0;
   int  rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open("mailDB.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   }else{
      fprintf(stdout, "Opened database successfully\n");
   }


  int sockfd, newsockfd, clntaddrlen, portNo, fd1, fd2, fd3;
  struct sockaddr_in servAddr, clntAddr;
  char buffer[256], nl = '\n', portNoBuffer[256], ipAddrBuffer[256];
  char* newline = &nl;

  //fd1 = creat("Message.txt", 0666);
  //close(fd1);
  
  fd2 = creat("PortNo.txt", 0666);
  close(fd2);
  
  fd3 = creat("IpAddr.txt", 0666);
  close(fd3);
  
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
      
      fd2 = open("IpAddr.txt", O_RDONLY);
      
      char command[50] = "grep ";
      char *ipaddress = (char*)malloc(20 * sizeof(char));
      
      ipaddress = inet_ntoa(clntAddr.sin_addr);
      /* inserting the ip into the database */
      sql = "INSERT INTO SENDER (ID,IP,PORTNO) "  \
         "VALUES (1,'"+ipaddress+",5616 ); ";
      /* Execute SQL statement */
   	  rc = sqlite3_exec(db, sql, callback, 0, &errmsg);
      if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
       }else{
      fprintf(stdout, "Records created successfully\n");
       }
       sqlite3_close(db);
      
      strcat(command, ipaddress);
      char file[20] = " IpAddr.txt";
      strcat(command, file);
      printf("%s\n", command);
      int rv = system(command);
      printf("%d\n", rv);
      if(rv == 0)
      {
      	printf("IP found\n");
      }
      else
      {
      	printf("IP not found\n");
      }
      close(fd2);

      if(fork() == 0)
	{
	  //Saving the Received Port Number into a file
	  int n = recv(newsockfd, portNoBuffer, sizeof(portNoBuffer), 0);
	  portNoBuffer[n] = '\0';
	  printf("Port Number received from client: %s\n", portNoBuffer);
	  fd2 = open("PortNo.txt", O_WRONLY | O_APPEND);
	  write(fd2, portNoBuffer, n);
	  write(fd2, newline, sizeof(char));
	  close(fd2);
	  send(newsockfd, portNoBuffer, n, 0);
	  
	  //Saving the Received Ip Address into a file
	  int n1 = recv(newsockfd, ipAddrBuffer, sizeof(ipAddrBuffer), 0);
	  ipAddrBuffer[n1] = '\0';
	  printf("IP address received from client: %s\n",ipAddrBuffer);
	  fd3 = open("IpAddr.txt", O_WRONLY | O_APPEND);
	  write(fd3, ipAddrBuffer, n1);
	  write(fd3, newline, sizeof(char));
	  close(fd3);
	  send(newsockfd, ipAddrBuffer, n1, 0);
	  printf("Sending Port Number and IP address back to client\n");
	  close(newsockfd);
	}
    }
  close(sockfd);
  return 0;
}
      

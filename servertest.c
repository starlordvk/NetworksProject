#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sqlite3.h>
#include<sys/wait.h>

#define QUEUE_SIZE 16
char pending_msg[100][100];
int count_pending_msg = 0;

//callback is used for sqlite3 functionality
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i = 0; i < argc; i++)
  {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    strcpy(pending_msg[count_pending_msg], argv[i]);
    count_pending_msg++;
  }
  printf("\n");
  return 0;
}

int main(int argc, char* argv[])
{
  int sockfd, newsockfd, clntaddrlen, portNo, idcount = 0, status;
  struct sockaddr_in servAddr, clntAddr;
  char msgbuffer[256], ipbuffer[256], id[20], sql_query[100];
  char *conn_ip;
  pid_t pid;

  //port no of server
  portNo = atoi(argv[1]);

  sqlite3 *db;
  int rc;
  char *Zerrmsg, *sql;

//opening Log database 
  rc = sqlite3_open("Log.db", &db);
  if(rc)
  {
    fprintf(stderr, "Database could not be opened Successfully: %s\n", sqlite3_errmsg(db));
    exit(1);
  }
  else
  {
    fprintf(stderr, "Database opened Successfully\n");
  }

  //socket creation at server side
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

       printf("Accepting Successful\n");

       //id for each row in the database
      // ++idcount;
      // sprintf(id, "%d", idcount);

       //handling multiple clients
       pid = fork();
      if(pid == 0)
	     {
            //converting connceting client's ip to string format'
            conn_ip = inet_ntoa(clntAddr.sin_addr);

            //query for checking pending messages for pending client
            strcpy(sql_query,"SELECT MESSAGE FROM sender_receiver WHERE RECEIVER_IP = ");
            strcat(sql_query, "'");
            strcat(sql_query, conn_ip);
            strcat(sql_query, "' AND ");
            strcat(sql_query, "VALID = 0;");

            sql = sql_query;

            rc = sqlite3_exec(db, sql, callback, 0, &Zerrmsg);
            if(rc != SQLITE_OK)
            {
              fprintf(stderr, "SQL Error: %s\n", Zerrmsg);
              sqlite3_free(Zerrmsg);
            }
            else
            {
              fprintf(stderr, "Data Retrieved Successfully from SENDER_RECEIVER\n");
            }

            //sending the number of pending messages to the recipient
            send(newsockfd, (int*)&count_pending_msg, sizeof(int), 0);
            int i;
            printf("No. of pending messages: %d\n", count_pending_msg);
            for(i = 0; i < count_pending_msg; i++)
            {
              printf("%s\n", pending_msg[i]);
            }
            //sending the pending messages to the recipient
            for(i = 0; i < count_pending_msg; i++)
            {
              send(newsockfd,pending_msg[i],sizeof(pending_msg[i]),0);
            }

            //recieving the ip of the recipient from sender
            int n = recv(newsockfd, ipbuffer, sizeof(ipbuffer), 0);
            ipbuffer[n] = '\0';

            //sending back the ip to sender client for verification of connection
            printf("IP address received: %s\n", ipbuffer);
            send(newsockfd, ipbuffer, n, 0);

            //recieving the message from the sender which we want to send to recipient
	          int n1 = recv(newsockfd, msgbuffer, sizeof(msgbuffer), 0);
            msgbuffer[n1] = '\0';
            printf("Message Received: %s\n", msgbuffer);

            //sending recvd msg back to sender for verification
            send(newsockfd, msgbuffer, n, 0);

            //query for inserting sender ip,reciever ip ,and message
            strcpy(sql_query, "INSERT INTO sender_receiver(SENDER_IP, RECEIVER_IP, MESSAGE) VALUES(");
            strcat(sql_query, "'");
            strcat(sql_query, conn_ip);
            strcat(sql_query, "'");
            strcat(sql_query, ", ");
            strcat(sql_query, "'");
            strcat(sql_query, ipbuffer);
            strcat(sql_query, "'");
            strcat(sql_query, ", ");
            strcat(sql_query, "'");
            strcat(sql_query, msgbuffer);
            strcat(sql_query, "'");
            strcat(sql_query, ");");
            sql = sql_query;
            //printf("%s\n", sql);

            //executing query
            rc = sqlite3_exec(db, sql, callback, 0,  &Zerrmsg);
            if(rc != SQLITE_OK)
            {
              fprintf(stderr, "SQL Error: %s\n", Zerrmsg);
              sqlite3_free(Zerrmsg);
            }
            else
            {
              fprintf(stderr, "Data inserted Successfully into RECEIVER\n");
            }
            close(newsockfd);
            exit(0);
	     }
       else if(pid != 0)
       {
         wait(&status);
       }
    }
  sqlite3_close(db);
  close(sockfd);
  return 0;
}
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000
#define CLIENT_BUFF_SIZE 100
#define FILE_BUFF_SIZE 512

/*
NAME : R DEVANARAYANAN
DATE : 20 jan 2024
TITLE : Trivial File Transmission Protocol : 
               This project implements an  optimizing file transfer efficiency and ensuring seamless data 
               exchange with help of TCP/IP (Transmission control Protocol/Internetprotocol).
*/
int main ()
{
    int flag=0,send_flag=0;
    char str[10],str1[20],*file;
    printf("type -help to get the features\n\n\ntftp -> ");
    scanf("%[^\n]",str);
    if(strcmp(str,"-help")==0)
    {
	printf("connect <server ip>  :  connect to server\n");
	printf("get <file name>      :  receive file from server\n");
	printf("put <file name>      :  send file to server\n");
	printf("quit                 :  Exit the application\n");
    }
    else
	return 0;
    int sock_fd,file_fd,c_size;
    char client_buffer[CLIENT_BUFF_SIZE],file_buffer[FILE_BUFF_SIZE];
    struct sockaddr_in serv_addr; 
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	printf("Error: Could not create socket\n");
	return 0;
    } 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT); 
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

    do
    {
	printf("\nselect operation : ");
	scanf(" %[^\n]",str1);
	if(strcmp(str1,"quit")==0)
	    flag=4;
	else if(strcmp(str1,"connect")==0)
	    flag=1;
	else if(strstr(str1,"get")!=NULL && strstr(str1,".txt")!=NULL)
	{
	    file=strstr(str1," ");
	    file=++file;
	    flag=2;
	}
	else if(strstr(str1,"put")!=NULL && strstr(str1,".txt")!=NULL)
	{
	    file=strstr(str1," ");
	    file=++file;
	    flag=3;
	}
	else
	    printf("Error : Please pass correct argements or file names \n");

	switch(flag)
	{
	    case 1:
		if(strcmp(str1,"connect")==0)
		{
		    if(connect(sock_fd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) != 0)
		    {
			perror("connect");
			printf("Error connecting to server\n");
			close(sock_fd);
			exit(1);
		    }
		    else
			printf("Connection successful\n");
		}
		break;
	    case 2:

		memset(file_buffer, 0, sizeof(file_buffer));
		send(sock_fd,"get", strlen("get")+1, 0);

		while(1)
		{
		    recv(sock_fd,(void *) client_buffer,CLIENT_BUFF_SIZE,0);
		    if(strcmp(client_buffer,"get received success")==0)
		    {
			printf("recevie get success\n");
			send(sock_fd,file, strlen(file)+1, 0);
			while(1)
			{
			    recv(sock_fd,(void *) client_buffer,CLIENT_BUFF_SIZE,0);
			    if(strcmp(client_buffer,"file open success")==0)
			    {
				printf("file is present from server and file open success \n");
				file_fd = open(file,O_WRONLY | O_CREAT,0777);
				send(sock_fd,"send", strlen("send")+1, 0);
				while(1)
				{
				    recv(sock_fd,(void *) file_buffer,FILE_BUFF_SIZE,0);
				    int ret = write(file_fd,file_buffer,strlen(file_buffer)-1);
				    int count = strlen(file_buffer);
				    if(count<FILE_BUFF_SIZE)
				    {
					printf("file will received\n");
					close(file_fd);
					break;
				    }
				    if(ret>0)
				    {
					send(sock_fd,"Received", strlen("Received")+1, 0);
				    }
				}
				break;
			    }
			    else
			    {
				printf("Error : file open not present from server \n");
				break;
			    }
			}
			break;
		    }
		    else
		    {
			printf("Error : get not received\n");
			break;
		    }
		}

		break;
	    case 3:
		memset(file_buffer, 0, sizeof(file_buffer));
		file_fd = open(file,O_RDONLY);
		if(file_fd<0)
		{
		    printf("Error : File not precent\n");
		}
		else
		{
		    send(sock_fd,"put", strlen("put")+1, 0);
		    while(1)
		    {
			recv(sock_fd,(void *) client_buffer,CLIENT_BUFF_SIZE,0);
			if(strcmp(client_buffer,"put received success")==0)
			{
			    send(sock_fd,file, strlen(file)+1, 0);
			    while(1)
			    {
				recv(sock_fd,(void *) client_buffer,CLIENT_BUFF_SIZE,0);
				if(strcmp(client_buffer,"file creation success")==0)
				{
				    while(1)
				    {
					int ret = read(file_fd,file_buffer,FILE_BUFF_SIZE);
					if(strlen(file_buffer)!=FILE_BUFF_SIZE)
					{
					    send_flag=1;
					}
					if(send_flag==1)
					{
					    send(sock_fd,file_buffer,strlen(file_buffer)+1,0);
					    close(file_fd);
					    break;
					}
					if(send_flag==0)
					{
					    send(sock_fd,file_buffer,strlen(file_buffer)+1,0);
					    while(1)
					    {
						recv(sock_fd,(void *) client_buffer,CLIENT_BUFF_SIZE,0);
						if(strcmp(client_buffer,"Received")==0)
						{
						    break;
						}
					    }
					}
				    }
				}
				break;
			    }
			}
			break;
		    }
		}
		break;
	    case 4 :
		send(sock_fd,"quit", strlen("quit")+1, 0);
		break;
	    default :
		break;
	}

    }while(flag!=4);

    close(sock_fd);
}


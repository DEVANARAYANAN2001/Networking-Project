#include <sys/socket.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP       "127.0.0.1"
#define SERVER_PORT 	5000
#define SERVER_LENGTH 2
#define SERVER_BUFF	20
#define FILE_BUFF_SIZE 512
/*
NAME : R DEVANARAYANAN
DATE : 20 jan 2024
TITLE : Trivial File Transmission Protocol : 
               This project implements an  optimizing file transfer efficiency and ensuring seamless data 
               exchange with help of TCP/IP (Transmission control Protocol/Internetprotocol).
*/
int main()
{
    int sock_fd, data_sock_fd,file_fd;
    struct sockaddr_in serv_addr; 
    char serv_buffer[SERVER_BUFF],file_buffer[FILE_BUFF_SIZE];
    printf("Server is waiting...\n");
    // Create a TCP socket 
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Bind it to a particular IP address & port) 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT); 
    bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    // listen for incoming connections 
    listen(sock_fd, SERVER_LENGTH);
    // If there are any, accept and create a new data socket 
    data_sock_fd = accept(sock_fd, (struct sockaddr*)NULL, NULL); 
    memset(serv_buffer, 0, sizeof(serv_buffer));
    int s_txt_flag=0,c_txt_flag=0,send_flag=0;
    while(1)
    {
	recv(data_sock_fd, (void *) serv_buffer,SERVER_BUFF,0);
	if(strcmp(serv_buffer,"get")==0)
	{
	    send(data_sock_fd,"get received success",strlen("receive get received success")+1,0);
	    while(1)
	    {
		recv(data_sock_fd, (void *) serv_buffer,SERVER_BUFF,0);
		if(strstr(serv_buffer,".txt")!=NULL)
		{
		    file_fd = open(serv_buffer,O_RDONLY);
		    if(file_fd<0&&s_txt_flag==0)
		    {
			send(data_sock_fd,"file not present",strlen("file not present")+1,0);
		    }
		    else if(s_txt_flag==0)
		    {
			s_txt_flag=1;
			send(data_sock_fd,"file open success",strlen("file open success")+1,0);
			while(1)
			{
			    recv(data_sock_fd, (void *) serv_buffer,SERVER_BUFF,0);
			    if(strcmp(serv_buffer,"send")==0)
			    {
				while(1)
				{
				    int ret = read(file_fd,file_buffer,FILE_BUFF_SIZE);
				    if(strlen(file_buffer)!=FILE_BUFF_SIZE)
				    {
					send_flag=1;
				    }
				    if(send_flag==0)
				    {
					send(data_sock_fd,file_buffer,strlen(file_buffer)+1,0);
					while(1)
					{
					    recv(data_sock_fd, (void *) serv_buffer,SERVER_BUFF,0);
					    if(strcmp(serv_buffer,"Received")==0)
					    {
						break;
					    }
					}
				    }
				    if(send_flag==1)
				    {
					send(data_sock_fd,file_buffer,strlen(file_buffer)+1,0);
					close(file_fd);
					break;
				    }
				}
			    }
			    break;
			}
		    }
		}
		break;
	    }
	}

	if(strcmp(serv_buffer,"put")==0)
	{
	    send(data_sock_fd,"put received success",strlen("put received success")+1,0);
	    while(1)
	    {
		recv(data_sock_fd, (void *) serv_buffer,SERVER_BUFF,0);
		if(strstr(serv_buffer,".txt")!=NULL)
		{
		    file_fd = open(serv_buffer,O_WRONLY | O_CREAT,0777);
		    if(file_fd<0&&c_txt_flag==0)
		    {
			send(data_sock_fd,"file creation failure",strlen("file craetion failure")+1,0);
		    }
		    else if(c_txt_flag==0)
		    {
			c_txt_flag=1;
			send(data_sock_fd,"file creation success",strlen("file creation success")+1,0);
			while(1)
			{
			    recv(data_sock_fd, (void *) file_buffer,FILE_BUFF_SIZE,0);
			    int ret = write(file_fd,file_buffer,strlen(file_buffer)-1);
			    int count = strlen(file_buffer);
			    if(count<FILE_BUFF_SIZE)
			    {
//				printf("File will be received\n");
				close(file_fd);
				break;
			    }
			    if(ret>0)
			    {
				send(data_sock_fd,"Received",strlen("Received")+1,0);

			    }
			}
		    }
		}
		break;
	    }
	}
//	printf("Here is the client data: %s\n",serv_buffer);
	if(strcmp(serv_buffer,"quit")==0)
	{
	    break;
	}
    }
    // Close the sockets now 
    close(data_sock_fd);
    close(sock_fd);
}


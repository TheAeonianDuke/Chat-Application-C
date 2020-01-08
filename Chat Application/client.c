#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define MAX_CONNECTIONS 100
#define BUFFER_SIZE 2048

int curr_conn[100];
int curr_conn_length = 0;
int sock_fd;
char username[64];


struct thread_arguments
{
	int socket_fd;
	int connection_fd;
};


void exit_catch()
{
system("clear");
printf("\033[0;32m");
printf(R"EOF(
████████╗██╗  ██╗ █████╗ ███╗   ██╗██╗  ██╗    ██╗   ██╗ ██████╗ ██╗   ██╗██╗
╚══██╔══╝██║  ██║██╔══██╗████╗  ██║██║ ██╔╝    ╚██╗ ██╔╝██╔═══██╗██║   ██║██║
   ██║   ███████║███████║██╔██╗ ██║█████╔╝      ╚████╔╝ ██║   ██║██║   ██║██║
   ██║   ██╔══██║██╔══██║██║╚██╗██║██╔═██╗       ╚██╔╝  ██║   ██║██║   ██║╚═╝
   ██║   ██║  ██║██║  ██║██║ ╚████║██║  ██╗       ██║   ╚██████╔╝╚██████╔╝██╗
   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝       ╚═╝    ╚═════╝  ╚═════╝ ╚═╝
                                                                             
                        ██████╗ ██╗   ██╗███████╗██╗                         
                        ██╔══██╗╚██╗ ██╔╝██╔════╝██║                         
                        ██████╔╝ ╚████╔╝ █████╗  ██║                         
                        ██╔══██╗  ╚██╔╝  ██╔══╝  ╚═╝                         
                        ██████╔╝   ██║   ███████╗██╗                         
                        ╚═════╝    ╚═╝   ╚══════╝╚═╝                         
	                                                                             

)EOF");
printf("\033[0m");	
	
}


void* sender_handler()
{

	char msg_buf[BUFFER_SIZE];
	char client_msg[BUFFER_SIZE];
	
	while(1)
	{
		printf("\033[1;36m");
		printf("%s : ",username);
		printf("\033[0m");
		fflush(stdout);

		fgets(msg_buf,BUFFER_SIZE,stdin);
		strtok(msg_buf,"\n");
		if((strstr(msg_buf, "bye")) || (strstr(msg_buf, "exit")) )
		{
			sprintf(client_msg,"%s: %s\n", username,msg_buf);
			send(sock_fd,client_msg,strlen(client_msg),0);
			exit_catch();
			break;
			
		}

		else
		{
			sprintf(client_msg,"%s: %s\n", username,msg_buf);
			send(sock_fd,client_msg,strlen(client_msg),0);
			
		}

		memset(msg_buf,0,sizeof(msg_buf));
		memset(client_msg,0,sizeof(client_msg));

	}

} 

void* recver_handler()
{

	char message[BUFFER_SIZE];
	while(1)
	{
		int recv_msg = recv(sock_fd,message,BUFFER_SIZE,0);
		if(recv_msg>0)
		{
		
			printf("\n%s", message);
			
			printf("\033[1;36m");
			printf("%s : ", username);
			printf("\033[0m");
			fflush(stdout);	
		} 

		else if (recv_msg==0){
			break;
		}

		else{}

		memset(message,0,sizeof(message));
	}

	// printf("User Connected_read");
	// pthread_exit(0);


} 


void init(){
	system("clear");
	printf(R"EOF(
 █████╗ ██████╗ ██╗  ██╗██╗         ██████╗██╗  ██╗ █████╗ ████████╗
██╔══██╗██╔══██╗██║  ██║██║        ██╔════╝██║  ██║██╔══██╗╚══██╔══╝
███████║██████╔╝███████║██║        ██║     ███████║███████║   ██║   
██╔══██║██╔══██╗██╔══██║██║        ██║     ██╔══██║██╔══██║   ██║   
██║  ██║██████╔╝██║  ██║██║        ╚██████╗██║  ██║██║  ██║   ██║   
╚═╝  ╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝         ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   
                                                                    
)EOF");
	printf("\033[0;32m");
	printf("Welcome to Abhi's Chatroom! This is the Chat Room.\nType `help` to see the help options. Happy Chatting!\n\n");
	printf("\033[0m");	
}



int main(int argc, char **argv)
{
	signal(SIGINT, exit_catch);
	// Input Error
	if (argc < 2)
	{
		printf("Usage: %s <port number>\n", argv[0]);
		exit(1);
	}

	// int new_sock_conn;

	// Socket Creation
	int port = atoi(argv[1]);
	
	int conn;
	char msg_buf[BUFFER_SIZE];
	
	struct sockaddr_in server_addr;
	printf("\033[1;34m");
	printf("Enter Username: ");
	printf("\033[0m");
	fgets(username,64,stdin);
	strtok(username,"\n");
	if ((sock_fd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("\033[1;31m");
		printf("Error: Socket not created!\n Error No: %d\n",errno);
		printf("\033[0m");
		exit(1);
	}

	// Clear server buffer and set up socket settings
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	
	// Connect to server
	if ((conn = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))<0)
	{
		printf("\033[1;31m");
		printf("Error: Connection to Server Failed!\n Error No: %d\n",errno);
		printf("\033[0m");
		exit(1);
	}

	send(sock_fd,username,64,0);
	
	
	//  chatroom design function //

	init();
	// Waiting for incoming connections
	pthread_t sender_thread;

	if((pthread_create(&sender_thread, NULL, (void*)sender_handler, NULL))!=0)
	{
		printf("\033[1;31m");
		printf("Error");
		printf("\033[0m");
	}


	pthread_t recver_thread;
	if((pthread_create(&recver_thread, NULL, (void*)recver_handler, NULL))!=0)
		{
			printf("\033[1;31m");
			printf("Error");
			printf("\033[0m");
		}

	
	pthread_join(sender_thread, NULL);
	pthread_join(recver_thread, NULL);


	close(sock_fd);
	return 0;
}
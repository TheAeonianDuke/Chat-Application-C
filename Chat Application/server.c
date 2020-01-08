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

static int client_count=0;
static int uid=10;

int curr_conn[100];
int curr_conn_length = 0;




typedef struct{
	struct sockaddr_in thread_address;
	int thread_sockfd;
	int uid;
	char username[64];
} client_t;

client_t *clients[MAX_CONNECTIONS];




void msg_send_handler(char *s, int uid, int size)
{
	for (int i = 0; i < MAX_CONNECTIONS; ++i)
	{
		if((clients[i]) && (clients[i]->uid != uid))
		{
			write(clients[i]->thread_sockfd,s,size);
		}
	}
}

void help_msg_send(char *s, int uid, int size)
{
	for (int i = 0; i < MAX_CONNECTIONS; ++i)
	{
		if((clients[i]) && (clients[i]->uid == uid))
		{
			write(clients[i]->thread_sockfd,s,size+1024);
		}
	}
}





void* handle_client(void *arg)
{
	client_t *thread_args=(client_t *)arg;

	char message[BUFFER_SIZE];
	char username[64];
	int exited=0;
	
	client_count++;
	memset(message,0,sizeof(message));
	int read_size;
	if((read_size=recv(thread_args->thread_sockfd,username,64,0))<= 0)
	{	
		
		printf("Please Enter Username.\n");
		
		exited=1;
	}
	else
	{		// printf(username);
			strcpy(thread_args->username,username);
			// printf("[\033032m");

			sprintf(message,"%s joined the chat!\n", thread_args->username);
			printf("\033[1;32m");
			printf("%s", message);
			printf("\033[0m");	

			msg_send_handler(message,thread_args->uid, read_size);
		
	}


	while(1)
	{
		if(exited)
		{
			break;
		}

		int recved = recv(thread_args->thread_sockfd,message,BUFFER_SIZE,0);


		if(recved>0)
		{
			char *copy_msg=malloc(strlen(message)+1);
			strcpy(copy_msg,message);
			if (strlen(message)>0)
			{
				char * first_token= strtok(copy_msg, " ");
				char * second_token=strtok(NULL," "); 
				
				if (second_token[0]!='@' && !(strstr(message, "help")) && !(strstr(message, "show_users")))
				{
					msg_send_handler(message,thread_args->uid, recved);
					printf("%s\n",strtok(message,"\n"));	
				}
								

				if(second_token[0]=='@' && !(strstr(message, "help")) && !(strstr(message, "show_users")))
				{
					if (second_token[0] == '@') second_token++;
					for (int i = 0; i < MAX_CONNECTIONS; ++i)
					{
						if((clients[i]))
						{
							if(strcmp(second_token,clients[i]->username)==0){
								write(clients[i]->thread_sockfd,message,recved);
								printf("%s\n",strtok(message,"\n"));											
							}									
						}
					}
				}

				if((strstr(message, "bye")) || (strstr(message, "exit")) )
				{

					sprintf(message,"%s left the chat!\n", thread_args->username);
					printf("\033[1;34m");
					printf("%s", message);
					printf("\033[0m");	
					msg_send_handler(message,thread_args->uid, read_size);
					exited=1;
					
				}

				if((strstr(message, "help")) )
				{
					// printf("%s", message);
					sprintf(message,"1. To message all other connected users, type directly.\n2. To message a specific user, start with `@<NAME OF USER TO CHAT>`.\nTo exit the chatroom, type `bye` or `exit`.\n");
					help_msg_send(message,thread_args->uid, read_size);
						
				}

				// if((strstr(message, "show_users")) )
				// {
				// 	// printf("%s", message);
				// 	char *copy_user=malloc(strlen(message)+1);
				// 	strcpy(copy_user,message);
				// 	char * user_arr[MAX_CONNECTIONS];
				// 	sprintf(message,"Connected Users:\n");								
				// 	help_msg_send(message,thread_args->uid, read_size);
				// 	for (int i = 0; i < MAX_CONNECTIONS; ++i)
				// 	{
				// 		if((clients[i]))
				// 		{
							
				// 			user_arr[i]=clients[i]->username;
				// 			printf(copy_user, user_arr[i]);							
				// 			help_msg_send(copy_user,thread_args->uid,read_size);
				// 		}
				// 	}

					
						
				// }

				
			}
			

			else
			{
				printf("Error! Exiting with Error Code %d\n", errno);
				return 0;
				exited=1;				
			}
		memset(message,0,sizeof(message));
		}

	}

	close(thread_args->thread_sockfd);
	pthread_exit(0);

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
	printf("Welcome to Abhi's Chatroom! This is the Server Room.\nYou can monitor every user and activity here!\nWith Great Power Comes Great Responsibility!\n\n");
	printf("\033[0m");	
}



int main(int argc, char **argv[])
{
	// Input Error
	if (argc < 2)
	{
		// printf("\033[1;31m");
		printf("Usage: %s <port number>\n", argv[0]);
		// printf("\033[0m");	
		exit(1);
	}

	// Socket Creation
	int sock_fd=0;
	int new_sock_conn=0;
	int port = atoi(argv[1]);
	int opt = 1;

	char msg_buf[BUFFER_SIZE];

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;	
	pthread_t new_thread;

	if ((sock_fd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Error: Socket not created!\n");
		exit(1);
	}

	// Clear server buffer and set up socket settings
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY; 

	// To fix port binding error 98 and ignore pipe signals
	// if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0){
	// 		perror("setsockopt");exit(EXIT_FAILURE);
	// 	}
	// if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0){
	// 		perror("setsockopt");exit(EXIT_FAILURE);
	// 	}

	// Bind
	if(bind(sock_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
	{
		if( errno == EADDRINUSE )
        {
           printf("\033[1;31m");
           printf("Error: Binding Failed. Specified port is busy! Error No: %d\n", errno);
           printf("\033[0m");	
           exit(1);
        } else {
           printf("\033[1;31m");
           printf("Error: Binding Failed. Error No: %d\n", errno);
           printf("\033[0m");	
           exit(1);
        }
		
	}

	// Listen
	listen(sock_fd, MAX_CONNECTIONS);
	

	//  chatroom design function //
	init();

	// Waiting for incoming connections

	while(1)
	{
		socklen_t client_len=sizeof(client_addr);
		new_sock_conn=accept(sock_fd,(struct sockaddr *)NULL,NULL);

		if ((client_count+1)==MAX_CONNECTIONS)
		{
			printf("\033[1;31m");
			printf("Maximum number of clients reached! Wait for your turn!\n");
			printf("\033[0m");	
			close(sock_fd);
			break;
		}

		// Setup Client
		client_t *thread=(client_t *)malloc(sizeof(client_t));
		thread->thread_address=client_addr;
		thread->thread_sockfd=new_sock_conn;
		thread->uid=uid++;

		// Add clients
		for (int i = 0; i < MAX_CONNECTIONS; ++i)
		{
			if(!clients[i])
			{
				clients[i]=thread;
				break;
			}
		}
		// printf("HERE!1\n");
		
		pthread_create(&new_thread, NULL, &handle_client, (void *)thread);
		// printf("HERE!3\n");
	}

	close(new_sock_conn);
	close(sock_fd);
	return 0;
}
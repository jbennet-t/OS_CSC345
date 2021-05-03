/*
 * Jordan Sinoway & Michael Ralea
 * CSC345-02
 * Network Programming
 * main_client.c
 * adapted from chat_client_full.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <pthread.h>

#define PORT_NUM 1030

#define NUM_ROOMS 5
#define MAX_PPL 32

int current_rooms =0;


void error(const char *msg)
{
	perror(msg);
	exit(0);
}

typedef struct _ThreadArgs {
	int clisockfd;
} ThreadArgs;


void* thread_main_recv(void* args)
{
	pthread_detach(pthread_self());

	int sockfd = ((ThreadArgs*) args)->clisockfd;
	free(args);

	// keep receiving and displaying message from server
	char buffer[512];
	int n;

	n = recv(sockfd, buffer, 512, 0);
	while (n > 0) {
		memset(buffer, 0, 512);
		n = recv(sockfd, buffer, 512, 0);
		if (n < 0) error("ERROR recv() failed");
		printf("\n%s\n", buffer);
	}

	return NULL;
}

void* thread_main_send(void* args)
{
	pthread_detach(pthread_self());

	int sockfd = ((ThreadArgs*) args)->clisockfd;
	free(args);

	// keep sending messages to the server
	char buffer[256]; //max characters per msg
	int n;
	int name_check = 0;

	while (1) {
		// You will need a bit of control on your terminal
		// console or GUI to have a nice input window.

		if(!name_check)
		{
			printf("\nChoose a username of less than 16 characters\n");
		}
		else
		{
			printf("\n[You]: ");
		}


		memset(buffer, 0, 256);
		fgets(buffer, 255, stdin);

		if (strlen(buffer) == 1) buffer[0] = '\0';

		if (buffer[strlen(buffer)-1] == 10) //gets rid of \n
		{
			buffer[strlen(buffer)-1] = '\0';
		}

		if(!name_check && strlen(buffer) > 12)
		{
			continue;
		}
		else
		{
			name_check =1;
		}

		n = send(sockfd, buffer, strlen(buffer), 0);
		if (n < 0) error("ERROR writing to socket");

		if (n == 0) break; //cancels send after empty string
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 2) error("Please specify hostname");

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	memset((char*) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(PORT_NUM);

	printf("Try connecting to %s...\n", inet_ntoa(serv_addr.sin_addr));

	int status = connect(sockfd, (struct sockaddr *) &serv_addr, slen);
	if (status < 0) error("ERROR connecting");
	//printf("Your first message will be your username");

	int room_indicated = 1;//check if 3rd arg is a digit or not, default yes
	int new_flag = 0;

	char buffer[256];
	memset(buffer, 0, 256);
	if (argc > 2)
	{
		strcpy(buffer, argv[2]);//send 3rd arg to buffer for server side processing
	}
	else 
	{
		buffer[0] = 10; //send 10 for server side check of non-digit
		room_indicated = 0; //no room indicated or new
	}

	// checking for room numbers
	int roomChk = atoi(buffer); //return either # or 0 if not a number

	//check if "room" was a #
	if(roomChk == 0)
	{
		room_indicated = 0; //something other than a #
	}

	if(strcmp(buffer,"new") == 0) //if buffer holds new, strcmp returns 0
	{
		new_flag = 1;
	}

	// If user entered a 3rd arg thats not new, ___, or a #
	if (argc > 2 && room_indicated == 0 && new_flag == 0)
	{
		error("ERROR Invalid new");
	}

 	// send room number or new or 10 to server
	status = send(sockfd, buffer, strlen(buffer), 0);
	if (status < 0) error("ERROR writing to socket"); 

	// checking validity of room #
	if (room_indicated)
	{
		memset(buffer, 0, 256);
		int nrcv = recv(sockfd, buffer, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");
		printf("\n%s", buffer);
		// if not valid room, return error to user
		if(strcmp(buffer,"Connected Successfully\n") != 0) error("ERROR Invalid room number");
	}

	// print out new room number by receiving a room count from the server
	if(new_flag)
	{
		// printf("Client is getting a new room\n");
		// Client receives a new room number and is announced through this recv()
		memset(buffer, 0, 256);
		int nrcv = recv(sockfd, buffer, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");
		printf("\n%s\n", buffer);
	}
	else if (argc < 3) // no 3rd arg, so not new or #
	// ask user or automatically connect if 1st user
	{
		char menuBuffer[2048];
		memset(menuBuffer, 0, 2048);

		int nrcv = recv(sockfd, menuBuffer, 2048, 0);
		if (nrcv < 0) error("ERROR recv() failed");

		printf("\n-------------------------------------\n%s-------------------------------------\n", menuBuffer);

		char introMessage[9]; //holds "Connected" - printed when joining a room
		strncpy(introMessage, menuBuffer, 9);

		//check output buffer when first connecting to a room
		if (strcmp(introMessage,"Connected") != 0)
		{
			//gets # of rooms based on lines in menu bufer
			int numRooms = 0;
			for (int i = 0; i<strlen(menuBuffer) - 1; i++)
			{
				numRooms += (menuBuffer[i] == 9); //for each tab char, increases # rooms
				//9 is tab in ASCII
			}
			//checking format of user's room input
			while (1)
			{
				memset(buffer, 0, 256);
				fgets(buffer, 255, stdin);

				int room_indicated2 = 1;
				int exist_flag = 0;
				int new_flag2 = 0;
				//initial room_indicated check didn't work, so using this old fashinoed check to look at char vals
				//char 0 - 9 are 48 - 57
				//if not 0-9, then its new or ___
				//this section referenced from Stack Overflow: https://stackoverflow.com/questions/14941740/get-int-from-number-in-char-buffer
				//--------------------------------------------
				for (int i=0;i<strlen(buffer)-1;i++)
				{
					if ((buffer[i] > 57) || (buffer[i] < 48)) 
					{
						room_indicated2 = 0;
						break;
					}
				}
				//--------------------------------------------

				if(strcmp(buffer,"new\n") == 0) //if buffer holds new, strcmp returns 0
				{
					new_flag2 = 1;
				}

				if(atoi(buffer) <= numRooms) //if room num is an existing room
				{
					exist_flag = 1;
				}

				//check if existing room or new
				if ((room_indicated2 && exist_flag) || new_flag2)
				{
					break;
				}
				else
				{
					printf("\nInput a room number or enter new to make a new room: \n");
				}
			}

			// Send new room number
			if (buffer[strlen(buffer)-1] == 10)
			{
				buffer[strlen(buffer)-1] = '\0';//removing /n char
			}

			int n = send(sockfd, buffer, strlen(buffer), 0);
			if (n < 0) error("ERROR writing to socket");
		}
	}

	pthread_t tid1;
	pthread_t tid2;

	ThreadArgs* args;
	
	args = (ThreadArgs*) malloc(sizeof(ThreadArgs));
	args->clisockfd = sockfd;
	pthread_create(&tid1, NULL, thread_main_send, (void*) args);

	args = (ThreadArgs*) malloc(sizeof(ThreadArgs));
	args->clisockfd = sockfd;
	pthread_create(&tid2, NULL, thread_main_recv, (void*) args);

	// parent will wait for sender to finish (= user stop sending message and disconnect from server)
	pthread_join(tid1, NULL);

	close(sockfd);

	return 0;
}

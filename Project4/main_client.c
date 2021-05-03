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

		if (buffer[strlen(buffer)-1] == 10) //maybe gets rid of \n?
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

	int status = connect(sockfd, 
			(struct sockaddr *) &serv_addr, slen);
	if (status < 0) error("ERROR connecting");
	//printf("Your first message will be your username");

	// see if the room number entered as an arg is a digit
	int digitFlag = 1;
	// send room number to server
	char buffer[256];
	// clear buffer
	memset(buffer, 0, 256);
	// set room number into buffer
	if (argc > 2) strcpy(buffer, argv[2]);
	else 
	{
		buffer[0] = 10;
		digitFlag = 0;
	}

	// Error handling on bad room number arguments
	/* Confirm whether every part of the message, besides \n, was an integer */
	for (int i=0;i<strlen(buffer)-1;i++)
	{
		if (buffer[i] < 48 || buffer[i] > 57) // chars 0 to 9 are 48 to 57
		{
			digitFlag = 0;
			break;
		}
	}
	// If client entered a string and it's not "new" punish them
	if (argc > 2 && digitFlag == 0 && strcmp(buffer,"new") != 0) error("ERROR Invalid new");

 	// send room number to server
	status = send(sockfd, buffer, strlen(buffer), 0);
	if (status < 0) error("ERROR writing to socket"); 

	// make sure the room NUMBER you sent is valid
	if (digitFlag)
	{
	// printf("You entered a digit\n");
		memset(buffer, 0, 256);
		int nrcv = recv(sockfd, buffer, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");
		printf("\n%s", buffer);
		// if unsuccessful in finding a valid room, kill the client
		if (strcmp(buffer,"Success\n") != 0) error("ERROR Invalid room number");
	}

	// print out new room number by receiving a room count from the server
	if (strcmp(buffer,"new") == 0)
	{
		// printf("Client is getting a new room\n");
		// Client receives a new room number and is announced through this recv()
		memset(buffer, 0, 256);
		int nrcv = recv(sockfd, buffer, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");
		printf("\n%s\n", buffer);
	}
	else if (argc < 3) // "new" keyboard nor room number specified, prompt client to choose room OR automatically connect if this is first room created
	{
		// printf("Client must be prompted or is getting first room\n");
		char bigbuf[4096];
		memset(bigbuf, 0, 4096);
		int nrcv = recv(sockfd, bigbuf, 4096, 0);
		if (nrcv < 0) error("ERROR recv() failed");
		printf("\n%s\n", bigbuf);

		char word[9]; // will hold the word "Connected" - the first word of a message that prints when you automatically join a room
		strncpy(word, bigbuf, 9);
		//printf("This is word: %s", word);

		// Checking the contents of the output buffer is the easiest way I can check to see if this is the very first chat room being made
		if (strcmp(word,"Connected") != 0)
		{
			// Count the number of rooms based on the number of lines from bigbuf
			int numRooms = 0;
			for (int i=0;i<strlen(bigbuf)-1;i++)
			{
				numRooms += (bigbuf[i] == 9); // check for TAB characters
			}
			// printf("Number of rooms: %d\n", numRooms);
			// Make sure client's input is in the right format
			while (1)
			{
				// Client specifies room number or "new"
				memset(buffer, 0, 256);
				fgets(buffer, 255, stdin);

				int digitFlag = 1;
				/* Confirm whether every part of the message, besides \n, was an integer */
				for (int i=0;i<strlen(buffer)-1;i++)
				{
					if (buffer[i] < 48 || buffer[i] > 57) // chars 0 to 9 are 48 to 57
					{
						digitFlag = 0;
						break;
					}
				}
				// printf("digitFlag: %d\n", digitFlag);
				// Did client enter a valid room number or "new"
				if ((digitFlag && atoi(buffer) <= numRooms)|| strcmp(buffer,"new\n") == 0) break;
				else printf("\nChoose a room number or type [new] to create a new room: ");
			}

			// Send new room number
			// Demolish that newline character \n
			if (buffer[strlen(buffer)-1] == 10) buffer[strlen(buffer)-1] = '\0';

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

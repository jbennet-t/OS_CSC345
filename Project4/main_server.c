/*
 * Jordan Sinoway and Michael Ralea
 * CSC345-02
 * Network Programming
 * main_server.c
 * adapted from chat_server_full.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>

#define PORT_NUM 1006

#define NUM_ROOMS 5
#define MAX_PPL 4

int roomList[NUM_ROOMS];

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

typedef struct _USR {
	int clisockfd;		// socket file descriptor
	struct _USR* next;	// for linked list queue
    char* username; //client's name
    int room; //what chat room user is in
    char* color;
} USR;

USR *head = NULL;
USR *tail = NULL;

void print_clients()
{
    USR* cur = head;

    printf("The following clients are connected: \n");

	if(head == NULL)
	{
		printf("No clients connected \n");
	}

	else
	{
		while(cur != NULL)
		{
			printf("User: [%s] - [%d]\n", cur->username, cur->room);
			cur = cur->next;
		}
	}
    return;
}


//adding user to end of linked list
void add_tail(int newclisockfd, char* name_new, char* color_new, int roomnum) //added name, roomnum
{
	if (head == NULL) 
    {
		head = (USR*) malloc(sizeof(USR));
		head->clisockfd = newclisockfd;

        head->room = roomnum; //doesn't need malloc bc int

        head->username = (char*)malloc(strlen(name_new)* sizeof(char)); //malloc bc string
        strcpy(head->username, name_new);

        head->color = (char*)malloc(strlen(color_new)* sizeof(char)); //malloc bc string
        strcpy(head->color, color_new);


		head->next = NULL;
		tail = head;

	}
    else 
    {
		tail->next = (USR*) malloc(sizeof(USR));
		tail->next->clisockfd = newclisockfd;

        tail->next->room = roomnum;

        tail->next->username = (char*)malloc(strlen(name_new)* sizeof(char)); //malloc bc string
        strcpy(tail->next->username, name_new);

        tail->next->color = (char*)malloc(strlen(color_new)* sizeof(char)); //malloc bc string
        strcpy(tail->next->color, color_new);


		tail->next->next = NULL;
		tail = tail->next;
	}
}


void broadcast(int fromfd, char* message) /* broadcast message from one client to other connected clients */
{
	// figure out sender address
	struct sockaddr_in cliaddr;
	socklen_t clen = sizeof(cliaddr);
	if (getpeername(fromfd, (struct sockaddr*)&cliaddr, &clen) < 0) error("ERROR Unknown sender!");

    //check username, color, and room # of sending client
    char* username;
    char* color;
    int room;

	// traverse through all connected clients
	USR* cur = head;

    while (cur != NULL) 
	//checking for who sent message
    {
		// check if cur IS who sent msg
		if (cur->clisockfd == fromfd) 
        {
			username = cur->username;
            color = cur->color;
            room = cur->room;
			break;
		}
		cur = cur->next;
	
    	cur = head;
		//checking all connected users

		// check if cur is not the one who sent the message
        // also check if they are in the same room
		if (cur->clisockfd != fromfd && cur->room == room) 
		{
			char buffer[512];

			// prepare message
			sprintf(buffer, "%s%s [%s]:%s", color, username, inet_ntoa(cliaddr.sin_addr), message);
			int nmsg = strlen(buffer);

			// send!
			int nsen = send(cur->clisockfd, buffer, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");
		}

		cur = cur->next;
	}
}

typedef struct _ThreadArgs {
	int clisockfd;
} ThreadArgs;

void* thread_main(void* args)
{
	// make sure thread resources are deallocated upon return
	pthread_detach(pthread_self());

	// get socket descriptor from argument
	int clisockfd = ((ThreadArgs*) args)->clisockfd;
	free(args);

	//-------------------------------
	// Now, we receive/send messages
	char buffer[256];
	int nsen, nrcv;

	nrcv = recv(clisockfd, buffer, 255, 0);

	USR* cur = head;
	USR* prev = NULL;

	if(nrcv < 0)
	{
		error("ERROR recv() failed");
	}
	//------------------- checking for client disconnect --------------------------
	else if(nrcv == 0) // == 0 implies user disconnected (?)
	{
		while(cur != NULL)
		{
			if(cur->clisockfd == clisockfd)
			{
				printf("User [%s] disconnected\n", cur->username);

				roomList[cur->room]--;

				if(prev == NULL)
				{
					head = cur->next;
					free(cur);
				}
				else
				{
					prev->next = cur->next;
					free(cur);
				}
				break;
				
			}
		}
		prev = cur;
		cur = cur->next;
	

	}



	while (nrcv > 0) {
		// we send the message to everyone except the sender
		broadcast(clisockfd, buffer);

		nrcv = recv(clisockfd, buffer, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");
	}

	close(clisockfd);
	//-------------------------------

	return NULL;
}


int main(int argc, char *argv[])
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	memset((char*) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	
	//serv_addr.sin_addr.s_addr = inet_addr("192.168.1.171");	
	serv_addr.sin_port = htons(PORT_NUM);

	int status = bind(sockfd, 
			(struct sockaddr*) &serv_addr, slen);
	if (status < 0) error("ERROR on binding");

	listen(sockfd, 5); // maximum number of connections = 5

	while(1) 
    {
		struct sockaddr_in cli_addr;
		socklen_t clen = sizeof(cli_addr);
		int newsockfd = accept(sockfd, 
			(struct sockaddr *) &cli_addr, &clen);
		if (newsockfd < 0) error("ERROR on accept");

        printf("New Client Connecting...\n");

        //room number
        //add actual code here
        int room_num = 0;

        //color
        //add actual color code here
        char* user_color = "red";

        //pulling username from client input
        char username[32];
        memset(username, 0, 32); //setting memory
        int nrcv = recv(newsockfd, username, 32, 0);
        if(nrcv < 0)
        {
            error("Error recv() failed");
        }

        printf("Connected: %s - %s\n", inet_ntoa(cli_addr.sin_addr), username);


		add_tail(newsockfd, username, user_color, room_num); // add this new client to the client list

		// prepare ThreadArgs structure to pass client socket
		ThreadArgs* args = (ThreadArgs*) malloc(sizeof(ThreadArgs));
		if (args == NULL) error("ERROR creating thread argument");

        print_clients(); //print out list of clients
		
		args->clisockfd = newsockfd;

		pthread_t tid;
		if (pthread_create(&tid, NULL, thread_main, (void*) args) != 0) error("ERROR creating a new thread");
	}

	return 0; 
}


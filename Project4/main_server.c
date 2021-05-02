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

#define PORT_NUM 1029

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

char* pick_color() {
	char* color = "red";
	char* options[MAX_PPL] = {"red", "yellow", "green", "blue"};
	int n = rand() % 4;
	int unique_flag = 0;
	char* cur_color;
	USR* cur = head;
	// Generate random indexes between 0 and MAX_PPL until
	while(unique_flag = 0) {		// do until color is unique
		int n = rand() % MAX_PPL; 	// generate an index between
		cur_color = cur->color;
		while(cur != NULL) {		
			if (options[n] == cur_color) {
				unique_flag = 1;
			}
		}
	}
	return options[n];
}

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
	printf("buffer: %s\n", message);
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
	}

	cur = head;
	//checking all connected users
	while(cur != NULL)
	{
		// check if cur is not the one who sent the message
        // also check if they are in the same room
		if (cur->clisockfd != fromfd) 
		{
			char buffer[512];
			memset(buffer, 0, 512);

			// prepare message
			sprintf(buffer, "[%s|%d| [%s]:%s]: %s", color, room, inet_ntoa(cliaddr.sin_addr), username, message);
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

	char buffer[256];
	int nsen, nrcv;

	nrcv = recv(clisockfd, buffer, 255, 0);

	USR* cur = head;
	USR* prev = head;
	int temp_fd;
	// find out which element in linked list we are on
	while (cur != NULL) {
		temp_fd = cur->clisockfd;
		if (temp_fd == clisockfd) {
			break;	// we have found the correct current struct
		}
		if (cur != head) {
			prev = prev->next;
		}
		cur = cur->next;
	}

	/* First message */
	if (nrcv < 0) {		
		printf("ERROR: recv() failed");
	}
	else if (nrcv == 0) {	// if user disconnects
		printf("User [%s] DISCONNECTED\n", cur->username);
		if (cur == head) {
			head = cur->next;
			free(cur);
		}
		else {
			prev->next = cur->next;
			free(cur);
		}
		close(clisockfd);
		return NULL;
	}
	else {
		broadcast(clisockfd, buffer);
	}

	/* All messages henceforth */
	while (nrcv != 0) {
		if (nrcv < 0) {		
			error("ERROR: recv() failed");
		}
		else {
			broadcast(clisockfd, buffer);
		}
		memset(buffer, 0, sizeof(buffer));
		nrcv = recv(clisockfd, buffer, 255, 0);
	}
	printf("User [%s] DISCONNECTED\n", cur->username);
	if (cur == head) {
		head = cur->next;
		free(cur);
	}
	else {
		prev->next = cur->next;
		free(cur);
	}
	close(clisockfd);
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
        char* user_color = pick_color();

		//+1 to people in room
		roomList[room_num-1] += 1;

        //pulling username from client input
        char username[32];

        memset(username, 0, 32); //setting memory
        int nrcv = recv(newsockfd, username, 32, 0);
        if(nrcv < 0)
        {
            error("Error recv() failed");
        }

		username[strlen(username) - 1] = '\0';		// introduces weird bug where user attempts to connect but cannot

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
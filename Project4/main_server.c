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

#define PORT_NUM 1030

#define NUM_ROOMS 5
#define MAX_PPL 7

int roomList[NUM_ROOMS]; //array to count num ppl in each room
int current_rooms; //# of rooms open

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
	char* options[MAX_PPL] = {"\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m"}; 
	//red, yellow, green, blue, magenta, cyan, white
	int n = rand() % 7;
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

int get_create_room(int clisockfd, struct sockaddr_in cli_addr, char* room, char *arg3)
{
	int digitFlag = 1;
	int noFlag = 0;
	int roomNum;
	/* Confirm whether every part of the message, besides \n, was an integer */
	for (int i=0;i<strlen(room)-1;i++)
	{
		if (room[i] < 48 || room[i] > 57) // chars 0 to 9 are 48 to 57
		{
			digitFlag = 0;
			break;
		}
	}

	// check if room was submitted as nothing
	if (room[0] == 10)
	{
		digitFlag = 0;
		noFlag = 1;
	}

	// room input is an integer room, check that it exists and return if true
	if (digitFlag)
	{
		// printf("Digit room specified\n");
		roomNum = atoi(room);

		// printf("roomNum: %d\n", roomNum);
		// Check to see if room num is valid
		if (roomNum > 0 && roomNum <= NUM_ROOMS && roomNum <= current_rooms) 
		{
			memset(room,0,256);
			sprintf(room,"Success\n");
			// Send junk array to client
			int nmsg = strlen(room);
			int nsen = send(clisockfd, room, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");
			return roomNum;
		}
		else // punish the client if invalid
		{
			memset(room,0,256);
			sprintf(room,"Well this is unfortunate\n");
			// Send junk array to client
			int nmsg = strlen(room);
			int nsen = send(clisockfd, room, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");
			return 0;
		}
		// else error("ERROR room number out of range or nonexistent");
	}
	// room input is a string, check that it = "new" and return new room number if true, broadcast to client
	else
	{
		int newFlag = strcmp(room,"new");
		// client asks for new room, create it
		if ((newFlag == 0 && current_rooms < NUM_ROOMS) || (noFlag == 1 && current_rooms == 0))
		{
			// printf("Creating new room\n");
			current_rooms += 1;
			roomNum = current_rooms;

			// Send message of new chat room back to client
			char buffer[256];
			memset(buffer, 0, 256);
			sprintf(buffer, "Connected to %s with new room number: %d\n", inet_ntoa(cli_addr.sin_addr), roomNum);

			int nmsg = strlen(buffer);
			int nsen = send(clisockfd, buffer, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");

			// Clear this again because we were having text problems
			memset(buffer, 0, 256);

			return roomNum;
		}
		else if (noFlag == 1 && current_rooms != 0 && current_rooms < NUM_ROOMS) // client input nothing, prompt client to choose a room from a list
		{
			// printf("No room specified from client\n");

			// Time to prompt user to choose a room
			int maxLen = 4096;
			// Create buffer array of all possible choices to send to client
			char buffer[4096];
			memset(buffer, 0, 4096);

			// Start putting stuff in the buffer array
			sprintf(buffer, "Available Rooms:\n If none are listed, please type new");
			for (int i=0;i<current_rooms;i++)
			{
				char room[32];
				int people = roomList[i];

				// Create a string for each room
				if (people > 1 || people == 0) sprintf(room, "\tRoom %d: %d people\n", i+1, people);
				else sprintf(room, "\tRoom %d: %d person\n", i+1, people);

				// Add room string to buffer array
				if (strlen(buffer) + strlen(room) < maxLen) strcat(buffer, room);
				else error("ERROR max length of room decision string exceeded");
			}

			strcat(buffer, "Choose a room number or type [new] to create a new room: ");

			// Send prompt to client and let them choose a room
			int nmsg = strlen(buffer);
			int nsen = send(clisockfd, buffer, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");

			// Get new input from client
			char newbuf[256];
			memset(newbuf, 0, 256);
			int nrcv = recv(clisockfd, newbuf, 255, 0);
			if (nrcv < 0) error("ERROR recv() failed");

			// Get room num using a new call to the function
			return get_create_room(clisockfd, cli_addr, newbuf, arg3);
		}
	}
	return 0;
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
	//printf("buffer: %s\n", message);
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
			//adding RESET color val to front of color
			//snprintf(color, sizeof(color), "\x1B[0m", color);
			//char* color_temp;
			//strcat(color_temp, "\x1B[0m");
			//strcat(color_temp, color);


			char buffer[512];
			memset(buffer, 0, 512);

			// prepare message
			sprintf(buffer, "\x1B[0m%s[|room:%d| [%s]:%s]: %s  \x1B[0m", color, room, inet_ntoa(cliaddr.sin_addr), username, message);
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
		printf("User [%s] DISCONNECTED\n", cur->username); //removing client from LL
		roomList[cur->room - 1]--;

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
	roomList[cur->room - 1]--;
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

		// Temporarily link with just-created client to retrieve room number
		char room[256];
		// Clear array for new room number and receive room number from client
		memset(room, 0, 256);
		int nrcv = recv(newsockfd, room, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");

		// Get either a new or existing ROOM NUMBER
		int room_num = get_create_room(newsockfd, cli_addr, room, argv[2]);
		// if (roomNum == 0) error("ERROR getRoomNum() failed");
		// getRoomNum failed - punish the client, not the server
		if (room_num == 0)
		{
			printf("getRoomNum() failed oh no\n");
			continue;
		}

        //color
        //add actual color code here
        char* user_color = pick_color();

		//+1 to people in room
		roomList[room_num-1] += 1;

        //pulling username from client input
        char username[256];

        memset(username, 0, 256); //setting memory
        nrcv = recv(newsockfd, username, 255, 0);
        if(nrcv < 0)
        {
            error("Error recv() failed");
        }	

		//gets rid of extra \n
		//username[strlen(username) - 1] = '\0';		// introduces weird bug where user attempts to connect but cannot

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
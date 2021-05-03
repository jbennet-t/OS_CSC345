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

int get_create_room(int clisockfd, struct sockaddr_in cli_addr, char* room)
{
	int room_indicated = 1; //default indicated
	int no_input = 0;
	int roomNum = atoi(room); //return either # or 0 if not a number


	//check if "room" was a #
	if(roomNum == 0)
	{
		room_indicated = 0; //something other than a #
	}

	// check if room was empty
	if (room[0] == 10) //"10" passed from client to indicate empty
	{
		room_indicated = 0;
		no_input = 1;
	}

	// arg3 was a room num, not "new" or ___
	if(room_indicated)
	{
		roomNum = atoi(room);

		// Check if room num is valid
		if (roomNum > 0 && roomNum <= NUM_ROOMS && roomNum <= current_rooms) 
		{
			memset(room,0,256);
			sprintf(room,"Connected Successfully\n");

			int nmsg = strlen(room);
			int nsen = send(clisockfd, room, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");
			return roomNum;
		}
		else // punish the client if invalid
		{
			memset(room,0,256);
			sprintf(room,"Invalid room #\n");

			int nmsg = strlen(room);
			int nsen = send(clisockfd, room, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");
			return 0;
		}
	}
	// else room is not a #, so its new or ___
	else
	{
		int request_new = strcmp(room,"new");

		if ((request_new == 0 && (current_rooms < NUM_ROOMS)) || (no_input == 1 && current_rooms == 0))
		{
			current_rooms += 1;
			roomNum = current_rooms;

			char buffer[256]; //pass connect msg to user
			memset(buffer, 0, 256);
			sprintf(buffer, "Connected to %s with new room number: %d\n", inet_ntoa(cli_addr.sin_addr), roomNum);

			int nmsg = strlen(buffer);
			int nsen = send(clisockfd, buffer, nmsg, 0);
			if (nsen != nmsg) error("ERROR send() failed");
			memset(buffer, 0, 256);

			return roomNum;
		}
		else if (no_input == 1 && current_rooms != 0 && current_rooms < NUM_ROOMS) 
		//if not "new", its no input, so user needs to choose a room
		{
			char menuBuffer[2048]; //creating menuBuffer for showing user room menu
			memset(menuBuffer, 0, 2048);

			sprintf(menuBuffer, "Open Rooms:\n If none are listed, please type new\n");
			for (int i=0;i<current_rooms;i++)
			{
				char room[32];
				int people = roomList[i];

				//make new line for each room
				if (people > 1 || people == 0) sprintf(room, "\tRoom #%d: %d users connected\n", i+1, people);
				else sprintf(room, "\tRoom #%d: %d users connected\n", i+1, people);

				if (strlen(menuBuffer) + strlen(room) < sizeof(menuBuffer)) strcat(menuBuffer, room);//add line to menuBuffer
				else error("ERROR too many chars in line");
			}

			strcat(menuBuffer, "Input a room number or enter 'new' to make a new room: \n");

			int nmsg = strlen(menuBuffer);
			int nsen = send(clisockfd, menuBuffer, nmsg, 0);//send menuBuffer to client
			if (nsen != nmsg) error("ERROR send() failed");

			char requestBuff[256]; //getting room request from client
			memset(requestBuff, 0, 256);
			int nrcv = recv(clisockfd, requestBuff, 255, 0);
			if (nrcv < 0) error("ERROR recv() failed");

			return get_create_room(clisockfd, cli_addr, requestBuff);//send back to top to enter room #
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

//send msg from one client to other clients in room
void broadcast(int fromfd, char* message)
{
	// figure out sender address
	struct sockaddr_in cliaddr;
	socklen_t clen = sizeof(cliaddr);
	if (getpeername(fromfd, (struct sockaddr*)&cliaddr, &clen) < 0) error("ERROR Unknown sender!");

    char* username; //username of client
    char* color; //color assigned to client
    int room; //room where they are

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
	if (nrcv < 0) 
	{		
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

	listen(sockfd, 5); // maximum connections

	while(1) 
    {
		struct sockaddr_in cli_addr;
		socklen_t clen = sizeof(cli_addr);
		int newsockfd = accept(sockfd, 
			(struct sockaddr *) &cli_addr, &clen);
		if (newsockfd < 0) error("ERROR on accept");

        printf("New Client Connecting...\n");

		char room[256];
		memset(room, 0, 256);
		//getting room # from client
		int nrcv = recv(newsockfd, room, 255, 0);
		if (nrcv < 0) error("ERROR recv() failed");

		int room_num = get_create_room(newsockfd, cli_addr, room);
		//^ get room number or create new room based on client request

		if (room_num == 0) //if get_create_room failed
		{
			printf("Could not create or find room\n");
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define PORT_NUM 1004

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	// Client must know to whom to connect
	if (argc < 2) error("Please speicify hostname");

	// Create client socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	// Retrieve host information using host name
	//
	//	If you know actual IP address of the server you want to
	//	connect (e.g. 192.168.x.x), you can use inet_addr(), and
	//	directly pass the returned value into serv_addr as
	//
	//		serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	struct hostent* server = gethostbyname(argv[1]);
	if (server == NULL) error("ERROR, no such host");

	// Describe client socket for connecting to the server
	//
	//	The server information will be populated using the hostent
	//	Since the address is in string format, we use memcpy()
	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	memset((char*) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char*)server->h_addr, 
			(char*)&serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(PORT_NUM);

	// Connect to the specified port on remote server address
	//	(BLOCKing function - will wait)
	int status = connect(sockfd, 
			(struct sockaddr *) &serv_addr, slen);
	if (status < 0) error("ERROR connecting");

	// Send/Receive in stream socket (BLOCKing function)
	char buffer[256];
	int n;

	//-------------------------------------------
	printf("Please enter the message: ");
	memset(buffer, 0, 256);
	fgets(buffer, 255, stdin);
	//-------------------------------------------
	
	n = send(sockfd, buffer, strlen(buffer), 0);
	if (n < 0) error("ERROR writing to socket");
	
	// always clear buffer before receiving a new message
	memset(buffer, 0, 256);
	n = recv(sockfd, buffer, 255, 0);
	if (n < 0) error("ERROR reading from socket");
	
	//-------------------------------------------
	printf("Message from server: %s\n", buffer);
	//-------------------------------------------
	
	close(sockfd);
	
	return 0;
}


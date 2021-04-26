#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 1004

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	// Create socket for server: This socket will not be used for
	//	actual data send/receive. It will be used to get new client
	//	connection (i.e. socket for actual send/receive operations)
	//
	//	@1: AF_INET (IPv4, ineternet) or AF_UNIX (local)
	//	@2: SOCK_STREAM or SOCK_DGRAM (socket type)
	//	@3: IPPROTO_TCP or IPPROTO_UDP (protocol; default 0)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	// Describe server address and port information
	//
	//	htons(): converts argument unsigned short integer 
	//		from host byte order to network byte order
	//		Similar functions: htonl(), ntohs(), ntohl()
	//
	//	sockaddr: structure defined in Socket API for
	//		describing address information in an address family
	//
	//		struct sockaddr {
	//			unsigned short sa_family;	// e.g. AF_INET
	//			char sa_data[14];
	//		}
	//
	//	sockaddr_in: Internet address (IPv4) version of sockaddr
	//		You can *cast* sockaddr_in to sockaddr since
	//		sockaddr has 14 bytes reserved space for address
	//		information after address family (AF_INET) as above.
	struct sockaddr_in serv_addr;
	socklen_t slen = sizeof(serv_addr);
	memset((char*) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// 4 bytes (32 bits)
	serv_addr.sin_port = htons(PORT_NUM);	// 2 bytes (16 bits)
	
	// Assign specified address and port to socket
	// You may skip this bind() for following cases:
	//	For stream socket,
	//		If you know the destination already at setup phase
	//		If you don't need to know the port sending from
	//	For datagram socket,
	//		If you are only sending, and not receiving
	int status = bind(sockfd, 
			(struct sockaddr*) &serv_addr, slen);
	if (status < 0) error("ERROR on binding");

	// Start listening (NON-BLOCKing function - returns immediately)
	//	@1: socket to start listening
	//	@2: number of clients that can wait for a connection
	//	ret: 0 on success, -1 on error
	listen(sockfd, 5);

	// Accept incoming connection (BLOCKing function - will wait)
	//	@1: server socket that was listening
	//	@2: new sockaddr_in structure to store client socket info
	//	@3: length (note that this is pass-by-value-result)
	//
	// This function will remove the next waiting connection from
	// the listening queue for the server socket
	struct sockaddr_in cli_addr;
	socklen_t clen = sizeof(cli_addr);
	int newsockfd = accept(sockfd, 
			(struct sockaddr *) &cli_addr, &clen);
	if (newsockfd < 0) error("ERROR on accept");

	// Send/Receive in stream socket (BLOCKing functions)
	//	@1: connection socket to use for transmission
	//	@2: message to send or buffer to receive message
	//	@3: number of bytes to send or buffer size 
	//	@4: special options; typically zero
	//	ret: actual number of bytes processed
	char buffer[256];
	int n;

	// always clear buffer before receiving a new message
	memset(buffer, 0, 256);
	n = recv(newsockfd, buffer, 255, 0);
	if (n < 0) error("ERROR reading from socket");

	//-----------------------------------------------
	printf("Message from client: %s\n", buffer);
	//-----------------------------------------------

	n = send(newsockfd, buffer, strlen(buffer), 0);
	if (n < 0) error("ERROR writing to socket");
	
	// close connection
	close(newsockfd);

	// close socket (0 on success, -1 on error)
	close(sockfd);
	
	return 0; 
}


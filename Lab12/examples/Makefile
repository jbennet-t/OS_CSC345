all:
	gcc -o BroadcastReceiver BroadcastReceiver.c DieWithError.c
	gcc -o BroadcastSender BroadcastSender.c DieWithError.c
	gcc -o MulticastReceiver MulticastReceiver.c DieWithError.c
	gcc -o MulticastSender MulticastSender.c DieWithError.c
	gcc -o SigAction SigAction.c DieWithError.c
	gcc -o TCPEchoClient TCPEchoClient.c DieWithError.c
	gcc -o TCPEchoServer TCPEchoServer.c DieWithError.c HandleTCPClient.c
	gcc -o TCPEchoServer-Fork TCPEchoServer-Fork.c DieWithError.c HandleTCPClient.c CreateTCPServerSocket.c AcceptTCPConnection.c
	gcc -o TCPEchoServer-ForkN TCPEchoServer-ForkN.c DieWithError.c HandleTCPClient.c CreateTCPServerSocket.c AcceptTCPConnection.c
	gcc -o TCPEchoServer-Fork-SIGCHLD TCPEchoServer-Fork-SIGCHLD.c DieWithError.c HandleTCPClient.c CreateTCPServerSocket.c AcceptTCPConnection.c
	gcc -o TCPEchoServer-Select TCPEchoServer-Select.c DieWithError.c HandleTCPClient.c CreateTCPServerSocket.c AcceptTCPConnection.c
	gcc -o TCPEchoServer-Thread TCPEchoServer-Thread.c DieWithError.c HandleTCPClient.c CreateTCPServerSocket.c AcceptTCPConnection.c -lpthread
	gcc -o UDPEchoClient UDPEchoClient.c DieWithError.c
	gcc -o UDPEchoClient-Timeout UDPEchoClient-Timeout.c DieWithError.c
	gcc -o UDPEchoServer UDPEchoServer.c DieWithError.c 
	gcc -o UDPEchoServer-SIGIO UDPEchoServer-SIGIO.c DieWithError.c 

clean:
	rm BroadcastReceiver
	rm BroadcastSender
	rm MulticastReceiver
	rm MulticastSender
	rm SigAction
	rm TCPEchoClient
	rm TCPEchoServer
	rm TCPEchoServer-Fork
	rm TCPEchoServer-ForkN
	rm TCPEchoServer-Fork-SIGCHLD
	rm TCPEchoServer-Select
	rm TCPEchoServer-Thread
	rm UDPEchoClient
	rm UDPEchoClient-Timeout
	rm UDPEchoServer
	rm UDPEchoServer-SIGIO

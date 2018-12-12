/*
	Single thread forwarder.
*/

#include<stdlib.h>	//strlen
#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<stdbool.h>

// -----------------------------------------------------------------------------

const int LISTEN_PORT = 6666;
const char* FORWARD_HOST = "127.0.0.1";
const int FORWARD_PORT = 6667;

// -----------------------------------------------------------------------------

int openForwardSocket(int *sock) {
	struct sockaddr_in server;

	//Create socket
	*sock = socket(AF_INET , SOCK_STREAM , 0);
	if (*sock == -1)	{
		printf("Could not create forward socket");
		return 1;
	}
	puts("Forward socket created");

	server.sin_addr.s_addr = inet_addr(FORWARD_HOST);
	server.sin_family = AF_INET;
	server.sin_port = htons(FORWARD_PORT);

	//Connect to remote server
	if (connect(*sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("Forward connect failed. Error");
		return 1;
	}

	puts("Forward socket connected\n");
}

// -----------------------------------------------------------------------------

int main(int argc , char *argv[])
{
	int socket_desc , sock1 , c , read_size;
	struct sockaddr_in server , client;
	char *message, client_message[2000];

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( LISTEN_PORT );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	sock1 = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (sock1 < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");


	int *sock2_desc;
	sock2_desc = malloc(1);
	openForwardSocket(sock2_desc);
	int sock2 = *(int*)sock2_desc;

	//Send some messages to the client
	message = "Greetings! I am your connection handler\n";
	write(sock1, message, strlen(message));
	message = "Now type something and i shall repeat what you type \n";
	write(sock1, message, strlen(message));

	bool doRun = true;
	while( doRun ) {

		// read from sock1:
		// read_size = recv(sock1 , client_message , 2000 , 0);
		read_size = recv(sock1 , client_message , 2000 , MSG_DONTWAIT);
		// read_size = read(sock1 , client_message , 2000);
		// printf("RS1: %i", read_size);
		if( read_size > 0) {
			write(sock2, client_message , read_size);	//forward the message
		} else if (read_size == 0) {
			puts("Client disconnected");
			fflush(stdout);
			doRun = false;
		} else if(read_size == -1) {
			// perror("recv failed");
			// doRun = false;
		}

		// read from sock2:
		// read_size = recv(sock2 , client_message , 2000 , 0);
		read_size = recv(sock2 , client_message , 2000 , MSG_DONTWAIT);
		// read_size = read(sock2 , client_message , 2000);
		// printf("RS2: %i", read_size);
		if( read_size > 0) {
			write(sock1, client_message , read_size);	//forward the message back
		} else if (read_size == 0) {
			puts("Forward disconnected");
			fflush(stdout);
			doRun = false;
		} else if(read_size == -1) {
			// perror("forward recv failed");
			// doRun = false;
		}

	}	// ~while

	close(sock1);
	close(sock2);
	free(sock2_desc);

	puts("KOHEU.");

	return 0;
}

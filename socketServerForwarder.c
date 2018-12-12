/*
	Multithreaded forwarder.
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write, close
#include<pthread.h> //for threading , link with lpthread
#include <sched.h>	//yield
#include<netdb.h>	//hostent

// -----------------------------------------------------------------------------

int listenPort = 666;
// char* forwardAddr = (char*)"127.0.0.1";
char forwardAddr[100];
int forwardPort = 667;

// -----------------------------------------------------------------------------

// forward declarations:
void *connection_handler(void *);
int openForwardSocket(int* forwardSocket);

// -----------------------------------------------------------------------------

int hostnameToIp(char* hostname , char* ip) {
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	if ( (he = gethostbyname( hostname ) ) == NULL) {
		// herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;

	for(i = 0; addr_list[i] != NULL; i++) {
		strcpy(ip , inet_ntoa(*addr_list[i]) );
		return 0;
	}

	return 1;
}

// -----------------------------------------------------------------------------

void processCmdArguments(int argc, char *argv[]) {
	if(argc != 3 && argc != 4 ) {
		puts("\nUsage: ./socketServerForwarder LOCAL_PORT REMOTE_IP_ADDR [REMOTE_PORT]\n\n");
		exit(1);
	}
	listenPort = atoi(argv[1]);
	char* forwardHost = argv[2];
	if(argc == 4) forwardPort = atoi(argv[3]);
	else forwardPort = listenPort;

	int err = hostnameToIp(forwardHost, forwardAddr);
	if(err) {
		printf("Could not resolve '%s'", forwardHost);
		exit(1);
	}

	printf("\n listening on local port %i\n forwarding to %s:%i\n\n", listenPort, forwardAddr, forwardPort);
}

// -----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
	puts("## socketServerForwarder ##");
	processCmdArguments(argc, argv);

	int socket_desc, client_sock, c, *new_sock;
	struct sockaddr_in server, client;

	//create server socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
		return 1;
	}
	puts("Server socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( listenPort );

	//bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("Bind failed. Error");
		return 1;
	}
	puts("Bind successful");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections..");
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
		// print out remote client IP addr:
		struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    /*int res = */getpeername(client_sock, (struct sockaddr *)&addr, &addr_size);
    char clientip[20];
    strcpy(clientip, inet_ntoa(addr.sin_addr));
		printf("Accepted connection from: %s\n", clientip);

		pthread_t worker_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;

		if( pthread_create( &worker_thread , NULL ,  connection_handler , (void*) new_sock) < 0)	{
			perror("Could not create thread");
			return 1;
		}

		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}

	if (client_sock < 0) {
		perror("accept failed");
		return 1;
	}

	return 0;
}

// -----------------------------------------------------------------------------

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc) {
	//Get the socket descriptor
	int sock1 = *(int*)socket_desc;
	int read_size;
	char dataBuffer[2048];

	int *sock2_desc;
	sock2_desc = malloc(1);
	openForwardSocket(sock2_desc);
	int sock2 = *(int*)sock2_desc;

	bool doRun = true;
	while( doRun ) {
		// read from sock1:
		read_size = recv(sock1 , dataBuffer , 2000 , MSG_DONTWAIT);	// non-blocking read
		if( read_size > 0) {
			write(sock2, dataBuffer , read_size);	//forward the message
		} else if (read_size == 0) {
			puts("Client disconnected");
			fflush(stdout);
			doRun = false;
		} else if(read_size == -1) {
			// = no data available
		}

		// read from sock2:
		read_size = recv(sock2 , dataBuffer , 2000 , MSG_DONTWAIT);	// non-blocking read
		if( read_size > 0) {
			write(sock1, dataBuffer , read_size);	//forward the message back
		} else if (read_size == 0) {
			puts("Forward disconnected");
			fflush(stdout);
			doRun = false;
		} else if(read_size == -1) {
			// = no data available
		}

		sched_yield();
	}

	close(sock1);
	free(socket_desc);
	close(sock2);
	free(sock2_desc);

	// puts("thread exit");

	return 0;
}

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

	server.sin_addr.s_addr = inet_addr( forwardAddr );
	server.sin_family = AF_INET;
	server.sin_port = htons( forwardPort );

	//Connect to remote server
	if (connect(*sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("Forward connect failed. Error");
		return 1;
	}

	puts("Forward socket connected");
}

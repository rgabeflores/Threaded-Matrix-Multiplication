// TCP Client program 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h>
#include <unistd.h>

#include "Client.h"

#define NUM_THREADS 2
#define PORT 5000 
#define MAXLINE 1024

int ID;

/*
	Thread for registration and file location requests to the server
	and file requests to a client.
*/
void * TCP_CALL(){

	// Registration
	int sockfd_TCP, rc; 
	char buffer[MAXLINE]; 
	struct sockaddr_in servaddr;
	char* message = "0,file2.txt"; // Name of file to register
	
	// Creating socket file descriptor 
	if ((sockfd_TCP = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("socket creation failed"); 
		exit(0); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 

	// Filling server address information
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	// Establish Connection to Destination TCP Port
	if (connect(sockfd_TCP, (struct sockaddr*)&servaddr, 
							sizeof(servaddr)) < 0) { 
		printf("\n Error : Connect Failed \n"); 
	} 

	// Send Initial TCP Message
	memset(buffer, 0, sizeof(buffer)); 
	strcpy(buffer, message); 
	rc = write(sockfd_TCP, buffer, sizeof(buffer));
	if (rc < 0) printf("\nThere was an error sending.\n");

	// Receive Response from Server
	printf("Message from server: "); 
	rc = read(sockfd_TCP, buffer, sizeof(buffer));
	if (rc < 0) printf("\nThere was an error receiving.\n");
	
	puts(buffer);

	close(sockfd_TCP);

	/*
		TO-DO:
			- File Location Request
				- Listen to User input for file request (infinite for loop)
				- TCP connection with server to get location (ID)

			- File Request
				- Broadcast UDP to find client with ID
				- TCP connection to another client
						servaddr.sin_addr.s_addr = inet_addr(<CLIENT ADDRESS>); 
	*/

}

void * UDP_CALL(){

	int sockfd_UDP;
	struct sockaddr_in servaddr;
	// char* message = ID + '0';
	char * message = "Hello"; 

	if ((sockfd_UDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		printf("UDP socket creation failed"); 
		exit(0); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 

	// Filling server address information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	// Send Initial UDP Message
	sendto(sockfd_UDP, (const char*)message, strlen(message), 
		0, (const struct sockaddr*)&servaddr, 
		sizeof(servaddr)); 

	// Send UDP Hello every 5 seconds
	for(;;){
		sleep(5);		
		sendto(sockfd_UDP, (const char*)message, strlen(message), 
		0, (const struct sockaddr*)&servaddr, 
		sizeof(servaddr)); 
	}
}

int main(){ 

	int rc;
	pthread_t thread_array[NUM_THREADS];
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	rc = pthread_create(&thread_array[0], NULL, requestFile, (void*) NULL);
	if(rc != 0) printf("There was a problem initializing the TCP connection.");

	rc = pthread_create(&thread_array[1], NULL, UDP_CALL, (void*) NULL);
	if(rc != 0) printf("There was a problem initializing the UDP messages.");
	
	for(int i = 0;i<2;i++){
		pthread_join(thread_array[i],NULL);
	}
} 

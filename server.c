#include "server.h"
#include "encryption.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void* receiverThread(void* args) {
	struct receiverArgsStruct* receiverArgs = args;

	int sockfd;
	struct sockaddr_in si_server, si_client, si_remote_server;
	socklen_t client_size;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&si_server, 0, sizeof(struct sockaddr_in));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(atoi(receiverArgs->myPortNumber));
	si_server.sin_addr.s_addr = inet_addr("127.0.0.1");

	si_remote_server.sin_family = AF_INET;
	si_remote_server.sin_port = htons(atoi(receiverArgs->remotePortNumber));
	si_remote_server.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(sockfd, (struct sockaddr*)&si_server, sizeof(si_server)) < 0) {
		perror("Error bind failed\n");
		exit(1);
	}
	client_size = sizeof(si_client);
	while(1) {
		if(recvfrom(sockfd, &buffer, BUFFER_SZ, 0, (struct sockaddr*)&si_client, &client_size) < 0) {
			perror("ERROR recvfrom\n");
			exit(1);
		}
		pthread_mutex_lock(&bufferMutex);
		//printf("[+] received message before decrypted: %s\n", buffer);
		decrypt(300, buffer);
		//printf("[+] received message decrypted: %s", buffer);
		if(strcmp(buffer, "!status\n") == 0) {
			bzero(buffer,BUFFER_SZ);
			sprintf(buffer, "!online\n");
			encrypt(300, buffer);
			if(sendto(sockfd, &buffer, BUFFER_SZ, 0, (struct sockaddr*)&si_remote_server, sizeof(si_remote_server)) < 0) {
				perror("ERROR sendto\n");
				exit(1);
			}
			decrypt(300, buffer);
			pthread_mutex_unlock(&bufferMutex);
			continue;
		}
		if(strcmp(buffer, "!online\n") == 0) {
			statusFlag = true;
			bzero(buffer, BUFFER_SZ);
			pthread_mutex_unlock(&bufferMutex);
			continue;
		}
		if(strcmp(buffer, "!exit\n") == 0 && exitFlag == false) {
			printf("!exit\n");
		}
		if(strcmp(buffer, "!exit\n") == 0) {
			/*if(sendto(sockfd, &buffer, BUFFER_SZ, 0, (struct sockaddr*)&si_remote_server, sizeof(si_remote_server)) < 0) {
				perror("ERROR sendto\n");
				exit(1);
			}*/
			exitFlag = true;
			//printf("!exit\n");
			//printf("client IP address: %s\n", inet_ntoa(si_remote_server.sin_addr));
			//printf("client port number: %d\n", ntohs(si_remote_server.sin_port));
			pthread_mutex_unlock(&bufferMutex);
			close(sockfd);
			break;
		}
		List_add(receiverArgs->receiverList, strdup(buffer));

		bzero(buffer, BUFFER_SZ);
		pthread_mutex_unlock(&bufferMutex);
	}
}

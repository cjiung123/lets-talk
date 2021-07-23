#include "client.h"
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

void* senderThread(void* args) {
	struct senderArgsStruct* senderArgs = args;

	int sockfd;
	struct addrinfo hints, *results, *rp;
	struct sockaddr_in si_server, si_my_server;
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if((rv = getaddrinfo(senderArgs->remoteMachineName, senderArgs->remotePortNumber, &hints, &results)) != 0) {
		perror("getaddrinfo error\n");
		exit(1);
	}

	for(rp = results; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sockfd == -1)
			continue;
		//printf("%d, %d, %d\n", rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		si_server = *(struct sockaddr_in*) rp->ai_addr;
		//printf("IP address: %s\n", inet_ntoa(si_server.sin_addr));
		//printf("port number: %d\n", ntohs(si_server.sin_port));
		break;
	}

	freeaddrinfo(results);

	si_my_server.sin_family = AF_INET;
	si_my_server.sin_port = htons(atoi(senderArgs->myPortNumber));
	si_my_server.sin_addr.s_addr = inet_addr("127.0.0.1");

	/*sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(atoi(senderArgs->remotePortNumber));
	si_server.sin_addr.s_addr = inet_addr("127.0.0.1");*/

	char* curMsg;
	while(1) {
		if(exitFlag == true) {
			close(sockfd);
			break;
		}
		if(List_count(senderArgs->senderList) > 0) {
			pthread_mutex_lock(&bufferMutex);
			if((curMsg = List_first(senderArgs->senderList)) != NULL) {
				encrypt(300, curMsg);
				if(sendto(sockfd, curMsg, BUFFER_SZ, 0, (struct sockaddr*)&si_server, sizeof(si_server)) < 0 ) {
					perror("ERROR sendto\n");
					exit(1);
				}
				//printf("[+] sent message: %s\n", curMsg);
				decrypt(300, curMsg);
				if(strcmp(curMsg, "!status\n") == 0) {
					pthread_mutex_unlock(&bufferMutex);
					sleep(1);
					pthread_mutex_lock(&bufferMutex);
					if(statusFlag == true) {
						printf("Online\n");
						statusFlag = false;
					}
					else {
						printf("Offline\n");
					}
				}
				if(strcmp(curMsg, "!exit\n") == 0) {
					encrypt(300, curMsg);
					if(sendto(sockfd, curMsg, BUFFER_SZ, 0, (struct sockaddr*)&si_my_server, sizeof(si_my_server)) < 0 ) {
						perror("ERROR sendto\n");
						exit(1);
					}
					exitFlag = true;
				}


				//printf("length of sent message %ld\n", strlen(curMsg));
				List* curNode = List_curr(senderArgs->senderList);
				//printf("Number of node: %d\n", List_count(senderArgs->senderList));
				List_free(senderArgs->senderList, free);
				List_remove(senderArgs->senderList);

			}
			pthread_mutex_unlock(&bufferMutex);
		}
	}
}

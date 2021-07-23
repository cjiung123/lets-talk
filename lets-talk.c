#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"
#include "server.h"
#include "client.h"


//char buffer[BUFFER_SZ];
//pthread_mutex_t bufferMutex;
void* keyboardInputHandler(void* senderList);
void* printMsgHandler(void* receiverList);

int main(int argc, char *argv[]) {

	if(argc != 4) {
		printf("Usage: %s [my port number] [remote machine name] [remote port number]\n", argv[0]);
		exit(1);
	}
	exitFlag = false;
	List* senderList = List_create();
	List* receiverList = List_create();
	struct receiverArgsStruct receiverArgs;
	receiverArgs.myPortNumber = argv[1];
	receiverArgs.remoteMachineName = argv[2];
	receiverArgs.remotePortNumber = argv[3];
	receiverArgs.receiverList = receiverList;

	struct senderArgsStruct senderArgs;
	senderArgs.myPortNumber = argv[1];
	senderArgs.remoteMachineName = argv[2];
	senderArgs.remotePortNumber = argv[3];
	senderArgs.senderList = senderList;




	pthread_t tid[4];
	pthread_create(&tid[0], NULL, keyboardInputHandler, (void*)senderList);
	pthread_create(&tid[1], NULL, senderThread, (void*)&senderArgs);
	pthread_create(&tid[2], NULL, receiverThread, (void*)&receiverArgs);
	pthread_create(&tid[3], NULL, printMsgHandler, (void*)receiverList);

	while(1) {
		if(exitFlag == true) {
			break;
		}
		sleep(1);
	}
	pthread_cancel(tid[0]);
	for(int i=0; i<4; i++) {
		pthread_join(tid[i], NULL);
	}
	List_free(senderList, free);
	List_free(receiverList, free);
	return 0;
}

void* keyboardInputHandler(void* senderList) {
	if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return NULL;
	bzero(buffer, BUFFER_SZ);

	printf("Welcome to Lets-Talk! Please type your messages now.\n");
	while(1) {
		if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) break;
		fgets(buffer, BUFFER_SZ, stdin);
		if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) break;

		pthread_mutex_lock(&bufferMutex);
		List_add(senderList, strdup(buffer));
		bzero(buffer, BUFFER_SZ);
		pthread_mutex_unlock(&bufferMutex);
	}
}

void* printMsgHandler(void* receiverList) {
	char* curMsg;
	while(1) {
		if(exitFlag == true) {
			break;
		}

		pthread_mutex_lock(&bufferMutex);
		if(List_count(receiverList) > 0) {
			if((curMsg = List_first(receiverList)) != NULL) {
				if(strcmp(curMsg, "!status\n") == 0) {
					if(statusFlag == true) {
						printf("Online\n");
					}
					else {
						printf("Offline\n");
					}
				}
				else if(strcmp(curMsg, "!exit\n") == 0) {
					//do nothing
				}
				else {
				printf("%s", curMsg);
				}
				fflush(stdout);
				List_free(receiverList, free);
				List_remove(receiverList);
			}
		}
		pthread_mutex_unlock(&bufferMutex);
	}
}

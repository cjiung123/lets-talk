#ifndef CLIENT_H_
#define CLIENT_H_
#include "list.h"
#include <pthread.h>
struct senderArgsStruct {
	char* myPortNumber;
	char* remoteMachineName;
	char* remotePortNumber;
	List* senderList;
	pthread_mutex_t* bufferMutexPtr;
};
void* senderThread(void* args);
#endif /* CLIENT_H_ */

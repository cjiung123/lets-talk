
#ifndef SERVER_H_
#define SERVER_H_
#include "list.h"
#include <pthread.h>
struct receiverArgsStruct {
	char* myPortNumber;
	char* remoteMachineName;
	char* remotePortNumber;
	List* receiverList;
	pthread_mutex_t* bufferMutexPtr;
};
void* receiverThread(void* args);

#endif /* SERVER_H_ */

#ifndef __THREADPOOL__
#define __THREADPOOL__

#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

typedef unsigned int DocID;

typedef struct _job {
	void (*functionPointer)(DocID arg1,char *arg2);
	DocID arg1;
	char *arg2;
} jobInfo;

typedef struct _tPool{
	int size;
	pthread_t *threads;
	int activeThreads;
	queue<pthread_t> threadQueue;
	queue<jobInfo *> jobQueue;
	pthread_mutex_t csectionQueue;
	pthread_mutex_t csectionSync;
	pthread_mutex_t isFinishedm;
	pthread_cond_t isFinishedc;
	pthread_cond_t workAvailable;
	int keepAlive;
} threadPool;

typedef struct _threadSeed{
	int index;
	threadPool *tPool;
} threadSeed;

threadPool *initThreadPool(int size);
int addWork(threadPool *tPool, void (*function)(DocID arg1,char  *arg2), DocID arg1, char  *arg2);
void * doWork(void *tPoola);//, int index);
void destroyPool(threadPool *tPool);
void waitNext(threadPool *tPool,unsigned int *size);


#endif

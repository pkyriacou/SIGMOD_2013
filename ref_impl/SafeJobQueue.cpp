#include "../include/SafeJobQueue.h"
#include <stdio.h>

SafeJobQueue::SafeJobQueue(){
	pthread_mutex_init(&mutex, NULL);
}

SafeJobQueue::~SafeJobQueue(){
	pthread_mutex_unlock(&mutex);
}

void SafeJobQueue::safePush(Job * newJob){
	pthread_mutex_lock(&mutex);
		push_back(newJob);
		//printf("Push: %u\n", size());
	pthread_mutex_unlock(&mutex);
}

Job * SafeJobQueue::safePop(){
	Job * theJob;

	pthread_mutex_lock(&mutex);
		if(size() == 0 ){
			pthread_mutex_unlock(&mutex);
			return NULL;
		}
		theJob = front();
		pop_front();

	pthread_mutex_unlock(&mutex);

	return theJob;
	
}

unsigned int SafeJobQueue::safeSize(){
	unsigned int theSize = 0;

	while(pthread_mutex_trylock(&mutex) != 0);
		theSize = size();
	pthread_mutex_unlock(&mutex);

	return theSize;
}

bool SafeJobQueue::empty(){
	return safeSize() ? false : true;
}

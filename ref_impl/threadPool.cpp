#include "./threadPool.h"

#include <cassert>

threadPool *initThreadPool(int size){

	// Allocate thread pool.
	//threadPool *tPool = (threadPool *) malloc(sizeof(threadPool));
	threadPool *tPool = new threadPool();

	if(tPool == NULL){
		fprintf(stderr, "Error allocating threadpool.\n");
		exit(1);
	}

	// Allocate thread array.
	tPool->threads = (pthread_t *) malloc(sizeof(pthread_t) * size);

	tPool->size = size;

	if(tPool->threads == NULL){
		fprintf(stderr, "Error allocating thread array.\n");
		exit(1);
	}

		// Initialize mutexes.
	pthread_mutex_init(&tPool->csectionQueue, NULL);
	pthread_mutex_init(&tPool->csectionSync, NULL);
	pthread_mutex_init(&tPool->isFinishedm, NULL);
	pthread_cond_init(&tPool->isFinishedc, NULL);
	pthread_cond_init(&tPool->workAvailable, NULL);

	tPool->activeThreads = size;
	tPool->keepAlive = 1;

	// Create threads.
	int index = 0;
	for(index = 0; index < size; index++){
		pthread_create(&(tPool->threads[index]), NULL,doWork,(tPool));
	}
	
	
//	printf("ahahah size = %d\n", tPool->jobQueue.size());
	// Return threadpool.
	return tPool;
}

int addWork(threadPool *tPool, void (*function)(DocID arg1, char  *arg2), DocID arg1, char  *arg2){

	// Lock queue mutex.
	pthread_mutex_lock(&tPool->csectionQueue);

	// Create new job.
	jobInfo *newJob = (jobInfo *) malloc(sizeof(jobInfo));

	//printf("size = %u\n", tPool->jobQueue.size());
	newJob->functionPointer=function;
	newJob->arg1=arg1;
	newJob->arg2=arg2;
	//int x=*(int *)newJob->args;

	// Add job into job queue.
	tPool->jobQueue.push(newJob);
	//x= *(int *)tPool->jobQueue.front()->args;
//	printf("size = %u\n", tPool->jobQueue.size());
	//printf("Beeeee: %d\n",x);
	// Notify next thread that a job is ready.

pthread_cond_signal(&tPool->workAvailable);
	// Unlock queue mutex.
	pthread_mutex_unlock(&tPool->csectionQueue);
	

}

		
void * doWork(void *tPoola){//, int index){
	threadPool * tPool=(threadPool *)tPoola;

	while(1){
			
				//fprintf(stderr,"{");
			//fflush(stderr);
			pthread_mutex_lock(&(tPool->csectionQueue));
				//fprintf(stderr,"}\n");
			//fflush(stderr);
			tPool->threadQueue.push(pthread_self());
		
			//--(tPool->activeThreads);
			
			if(tPool->jobQueue.empty() && tPool->threadQueue.size()==tPool->size){
				//fprintf(stderr,"QUEUE: %lu   INACTIVE: %lu\n",tPool->jobQueue.size(),tPool->threadQueue.size());
				pthread_cond_signal(&tPool->isFinishedc);
				
			}
			//else {
				//fflush(stderr);
				//fprintf(stderr,"QUEUE: %lu   INACTIVE: %lu\n",tPool->jobQueue.size(),tPool->threadQueue.size());
				
			//}
				
				// Unlock queue mutex.
			
			pthread_mutex_unlock(&(tPool->csectionQueue));
		
			
			pthread_mutex_lock(&(tPool->csectionSync));
			
			while(tPool->jobQueue.empty())
			{
				pthread_cond_wait(&(tPool->workAvailable), &(tPool->csectionSync));
			}
			
			
			// Unlock critical section.
			pthread_mutex_unlock(&(tPool->csectionSync));

			//fprintf(stdout, "Preparing to lock queue so we can ....\n");
			// Lock queue mutex.
			pthread_mutex_lock(&tPool->csectionQueue);
			tPool->threadQueue.pop();
			if(tPool->jobQueue.size()){
				assert(tPool->jobQueue.size());
				jobInfo *job = tPool->jobQueue.front();
				tPool->jobQueue.pop();
	
			

				// Unlock queue mutex.
				pthread_mutex_unlock(&tPool->csectionQueue);
	
					job->functionPointer(job->arg1,job->arg2);

					//pthread_cond_signal(&tPool->isFinishedc);

				//printf("afhihihihnighni %d\n", *(int *)job->args);
				free(job->arg2);
				free(job);
			}
			else
				pthread_mutex_unlock(&tPool->csectionQueue);

			

			

		



	}

}

void destroyPool(threadPool *tPool){

	//tPool->keepAlive = 0;

	//while(!tPool->jobQueue.empty() && tPool->threadQueue.size()!=tPool->size)fprintf(stderr,"SIZE:%lu  INACTIVE:%lu\n",tPool->jobQueue.size(),tPool->threadQueue.size());
	pthread_mutex_lock(&tPool->isFinishedm);
	while(!tPool->jobQueue.empty() && tPool->threadQueue.size()!=tPool->size){
		pthread_cond_wait(&(tPool->isFinishedc), &(tPool->isFinishedm));
		//fprintf(stderr,"SIZE:%lu  INACTIVE:%lu\n",tPool->jobQueue.size(),tPool->threadQueue.size());
	}
	pthread_mutex_unlock(&tPool->isFinishedm);
	/// FREEEEEEEEEEE EVERYTHING
}

void waitNext(threadPool *tPool,unsigned int *size){

	pthread_mutex_lock(&tPool->isFinishedm);

	while(*size==0)
		pthread_cond_wait(&(tPool->isFinishedc), &(tPool->isFinishedm));

	pthread_mutex_unlock(&tPool->isFinishedm);
	

}





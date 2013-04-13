#include "../include/ThreadPool.h"

ThreadPool::ThreadPool(unsigned size): size(size), keepAlive(true) {

	// Set current active threads to size. 
	// This will get updated as soon as the threads start working.
	activeThreads = size;
	deadThreads = 0;

	waitingJobs[START] = 0;
	waitingJobs[END] = 0;
	waitingJobs[MATCH] = 0;
	waitingJobs[GETNEXT] = 0;

	// Initialize mutexes.
	pthread_mutex_init(&waitMutex, NULL);
	pthread_mutex_init(&exitMutex, NULL);
	pthread_mutex_init(&jobWaitMutex[START], NULL);
	pthread_mutex_init(&jobWaitMutex[END], NULL);
	pthread_mutex_init(&jobWaitMutex[MATCH], NULL);
	pthread_mutex_init(&jobWaitMutex[GETNEXT], NULL);

	// Initialize conditions.
	pthread_cond_init(&workAvailable, NULL);
	pthread_cond_init(&exited, NULL);
	pthread_cond_init(&finishedJob, NULL);
	pthread_cond_init(&jobDone[START], NULL);
	pthread_cond_init(&jobDone[END], NULL);
	pthread_cond_init(&jobDone[MATCH], NULL);
	pthread_cond_init(&jobDone[GETNEXT], NULL);

	// Allocate thread array.
	threads = (pthread_t *) malloc(sizeof(pthread_t) * size);

	if(threads == NULL){
		fprintf(stderr, "Error allocating thread array.\n");
		exit(1);
	}

	// Create threads.
	for(unsigned int index = 0; index < size; index++){
		pthread_create(&threads[index], NULL, &doWork, this);
	}



}

void ThreadPool::addJob(Job * newJob){

	// Increment job counter for the specific job type.
	pthread_mutex_lock(&jobWaitMutex[newJob->getJobType()]);
		++waitingJobs[newJob->getJobType()];
	pthread_mutex_unlock(&jobWaitMutex[newJob->getJobType()]);


	// Push new job into the back of the queue.
	jobQueue.safePush(newJob);


	// Signal the threads that there's work available.
	pthread_cond_signal(&workAvailable);
		
}


void * doWork(void *arg){
	ThreadPool * tPool = (ThreadPool *) arg;
	Job * newJob = NULL;
	
	while(tPool->keepAlive || tPool->jobQueue.safeSize()){
		
		// Count the amount of active threads.
		pthread_mutex_lock(&tPool->waitMutex);
		{
			--tPool->activeThreads;

			// Keep waiting until the threadpool has work available.
			while(tPool->keepAlive && tPool->jobQueue.empty())
				pthread_cond_wait(&tPool->workAvailable, &tPool->waitMutex);

			++tPool->activeThreads;
		}
		pthread_mutex_unlock(&tPool->waitMutex);

		// If we're supposed to be shutting down and the job queue is empty, break from the loop.
		if(tPool->keepAlive == false && tPool->jobQueue.empty()){
			break;
		}

		// Get a job from the job queue.
		newJob = tPool->jobQueue.safePop();
		
		// Whoops, the queue was really empty?!
		if(newJob == NULL)
			continue;		

		// Execute the job.
		newJob->execute();

		// De-increment waiting jobs counter for the specific job type.
		pthread_mutex_lock(&tPool->jobWaitMutex[newJob->getJobType()]);
			--tPool->waitingJobs[newJob->getJobType()];
		pthread_mutex_unlock(&tPool->jobWaitMutex[newJob->getJobType()]);


		// Signal barrierAll/barrierNext 
		pthread_cond_signal( &tPool->jobDone[newJob->getJobType()] );
		pthread_cond_signal( &tPool->finishedJob );

		delete newJob;
	
	}

	// Might be unnecessary
	pthread_mutex_lock(&tPool->exitMutex);
	{
		++tPool->deadThreads;
	}
	pthread_mutex_unlock(&tPool->exitMutex);

	// Might be unnecessary
	pthread_cond_signal(&tPool->exited);

	return NULL;
}


void ThreadPool::barrierNext(JobType type){
	pthread_mutex_lock(&jobWaitMutex[type]);

	unsigned int temp = waitingJobs[type];
	
	while(waitingJobs[type] && temp == waitingJobs[type])
		pthread_cond_wait(&jobDone[type], &jobWaitMutex[type]);

	pthread_mutex_unlock(&jobWaitMutex[type]);
}

void ThreadPool::barrierAll(JobType type){
	pthread_mutex_lock(&jobWaitMutex[type]);
	
		while(waitingJobs[type] > 0)
			pthread_cond_wait(&jobDone[type], &jobWaitMutex[type]);

	pthread_mutex_unlock(&jobWaitMutex[type]);
}

void ThreadPool::destroy(){
	keepAlive = false;
	
	// If the job queue is empty, tell all threads that there's work available,
	// so they are tricked out of the wait loop and finish execution.
	if(jobQueue.empty())
		pthread_cond_broadcast(&workAvailable);
	else{
		// Wait until all threads have finished executing.
		pthread_mutex_lock(&exitMutex);
			while(jobQueue.safeSize())
				pthread_cond_wait(&finishedJob, &exitMutex);
		pthread_mutex_unlock(&exitMutex);
	}	

	// Join until all threads have finished. 
	for(unsigned int i = 0; i < size; ++i)
		pthread_join(threads[i], NULL);

	// Check if all threads have died.
	if(deadThreads == size)
		printf("All threads have died!\n");
	else
		printf("Whoops! Not all threads have died! %u dead threads out of %u threads.\n", deadThreads, size);

	// Delete thread array.
	free(threads);
}

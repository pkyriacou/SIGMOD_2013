#ifndef __THREADPOOL__
#define __THREADPOOL__

#include <stdio.h>
#include <pthread.h>
#include <queue>
#include "./core.h"
#include "./Job.h"
#include "./SafeJobQueue.h"
#include <stdlib.h>

using namespace std;

class ThreadPool{

public:
	// Counters.
	unsigned int size;
	unsigned int deadThreads;
	unsigned int activeThreads;
	unsigned long long waitingJobs[4];

	// Thread array.
	pthread_t * threads;

	// Conditions.
	pthread_cond_t workAvailable;
	pthread_cond_t jobDone[4];
	pthread_cond_t finishedJob;
	pthread_cond_t exited;

	// Mutexes.
	pthread_mutex_t waitMutex;
	pthread_mutex_t exitMutex;
	pthread_mutex_t jobWaitMutex[4];
	pthread_mutex_t jobDoneMutex[4];

	// Job queue.
	SafeJobQueue jobQueue;

	// Threadpool on/off switch.
	bool keepAlive;


	/******************** Functions *********************/

	// Constructor. Creates a threadpool with $size threads.
	ThreadPool(unsigned int size);

	// Adds a job into the job queue.
	void addJob(Job * newJob);

	// Stalls until AT LEAST ONE job of the given type is processed.
	void barrierNext(JobType type);

	// Stalls until ALL jobs of the given type are processed.
	void barrierAll(JobType type);

	// Destroys the threadpool.
	void destroy();
};

// Thread wrapper function.
void * doWork(void *arg);

#endif

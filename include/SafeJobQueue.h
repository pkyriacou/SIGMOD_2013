#ifndef __SAFEJOBQUEUE__
#define __SAFEJOBQUEUE__

#include <queue>
#include <deque>
#include <pthread.h>
#include "./Job.h"

using namespace std;

class SafeJobQueue : public deque<Job *> {
	pthread_mutex_t mutex;

	public:
	SafeJobQueue();
	~SafeJobQueue();

	void safePush(Job *);
	Job * safePop();
	unsigned int safeSize();
	bool empty();
};

#endif

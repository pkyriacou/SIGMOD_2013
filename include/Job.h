#ifndef __JOB__
#define __JOB__

#include "./core.h"

typedef enum { START=0, END, MATCH, GETNEXT } JobType;

class Job {
	JobType type;	

	public:
		Job();
		~Job();
		Job(JobType type): type(type) {};

		JobType getJobType();

		virtual void execute() {};
};


class MatchJob : public Job{

	public:

	void (*funcPtr) (DocID doc_id, char *doc_str);
	DocID doc_id;
	char * doc_str;

	MatchJob(	void (*funcPtr) (DocID doc_id, char *doc_str), 
			DocID doc_id, 
			char * doc_str) : 	Job(MATCH),
						funcPtr(funcPtr), 
						doc_id(doc_id), 
						doc_str(doc_str)
						{};

	virtual void execute();
};

class GetNextJob : public Job{

	public:

	void (*funcPtr) (DocID * p_doc_id, unsigned int * p_num_res, QueryID ** p_query_ids);
	DocID* p_doc_id;
	unsigned int * p_num_res;
	QueryID ** p_query_ids;

	GetNextJob(	void (*funcPtr) (DocID * p_doc_id, unsigned int * p_num_res, QueryID ** p_query_ids), 
			DocID * p_doc_id, 
			unsigned int * p_num_res, 
			QueryID ** p_query_ids) :	Job(GETNEXT),
							funcPtr(funcPtr), 
							p_doc_id(p_doc_id), 
							p_num_res(p_num_res),
							p_query_ids(p_query_ids)
							{};

	virtual void execute();
};

class StartQueryJob : public Job{

	public:

	void (*funcPtr) (QueryID query_id, char* query_str, MatchType match_type, unsigned int match_dist);
	QueryID query_id;
	char* query_str;
	MatchType match_type;
	unsigned int match_dist;

	StartQueryJob(	void (*funcPtr) (QueryID query_id, char* query_str, MatchType match_type, unsigned int match_dist),
			QueryID query_id,
			char* query_str,
			MatchType match_type,
			unsigned int match_dist) :	Job(START),
							funcPtr(funcPtr),
							query_id(query_id),
							match_type(match_type),
							match_dist(match_dist)
							{};

	virtual void execute();
};

class EndQueryJob : public Job{
	void (*funcPtr) (QueryID query_id);
	QueryID query_id;

	public:

	EndQueryJob(	void (*funcPtr) (QueryID query_id),
			QueryID query_id) :	Job(END),
						funcPtr(funcPtr),
						query_id(query_id)
						{};

	virtual void execute();
};


#endif

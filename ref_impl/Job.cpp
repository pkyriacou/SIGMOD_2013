#include "../include/Job.h"

#include <stdio.h>

Job::Job(){}
Job::~Job(){}

JobType Job::getJobType(){
	return type;
}

void MatchJob::execute(){
	funcPtr(doc_id, doc_str);
}

void GetNextJob::execute(){
	funcPtr(p_doc_id, p_num_res, p_query_ids);
}

void StartQueryJob::execute(){
	funcPtr(query_id, query_str, match_type, match_dist);
}

void EndQueryJob::execute(){
	funcPtr(query_id);
}

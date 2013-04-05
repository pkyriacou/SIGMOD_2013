/*
 * core.cpp version 1.0
 * Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
 * Author: Amin Allam
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "../include/core.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string.h>
#include <unordered_map>
#include <iostream>
#include "threadPool.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

// Computes edit distance between a null-terminated string "a" with length "na"
//  and a null-terminated string "b" with length "nb" 
int EditDistance(const char* a, int na,const char* b, int nb)
{
	int oo=0x7FFFFFFF;

	 int T[2][MAX_WORD_LENGTH+1];

	int ia, ib;

	int cur=0;
	ia=0;

	for(ib=0;ib<=nb;ib++)
		T[cur][ib]=ib;

	cur=1-cur;

	for(ia=1;ia<=na;ia++)
	{
		for(ib=0;ib<=nb;ib++)
			T[cur][ib]=oo;

		int ib_st=0;
		int ib_en=nb;

		if(ib_st==0)
		{
			ib=0;
			T[cur][ib]=ia;
			ib_st++;
		}

		for(ib=ib_st;ib<=ib_en;ib++)
		{
			int ret=oo;

			int d1=T[1-cur][ib]+1;
			int d2=T[cur][ib-1]+1;
			int d3=T[1-cur][ib-1]; if(a[ia-1]!=b[ib-1]) d3++;

			if(d1<ret) ret=d1;
			if(d2<ret) ret=d2;
			if(d3<ret) ret=d3;

			T[cur][ib]=ret;
		}

		cur=1-cur;
	}

	int ret=T[1-cur][nb];

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Computes Hamming distance between a null-terminated string "a" with length "na"
//  and a null-terminated string "b" with length "nb" 
unsigned int HammingDistance(const char* a, int na,const char* b, int nb)
{
	int j, oo=0x7FFFFFFF;
	if(na!=nb) return oo;
	
	unsigned int num_mismatches=0;
	for(j=0;j<na;j++) if(a[j]!=b[j]) num_mismatches++;
	
	return num_mismatches;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all information related to an active query
struct Query
{
	QueryID query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int match_dist;
	bool valid;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all query ID results associated with a dcoument
struct Document
{
	DocID doc_id;
	unsigned int num_res;
	QueryID* query_ids;


};

struct Query_st{
	Query query;
	vector <string>tokens;

};
threadPool *tPool;
pthread_mutex_t lock;
///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all currently active queries
vector<Query> queries;

//Hash of queries
vector<Query_st> query_hash;

// Keeps all currently available results that has not been returned yet
vector<Document> docs;

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode InitializeIndex(){
	tPool = initThreadPool(1);
	pthread_mutex_init(&lock, NULL);
	return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode DestroyIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{
	Query query;
	query.query_id=query_id;
	//printf("%u\n",query_id);
	strcpy(query.str, query_str);

	query.match_type=match_type;
	query.match_dist=match_dist;
	query.valid=true;
	// Add this query to the active query set
	queries.push_back(query);
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode EndQuery(QueryID query_id)
{
	//destroyPool(tPool);
	// Remove this query from the active query set
	unsigned int i, n=queries.size();
	//vector<Query>::const_iterator q;
	//q=queries.begin();
	for(i=0;i<n;i++)
	{

		if(queries[i].query_id==query_id)
		{
			//printf("%u \n",queries.size());
			//queries.erase(queries.begin()+i);
			queries[i].valid=false;
			query_hash[i].query.valid=false;
			//printf("2  %u \n",queries.size());
			break;
		}
	}
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
ErrorCode MatchDocument(DocID doc_id, const char* doc_str){
	//static int times=0;
	static int n=0;
	char * ph;
	char doc_str1[MAX_DOC_LENGTH];
	strcpy(doc_str1, doc_str);

	char *icy = (char*)malloc(sizeof(char) * MAX_DOC_LENGTH);
	strcpy(icy, doc_str);
	/////////


	int n1=queries.size();

	if(n!=n1){
		//printf("times\n");
		//n++;
		//printf("N:%d N1:%d\n",n,n1);
		for(int i=n;i<n1;i++){
			Query_st x;
			x.query=queries[i];
			//printf("ID:%u \n",queries[i].query_id);
			ph=strtok(queries[i].str," ");
			while(ph!=NULL){
				//printf("%s\n",ph);
				//char *temp=(char *)malloc(sizeof(char)*MAX_DOC_LENGTH);
				//strcpy(temp,ph);
				//char temp[MAX_DOC_LENGTH];
				//strcpy(temp,ph);
				if(ph){


					x.tokens.push_back(ph);

				}
				ph=strtok(NULL," ");
			}
			query_hash.push_back(x);
			n=n1;
		}
	}
	/////////

	addWork(tPool, MatchDocument2,doc_id, icy);

	return EC_SUCCESS;
}
void MatchDocument2(DocID doc_id,char* doc_str)
{

	char cur_doc_str [MAX_DOC_LENGTH];
	char *tok;
	strcpy(cur_doc_str, doc_str);
	unsigned int matched=0;
	unordered_map<string,DocID>doc_hash;

	unordered_map<string,DocID>::const_iterator got;

	char *ph=strtok_r(cur_doc_str," ", &tok);

	while(ph!=NULL){

		ph=strtok_r(NULL," ", &tok);

		if(ph)
			doc_hash.insert({ph,doc_id});
	}

	vector<unsigned int> query_ids;

	for(unsigned int j=0;j<query_hash.size();++j){

		if(query_hash[j].query.match_type==MT_EXACT_MATCH)
		{


			for(unsigned int i=0;i<query_hash[j].tokens.size();i++){

				got=doc_hash.find(query_hash[j].tokens[i]);

				if(got!=doc_hash.end()){
						++matched;

				}else{
						--matched;

				}

			}

			if(matched==query_hash[j].tokens.size()&& query_hash[j].query.valid){
				query_ids.push_back(query_hash[j].query.query_id);
				matched=0;

			}else{

				matched=0;
			}


		}
		else if(query_hash[j].query.match_type==MT_HAMMING_DIST)
		{

			for(unsigned int i=0;i<query_hash[j].tokens.size();i++){

				for(unordered_map<string,DocID>::const_iterator d=doc_hash.begin();d!=doc_hash.end();++d){

					unsigned int num_mismatches=HammingDistance(query_hash[j].tokens[i].c_str(), query_hash[j].tokens[i].size(), d->first.c_str(), d->first.size());

					if(num_mismatches<=query_hash[j].query.match_dist){matched++;/*printf("Matched :%u %u %s %s\n",q->query.query_id,d->second,q->tokens[i].c_str(),d->first.c_str());*/break;}//query_ids.push_back(q->second.query_id);

				}

			}
				if(matched>=query_hash[j].tokens.size()&& query_hash[j].query.valid){
									query_ids.push_back(query_hash[j].query.query_id);
									matched=0;

							}else{

									matched=0;
								}


			}

		else if(query_hash[j].query.match_type==MT_EDIT_DIST)
		{

			for(unsigned int i=0;i<query_hash[j].tokens.size();i++){

				for(unordered_map<string,DocID>::const_iterator d=doc_hash.begin();d!=doc_hash.end();++d){

					unsigned int edit_dist=EditDistance(query_hash[j].tokens[i].c_str(), query_hash[j].tokens[i].size(), d->first.c_str(), d->first.size());

					if(edit_dist<=query_hash[j].query.match_dist) {matched++;/*printf("Matched :%u %u %s %s\n",q->query.query_id,d->second,q->tokens[i].c_str(),d->first.c_str());*/break;}//query_ids.push_back(q->second.query_id);

				}

			}
			if(matched>=query_hash[j].tokens.size()&& query_hash[j].query.valid){
					query_ids.push_back(query_hash[j].query.query_id);
					matched=0;

			}else{

					matched=0;
				}
		}
	}


	Document doc;
	doc.doc_id=doc_id;
	doc.num_res=query_ids.size();
	doc.query_ids=0;
	if(doc.num_res) doc.query_ids=(unsigned int*)malloc(doc.num_res*sizeof(unsigned int));
	for(unsigned i=0;i<doc.num_res;i++){ doc.query_ids[i]=query_ids[i]; /*printf("%u ", query_ids[i]);*/}
	docs.push_back(doc);

}

///////////////////////////////////////////////////////////////////////////////////////////////
//bool comp(const Document &d1,const Document &d2){return d1.doc_id<d2.doc_id;}
ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids)
{
	// Get the first undeliverd resuilt from "docs" and return it
	//destroyPool(tPool);//this is wait for now
	unsigned int size=docs.size();
	destroyPool(tPool);
	*p_doc_id=0; *p_num_res=0; *p_query_ids=0;
	if(docs.size()==0) return EC_NO_AVAIL_RES;
	*p_doc_id=docs[0].doc_id; *p_num_res=docs[0].num_res; *p_query_ids=docs[0].query_ids;
	docs.erase(docs.begin());
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

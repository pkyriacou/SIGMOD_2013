

#include "hashtable.h"
#include <string.h>
#define STRLEN 500

hnode *hashInit(int size){
	hnode *ht;
	int i;
	ht=(hnode *)malloc(sizeof(hnode)*size);
	if(!ht)
		perror("malloc");
	for(i=0;i<size;i++){
		ht[i].str=(char *)malloc(sizeof(char)*STRLEN);
		if(!ht[i].str)
			perror("malloc");
		ht[i].valid=-1;
		ht[i].next=NULL;
	}
	return ht;
	
}

int hashInsert(hnode *ht,char *str,int size){
	hnode *cur = &ht[hashFunction(str,size)];
	if(cur->valid==-1)
		{
			strcpy(cur->str,str);
			cur->valid=1;
			return 1;
		}
	while(cur->next)
		cur=cur->next;
	cur->next=(hnode *)malloc(sizeof(hnode));
	if(!cur->next)
		perror("malloc");
	cur->next->next=NULL;
	cur->next->valid=1;
	cur->next->str=(char *)malloc(sizeof(char)*strlen(str));
	if(!cur->next->str)
		perror("malloc");
	strcpy(cur->next->str,str);
	return 1;
}

int isInside(hnode *ht,char *str,int size){
	hnode *cur=&ht[hashFunction(str,size)];
	while(cur){
		if(strcmp(cur->str,str)==0)
			return 1;
		cur=cur->next;
	}
	return 0;
}

int hashFunction(char *str,int size){
	int sum=0;
	int i=0;
	while(str[i]!='\0')
		sum+=str[i++];
	return sum%size;
}

int freeHash(hnode *ht,int size){
	int i;
	hnode *cur;
	hnode *temp;
	for(i=0;i<size;i++){
		cur=&ht[i];
		cur=cur->next;
		free(ht[i].str);
		while(cur){
			temp=cur;
			cur=cur->next;
			free(temp);
		}
	}
	free(ht);
}

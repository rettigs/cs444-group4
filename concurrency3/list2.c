#define NUM_SEARCHERS 4
#define NUM_INSERTERS 2
#define NUM_DELETERS 2

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "lib.h"

void * searcher(void * list);
void * inserter(void * list);
void * deleter(void * list);

sem_t read_sem, write_sem;

int main()
{
	int rc;
	long t;
	node_t * list = new_list();
	list->data = 4;
	pthread_t searchers[NUM_SEARCHERS];
	pthread_t inserters[NUM_INSERTERS];
	pthread_t deleters[NUM_DELETERS];

	sem_init(&read_sem, 1, NUM_SEARCHERS);
	sem_init(&write_sem, 0, 1);

	for (t = 0; t < NUM_INSERTERS; t++) {
		rc = pthread_create(&inserters[t], NULL, inserter, (void *)list);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_SEARCHERS; t++){
		rc = pthread_create(&searchers[t], NULL, searcher, (void *)list);
		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_DELETERS; t++){
		rc = pthread_create(&deleters[t], NULL, deleter, (void *)list);
		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for (t = 0; t < NUM_INSERTERS; t++) {
		pthread_join(inserters[t], NULL);
	}

	for (t = 0; t < NUM_SEARCHERS; t++) {
		pthread_join(searchers[t], NULL);
	}

	for (t = 0; t < NUM_DELETERS; t++) {
		pthread_join(deleters[t], NULL);
	}	

	int value;
	sem_getvalue(&read_sem, &value);
	printf("read_sem: %d/%d\n", value, NUM_SEARCHERS);
	sem_getvalue(&write_sem, &value);
	printf("write_sem: %d/%d\n", value, 1);

	sem_destroy(&read_sem);
	sem_destroy(&write_sem);

	free_list(&list);
	exit(0);
}

void * searcher(void * list)
{
	node_t * nlist = (node_t *) list;

	sem_wait(&read_sem);
	printf("read_sem LOCKED\n");

	printf("searching...\n");
	print_list(nlist);

	sem_post(&read_sem);
	printf("read_sem UNLOCKED\n");

	return NULL;
}

void * inserter(void * list)
{
	node_t * nlist = (node_t *) list;

	sem_wait(&write_sem);
	printf("write_sem LOCKED\n");

	printf("inserting...\n");
	print_list(nlist);

	sem_post(&write_sem);
	printf("write_sem UNLOCKED\n");

	return NULL;
}

void * deleter(void * list)
{
	long t;

	sem_wait(&write_sem);
	printf("write_sem LOCKED\n");
	for(t = 0; t < NUM_SEARCHERS; t++){
		sem_wait(&read_sem);
		printf("read_sem LOCKED\n");
	}

	printf("deleting...");

	for(t = 0; t < NUM_SEARCHERS; t++){
		sem_post(&read_sem);
		printf("read_sem UNLOCKED\n");
	}
	sem_post(&write_sem);
	printf("write_sem UNLOCKED\n");

	return NULL;
}
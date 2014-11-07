#define NUM_SEARCHERS 4
#define NUM_INSERTERS 4
#define NUM_DELETERS 2

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>		// required for srand use only
#include <pthread.h>
#include <semaphore.h>
#include "lib.h"

void * searcher();
void * inserter();
void * deleter();

node_t * list = NULL;
sem_t read_sem, write_sem;

int main()
{
	int rc;
	long t;
	pthread_t searchers[NUM_SEARCHERS];
	pthread_t inserters[NUM_INSERTERS];
	pthread_t deleters[NUM_DELETERS];

	srand(time(NULL));
	sem_init(&read_sem, 1, NUM_SEARCHERS);
	sem_init(&write_sem, 0, 1);

	for (t = 0; t < NUM_INSERTERS; t++) {
		rc = pthread_create(&inserters[t], NULL, inserter, NULL);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_SEARCHERS; t++){
		rc = pthread_create(&searchers[t], NULL, searcher, NULL);
		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_DELETERS; t++){
		rc = pthread_create(&deleters[t], NULL, deleter, NULL);
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

	sem_destroy(&read_sem);
	sem_destroy(&write_sem);

	free_list(&list);
	exit(0);
}

void * searcher()
{
	sem_wait(&read_sem);

	int val = rand()%100;
	printf("search for %d, result: %d\n", val, search_by_value(list, val));
	print_list(list);

	sem_post(&read_sem);

	return NULL;
}

void * inserter()
{
	int val;
	long t;

	sem_wait(&write_sem);

	for (t = 0; t < 5; t++) {
		val = rand()%100;
		printf("inserting %d\n", val);
		list = insert_node(list, val);
		print_list(list);
	}

	sem_post(&write_sem);

	return NULL;
}

void * deleter()
{
	int val = rand()%100;
	long t;

	sem_wait(&write_sem);
	for(t = 0; t < NUM_SEARCHERS; t++){
		sem_wait(&read_sem);
	}

	printf("removing %d\n", val);
	list = remove_node(list, val);
	print_list(list);

	for(t = 0; t < NUM_SEARCHERS; t++){
		sem_post(&read_sem);
	}
	sem_post(&write_sem);

	return NULL;
}
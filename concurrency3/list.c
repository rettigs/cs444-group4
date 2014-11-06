#define NUM_SEARCHERS 2
#define NUM_INSERTERS 2
#define NUM_DELETERS 2

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


void *search(void *list);
void *insert(void *list);
void *delete(void *list);

sem_t noSearchers, noInserters, insertlock, searchlock, insertMutex;

struct node {
	int			data;
	struct node	*next;
};

int numsearchers = 0;
int numinserters = 0;


int main()
{
	struct node *root;
	root = (struct node *) malloc(sizeof(struct node));
	root->next = NULL;
	root->data = 1;
	
	//Semaphore Initialization
	sem_init(&noSearchers, 0, 1);
	sem_init(&noInserters, 0, 1);
	sem_init(&insertlock, 0, 1);
	sem_init(&searchlock, 0, 1);
	sem_init(&insertMutex, 0, 1);
	

	pthread_t searchers[NUM_SEARCHERS];
	pthread_t inserters[NUM_INSERTERS];
	//pthread_t deleters[NUM_DELETERS];

	int rc;
	long t;
	for (t = 0; t < NUM_SEARCHERS; t++) {
		rc = pthread_create(&searchers[t], NULL, search, (void *)root);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_INSERTERS; t++){
		rc = pthread_create(&inserters[t], NULL, insert, (void *)root);
		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	pthread_exit(NULL);
}

void *search(void *list)
{
	struct node *root = (struct node *) list;
	struct node *conductor;

	while(1){    
		sem_wait(&searchlock);
		numsearchers++;
		printf("added searcher! current # of searchers  %d\n", numsearchers);
		if(numsearchers == 1) sem_wait(&noSearchers);
		sem_post(&searchlock);
		
		conductor = root;
		while ( conductor != NULL ) {
	    		printf( "Searcher %u found %d\n", pthread_self(), conductor->data);
	    		conductor = conductor->next;
		}


		sem_wait(&searchlock);
		numsearchers--;
		printf("search complete! current # of searchers %d\n", numsearchers);
		if(numsearchers == 0) sem_post(&noSearchers);
		sem_post(&searchlock);
	}
}

void *insert(void *list)
{

	
	struct node *root = (struct node *)list;
	struct node *conductor;

	while(1){
		sem_wait(&insertlock);
		numinserters++;
		printf("Number of Inserters: %d\n", numinserters);
		if(numinserters == 1); sem_wait(&noInserters);
		sem_post(&insertlock);
		sem_wait(&insertMutex);
		//iterate through linked list until we find the last element
		conductor = root;
		while(conductor->next != NULL){
			conductor = conductor->next;
		}
		//create new element
		struct node *newelement;
		newelement =  malloc(sizeof(struct node));
		newelement->data = conductor->data + 1;
		newelement->next = NULL;
	
		//add it to the list
		conductor->next = newelement;
		printf("Thread %u inserting %d into list\n", pthread_self(), newelement->data);
		sem_post(&insertMutex);
		sem_wait(&insertlock);
		numinserters--;
		if(numinserters == 0) sem_post(&noInserters);
		sem_post(&insertlock);
	}
}

void *delete(void *list)
{
//delete code (minus mutexing)

	struct node *root = (struct node *)list;
	struct node *conductor;
	struct node *conductortrail;
	
	//find last element in the linked list (I think we probably should just make a functio for this)
	conductor = root;
	if(root->next == NULL){
		free(root);
		root = NULL;
	}
	else{
		conductortrail = root;
		conductor = root->next;
		while(conductor->next != NULL){
			conductortrail = conductor;
			conductor = conductor->next;
		}
		free(conductor);
		conductortrail->next = NULL;
	}
	//remove the element 
	return 0;
}

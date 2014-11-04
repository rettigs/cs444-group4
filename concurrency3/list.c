#define NUM_SEARCHERS 2
#define NUM_INSERTERS 2
#define NUM_DELETERS 2

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *search(void *list);
void *insert(void *list);
void *delete(void *list);

struct node {
	int			data;
	struct node	*next;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	struct node *root;
	root = (struct node *) malloc(sizeof(struct node));
	root->next = 0;
	root->data = 5;

	pthread_t searchers[NUM_SEARCHERS];
	//pthread_t inserters[NUM_INSERTERS];
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

	pthread_exit(NULL);
}

void *search(void *list)
{
	struct node *root = (struct node *) list;
	struct node *conductor;

	conductor = root;
	while ( conductor != NULL ) {
	    printf( "Searcher %u found %d\n", pthread_self(), conductor->data);
	    conductor = conductor->next;
	}

	return 0;
}

void *insert(void *list)
{
//insert code (minus mutexing)

	struct node *root = (struct node *)list;
	struct node *conductor;


	//iterate through linked list until we find the last element
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
	return 0;
}

void *delete(void *list)
{
//delete code (minus mutexing)
	return 0;
}

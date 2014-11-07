#define NUM_SEARCHERS 4
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


sem_t search_sem, insert_sem;

struct node {
	int			data;
	struct node	*next;
};

struct node *root;	/* start of link list */

	

/*
 * Insert info to pass to a thread
 * So in can insert values between start and end.
 * 
 */
struct insert_info
{
	int start;
	int end;
} insert_info[NUM_INSERTERS];

struct delete_info
{
	int start;
	int end;
} delete_info[NUM_DELETERS];

/*
 * Insert value into link list
 * NOTE: this takes a pointer to the root pointer since we
 * 	that will change on first insert.
 */
void
insert_func(struct node **list, int value)
{
	struct node *p;
	struct node *new;

	new = malloc(sizeof(struct node));
	new->next = 0;
	new->data = value;
	
	sem_wait(&insert_sem);

	/*
	 * empty list - insert at beginning
	 */
	if (*list == 0) {
		*list = new;
	} else {
		/*
		 * find end of list to insert
		 */
		for (p = *list; p->next; p = p->next) {
			;	
		}
		/* p points to last entry (with null next) */
		p->next = new;
	}
	sem_post(&insert_sem);
}

/*
 * Delete value from list.
 * returns 1 if found and deleted.
 * returns 0 if value not found in list. 
 */

int
delete_func(struct node **list, int value)
{
	int ret = 0;
	int i;
	struct node *p;
	struct node *prev;

	sem_wait(&insert_sem);	// prevent inserters
	printf("waiting on search semaphor\n");
	for (i = 0; i < NUM_SEARCHERS; i++) {	// prevent all searchers
		sem_wait(&search_sem);
	}

	prev = p = *list;
	while (p) {
		if (p->data == value) {
			/* found value to delete */
			if (prev == *list) {
				/* front of list */
				*list = p->next;
			} else {
				/* not front */
				prev->next = p->next;
			}
			ret = 1;	/* found and deleted */
			free(p);
			break;
		}
		prev = p;
		p = p->next;
	}
		

	for (i = 0; i < NUM_SEARCHERS; i++) {	// allow all searchers
		sem_post(&search_sem);
	}

	sem_post(&insert_sem);	// allow inserters

	return ret;
}
	

/*
 * Returns 1 if value is found in nlist
 * returns 0 if not found.
 */
int
search_func(struct node *list, int value)
{
	int ret = 0;
	sem_wait(&search_sem);
	while (list) {
		if (list->data == value)  {
			ret = 1;
			break;	// still must unlock
		}
		list = list->next;
	}
	sem_post(&search_sem);
	return ret;
}


int main()
{
#if 0
	root = (struct node *) malloc(sizeof(struct node));
	root->next = NULL;
	root->data = 1;
	int val;
#endif	
	//Semaphore Initialization
	sem_init(&search_sem, 0, NUM_SEARCHERS);
	sem_init(&insert_sem, 0, 1);

#if 0
	insert_func(&root, 1);
	insert_func(&root, 2);
	printf("added 1 and 2\n");
	printf("search(1) = %d\n", search_func(root, 1));
	printf("search(2) = %d\n", search_func(root, 2));
	printf("search(3) = %d\n", search_func(root, 3));
	printf("delete(1) = %d\n", delete_func(num_searchers, &root, 1));
	printf("search(1) = %d\n", search_func(root, 1));
	printf("search(2) = %d\n", search_func(root, 2));
	printf("search(3) = %d\n", search_func(root, 3));

	sem_init(&noInserters, 0, 1);
	sem_init(&insertlock, 0, 1);
	sem_init(&searchlock, 0, 1);
	sem_init(&insertMutex, 0, 1);
	
#endif
	pthread_t searchers[NUM_SEARCHERS];
	pthread_t inserters[NUM_INSERTERS];
	pthread_t deleters[NUM_DELETERS];

	int i;
	for(i = 0; i < NUM_INSERTERS; i++){
		insert_info[i].start  = 10*i;
		insert_info[i].end  = (10*i)+ 10; 
		printf("Inserter %d START %d END %d\n", i, insert_info[i].start, insert_info[i].end); 
	}
	for(i = 0; i < NUM_DELETERS; i++){
		delete_info[i].start  = 9*i;
		delete_info[i].end  = (9*i)+ 9; 
		printf("Deleter %d START %d END %d\n", i, delete_info[i].start, delete_info[i].end); 
	}

	int rc; long t;
	for (t = 0; t < NUM_INSERTERS; t++) {
		rc = pthread_create(&inserters[t], NULL, insert, (void *)&insert_info[t]);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_SEARCHERS; t++){
		rc = pthread_create(&searchers[t], NULL, search, (void *)(10*(i)));
		if(rc){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(t = 0; t < NUM_DELETERS; t++){
		rc = pthread_create(&deleters[t], NULL, delete, (void *)&delete_info[t]);
		if(rc){	
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	//join threads (I am doing this free the list post operation)
	for(t = 0; t < NUM_SEARCHERS; t++) pthread_join(searchers[t], NULL);
	for(t = 0; t < NUM_INSERTERS; t++) pthread_join(inserters[t], NULL);
	for(t = 0; t < NUM_DELETERS; t++) pthread_join(deleters[t], NULL);
	//pthread_exit(NULL);

	//post join I want to iterater throught the list and print the elements that are left and then free them before exit
 	return 0;
}



void *search(void *value)
{	

	int ret, i, notfound, found;
	notfound = 0;

	for(i = 0; i < 1000000; i++){
		ret = search_func(root, (int)value);
		if(ret) found++;
		else notfound++;
	}
	printf("Thread %u found %d %d times and did not find it %d times\n", pthread_self(), (int)value, found, notfound);
	return 0;
}

void *insert(void *insertvalues)
{
	int i;
	struct insert_info *stufftoinsert = (struct insert_info *)insertvalues;
	
	for(i = stufftoinsert->start; i <= stufftoinsert->end; i++){
		insert_func(&root, i);
		printf("Thread %u inserted %d\n", pthread_self(), i);	
	}
	return 0;
}

void *delete(void *deletevalues)
{
	int i, ret;
	struct delete_info *stufftodelete = (struct delete_info *)deletevalues;

	printf("Deleter %d START %d END %d\n", i, delete_info[i].start, delete_info[i].end); 
	for(i = stufftodelete->start; i <= stufftodelete->end; i++){
		ret = delete_func(&root, i);
		if(ret) printf("Thread %u deleted %d\n", pthread_self(), i);
		else printf("Tread %u could not find %d\n", pthread_self(), i); 
	}
	return 0;
}

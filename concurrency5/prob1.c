#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		// sleep()
#include <pthread.h>
#include <semaphore.h>

#define THREAD_MAX 30

sem_t resource;	// resource lock that allows only 3 threads to use the resource at any given time
sem_t entry;	// restricts threads from attempting to obtain resource until all threads have exited
pthread_barrier_t exit; // threshold for releasing entry lock to allow new threads to obtain the resource

void * thread_task();

int main(int argc, char *argv[])
{
	pthread_t threads[10];
	
	pthread_barrier_init(&barrier, NULL, 3);
	sem_init

	if (argc != 2) {
		printf("use: prob1 <num_threads>\n");
		exit(-1);
	}

	int num_threads = atoi(argv[1]);

	if (num_threads > THREAD_MAX) {
		printf("error: maximum number of threads allowed is %d.\n", THREAD_MAX);
		exit(-1);
	}


	sem_init(&waiting_room, 0, num_chairs);
	sem_init(&barber_chair, 0, 1);
	sem_init(&in_progress, 0, 0);
	sem_init(&nap_time, 0, 0);



	for (t = 0; t < NUM_INSERTERS; t++) {
		rc = pthread_create(&inserters[t], NULL, inserter, NULL);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}



	

	

	for (i = 0; i < MAX_CUSTOMERS; i++) {
		count[i] = i;
	}

	sem_init(&waiting_room, 0, num_chairs);
	sem_init(&barber_chair, 0, 1);
	sem_init(&in_progress, 0, 0);
	sem_init(&nap_time, 0, 0);

	

	for (i = 0; i < num_customers; i++) {
		pthread_create(&customer[i], NULL, customer_proc, (void *) &count[i]);
	}

	for (i = 0; i < num_customers; i++) {
		pthread_join(customer[i], NULL);
	}

	end_of_day = 1; // all customers serviced, time to exit barber thread
	sem_post(&nap_time);
	pthread_join(barber, NULL);

	exit(0);
}

void * customer_proc(void * number)
{
	int num = *(int *) number;

	randwait(5);
	printf("customer %d arrived at barber shop.\n", num);

	get_hair_cut(num);

	return 0;
}

void * barber_proc(void * none)
{
	while (!end_of_day) {
		printf("barber is sleeping.\n");
		sem_wait(&nap_time);

		cut_hair();
	}

	return 0;
}
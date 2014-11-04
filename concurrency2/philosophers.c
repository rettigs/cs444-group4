#define NUM_THREADS 5
#define asm __asm__ __volatile__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *table(void *philosopher);
void think(void *philosopher);
void get_forks(void *philosopher);
void eat(void *philosopher);
void put_forks(void *philosopher);

sem_t mutex;

int main()
{
	pthread_t threads[NUM_THREADS];
	sem_init(&mutex, 0, 2);
	char *philosophers[] = {"Aristotle", "Plato", "Rene Descartes", "Confucius", "John Locke"};

	int rc;
	long t;
	for (t = 0; t < NUM_THREADS; t++) {
		rc = pthread_create(&threads[t], NULL, table, (void *)philosophers[t]);
		if (rc) {
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	pthread_exit(NULL);
}

void *table(void *philosopher)
{
	char *ph = (char *) philosopher;
	printf("%s has arrived at the table\n", ph);

	while (true) {
		think((void *)ph);
		get_forks((void *)ph);
		eat((void *)ph);
		put_forks((void *)ph);
	}
	
	pthread_exit(NULL);
}

void think(void *philosopher)
{
	char *ph = (char *) philosopher;
	
	int think_time = 0;
	asm("rdrand %0" : "=r" (think_time));
	think_time = abs(think_time) % 20 + 1;

	printf("%s is thinking for %d seconds\n", ph, think_time);
	sleep(think_time);
}

void get_forks(void *philosopher)
{
	char *ph = (char *) philosopher;
	sem_wait(&mutex);
	printf("%s got two forks\n", ph);
}

void eat(void *philosopher)
{
	char *ph = (char *) philosopher;
	srand(time(NULL));

	int eat_time = 0;
	asm("rdrand %0" : "=r" (eat_time));
	eat_time = abs(eat_time) % 9 + 2;

	printf("%s is eating for %d seconds\n", ph, eat_time);
	sleep(eat_time);
}

void put_forks(void *philosopher)
{
	char *ph = (char *) philosopher;
	sem_post(&mutex);
	printf("%s returned two forks\n", ph);
}
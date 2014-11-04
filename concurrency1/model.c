#define LIMITER 20
#define asm __asm__ __volatile__

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer_lib.h"

void *produce(void *buffer);	//producer thread
void *consume(void *buffer);	//consumer thread

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	Buffer b;
	init_b(&b, 32);

	pthread_t producer;
	pthread_t consumer;

	if(pthread_create(&producer, NULL, produce, &b)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	if(pthread_create(&consumer, NULL, consume, &b)) {
		fprintf(stderr, "Error creating thread\n");
		return 2;
	}

	//wait for the producer thread to finish
	if(pthread_join(producer, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 3;
	}

	///wait for the consumer thread to finish
	if(pthread_join(consumer, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 4;
	}

	print_b(&b);

	free_b(&b);
	return 0;
}

// producer thread
void *produce(void *buffer)
{
	Buffer *b = (Buffer *)buffer;
	int step = 0;

	while(step++ < LIMITER) {
		int sleep_time = 0;
		asm("rdrand %0" : "=r" (sleep_time));
		sleep_time = abs(sleep_time) % 7 + 3;

		Element e;
		asm("rdrand %0" : "=r" (e.value));
		asm("rdrand %0" : "=r" (e.wait_time));
		e.wait_time = abs(e.wait_time) % 9 + 2;

		//LOCK
		pthread_mutex_lock(&buffer_mutex);

		insert_b(b, &e);
		
		pthread_mutex_unlock(&buffer_mutex);
		//UNLOCK

		printf("produced %d, sleeping for %d\n", e.value, sleep_time);
		sleep(sleep_time);
	}

	return NULL;
}

// consumer thread
void *consume(void *buffer)
{	
	Buffer *b = (Buffer *)buffer;
	int step = 0;

	while(step++ < LIMITER) {
		Element e;
		e.value = -1;

		//LOCK
		pthread_mutex_lock(&buffer_mutex);

		remove_b(b, &e);

		pthread_mutex_unlock(&buffer_mutex);
		//UNLOCK

		if (e.value != -1) {
			printf("consumer sleeping for %d\n", e.wait_time);
			sleep(e.wait_time);
			printf("consumed %d\n", e.value);
		}
	}
	return NULL;
}

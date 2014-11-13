#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		// sleep()
#include <pthread.h>
#include <semaphore.h>

#define MAX_CUSTOMERS 25

void * customer_proc(void * num);
void * barber_proc(void *);
void get_hair_cut(int num);
void cut_hair();
void randwait(int secs);

sem_t waiting_room;	// restricts number of customers waiting in the barbershop
sem_t barber_chair;	// restricts barber chair to only one customer at a time
sem_t in_progress;	// customer is forced to wait until the haircut is done
sem_t nap_time;		// barber sleeps until customers wake him up
int end_of_day = 0;	// flag to halt the barber when all customers have been serviced

int main(int argc, char *argv[])
{
	pthread_t barber;
	pthread_t customer[MAX_CUSTOMERS];
	long count[MAX_CUSTOMERS];
	long i, num_chairs, num_customers;
	srand48(23);

	if (argc != 3) {
		printf("use: barbershop <num_chairs> <num_customers>\n");
		exit(-1);
	}

	num_chairs = atoi(argv[1]);
	num_customers = atoi(argv[2]);

	if (num_customers > MAX_CUSTOMERS) {
		printf("error: maximum number of customers allowed is %d.\n", MAX_CUSTOMERS);
		exit(-1);
	}

	for (i = 0; i < MAX_CUSTOMERS; i++) {
		count[i] = i;
	}

	sem_init(&waiting_room, 0, num_chairs);
	sem_init(&barber_chair, 0, 1);
	sem_init(&in_progress, 0, 0);
	sem_init(&nap_time, 0, 0);

	pthread_create(&barber, NULL, barber_proc, NULL);

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

void get_hair_cut(int num)
{
	// wait for a seat to open up in the shop
	sem_wait(&waiting_room);
	printf("customer %d took a seat in the waiting room.\n", num);

	// wait for the barber to be free
	sem_wait(&barber_chair);

	// time for a cut, leave your waiting room spot
	sem_post(&waiting_room);

	printf("customer %d is waking up the barber.\n", num);
	sem_post(&nap_time);

	// stay seated while the barber cuts your hair
	sem_wait(&in_progress);

	// leave the chair when the haircut is done
	sem_post(&barber_chair);
	printf("customer %d finished with haircut, leaving shop.\n", num);
}

void cut_hair()
{
	if (!end_of_day) {
		printf("barber is cutting hair\n");
		randwait(3);
		printf("barber is finished cutting hair.\n");

		// allow the customer to get up and leave
		sem_post(&in_progress);
	}
	else {
		printf("barber is done for the day, closing shop.\n");
	}
}

void randwait(int secs)
{
	int duration;

	duration = (int) ((drand48() * secs)+1);
	sleep(duration);
}
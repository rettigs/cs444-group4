#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>


#define BUFSIZE 10

typedef struct stuff{
	int number;
	int waittime;
} stuff;

typedef struct bufstruct{
	int numelements;
	stuff buffer[BUFSIZE];
} bufstruct;

pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
bufstruct the_buffer;


void *producer(void *wtf) {
	//obtain mutex
	pthread_mutex_lock(&the_mutex);
	while(the_buffer.numelements == 0) 
		return NULL;
}


int main(int __argc, char **_argv){
	
	//mutex initialazation stuff
	the_buffer.numelements = 0;
	memset(&the_buffer.buffer[0], -1, sizeof(stuff) * BUFSIZE); 
	return 0;
}

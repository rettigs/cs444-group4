#define NUM_SEARCHERS 2
#define NUM_INSERTERS 2
#define NUM_DELETERS 2

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "lib.h"

int main()
{
	node_t * list = new_list(4);
	print_list(list);

	push(7, &list);
	push(5, &list);
	print_list(list);

	printf("ret: %d\n", pop(&list));
	print_list(list);	

	free_list(&list);
	return 0;
}
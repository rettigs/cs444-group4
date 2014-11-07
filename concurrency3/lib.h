#ifndef LIB_H_
#define LIB_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data;
	struct node * next;
} node_t;

node_t * new_list();
void free_list(node_t ** head);
void print_list(node_t * head);

void push(node_t ** head, int val);	// add node to front of list
int pop(node_t ** head);			// remove node from front of list
int search_by_value(node_t * head, int val); // returns 1 if val is in list, returns 0 otherwise
node_t * remove_by_value(node_t * curr, int val); // recursively remove node containing val

#endif
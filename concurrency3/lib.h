#ifndef LIB_H_
#define LIB_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data;
	struct node * next;
} node_t;

node_t * new_list(int val);
void free_list(node_t ** head);
void print_list(node_t * head);

void push(int val, node_t ** head);	// add node to front of list
int pop(node_t ** head);			// remove node from front of list

#endif
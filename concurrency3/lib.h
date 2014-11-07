#ifndef LIB_H_
#define LIB_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data;
	struct node * next;
} node_t;

void free_list(node_t ** head);
void print_list(node_t * head);

node_t * create_node(int val);
node_t * insert_node(node_t * head, int val);
node_t * remove_node(node_t * curr, int val); // recursive removal
int search_by_value(node_t * head, int val); // returns 1 if val exists in linked list, returns 0 otherwise

#endif
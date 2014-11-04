#ifndef BUFFER_LIB_H_
#define BUFFER_LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
	int value;
	int wait_time;
} Element;

typedef struct {
	Element *array;
	size_t size;
	size_t used;
} Buffer;

void init_b(Buffer *buf, int init_size);
void free_b(Buffer *buf);
void print_b(Buffer *buf);
bool insert_b(Buffer *buf, Element *e);
bool remove_b(Buffer *buf, Element *e);

#endif
#include "buffer_lib.h"

void init_b(Buffer *buf, int init_size)
{
	buf->array = (Element *) malloc(init_size * sizeof(Element));
	buf->used = 0;
	buf->size = init_size;
}

void free_b(Buffer *buf)
{
	free(buf->array);
	buf->array = NULL;
	buf->used = buf->size = 0;
}

void print_b(Buffer *buf)
{
	//check if the buffer is empty
	if (buf->used == 0) {
		printf("[empty]\n");
		return;
	}

	int i;
	for (i = 0; i < buf->used; i++) {
		printf("%d:\t value: %d, wait_time: %d\n", i+1, buf->array[i].value, buf->array[i].wait_time);
	}
}

bool insert_b(Buffer *buf, Element *e)
{
	//check if the buffer is full
	if (buf->used == buf->size) {
		return false;
	} else {
		//set the current element in the buffer and increment fill counter
		buf->array[buf->used].value = e->value;
		buf->array[buf->used].wait_time = e->wait_time;
		buf->used++;
		return true;
	}
}

bool remove_b(Buffer *buf, Element *e)
{
	//check if the buffer is empty
	if (buf->used == 0) {
		return false;
	} else {
		//retreive the last element placed in the buffer
		*e = buf->array[buf->used-1];

		//set the element to 0 in array and decrement fill counter
		buf->array[buf->used-1].value = 0;
		buf->array[buf->used-1].wait_time = 0;
		buf->used--;
		return true;
	}
}
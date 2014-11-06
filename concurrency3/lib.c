#include "lib.h"

node_t * new_list(int val)
{
	node_t * head = malloc(sizeof(node_t));
	head->data = val;
	head->next = NULL;

	return head;
}

void free_list(node_t ** head)
{
	node_t * temp;

	while (*head != NULL) {
		temp = *head;
		*head = (*head)->next;
		free(temp);
	}
}

void print_list(node_t * head)
{
    node_t * current = head;

    printf("list: ");
    while (current) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

void push(int val, node_t ** head)
{
	node_t * new_node = malloc(sizeof(node_t));
	new_node->data = val;
	new_node->next = *head;
	*head = new_node;
}

int pop(node_t ** head)
{
    int retval = -1;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->data;
    free(*head);
    *head = next_node;

    return retval;
}
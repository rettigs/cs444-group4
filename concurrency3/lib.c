#include "lib.h"

node_t * new_list()
{
	node_t * head = malloc(sizeof(node_t));
	head->data = -1;
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

void push(node_t ** head, int val)
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

int search_by_value(node_t * head, int val)
{
    node_t * temp = head;

    while(temp) {
        if (temp->data == val) {
            return 1;
        } else {
            temp = temp->next;
        }
    }

    return 0;
}

node_t * remove_by_value(node_t * curr, int val)
{
    // are we at the end of the list
    if (curr == NULL) {
        return NULL;
    }

    // is curr node the one to be deleted
    if (curr->data == val) {
        node_t * temp = curr->next;

        free(curr);

        // return new node in order to skip over the removed node
        return temp;
    }

    // check rest of list and fix next ptr in case of removed node
    curr->next = remove_by_value(curr->next, val);

    return curr;
}
#include "lib.h"

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

node_t * create_node(int val)
{
    node_t * new_node = malloc(sizeof(node_t));

    if(new_node == NULL) {
        printf("malloc failed to create node\n");
    }  
    new_node->data = val;
    new_node->next = NULL;

    return new_node;
}


node_t * insert_node(node_t * head, int val)
{
    if (head == NULL) {
        head = create_node(val);
    }
    else {
        node_t * temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = create_node(val);
    }

    return head;
}

node_t * remove_node(node_t * curr, int val)
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
    curr->next = remove_node(curr->next, val);

    return curr;
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
//
// Created by jake on 12/17/19.
//

#include "linkedlist.h"

LinkedList *linkedlist_new() {
    LinkedList *tmp = (LinkedList *) malloc(sizeof(LinkedList));
    tmp->data = NULL;
    return tmp;
}

/**
 * Add a char * to our linked list and set up a new head.
 * @param head The first node in the list.
 * @param data Our data to add to the list.
 * @return
 */
void *linkedlist_add(LinkedList *head, char *data) {
    if (head == NULL) {
        head = linkedlist_new();
    }
    if (head->data == NULL) {
        head->data = data;
    }
    LinkedList *tmp = linkedlist_new();
    // Set the head pointer to the next node.
    head->next = tmp;

    // Set head to tmp.
    head = tmp;
}

void free_node(LinkedList *node) {
    free(node->data);
    free(node);
}
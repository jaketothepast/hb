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
 */
void linkedlist_add(LinkedList *head, char *data) {
    // Create a pointer to the head element.
    LinkedList *tmp = head;

    // Head was null? Create a new node.
    if (head == NULL) {
        head = linkedlist_new();
        head->data = data;
    }
    else {
        // Iterate until we don't have a next node.
        while ((tmp = tmp->next) != NULL) ;

        tmp->next = linkedlist_new();
        tmp->next->data = data;
        tmp->next->next = NULL;
    }
}

void free_node(LinkedList *node) {
    free(node->data);
    free(node);
}
//
// Created by jake on 12/17/19.
//

#include <stdio.h>
#include <string.h>
#include "linkedlist.h"

LinkedList *linkedlist_new() {
    LinkedList *tmp = (LinkedList *) malloc(sizeof(LinkedList));
    tmp->data = NULL;
    tmp->next = NULL;
    return tmp;
}

static LinkedList *_linkedlist_new_with_data(char *data) {
    LinkedList *tmp = linkedlist_new();
    tmp->data = strdup(data);
    return tmp;
}

/**
 * Add a char * to our linked list and set up a new head.
 * @param head The first node in the list.
 * @param data Our data to add to the list.
 */
void linkedlist_add(LinkedList **head, char *data) {
    // Create a pointer to the head element.
    LinkedList *tmp = *head;

    if (tmp == NULL) {
        *head = _linkedlist_new_with_data(data);
    }
    else {
        // Iterate until we don't have a next node.
        while (tmp->next != NULL)
            tmp = tmp->next;

        tmp->next = _linkedlist_new_with_data(data);
        tmp->next->next = NULL;
    }
}

void linkedlist_print(LinkedList *head) {
    LinkedList *tmp = head;
    while (tmp != NULL) {
        fprintf(stderr, "Node: %s\n", tmp->data);
        tmp = tmp->next;
    }
}

void free_list(LinkedList **head) {
    LinkedList *ptr = *head;
    LinkedList *prev = NULL;

    if (ptr == NULL)
        return;

    while (ptr != NULL) {
        prev = ptr;
        ptr = ptr->next;
        free(prev->data);
        free(prev);
    }
}
void free_node(LinkedList *node) {
    free(node->data);
    free(node);
}
//
// Created by jake on 12/17/19.
//

#include <stdio.h>
#include "linkedlist.h"

LinkedList *linkedlist_new() {
    LinkedList *tmp = (LinkedList *) malloc(sizeof(LinkedList));
    tmp->data = NULL;
    tmp->next = NULL;
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

    if (tmp->data == NULL) {
        tmp->data = data;
    }
    else {
        fprintf(stderr, "Adding to the body\n");
        // Iterate until we don't have a next node.
        while ((tmp = tmp->next) != NULL) ;

        tmp->next = linkedlist_new();
        tmp->next->data = data;
        tmp->next->next = NULL;
    }
}

void linkedlist_print(LinkedList *head) {
    fprintf(stderr, "Entering linked list print\n");
    LinkedList *tmp = head;
    while (tmp != NULL) {
        fprintf(stderr, "Node: %s\n", tmp->data);
        tmp = tmp->next;
    }
}
void free_list(LinkedList *head) {

}
void free_node(LinkedList *node) {
    free(node->data);
    free(node);
}
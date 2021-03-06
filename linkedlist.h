//
// Created by jake on 12/17/19.
//

#ifndef HOSTBLOCKER_LINKEDLIST_H
#define HOSTBLOCKER_LINKEDLIST_H

#include <stdlib.h>

/**
 * Our linked list structure to be used for holding configuration files.
 */
typedef struct linkedlist {
    char *data;
    struct linkedlist *next;
} LinkedList;

LinkedList *linkedlist_new();
void linkedlist_add(LinkedList **head, char *data);
void linkedlist_print(LinkedList *head);
void free_list(LinkedList **head);
#endif //HOSTBLOCKER_LINKEDLIST_H

// Copyright 2021 Nita Maria Alexandra

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

// Aloca memorie pentru o lista liniara simplu inlantuita.

linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));
    DIE(ll == NULL, "linked_list malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

// Adauga nod nou intr-o lista simplu inlantuita, pe pozitia n.

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (list == NULL) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    } else {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

// Elimina nodul de pe pozitia n din lista al carei pointer este trimis
// ca parametru.

ll_node_t *
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (list == NULL) {
        return NULL;
    }

    if (list->head == NULL) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

// Functia intoarce numarul de noduri dintr-o lista liniara simplu inlantuita.

unsigned int
ll_get_size(linked_list_t* list)
{
    if (list == NULL) {
        return -1;
    }

    return list->size;
}

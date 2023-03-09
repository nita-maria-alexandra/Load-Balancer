// Copyright 2021 Nita Maria Alexandra

#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

// Se aloca memoria intr-un hashtable.

server_memory* init_server_memory() {
	server_memory *ht;
	unsigned int i;

	ht = malloc(sizeof(server_memory));
	DIE(ht == NULL, "malloc() failed");

	ht->buckets = malloc(100 * sizeof(linked_list_t *));
	DIE(ht->buckets == NULL, "malloc() failed");

	ht->hmax = 100;

	for (i = 0; i < ht->hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(struct info));
	}

	ht->size = 0;

	return ht;
}

// Functie de hash.

unsigned int hash_function(void *a){
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// Stocheaza o preche key-value pe un server.

void server_store(server_memory* server, char* key, char* value) {
	int index = hash_function(key) % server->hmax, k = 0;
    struct info aux;
    aux.key = malloc(strlen(key) + 1);
    DIE(aux.key == NULL, "malloc() failed");
    strcpy(aux.key, key);
    aux.value = malloc(strlen(value) + 1);
    DIE(aux.value == NULL, "malloc() failed");
    strcpy(aux.value, value);
    aux.hash = hash_function(key);

    ll_node_t *curr = server->buckets[index]->head;

    while (curr != NULL) {
        if (strcmp(key, ((struct info *)curr->data)->key) == 0) {
            strcpy(((struct info *)curr->data)->value, value);
            free(aux.key);
            free(aux.value);
            k = 1;
            break;
        }

        curr = curr->next;
    }

    if (k == 0) {
        ll_add_nth_node(server->buckets[index], server->hmax, &aux);
    }
}

// Elimina o perche key-value de pe server.

void server_remove(server_memory* server, char* key) {
	int index = hash_function(key) % server->hmax, pos = 0;
    ll_node_t *curr = server->buckets[index]->head;

    while (curr != NULL) {
        if (strcmp(key, ((struct info *)curr->data)->key) == 0) {
            free(((struct info *)curr->data)->key);
            free(((struct info *)curr->data)->value);
            curr = ll_remove_nth_node(server->buckets[index], pos);
            free(curr->data);
            free(curr);
            server->size--;
            return;
        }

        pos++;
        curr = curr->next;
    }
}

// Returneaza valoarea obiectului cu o anumita cheie.

char* server_retrieve(server_memory* server, char* key) {
	int index = hash_function(key) % server->hmax;
    ll_node_t *curr = server->buckets[index]->head;

    while (curr != NULL) {
        if (strcmp(key, ((struct info *)curr->data)->key) == 0) {
            return ((struct info *)curr->data)->value;
        }

        curr = curr->next;
    }

    return NULL;
}

// Elibereaza memoria ocupata de un server.

void free_server_memory(server_memory* server) {
	unsigned int i;
    ll_node_t *curr, *next;

    for (i = 0; i < server->hmax; i++) {
        curr = server->buckets[i]->head;
        while (curr != NULL) {
            next = curr->next;
            free(((struct info *)curr->data)->key);
            free(((struct info *)curr->data)->value);
            free(curr->data);
            free(curr);
            curr = next;
        }
        free(server->buckets[i]);
    }

    free(server->buckets);
    free(server);
}

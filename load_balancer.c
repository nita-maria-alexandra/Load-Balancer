// Copyright 2021 Nita Maria Alexandra

#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "utils.h"

struct load_balancer {
    doubly_linked_list_t *hash_ring;
};

unsigned int hash_function_servers(void *a){
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a){
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// Se aloca memoria intr-o lista circulara dublu inlantuita(hash ring).

load_balancer* init_load_balancer(){
    load_balancer *hr = malloc(sizeof(load_balancer));
    DIE(!hr, "malloc() failed");
    hr->hash_ring = malloc(sizeof(doubly_linked_list_t));
    DIE(!hr->hash_ring, "malloc() failed");
    hr->hash_ring->head = NULL;
    hr->hash_ring->size = 0;
    hr->hash_ring->data_size = sizeof(server_memory*);
    return hr;
}

// Verifica daca o cheie exista intr-un server.

int ht_has_key(server_memory *ht, char *key)
{
    int index = hash_function_key(key) % ht->hmax;
    ll_node_t *curr = ht->buckets[index]->head;

    while (curr != NULL) {
        if (strcmp(key, ((struct info *)curr->data)->key) == 0) {
            return 1;
        }

        curr = curr->next;
    }

    return 0;
}

// Stocheaza o perche cheie-valoare pe serverul potrivit.

void loader_store(load_balancer* main, char* key, char* value, int* server_id){
    dll_node_t* node = main->hash_ring->head;
    unsigned int i, k = 0;
    unsigned int hash = hash_function_key(key);

    for (i = 0; i < main->hash_ring->size; i++) {
        if (hash < ((server_memory*)node->data)->hash) {
            server_store(((server_memory*)node->data), key, value);
            *server_id = ((server_memory*)node->data)->server_id;
            k = 1;
            break;
        }
        node = node->next;
    }

    if (k == 0) {
        server_store(((server_memory*)main->hash_ring->head->data), key, value);
        *server_id = ((server_memory*)main->hash_ring->head->data)->server_id;
    }
}

// Returenaza perechea cheie-valoare gasita pe un server si ID-ul serverului;
// Returneaza NULL daca perechea nu exista.

char* loader_retrieve(load_balancer* main, char* key, int* server_id){
    dll_node_t* node = main->hash_ring->head;
    unsigned int i;

    node = main->hash_ring->head;
    for (i = 0; i < main->hash_ring->size; i++) {
        if (ht_has_key(((server_memory*)node->data), key)) {
            *server_id = ((server_memory*)node->data)->server_id;
            return server_retrieve(((server_memory*)node->data), key);
        }

        node = node->next;
    }
	return NULL;
}

// Returneaza pozitia pe care trebuie adaugat un server.

int find_pos(load_balancer* main, unsigned int hash, int server_id){
    dll_node_t* node = main->hash_ring->head;
    unsigned int i;

    for (i = 0; i < main->hash_ring->size; i++) {
        if (((server_memory *)node->data)->hash > hash) {
            return i;
        } else {
            if (((server_memory *)node->data)->hash == hash) {
                if (((server_memory *)node->data)->server_id > server_id) {
                    return i;
                }

                return i + 1;
            }
        }
        node = node->next;
    }
    return main->hash_ring->size;
}

// Se face remaparea obiectelor dupa ce a fost adaugat un nou server in lista.

void move_object(load_balancer* main, int pos){
    int i;
    unsigned int j;
    dll_node_t* node = main->hash_ring->head;
    ll_node_t* object, *aux2;

    for (i = 0; i < pos; i++) {
        node = node->next;
    }

    if (node != main->hash_ring->head && pos != 1) {
        for (j = 0; j < ((server_memory*)node->data)->hmax; j++) {
            object = ((server_memory*)node->data)->buckets[j]->head;
            while (object != NULL) {
                if (((struct info*)object->data)->hash <
                    ((server_memory*)node->prev->data)->hash) {
                    aux2 = object->next;
                    server_store(((server_memory*)node->prev->data),
                        ((struct info*)object->data)->key,
                        ((struct info*)object->data)->value);
                    server_remove(((server_memory*)node->data),
                        ((struct info*)object->data)->key);
                    object = aux2;
                } else {
                    object = object->next;
                }
            }
        }
    }

    if (pos == 1) {
        for (j = 0; j < ((server_memory*)node->data)->hmax; j++) {
            object = ((server_memory*)node->data)->buckets[j]->head;
            while (object != NULL) {
                if (((struct info*)object->data)->hash >
                    ((server_memory*)node->prev->prev->data)->hash ||
                    ((struct info*)object->data)->hash <
                    ((server_memory*)node->prev->data)->hash) {
                    aux2 = object->next;
                    server_store(((server_memory*)node->prev->data),
                        ((struct info*)object->data)->key,
                        ((struct info*)object->data)->value);
                    server_remove(((server_memory*)node->data),
                        ((struct info*)object->data)->key);
                    object = aux2;
                } else {
                    object = object->next;
                }
            }
        }
    }

    if (node == main->hash_ring->head) {
       for (j = 0; j < ((server_memory*)node->data)->hmax; j++) {
            object = ((server_memory*)node->data)->buckets[j]->head;
            while (object != NULL) {
                if (((struct info*)object->data)->hash >
                    ((server_memory*)node->prev->prev->data)->hash &&
                    ((struct info*)object->data)->hash <
                    ((server_memory*)node->prev->data)->hash) {
                    aux2 = object->next;
                    server_store(((server_memory*)node->prev->data),
                        ((struct info*)object->data)->key,
                        ((struct info*)object->data)->value);
                    server_remove(((server_memory*)node->data),
                        ((struct info*)object->data)->key);
                    object = aux2;
                } else {
                    object = object->next;
                }
            }
        }
    }
}

// Se adauga se adauga serverul pe hash ring, pe pozitia n.

void add(doubly_linked_list_t* list, unsigned int n, int id, int replica,
    int eticheta, unsigned int hash)
{
    dll_node_t* curr, *new, *last;
    unsigned int pos = 0;

    if (list->head != NULL){
        last = list->head->prev;
    }

    if (list == NULL || n > list->size) {
        return;
    }

    new = (dll_node_t*)malloc(sizeof(dll_node_t));
    DIE(!new, "malloc() failed");
    new->data = init_server_memory();
    ((server_memory *)new->data)->server_id = id;
    ((server_memory *)new->data)->replica_id = replica;
    ((server_memory *)new->data)->eticheta = eticheta;
    ((server_memory *)new->data)->hash = hash;
    ((server_memory *)new->data)->size = 0;

    if (list->head == NULL && n == 0) {
        list->head = new;
        list->head->next = list->head;
        list->head->prev = list->head;
        list->size++;
        return;
    }

    if (n == 0) {
        new->next = list->head;
        new->prev = list->head->prev;
        list->head->prev->next = new;
        list->head->prev = new;
        list->head = new;
        list->size++;
        return;
    }

    if (n == list->size) {
        last->next = new;
        new->prev = last;
        new->next = list->head;
        list->head->prev = new;
        list->size++;
        return;
    }

    curr = list->head;
    while (pos != n) {
        pos++;
        curr = curr->next;
    }
    new->next = curr;
    new->prev = curr->prev;
    curr->prev->next = new;
    curr->prev = new;
    list->size++;
}

// Adaugarea unui server si a replicilor lui pe hash ring.
// Remaparea obiectelor.

void loader_add_server(load_balancer* main, int server_id){
    unsigned int hash = hash_function_servers(&server_id);
    int pos = find_pos(main, hash, server_id), eticheta;

    add(main->hash_ring, pos, server_id, 0, server_id, hash);
    move_object(main, pos + 1);
    eticheta = 100000 + server_id;
    hash = hash_function_servers(&eticheta);
    pos = find_pos(main, hash, server_id);

    add(main->hash_ring, pos, server_id, 1, eticheta, hash);
    move_object(main, pos + 1);

    eticheta = 200000 + server_id;
    hash = hash_function_servers(&eticheta);
    pos = find_pos(main, hash, server_id);
    add(main->hash_ring, pos, server_id, 2, eticheta, hash);
    move_object(main, pos + 1);
}

// Este eliminat dintr-o lista circulara dublu inlantuita nodul de pe pozitia n.

void dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    dll_node_t* removed, *curr;
    unsigned int pos = 1;

    if (list->size == 1 && n == 0) {
        free_server_memory(((server_memory*)list->head->data));
        free(list->head);
        list->head = NULL;
        list->size = 0;
        return;
    }
    if (n == 0) {
        removed = list->head;
        list->head->next->prev = list->head->prev;
        list->head->prev->next = list->head->next;
        list->head = list->head->next;
        list->size--;
        free_server_memory(((server_memory*)removed->data));
        free(removed);
        return;
    }
    if (n == list->size - 1) {
        if (list->head->next == list->head) {
            free_server_memory(((server_memory*)list->head->data));
            free(list->head);
        }

        curr = list->head;
        while (curr->next != list->head) {
            curr = curr->next;
        }
        curr->prev->next = list->head;
        list->head->prev = curr->prev;
        list->size--;
        free_server_memory(((server_memory*)curr->data));
        free(curr);
        return;
    }

    removed = list->head->next;
    while (pos != n) {
        pos++;
        removed = removed->next;
    }

    removed->prev->next = removed->next;
    removed->next->prev = removed->prev;
    list->size--;
    free_server_memory(((server_memory*)removed->data));
    free(removed);
}

// Remapeaza obiectele pe cel mai apropiat server din dreapta celui
// care trebuie eliminat;
// Elimina serverul de pe hash ring.

void loader_remove_server(load_balancer* main, int server_id){
    dll_node_t* node = main->hash_ring->head, *aux;
    ll_node_t* object;
    unsigned int i, j;

    for (i = 0; i < main->hash_ring->size; i++) {
        if (((server_memory*)node->data)->server_id == server_id) {
            for (j = 0; j < ((server_memory *)node->data)->hmax; j++) {
                object = ((server_memory*)node->data)->buckets[j]->head;
                while (object) {
                    aux = node->next;
                    while (((server_memory *)aux->data)->hash ==
                        ((struct info*)object->data)->hash) {
                        aux = aux->next;
                    }
                    server_store(((server_memory *)aux->data),
                        ((struct info*)object->data)->key,
                        ((struct info*)object->data)->value);
                    object = object->next;
                }
            }
            node = node->next;
            dll_remove_nth_node(main->hash_ring, i);
            i--;

        } else {
            node = node->next;
        }
    }
}

// Elibereaza memoria ocupata de hash ring.

void free_load_balancer(load_balancer* main){
    dll_node_t* curr = main->hash_ring->head->next, *n  = NULL;
    dll_node_t* aux = main->hash_ring->head;

    while (curr != aux) {
        n = curr->next;
        free_server_memory(((server_memory *)curr->data));
        free(curr);
        curr = n;
    }

    free_server_memory(((server_memory *)curr->data));
    free(aux);
    free(main->hash_ring);
    free(main);
}

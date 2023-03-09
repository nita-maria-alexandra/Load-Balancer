// Copyright 2021 Nita Maria Alexandra

#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include "server.h"

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;
    dll_node_t *prev, *next;
};

struct load_balancer;
typedef struct load_balancer load_balancer;

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t
{
    dll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

load_balancer* init_load_balancer();

void free_load_balancer(load_balancer* main);

void loader_store(load_balancer* main, char* key, char* value, int* server_id);

char* loader_retrieve(load_balancer* main, char* key, int* server_id);

void loader_add_server(load_balancer* main, int server_id);

void loader_remove_server(load_balancer* main, int server_id);

int ht_has_key(server_memory *ht, char *key);

int find_pos(load_balancer* main, unsigned int hash, int server_id);

void move_object(load_balancer* main, int pos);

void
add(doubly_linked_list_t* list, unsigned int n, int id, int replica,
	int eticheta, unsigned int hash);
#endif  //  LOAD_BALANCER_H_

// Copyright 2021 Nita Maria Alexandra

#ifndef SERVER_H_
#define SERVER_H_

#include "LinkedList.h"

struct info {
	char *key;
	char *value;
	unsigned int hash;
};

typedef struct server_memory server_memory;

struct server_memory {
	linked_list_t **buckets;
	int server_id;
	int replica_id;
	int eticheta;
	unsigned int hash;
	unsigned int size;
	unsigned int hmax;
};

server_memory* init_server_memory();

void free_server_memory(server_memory* server);

void server_store(server_memory* server, char* key, char* value);

void server_remove(server_memory* server, char* key);

char* server_retrieve(server_memory* server, char* key);

#endif  // SERVER_H_

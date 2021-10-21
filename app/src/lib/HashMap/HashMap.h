#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../LinkedList/LinkedList.h"

typedef struct HashTable HashTable;

struct HashTable
{
  Ht_item **items;
  LinkedList **overflow_buckets;
  int size;
  int count;
};

unsigned long hash_function(char *str);
LinkedList **create_overflow_buckets(HashTable *table);
void free_overflow_buckets(HashTable *table);
Ht_item *create_item(char *key, char *value);
HashTable *create_table(int size);
void free_item(Ht_item *item);
void free_hashtable(HashTable *table);
void handle_collision(HashTable *table, unsigned long index, Ht_item *item);
void ht_insert(HashTable *table, char *key, char *value);
char *ht_search(HashTable *table, char *key);
void ht_delete(HashTable *table, char *key);
void print_search(HashTable *table, char *key);
void print_hashtable(HashTable *table);
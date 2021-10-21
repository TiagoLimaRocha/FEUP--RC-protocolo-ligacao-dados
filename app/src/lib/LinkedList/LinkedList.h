#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ht_item Ht_item;

struct Ht_item
{
  char *key;
  char *value;
};

typedef struct LinkedList LinkedList;

struct LinkedList
{
  Ht_item *item;
  LinkedList *next;
};

LinkedList *allocate_list();
LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item);
Ht_item *linkedlist_remove(LinkedList *list);
void free_linkedlist(LinkedList *list);

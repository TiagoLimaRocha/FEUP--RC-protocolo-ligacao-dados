#include "LinkedList.h"

LinkedList *allocate_list()
{
  // Allocates memory for a Linkedlist pointer
  LinkedList *list = (LinkedList *)calloc(1, sizeof(LinkedList));
  return list;
}

LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item)
{
  // Inserts the item onto the Linked List
  if (!list)
  {
    LinkedList *head = allocate_list();
    head->item = item;
    head->next = NULL;
    list = head;
    return list;
  }

  else if (list->next == NULL)
  {
    LinkedList *node = allocate_list();
    node->item = item;
    node->next = NULL;
    list->next = node;
    return list;
  }

  LinkedList *temp = list;
  while (temp->next)
  {
    temp = temp->next;
  }

  LinkedList *node = allocate_list();
  node->item = item;
  node->next = NULL;
  temp->next = node;

  return list;
}

Ht_item *linkedlist_remove(LinkedList *list)
{
  // Removes the head from the linked list
  // and returns the item of the popped element
  if (!list)
    return NULL;
  if (!list->next)
    return NULL;
  LinkedList *node = list->next;
  LinkedList *temp = list;
  temp->next = NULL;
  list = node;
  Ht_item *it = NULL;
  memcpy(temp->item, it, sizeof(Ht_item));
  free(temp->item->key);
  free(temp->item->value);
  free(temp->item);
  free(temp);
  return it;
}

void free_linkedlist(LinkedList *list)
{
  LinkedList *temp = list;
  if (!list)
    return;
  while (list)
  {
    temp = list;
    list = list->next;
    free(temp->item->key);
    free(temp->item->value);
    free(temp->item);
    free(temp);
  }
}
#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/**
 * struct node_s - Node of a doubly linked list
 *
 * @content: Address of the content stored in the node
 * @prev: Pointer to the previous node of the list
 * @next: Pointer to the next node of the list
 */
typedef struct node_s
{
	void *content;
	struct node_s *prev;
	struct node_s *next;
} node_t;

/**
 * struct list_s - Doubly linked list
 *
 * @head: Pointer to the first node of the list
 * @tail: Pointer to the last node of the list
 * @size: Number of nodes stored in the list
 */
typedef struct list_s
{
	node_t *head;
	node_t *tail;
	size_t size;
} list_t;

typedef void (*node_func_t)(void *content);

int list_init(list_t *list);
int list_add(list_t *list, void *content);
void list_each(list_t const *list, node_func_t func);
void list_destroy(list_t *list, node_func_t func);

#endif /* LIST_H */

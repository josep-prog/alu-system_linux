#include <stdlib.h>
#include "list.h"

/**
 * list_init - Initialize a list
 * @list: Pointer to the list to initialize
 *
 * Return: 1 on success, 0 on failure
 */
int list_init(list_t *list)
{
	if (!list)
		return (0);

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	return (1);
}

/**
 * list_add - Add a new node holding content at the end of a list
 * @list: Pointer to the list to add the node to
 * @content: Address of the content to store in the new node
 *
 * Return: 1 on success, 0 on failure
 */
int list_add(list_t *list, void *content)
{
	node_t *node;

	if (!list)
		return (0);

	node = malloc(sizeof(*node));
	if (!node)
		return (0);

	node->content = content;
	node->next = NULL;
	node->prev = list->tail;

	if (list->tail)
		list->tail->next = node;
	else
		list->head = node;
	list->tail = node;
	list->size++;
	return (1);
}

/**
 * list_each - Call a function for every node of a list
 * @list: Pointer to the list to go through
 * @func: Pointer to the function to call with every node's content
 */
void list_each(list_t const *list, node_func_t func)
{
	node_t *node;

	if (!list || !func)
		return;

	for (node = list->head; node; node = node->next)
		func(node->content);
}

/**
 * list_destroy - Free every node of a list
 * @list: Pointer to the list to destroy
 * @func: Pointer to a function to call with every node's content, or NULL
 */
void list_destroy(list_t *list, node_func_t func)
{
	node_t *node, *next;

	if (!list)
		return;

	for (node = list->head; node; node = next)
	{
		next = node->next;
		if (func)
			func(node->content);
		free(node);
	}
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

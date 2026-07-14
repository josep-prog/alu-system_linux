#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "todo_api.h"

#define TODO_API_MAX_TODOS 1024

static todo_t todos[TODO_API_MAX_TODOS];
static size_t todo_count;

/**
 * create_todo - Create a new todo and add it to the in memory list
 * @title: Title of the todo
 * @description: Description of the todo
 *
 * Return: Pointer to the newly created todo
 */
todo_t *create_todo(char const *title, char const *description)
{
	todo_t *todo;

	todo = &todos[todo_count];
	todo->id = (int)todo_count;
	todo->title = strdup(title);
	todo->description = strdup(description);
	todo_count++;

	return (todo);
}

/**
 * todo_to_json - Serialize a single todo to a JSON object
 * @todo: Todo to serialize
 *
 * Return: A malloc'd, NULL terminated JSON string
 */
char *todo_to_json(todo_t const *todo)
{
	char *json;
	int len;

	len = snprintf(NULL, 0,
		"{\"id\":%d,\"title\":\"%s\",\"description\":\"%s\"}",
		todo->id, todo->title, todo->description);
	json = malloc((size_t)len + 1);
	snprintf(json, (size_t)len + 1,
		"{\"id\":%d,\"title\":\"%s\",\"description\":\"%s\"}",
		todo->id, todo->title, todo->description);

	return (json);
}

/**
 * todos_to_json - Serialize every stored todo to a JSON array
 *
 * Return: A malloc'd, NULL terminated JSON string
 */
char *todos_to_json(void)
{
	char *result, *item;
	size_t total, i, extra;

	result = malloc(2);
	result[0] = '[';
	result[1] = '\0';
	total = 1;

	for (i = 0; i < todo_count; i++)
	{
		item = todo_to_json(&todos[i]);
		extra = strlen(item) + (i > 0 ? 1 : 0);
		result = realloc(result, total + extra + 2);
		if (i > 0)
			result[total++] = ',';
		memcpy(result + total, item, strlen(item));
		total += strlen(item);
		free(item);
	}

	result[total] = ']';
	result[total + 1] = '\0';

	return (result);
}

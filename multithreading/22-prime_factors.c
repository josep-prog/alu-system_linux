#include <stdlib.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * create_task - Create a new task
 * @entry: Pointer to the entry function of the task
 * @param: Parameter to later pass to the entry function
 *
 * Return: Pointer to the created task, or NULL on failure
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task;

	task = malloc(sizeof(*task));
	if (!task)
		return (NULL);

	task->entry = entry;
	task->param = param;
	task->status = PENDING;
	task->result = NULL;
	pthread_mutex_init(&task->lock, NULL);

	return (task);
}

/**
 * destroy_task - Destroy a task, freeing its result if any
 * @task: Pointer to the task to destroy
 */
void destroy_task(task_t *task)
{
	list_t *result;

	if (!task)
		return;

	result = (list_t *)task->result;
	if (result)
	{
		list_destroy(result, free);
		free(result);
	}

	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * reserve_task - Atomically reserve a pending task for execution
 * @task: Pointer to the task to reserve
 *
 * Return: 1 if the task was reserved by the caller, 0 otherwise
 */
static int reserve_task(task_t *task)
{
	int reserved;

	pthread_mutex_lock(&task->lock);
	reserved = (task->status == PENDING);
	if (reserved)
		task->status = STARTED;
	pthread_mutex_unlock(&task->lock);

	return (reserved);
}

/**
 * exec_tasks - Execute every pending task of a list, thread entry point
 * @tasks: Pointer to the list of tasks to execute
 *
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	size_t i;

	i = 0;
	for (node = tasks->head; node; node = node->next)
	{
		task = (task_t *)node->content;
		if (reserve_task(task))
		{
			tprintf("[%02lu] Started\n", (unsigned long)i);
			task->result = task->entry(task->param);
			task->status = SUCCESS;
			tprintf("[%02lu] Success\n", (unsigned long)i);
		}
		i++;
	}

	return (NULL);
}

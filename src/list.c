#include <stdlib.h>
#include "unittest_priv.h"


struct list *
list_append(struct list *list, void *data)
{
	struct list *last,
				*new;

	if ((new = (struct list *) calloc(1, sizeof(struct list))) == NULL)
		err_sys("malloc");
	new->data = data;
	if (list == NULL)
		return new;
	last = list_last(list);
	last->next = new;
	return list;
}

void
list_free(struct list *list, void(*free_data)(void *))
{
	struct list *tmp;

	while (list != NULL) {
		if (free_data != NULL)
			free_data(list->data);
		tmp = list;
		list = list->next;
		free(tmp);
	}
}

unsigned int
list_len(struct list *list)
{
	unsigned int c;

	c = 0;
	while (list != NULL) {
		list = list->next;
		c++;
	}
	return c;
}

struct list *
list_last(struct list *list)
{
	if (list != NULL)
		while (list->next != NULL)
			list = list->next;
	return list;
}

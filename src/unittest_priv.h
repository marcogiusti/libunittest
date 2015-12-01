#ifndef __UNITTEST_PRIV_H
#define __UNITTEST_PRIV_H

#define MAXLINE 4096

void err_sys(const char *, ...);

struct list {
	void *data;
	struct list *next;
};

struct list *list_append(struct list *list, void *data);
void list_free(struct list *list, void (*free_el)(void *));
unsigned int list_len(struct list *list);
struct list *list_last(struct list *list);

#endif /* __UNITTEST_PRIV_H */

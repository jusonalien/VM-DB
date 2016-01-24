#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct list_head {
        struct list_head *prev, *next;
}gc_list;


#define INIT_LIST_HEAD(name_ptr)        do {    (name_ptr)->next = (name_ptr); \
                                                (name_ptr)->prev = (name_ptr); \
                                        }while (0)

#define OFFSET(type, member)            (char *)&(((type *)0x0)->member)

#define container_of(ptr, type, member) ({                                      \
                        (type *)((char * )ptr - OFFSET(type, member)); });

#define list_for_each(pos, head)        for (pos = head->next; pos != head; pos = pos->next)
#define list_for_each_prev(pos, head)   for (pos = head->prev; pos != head; pos = pos->prev)
#define list_entry(ptr, type, member)   container_of(ptr, type, member)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     pos->member.next, &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

static inline void __list_add(gc_list *new,
			      gc_list *prev,
			      gc_list *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}


static inline void list_add_tail(gc_list *new, gc_list *head)
{
        __list_add(new, head->prev, head);
}


static inline void list_add(gc_list *new, gc_list *head)
{
        __list_add(new, head, head->next);
}

static inline void list_del(gc_list *p)
{
        p->prev->next = p->next;
        p->next->prev = p->prev;
}

#endif

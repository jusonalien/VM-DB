#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

/*
 *This was the implementation of the circular linked list which
 *was GREATLY INSPIRED by the implementation of the list in the
 *                  Linux Kernel 4.2.0
 */

#include <stddef.h>
//#include <stdlib.h>

#define INIT_LIST_HEAD(name_ptr) do { (name_ptr)->next = (name_ptr);\
                                      (name_ptr)->prev = (name_ptr);\
                                }while(0)

typedef struct cir_list_head {
    struct cir_list_head *prev, *next;
}gc_list;

/*------------------------------CONTAINER_OF----------------------------*/

#define OFFSETOF(type, member)        (size_t)&(((type *)0)->member)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - OFFSETOF(type,member) );})


/*------------------------------CONTAINER_OF----------------------------*/

/*------------------------------LIST_VISIT------------------------------*/


#define list_entry(ptr, type, member)                              \
        container_of(ptr, type, member)
#define list_for_each_next(pos, head)                              \
        for (pos = head->next; pos != head; pos = pos->next)
#define list_for_each_prev(pos,head)                               \
        for((pos) = (head)->prev; (pos) != (head); (pos) = (pos)->prev)

#define list_for_each_entry(pos, head, member)                     \
        for(pos = list_entry((head)->next, typeof(*pos)), member); \
            pos->member.next, &pos->member != (head);              \
            pos = list_entry(pos->member.next, typeof(*pos), member))

/*------------------------------LIST_VISIT------------------------------*/


/*------------------------------LIST_OPE_FUNCTION-----------------------*/

static inline void __list_add(gc_list *add_node,
                              gc_list *current_node,
                              gc_list *next_node)
{
    next_node->prev    = add_node;
    add_node->next     = next_node;
    add_node->prev     = current_node;
    current_node->next = add_node;
}

static inline void __list_del(gc_list *prev, gc_list *next)
{
    next->prev = prev;
    prev->next = next;
}



static inline void list_add(gc_list *add_node, gc_list *current_node)
{
    __list_add(add_node, current_node, current_node->next);
}

static inline void list_add_tail(gc_list *add_node, gc_list *head)
{
    __list_add(add_node, head->prev,head);
}

#define DEL_MEN(entry) (free((entry)))

static inline void list_node_del(gc_list *entry)
{
    __list_del(entry->prev, entry->next);

}

/*------------------------------LIST_OPE_FUNCTION-----------------------*/
#endif // LIST_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <libaio.h>
#include <sys/uio.h>


#include "list.h"
#include "radix_tree.h"

#define GEARCACHE_UPPER_LIMIT  131072 // allocate 64MB

typedef struct page_node {
	uint64_t 	pageAddr;
	int           size;
	char*         buf;
	gc_list       list;
} page_node_t;

page_node_t *page_node_pool = NULL;
char *buf_pool = NULL;

gc_portal current_portal;
gc_list current_list;
static unsigned long current_pos = 0;

typedef unsigned int U32;
typedef int BOOl;
gc_list **p_head;
unsigned long Node_num;

void list_tran(gc_list*);
void Sort_dui(void);
inline int flush_heap_all(int file_fd,
			  gc_list *,
			  gc_portal *);

inline page_node_t *search_in_gearcache(uint64_t offset, gc_portal *portal)
{
	unsigned long index;
	page_node_t *gcNode = NULL;

	index = offset ;
	gcNode = radix_tree_lookup(portal, index);

	return gcNode;
}

inline int clean_stale_write(page_node_t *tmpNode,
					 gc_portal *gc_pl,
					 uint64_t offset)
{

	list_del( &(tmpNode->list) );
	radix_tree_delete(gc_pl, offset);

	return 0;
}

inline int insert_this_write(gc_list *gl, //current_list
                     gc_portal *gp,
					 page_node_t *tmpNode,
					 uint64_t offset,
					 int size, char *buf)
{
	unsigned long index;


	memcpy(tmpNode->buf, buf, size);
	tmpNode->pageAddr = offset;
	tmpNode->size = size;
	//tmpNode->isDirty = 1;
	index = offset;

	if ( radix_tree_insert(gp,index,tmpNode) == 0){

		list_add_tail( &(tmpNode->list), gl );
		return 0;

	}
	else
	    return 1;

}

inline int overwrite_this_write(page_node_t *tmpNode,
						uint64_t offset,
						int size, char *buf)
{

	memcpy(tmpNode->buf, buf, size);
	tmpNode->size = size;

	return 0;
}

void list_tran(gc_list* head)
{
	int i = 0;
	gc_list *p = NULL;
  	Node_num = 0;

	for(p = head->next; p != head; p = p->next)
		++Node_num;

	p_head = (gc_list **)malloc(sizeof(gc_list *) * (Node_num+1));

	for(p = head->next; p!=head; p = p->next)
		p_head[++i] = p;
}

void HeapAdjust(int s,int m)
{
	gc_list  *rc = NULL;
    int j;

    page_node_t *node_m = NULL;
    page_node_t *node_n = NULL;

    rc= p_head[s];

    for(j=2*s;j<=m;j*=2){

		node_m = list_entry(p_head[j], page_node_t, list);
		node_n = list_entry(p_head[j+1], page_node_t, list);

		if( j < m && (node_m->pageAddr < node_n->pageAddr))
			++j;

		node_m = list_entry(p_head[j], page_node_t, list);
		node_n = list_entry(rc, page_node_t, list);

		if(node_m->pageAddr < node_n->pageAddr)
			break;

		p_head[s] = p_head[j];
		s=j;
    }

    p_head[s]=rc;
}

void Sort_dui(void)
{
    gc_list *t = NULL;
    int i;

	for(i = Node_num/2; i>0; --i)
	HeapAdjust(i,Node_num);

	for(i = Node_num; i>1; --i){
     t = p_head[1];
     p_head[1] = p_head[i];
     p_head[i] = t;
     HeapAdjust(1,i-1);
	}

}

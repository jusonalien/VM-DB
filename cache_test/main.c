#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU 1
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include "list.h"
#include "radix_tree.h"
#include "vmdb_cache.h"

#ifdef __Linux__

#endif



/*DEBUG*/

void PrintInfo(char *filename, char *info, int size, int offset)
{
    FILE *fp = NULL;
    fp = fopen(filename, "a+");
    fprintf(fp, "%s %d %d\n", info, size, offset);
    fflush(fp);
    fclose(fp);
}


static int raw_open(char *FileName, int OPEN_MODE);
static int raw_close(int file_fd);
static void error_exit(char *s1, char *s2);
static ssize_t vmdb_cache_pwrite(int fd, void *buf, size_t nbytes, off_t offset);
//inline int flush_heap_all(int file_fd, gc_list *head, gc_portal *this_portal);
/*
inline int flush_heap_all(int file_fd,
			  gc_list **this_list,
			  gc_portal *this_portal);
*/


#define COPYMODE     0644
#define IOGRANT      512 // 512 bytes
#define BUFFER_SIZE  4096


int main(int argc,char **argv)
{
    int in_fd, out_fd,n_chars;
    int offset = 0;
    char buf[BUFFER_SIZE];

    if(argc != 3) {
        fprintf(stderr,"Please input the file name!\n");
        exit(1);
    }

    in_fd = raw_open(argv[1], O_RDWR);

    if ( (out_fd=open( argv[2], O_RDWR | O_CREAT | O_TRUNC/* | O_DIRECT | O_SYNC */)) == -1 ){
        error_exit( "Cannot creat", argv[2]);
    }

    while((n_chars = pread(in_fd, buf, BUFFER_SIZE, offset)) > 0) {
        if(n_chars % IOGRANT == 0) {
            if(vmdb_cache_pwrite(out_fd, buf, BUFFER_SIZE, offset) != n_chars) {
                error_exit("Write error to ", argv[2]);
            }
        } else {
            if(pwrite(out_fd, buf, BUFFER_SIZE, offset) != n_chars) {
                error_exit("Write error to ", argv[2]);
            }
        }
        offset += n_chars;
    }
    raw_close(out_fd);
    close(in_fd);
    return 0;
}


static void error_exit(char *s1, char *s2)
{
    fprintf(stderr,"Error: %s ", s1);
    perror(s2);
    exit(1);
}


static int raw_open(char *FileName, int OPEN_MODE)
{
    INIT_LIST_HEAD(&current_list);
    INIT_RADIX_TREE(&current_portal);
    radix_tree_init();
    page_node_pool = (page_node_t *)malloc(sizeof(page_node_t) * GEARCACHE_UPPER_LIMIT);
    buf_pool = (char *)valloc(IOGRANT * GEARCACHE_UPPER_LIMIT);
    memset(page_node_pool, 0, sizeof(page_node_t) * GEARCACHE_UPPER_LIMIT);

    int fd = open(FileName,OPEN_MODE);
    if(fd == -1) {
        error_exit("Cannot create",FileName);
    }

    return fd;
}


static ssize_t vmdb_cache_pwrite(int fd, void *buf, size_t nbytes, off_t offset)
{
    page_node_t *tmpNode = NULL;
    int cur_offset = 0;
    while(cur_offset < nbytes) {
        tmpNode = search_in_gearcache(offset + cur_offset, &current_portal);
    if(!tmpNode) {
        tmpNode = &page_node_pool[current_pos];
        tmpNode->buf = buf_pool + current_pos*IOGRANT;
        insert_this_write(&current_list, &current_portal, tmpNode,
                          offset + cur_offset, IOGRANT, buf + cur_offset);
        current_pos++;
        if(current_pos == GEARCACHE_UPPER_LIMIT - 1) {
            //list_tran(&current_list);
            //SortHeap();
            flush_heap_all(fd, &current_list, &current_portal);
            current_pos = 0;
        }
    } else {
        PrintInfo("/home/over_write.txt","size and offset",IOGRANT,offset + cur_offset);
        overwrite_this_write(tmpNode, offset + cur_offset, tmpNode->size, buf + cur_offset);
    }
    cur_offset += IOGRANT;
    }
    return cur_offset;
}
static int raw_close(int file_fd)
{
   //list_tran(&current_list);
   //SortHeap();
   flush_heap_all(file_fd, &current_list, &current_portal);
   free(page_node_pool);
   free(buf_pool);
   close(file_fd);
   return 1;
}
/*
 *Simplify the logic of the flush_heap_all
 */
inline int flush_heap_all(int file_fd,
			  gc_list *head, // <- &current_list
			  gc_portal *this_portal)
{
    gc_list *p = NULL;
    page_node_t *node;
    int n;
    //PrintInfo("/home/flush.log","----Start Flush----", 0, 0);
    for(p = head->next; p != head; p = p->next) {
            node = list_entry(p,page_node_t,list);
            lseek(file_fd, node->pageAddr, SEEK_SET);
            n = write(file_fd, node->buf, node->size);
            //n = pwrite(file_fd,node->buf,node->size,node->pageAddr);
        //PrintInfo("/home/flush.log","return_size and pageaddr", n, node->pageAddr);
            clean_stale_write(node, this_portal, node->pageAddr);
    }
    //PrintInfo("/home/flush.log","----Stop Flush----", 0, 0);
	return 0;
}

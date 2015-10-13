#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

gc_list list_head;

typedef struct test_list{
    gc_list list;
    int val;

} test_list;

void scan_list(gc_list *head_list)
{
    test_list *p = NULL;
    gc_list *s = NULL;
    list_for_each_next(s, head_list) {
        p = list_entry(s, test_list, list);
        assert(p);
        printf("%d",p->val);
    }
}

// void scanf_list_entry(test_list)
int main()
{
    int n;

    INIT_LIST_HEAD(&list_head);
    printf("Enter the num of the elem:");
    scanf("%d",&n);
    while(n--) {
        test_list *new_node = (test_list *)malloc(sizeof(test_list));
        assert(new_node);
        printf("enter a integer:");
        scanf("%d",&new_node->val);
        list_add(&new_node->list, &list_head);

    }
    printf("now we output the list: \n");
    scan_list(&list_head);
    return 0;
}


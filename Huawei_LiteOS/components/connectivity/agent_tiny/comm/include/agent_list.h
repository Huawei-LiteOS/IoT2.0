#ifndef _ATINY_LIST_H 

#define _ATINY_LIST_H 


#define ATINY_INLINE static inline


//#include "los_typedef.h" 




#ifdef __cplusplus 
#if __cplusplus 

extern "C" { 

#endif /* __cplusplus */ 

#endif /* __cplusplus */ 


/** 

 *@ingroup atiny_list 

 *Structure of a node in a doubly linked list. 

 */ 

typedef struct atiny_dl_list 
{ 
    struct atiny_dl_list *prev;            /**< Current node's pointer to the previous node*/ 
    struct atiny_dl_list *next;            /**< Current node's pointer to the next node*/ 

} atiny_dl_list; 


ATINY_INLINE void atiny_list_init(atiny_dl_list *list) 
{ 
    list->next = list; 
    list->prev = list; 
} 




#define ATINY_DL_LIST_FIRST(object) ((object)->next) 


ATINY_INLINE void atiny_list_add(atiny_dl_list *list, atiny_dl_list *node) 
{ 
    node->next = list->next; 
    node->prev = list; 
    list->next->prev = node; 
    list->next = node; 
} 


ATINY_INLINE void atiny_list_insert_tail(atiny_dl_list *list, atiny_dl_list *node) 
{ 
    atiny_list_add(list->prev, node); 
} 

ATINY_INLINE atiny_dl_list * atiny_list_get_head(atiny_dl_list *header) 
{ 
    return header->next;
} 

ATINY_INLINE void atiny_list_delete(atiny_dl_list *node) 
{ 
    node->next->prev = node->prev; 
    node->prev->next = node->next; 
    node->next = (atiny_dl_list *)NULL; 
    node->prev = (atiny_dl_list *)NULL; 
} 

ATINY_INLINE int atiny_list_empty(atiny_dl_list *node) 
{ 
    return (node->next == node); 
} 


#define OFFSET_OF_FIELD(type, field)    ((UINT32)&(((type *)0)->field)) 
#define ATINY_OFF_SET_OF(type, member) ((long)&((type *)0)->member)   /*lint -e(413) */ 


#define ATINY_DL_LIST_ENTRY(item, type, member)\
    ((type *)((char *)item - ATINY_OFF_SET_OF(type, member)))\


#define ATINY_DL_LIST_FOR_EACH_ENTRY(item, list, type, member)\
    for (item = ATINY_DL_LIST_ENTRY((list)->next, type, member);\
        &item->member != (list);\
        item = ATINY_DL_LIST_ENTRY(item->member.next, type, member)) 


#define ATINY_DL_LIST_FOR_EACH_ENTRY_SAFE(item, next, list, type, member)\
    for (item = ATINY_DL_LIST_ENTRY((list)->next, type, member),\
        next = ATINY_DL_LIST_ENTRY(item->member->next, type, member);\
        &item->member != (list);\
        item = next, item = ATINY_DL_LIST_ENTRY(item->member.next, type, member)) 


ATINY_INLINE void ATINY_ListDel(atiny_dl_list *pstPrevNode, atiny_dl_list *pstNextNode) 
{ 
    pstNextNode->prev = pstPrevNode; 
    pstPrevNode->next = pstNextNode; 
} 


ATINY_INLINE void ATINY_ListDelInit(atiny_dl_list *pstList) 
{ 
    ATINY_ListDel(pstList->prev, pstList->next); 
    atiny_list_init(pstList); 
} 




#define ATINY_DL_LIST_FOR_EACH(item, list)\
    for ((item) = (list)->next;\
        (item) != (list);\
        (item) = (item)->next) 


#define ATINY_DL_LIST_FOR_EACH_SAFE(item, next, list)\
    for (item = (list)->next, next = item->next; item != (list);\
        item = next, next = item->next) 


#define ATINY_DL_LIST_HEAD(list)\
            atiny_dl_list list = { &(list), &(list) } 

#ifdef __cplusplus 

#if __cplusplus 

} 

#endif /* __cplusplus */ 
#endif /* __cplusplus */ 
#endif /* _ATINY_LIST_H */

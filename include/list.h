#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

struct list_head {
	struct list_head *next, *prev;
};

/**
 * 这里是双向链表，通过宏能够插入任意类型的数据
 * C语言的宏，是将参数替换成传入的值放回源代码中
 * 这里很好地展示了如何在C语言中通过使用宏将链表去数据化，只留下结构和功能
 * 这里可以和我在PPTD算法里所用的方法进行对比
 */

#define list_empty(list) ((list)->next == (list))

/**
 * 返回一个表达式，该表达式返回指向（ptr所指向的元素）的指针
 * @param ptr       想要得到的元素位置的指针
 * @param type      元素类型
 * @param member    链表元素内部的用于遍历的链表指针元素名称
 */
#define list_entry(ptr, type, member) \
		(type *)((char *)ptr - offsetof(type, member))

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * 返回一个for循环，可用于遍历以head开头的链表
 *
 * @param pos 		从哪个元素开始，如果是从头开始遍历，则传入空元素即可，通过此参数使用链表内的每一个元素
 * @param head 		待遍历链表的表头，表头数据域为空
 * @param member    链表元素内部的用于遍历的链表指针元素名称
 */
#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
        	&pos->member != (head); \
        	pos = list_entry(pos->member.next, typeof(*pos), member)) 

#define list_for_each_safe(pos, q, head) \
	for (pos = (head)->next, q = pos->next; pos != (head); \
			pos = q, q = pos->next)

#define list_for_each_entry_safe(pos, q, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member), \
	        q = list_entry(pos->member.next, typeof(*pos), member); \
	        &pos->member != (head); \
	        pos = q, q = list_entry(pos->member.next, typeof(*q), member))

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

static inline void init_list_head(struct list_head *list)
{
	list->next = list->prev = list;
}

static inline void list_insert(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	prev->next = new;
	new->next = next;
	new->prev = prev;
}

static inline void list_add_head(struct list_head *new, struct list_head *head)
{
	list_insert(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	list_insert(new, head->prev, head);
}

static inline void list_delete_entry(struct list_head *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

#define delete_list(list, type, member) \
do { \
	struct list_head *p = (list)->next, \
					 *t = NULL; \
	while (p != (list)) { \
		t = p; \
		p = p->next; \
		free(list_entry(t, type, member)); \
		list_delete_entry(t); \
	} \
} while (0)

#endif

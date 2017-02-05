/*
**  CDXLPlay (C) 2009 Fredrik Wikstrom
**/

#ifndef LISTS_H
#define LISTS_H

#include <stdint.h>

struct node {
	struct node *succ;
	struct node *pred;
};

struct list {
	struct node *head;
	struct node *tail;
	struct node *tailpred;
};

static inline void init_list(struct list *list) {
	list->head = (struct node *)&list->tail;
	list->tail = NULL;
	list->tailpred = (struct node *)&list->head;
}

static inline int list_is_empty(struct list *list) {
	return (list->head == (struct node *)&list->tail);
}

static inline void add_head(struct list *list, struct node *node) {
	node->succ = list->head;
	node->pred = (struct node *)&list->head;
	node->succ->pred = node;
	node->pred->succ = node;
}

static inline void add_tail(struct list *list, struct node *node) {
	node->succ = (struct node *)&list->tail;
	node->pred = list->tailpred;
	node->succ->pred = node;
	node->pred->succ = node;
}

static inline struct node *rem_node(struct node *node) {
	if (node && node->succ && node->pred) {
		node->succ->pred = node->pred;
		node->pred->succ = node->succ;
		node->succ = NULL;
		node->pred = NULL;
		return node;
	}
	return NULL;
}

static inline struct node *rem_head(struct list *list) {
	return rem_node(list->head);
}

static inline struct node *rem_tail(struct list *list) {
	return rem_node(list->tail);
}

#endif

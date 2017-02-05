#include <SDL/SDL.h>
#include <exec/exec.h>
#include <proto/exec.h>
#ifdef __AMIGAOS4__
#include <inline4/exec.h>
#endif
#include "lists.h"

typedef struct SDL_cond_node {
	struct node node;
	struct Task *task;
	int32_t signal;
} SDL_cond_node;

struct SDL_cond {
	struct list list;
};

SDL_cond *SDL_CreateCond(void) {
	SDL_cond *cond;
	cond = malloc(sizeof(*cond));
	if (cond) {
		init_list(&cond->list);
	}
	return cond;
}

void SDL_DestroyCond(SDL_cond *cond) {
	if (cond) {
		free(cond);
	}
}

int SDL_CondWait(SDL_cond *cond, SDL_mutex *mut) {
	SDL_cond_node node;
	node.task = FindTask(NULL);
	node.signal = AllocSignal(-1);
	add_tail(&cond->list, &node.node);
	SDL_UnlockMutex(mut);
	Wait(1 << node.signal);
	FreeSignal(node.signal);
	SDL_LockMutex(mut);
	return 0;
}

int SDL_CondSignal(SDL_cond *cond) {
	SDL_cond_node *node;
	node = (SDL_cond_node *)rem_head(&cond->list);
	if (node) {
		Signal(node->task, 1 << node->signal);
		return 0;
	}
	return -1;
}

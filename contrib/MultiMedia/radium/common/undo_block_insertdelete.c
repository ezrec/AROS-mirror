

#include "nsmtracker.h"
#include "undo.h"
#include "block_insert_proc.h"
#include "block_delete_proc.h"
#include "OS_Bs_edit_proc.h"
#include "list_proc.h"

#include "undo_block_insertdelete_proc.h"

extern struct Root *root;

struct Undo_Block_InsertDelete{
	NInt blockpos;
	struct WBlocks *wblock;
};



void *Undo_Do_Block_Delete(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	struct WTracks *wtrack,
	int realline,
	void *pointer
);

void *Undo_Do_Block_Insert(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	struct WTracks *wtrack,
	int realline,
	void *pointer
);


void Undo_Block_Insert(
	NInt blockpos
){
	struct Tracker_Windows *window=root->song->tracker_windows;
	struct WBlocks *wblock=window->wblock;

	struct Undo_Block_InsertDelete *ubi=talloc_atomic(sizeof(struct Undo_Block_InsertDelete));
	ubi->blockpos=blockpos;
	ubi->wblock=NULL;

	Undo_New(
		window->l.num,
		wblock->l.num,
		window->curr_track,
		wblock->curr_realline,
		ubi,
		Undo_Do_Block_Insert
	);
}


void Undo_Block_Delete(
	NInt blockpos
){
	struct Tracker_Windows *window=root->song->tracker_windows;
	struct WBlocks *wblock=window->wblock;

	struct Undo_Block_InsertDelete *ubi=talloc_atomic(sizeof(struct Undo_Block_InsertDelete));
	ubi->blockpos=blockpos;
	ubi->wblock=(struct WBlocks *)ListFindElement1(&window->wblocks->l,blockpos);

	Undo_New(
		window->l.num,
		wblock->l.num,
		window->curr_track,
		wblock->curr_realline,
		ubi,
		Undo_Do_Block_Delete
	);
}


void *Undo_Do_Block_Insert(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	struct WTracks *wtrack,
	int realline,
	void *pointer
){
	struct Undo_Block_InsertDelete *ubi=(struct Undo_Block_InsertDelete *)pointer;

	if(ubi->wblock!=NULL){
		return Undo_Do_Block_Delete(window,wblock,wtrack,realline,pointer);
	}

	fprintf(stderr,"ubi->blockpos: %d\n",(int)ubi->blockpos);
	ubi->wblock=(struct WBlocks *)ListFindElement1(&window->wblocks->l,ubi->blockpos);
	DeleteBlock(ubi->blockpos);

	BS_UpdateBlockList();
	BS_UpdatePlayList();

	return ubi;
}

void *Undo_Do_Block_Delete(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	struct WTracks *wtrack,
	int realline,
	void *pointer
){
	struct Undo_Block_InsertDelete *ubi=(struct Undo_Block_InsertDelete *)pointer;

	if(ubi->wblock==NULL){
		return Undo_Do_Block_Insert(window,wblock,wtrack,realline,pointer);
	}

	InsertBlock_IncBlockNums(ubi->blockpos);
	ListAddElement1(&root->song->blocks,&ubi->wblock->block->l);
	ListAddElement1(&window->wblocks,&ubi->wblock->l);

	BS_UpdateBlockList();
	BS_UpdatePlayList();

	ubi->wblock=NULL;
	return ubi;
}


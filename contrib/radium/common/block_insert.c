


#include "nsmtracker.h"
#include "OS_Bs_edit_proc.h"
#include "blocks_proc.h"
#include "wblocks_proc.h"
#include "list_proc.h"
#include "undo_block_insertdelete_proc.h"
#include "player_proc.h"

#include "block_insert_proc.h"


extern struct Root *root;

void InsertBlock_IncBlockNums(
	NInt blockpos
){
	struct Tracker_Windows *window=root->song->tracker_windows;
	struct WBlocks *wblock;
	struct Blocks *block=ListFindElement1_r0(&root->song->blocks->l,blockpos);

	while(block!=NULL){
		block->l.num++;
		block=NextBlock(block);
	}
	root->song->num_blocks++;

	while(window!=NULL){
		wblock=ListFindElement1_r0(&window->wblocks->l,blockpos);
		while(wblock!=NULL){
			wblock->l.num++;
			wblock=NextWBlock(wblock);
		}
		window=NextWindow(window);
	}
}

void InsertBlock(
	NInt blockpos,
	NInt num_tracks,
	int num_lines,
	char *name
){
	struct Tracker_Windows *window=root->song->tracker_windows;
	struct WBlocks *wblock;

	struct Blocks *block;

	InsertBlock_IncBlockNums(blockpos);

	block=talloc(sizeof(struct Blocks));

	block->l.num=blockpos;
	NewBlock(block,num_tracks,num_lines,name);

	while(window!=NULL){
		wblock=talloc(sizeof(struct WBlocks));
		wblock->l.num=blockpos;

		NewWBlock(window,wblock,block);

		window=NextWindow(window);
	}

}


void InsertBlock_CurrPos(
	struct Tracker_Windows *window
){
	struct WBlocks *wblock=window->wblock;
	NInt blockpos=wblock->l.num;

	PlayStop();

	Undo_Block_Insert(blockpos);

	InsertBlock(blockpos,wblock->block->num_tracks,wblock->block->num_lines,"NN");

	SelectWBlock(window,(struct WBlocks *)ListFindElement1(&window->wblocks->l,blockpos));

	BS_UpdateBlockList();
	BS_UpdatePlayList();
}


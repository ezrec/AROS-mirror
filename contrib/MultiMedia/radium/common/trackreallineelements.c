/* Copyright 2000 Kjetil S. Matheussen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */







#include "nsmtracker.h"
#include <gc.h>
#include "trackreallines_proc.h"
#include "fxlines_proc.h"
#include "temponodes_proc.h"


#include "trackreallineelements_proc.h"

#define NUM_TREINHOLDER 40000

bool collectTREgarbage=false;			// Variable for main.


struct TREelementHolder{
	struct TREelementHolder *next;
	int notused;
	struct TrackReallineElements tre[NUM_TREINHOLDER];
};

struct TrackReallineElements *nextfreeelement=NULL;

extern size_t allocated;


void ReturnRTEelement(struct TrackReallineElements *tre){
	tre->next=nextfreeelement;
//	tre->pointer=NULL;
	nextfreeelement=tre;
}


void FreeAllRTEelements_fromroot(
	struct TrackReallineElements **to
){
	struct TrackReallineElements *rte=*to;
	struct TrackReallineElements *temp;

	if(rte==NULL) return;

	for(;rte!=NULL;temp=rte,rte=rte->next);

	temp->next=nextfreeelement;
	nextfreeelement=*to;
	*to=NULL;

#if 0
	while(rte!=NULL){
		temp=rte->next;
		ReturnRTEelement(rte);
		rte=temp;
	}
	*to=NULL;
#endif
}

#if 0
void FreeAllRTEelements_fromroot(
	struct TrackReallineElements **to
){
	struct TrackReallineElements *rte= *to;
	struct TrackReallineElements *temp;

	while(rte!=NULL){
		temp=rte->next;
		ReturnRTEelement(rte);
		rte=temp;
	}
	*to=NULL;
}
#endif



void SetNextsInATreElementHolder(struct TREelementHolder *treholder){
	struct TrackReallineElements *end=&treholder->tre[NUM_TREINHOLDER-1];

	struct TrackReallineElements *tre=&treholder->tre[0];
	struct TrackReallineElements *next=&treholder->tre[1];

	while(next<=end){
		tre->next=next;
		tre=next;
		next=next+1;
	}

	end->next=NULL;
}

bool nomoretreelementholders=false;

bool AllocateNewTREelementHolder(void){
	struct TREelementHolder *treholder;

	fprintf(stderr,"\n\n\nALLOCATING NEW TREELEMENTHOLDER\n\n\n\n");

	treholder=tracker_alloc_clean(sizeof(struct TREelementHolder),GC_malloc_atomic_uncollectable);
	if(treholder==NULL){
		RError("Warning. Could not allocate new TREElementHolder.\n");
		nomoretreelementholders=true;
		return false;
	}

	SetNextsInATreElementHolder(treholder);

	nextfreeelement=&treholder->tre[0];

	fprintf(stderr,"Success\n");
	return true;
}



extern struct Root *root;

bool FreeASpesifiedWBlockTREelement(
	struct Tracker_Windows *window,
	struct WBlocks *wblock
){
	struct WTracks *wtrack;

	if( ! wblock->isgfxdatahere ) return false;

	FreeAllWTempoNodes(wblock);

	tfree(wblock->wtemponodes);
	wblock->wtemponodes=NULL;

	wtrack=wblock->wtracks;
	while(wtrack!=NULL){
		FreeAllRTEelements(wblock,wtrack);
		tfree(wtrack->trackreallines);
		wtrack->trackreallines=NULL;

		FreeAllWFXnodes(wblock,wtrack);
		tfree(wtrack->wfxnodes);
		wtrack->wfxnodes=NULL;

		wtrack=NextWTrack(wtrack);
	}
	wblock->isgfxdatahere=false;

	return true;
}

bool FreeANotShowedWBlockTREelement(void){
	struct Tracker_Windows *window=root->song->tracker_windows;
	struct WBlocks *wblock;

	while(window!=NULL){

		wblock=window->wblocks;

		while(wblock!=NULL){
			if(wblock!=window->wblock){
				if( FreeASpesifiedWBlockTREelement(window,wblock) ) return true;
			}
			wblock=NextWBlock(wblock);
		}

		window=NextWindow(window);
	}

	return false;
}




struct TrackReallineElements *TRE_GetTREelementHard(void){
	struct TrackReallineElements *temp=nextfreeelement;

	if(temp==NULL){
	  printf("jaaaa\n");
		if(FreeANotShowedWBlockTREelement()){
			temp=TRE_GetTREelementHard();
		}else{
			if(nomoretreelementholders){
				temp=talloc_atomic_uncollectable(sizeof(struct TrackReallineElements));
			}else{
				if( ! AllocateNewTREelementHolder()){
					temp=talloc_atomic_uncollectable(sizeof(struct TrackReallineElements));
				}else{
					temp=TRE_GetTREelementHard();
				}
			}
		}
	}else{
		nextfreeelement=temp->next;
	}

	temp->next=NULL;			//This should not be necesarry. There is a bug somewhere else in the program. (probably removed now)
	return temp;

}












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
#include "common_proc.h"
#include "area_proc.h"
#include "mouse_vellinenode_proc.h"
#include "mouse_vellinestart_proc.h"
#include "mouse_vellineend_proc.h"
#include "mouse_fxnode_proc.h"
#include "fxlines_proc.h"
#include "gfx_wtracks_proc.h"
#include "gfx_subtrack_proc.h"
#include "trackreallines_proc.h"
#include "velocities_proc.h"
#include "undo_notes_proc.h"
#include "undo_fxs_proc.h"
#include "gfx_window_title_proc.h"
#include "player_proc.h"

#include "mouse_fxarea_proc.h"



void SetMouseActionVelocities(
	struct Tracker_Windows *window,
	struct MouseAction *action,
	struct WTracks *wtrack,
	int x,int y,
	int realline,
	int click
){
	struct WBlocks *wblock=window->wblock;
	struct TrackReallineElements *element;

	element=wtrack->trackreallines[realline].trackreallineelements;

	while(element!=NULL){
		if(element->type==TRE_VELLINENODE || element->type==TRE_VELLINESTART || element->type==TRE_VELLINEEND){
			if(insideNArea(
				x,y,
				wtrack->fxarea.x,Common_oldGetReallineY1Pos(window,wblock,realline),
				element->x1,element->y1,
				element->x2,element->y2
			)){
				switch(element->type){
					case TRE_VELLINENODE:
						SetMouseActionVelline(
							window,action,wtrack,
							(struct Velocities *)element->pointer,
							realline,
							element->subtype,
							x,y,click
						);
						break;
					case TRE_VELLINESTART:
						SetMouseActionVellineStart(
							window,action,wtrack,
							(struct Notes *)element->pointer,
							realline,
							element->subtype,
							x,y,click
						);
						return;
					case TRE_VELLINEEND:
						SetMouseActionVellineEnd(
							window,action,wtrack,
							(struct Notes *)element->pointer,
							realline,
							element->subtype,
							x,y,click
						);
						return;
				}
			}
		}
		element=element->next;
	}
}


void SetMouseActionFXNodes(
	struct Tracker_Windows *window,
	struct MouseAction *action,
	struct WTracks *wtrack,
	int x,int y,
	int realline,
	int click
){
	struct WBlocks *wblock=window->wblock;
	WFXNodes *wfxnode;
	wfxnode=wtrack->wfxnodes[realline];

	while(wfxnode!=NULL){
		if(wfxnode->type==TRE_FXNODE){
			if(insideNArea(
				x,y,
				wtrack->fxarea.x,Common_oldGetReallineY1Pos(window,wblock,realline),
				wfxnode->x1,wfxnode->y1,
				wfxnode->x2,wfxnode->y2
			)){
				SetMouseActionFXNode(
					window,action,wtrack,
					wfxnode,
					click
				);
				return;
			}
		}
		wfxnode=wfxnode->next;
	}
}

void SetMouseActionFXarea(
	struct Tracker_Windows *window,
	struct MouseAction *action,
	struct WTracks *wtrack,
	int x,int y,
	int click
){
	struct WBlocks *wblock=window->wblock;
	WFXNodes *wfxnode;
	struct TrackReallineElements *element;
	WFXNodes *wfxnodemin=NULL;
	struct FX *fx;
	int subtrack;
	int mintype=0;
	int tempdist;
	int mindist= -1;
	int realline;
	Place place;

	action->action=NOACTION;

	realline=GetReallineAndPlaceFromY(window,wblock,y,&place,NULL,NULL);
	if(realline<0) return;

	SetMouseActionVelocities(window,action,wtrack,x,y,realline,click);
	if(
		action->action==TRE_VELLINESTART ||
		action->action==TRE_VELLINEEND
	)return;

	SetMouseActionFXNodes(window,action,wtrack,x,y,realline,click);

	if(action->action!=NOACTION || 0==click) return;

	y-=Common_oldGetReallineY1Pos(window,wblock,realline);
	x-=wtrack->fxarea.x;

	wfxnode=wtrack->wfxnodes[realline];

	while(wfxnode!=NULL){
		if(wfxnode->type==TRE_FXLINE){
			if(wfxnode->y1<=y && wfxnode->y2>=y){
				if(wfxnode->y2!=wfxnode->y1){
					tempdist=x-((y-wfxnode->y1)*(wfxnode->x2-wfxnode->x1)/(wfxnode->y2-wfxnode->y1)+wfxnode->x1);
					tempdist=abs(tempdist);
					if(tempdist<mindist || mindist==-1){
						mindist=tempdist;
						wfxnodemin=wfxnode;
					}
				}
			}
		}
		wfxnode=wfxnode->next;
	}

	element=wtrack->trackreallines[realline].trackreallineelements;
	subtrack=GetSubTrack(wtrack,x);

	if(subtrack>=0){
		while(element!=NULL){
			if(element->type==TRE_VELLINE){
				if(element->subtype==subtrack && element->y1<=y && element->y2>=y){
					if(element->y2!=element->y1){
						tempdist=x-((y-element->y1)*(element->x2-element->x1)/(element->y2-element->y1)+element->x1);
						tempdist=abs(tempdist);
						if(tempdist<mindist || mindist==-1){
							mindist=tempdist;
							mintype=1;
						}
					}
				}
			}
			element=element->next;
		}
	}

	if(mindist==-1) return;

	if(0==mintype){
		Undo_FXs(window,wblock->block,wtrack->track,window->wblock->curr_realline);

		fx=((struct FXs *)(wfxnodemin->pointer))->fx;

		PlayStop();

		AddFXNodeLine(
			window,
			wblock,
			wtrack,
			fx,
			((fx->max-fx->min)*x/(wtrack->fxwidth-1))+fx->min,
			&place
		);

		GFX_SetChangeInt(window,wblock,fx->name,((fx->max-fx->min)*x/(wtrack->fxwidth-1))+fx->min);
		GFX_DrawWindowTitle(window,wblock);

		UpdateFXNodeLines(window,wblock,wtrack);

//		ClearTrack(window,wblock,wtrack,wblock->top_realline,wblock->bot_realline);

		UpdateWTrack(window,wblock,wtrack,wblock->top_realline,wblock->bot_realline);

		y+=Common_oldGetReallineY1Pos(window,wblock,realline);
		x+=wtrack->fxarea.x;

		SetMouseActionFXNodes(window,action,wtrack,x,y,realline,0);

	}else{
		PlayStop();

		Undo_Notes(window,wblock->block,wtrack->track,window->wblock->curr_realline);

		AddVelocity(
			window,
			wblock,
			wtrack,
			subtrack,
			(((*wtrack->track->instrument->getMaxVelocity)(wtrack->track))*(x-GetRelXSubTrack1(wtrack,subtrack))/(GetSubTrackWidth(wtrack,subtrack))),
			&place,
			realline
		);

		GFX_SetChangeInt(window,wblock,"Velocity",(((*wtrack->track->instrument->getMaxVelocity)(wtrack->track))*(x-GetRelXSubTrack1(wtrack,subtrack))/(GetSubTrackWidth(wtrack,subtrack))));
		GFX_DrawWindowTitle(window,wblock);

		UpdateTrackReallines(window,wblock,wtrack);
//		ClearTrack(window,wblock,wtrack,wblock->top_realline,wblock->bot_realline);
		UpdateWTrack(window,wblock,wtrack,wblock->top_realline,wblock->bot_realline);

		y+=Common_oldGetReallineY1Pos(window,wblock,realline);
		x+=wtrack->fxarea.x;

		SetMouseActionVelocities(window,action,wtrack,x,y,realline,0);

	}

}













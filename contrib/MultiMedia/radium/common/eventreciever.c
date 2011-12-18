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
#include "windows_proc.h"
#include "mouse_proc.h"
#include "notes_proc.h"
#include "reallines_proc.h"
#include "cursor_proc.h"
#include "cursor_updown_proc.h"
#include "velocities_proc.h"
#include "tempos_proc.h"
#include "LPB_proc.h"
#include "temponodes_proc.h"
#include "visual_proc.h"
#include "fxlines_proc.h"
#include "wblocks_proc.h"
#include "player_proc.h"
#include "patch_proc.h"
#include "clipboard_range_proc.h"
#include "clipboard_range_copy_proc.h"
#include "clipboard_range_paste_proc.h"
#include "clipboard_range_cut_proc.h"
#include "transpose_proc.h"
#include "disk_save_proc.h"
#include "disk_load_proc.h"
#include "clipboard_track_copy_proc.h"
#include "clipboard_track_paste_proc.h"
#include "clipboard_track_cut_proc.h"
#include "lines_proc.h"
#include "reallines_insert_proc.h"
#include "block_properties_proc.h"
#include "font_proc.h"
#include "clipboard_block_copy_proc.h"
#include "clipboard_block_paste_proc.h"
#include "undo.h"
#include "track_insert_proc.h"
#include "quantitize_proc.h"
#include "gfx_upperleft_proc.h"
#include "wtracks_proc.h"
#include "window_config_proc.h"
#include "block_insert_proc.h"
#include "block_delete_proc.h"
#include "track_onoff_proc.h"
#include "blts_proc.h"
#include "new/backwards_proc.h"
#include "new/invert_proc.h"

#ifdef AMIAROS
#include "Amiga_colors_proc.h"
#endif

#include <string.h>

#include "eventreciever_proc.h"


extern struct Root *root;

extern int num_undos;
extern int max_num_undos;

bool Quit(struct Tracker_Windows *window){

	char temp[200];
	char *ret=NULL;

	if(num_undos>0){
		sprintf(
			temp,
			"%s%d change%s has been made to file.\nAre you shure? (yes/no) >"
			,num_undos>=max_num_undos-1?"At least":"",
			num_undos,
			num_undos>1?"s":""
		);
		while(
			ret==NULL || (
				strcmp("yes",ret) &&
				strcmp("no",ret)
			)
		){
			ret=GFX_GetString(
				window,
				(ReqType)0,
				temp
			);
		}
		if(!strcmp("no",ret)) return false;
	}

	return true;
}

static __inline int ER_getNoteNum(int subid){
	static int s2n[EVENT_MAX+1]={0};
	static bool initialized=false;

	if(initialized==false){
     s2n[EVENT_Z]=12;
     s2n[EVENT_S]= 13;
     s2n[EVENT_X]= 14;
     s2n[EVENT_D]= 15;
     s2n[EVENT_C]= 16;
     s2n[EVENT_V]= 17;
     s2n[EVENT_G]= 18;
     s2n[EVENT_B]= 19;
     s2n[EVENT_H]= 20;
     s2n[EVENT_N]= 21;
     s2n[EVENT_J]= 22;
     s2n[EVENT_M]= 23;
     s2n[EVENT_MR1]= 24;
     s2n[EVENT_L]= 25;
     s2n[EVENT_MR2]= 26;
     s2n[EVENT_LR1]= 27;
     s2n[EVENT_MR3]= 28;
     s2n[EVENT_LR3]= 30;
     s2n[EVENT_Q]= 24;
     s2n[EVENT_2]= 25;
     s2n[EVENT_W]= 26;
     s2n[EVENT_3]= 27;
     s2n[EVENT_E]= 28;
     s2n[EVENT_R]= 29;
     s2n[EVENT_5]= 30;
     s2n[EVENT_T]= 31;
     s2n[EVENT_6]= 32;
     s2n[EVENT_Y]= 33;
     s2n[EVENT_7]= 34;
     s2n[EVENT_U]= 35;
     s2n[EVENT_I]= 36;
     s2n[EVENT_9]= 37;
     s2n[EVENT_O]= 38;
     s2n[EVENT_0]= 39;
     s2n[EVENT_P]= 40;
     s2n[EVENT_PR1]= 41;
     s2n[EVENT_0R2]= 42;
     s2n[EVENT_PR2]= 43;
     s2n[EVENT_0R3]= 44;
     initialized=true;
	}

	return s2n[subid]==0?0:s2n[subid]+root->keyoct-12;
}



/* TODO. Insert ER_getNoteNum functions in the case TR_KEYBOARDDOWN switches. */

int EventTreater(struct TEvent *in_tevent,struct Tracker_Windows *window){
	uint32_t a=in_tevent->keyswitch;
	int notenum;
	int temp;

	switch(in_tevent->ID){
		case TR_KEYBOARDUP:
			notenum=ER_getNoteNum(in_tevent->SubID);
			if(notenum>0 && window->curr_track>=0 && (OnlyLeftShift(a) || NoSwitch(a))) PATCH_stopNoteCurrPos(window,notenum);
			break;
		case TR_KEYBOARD:
			notenum=ER_getNoteNum(in_tevent->SubID);
			if(notenum>0 && window->curr_track>=0 && (OnlyLeftShift(a) || NoSwitch(a))){
				PATCH_playNoteCurrPos(window,notenum);

				InsertNoteCurrPos(window,notenum,(int)(a&EVENT_LEFTSHIFT));

			}

			switch(in_tevent->SubID){
				case EVENT_F1:
					root->keyoct=0;
					GFX_UpdateKeyOctave(window,window->wblock);
					break;
				case EVENT_F2:
					root->keyoct=24;
					GFX_UpdateKeyOctave(window,window->wblock);
					break;
				case EVENT_F3:
					root->keyoct=48;
					GFX_UpdateKeyOctave(window,window->wblock);
					break;
				case EVENT_F4:
					root->keyoct=72;
					GFX_UpdateKeyOctave(window,window->wblock);
					break;
				case EVENT_F5:
					root->keyoct=96;
					GFX_UpdateKeyOctave(window,window->wblock);
					break;
				case EVENT_TAB:
					if(AnyShift(a)){
						root->keyoct=max(0,root->keyoct-24);
					}else{
						if(root->keyoct<128-24) root->keyoct+=24;
					}
					GFX_UpdateKeyOctave(window,window->wblock);
					break;
				case EVENT_DOWNARROW:
					if(NoSwitch(a)) ScrollEditorDown(window,1);
					if(LeftShift(a)){Zoom(window,window->wblock,1);break;}
					if(RightShift(a)){
						ScrollEditorDown(window,10);
						break;
					}
					if(RightAlt(a)){
						ScrollEditorNextNote(window);
					}
					break;
				case EVENT_UPARROW:
					if(NoSwitch(a)) ScrollEditorUp(window,1);
					if(LeftShift(a)){Zoom(window,window->wblock,-1);break;}
					if(RightShift(a)){
						ScrollEditorUp(window,10);
						break;
					}
					if(RightAlt(a)){
						ScrollEditorPrevNote(window);
					}
					break;
				case EVENT_RIGHTARROW:
					if(NoSwitch(a)){
						CursorRight_CurrPos(window);
						break;
					}
					if(RightAlt(a)){
						if(RightShift(a)){
							IncreaseVelocityCurrPos(window,10);
						}else{
							CursorNextTrack_CurrPos(window);
						}
						break;
					}
					if(RightShift(a)) IncreaseVelocityCurrPos(window,1);
					break;
				case EVENT_LEFTARROW:
					if(NoSwitch(a)){
						CursorLeft_CurrPos(window);
						break;
					}
					if(RightAlt(a)){
						if(RightShift(a)){
							IncreaseVelocityCurrPos(window,-10);
						}else{
							CursorPrevTrack_CurrPos(window);
						}
						break;
					}
					if(RightShift(a)) IncreaseVelocityCurrPos(window,-1);
					break;
				case EVENT_BACKSPACE:
					if(LeftAlt(a)){
						InsertRealLines_CurrPos(window,1);
					}else{
						InsertRealLines_CurrPos(window,-1);
					}
					break;
//				case EVENT_F1:
//					OpenTrackerWindow(20,20,400,300);
//					break;
				case EVENT_1L1:
					ExpandLineCurrPos(window,2);
					break;
				case EVENT_1:
					UnexpandCurrPos(window);
					break;
				case EVENT_4:
//					AddFXNodeLineCurrPos(window);
					break;
				case EVENT_ESC:
					if(LeftCtrl(a)){
						root->scrollplayonoff=root->scrollplayonoff?false:true;
					}else{
						if(LeftAlt(a)){
							window->playalong=window->playalong?false:true;
						}else{
							root->editonoff=root->editonoff?false:true;
						}
					}
					break;
				case EVENT_DEL:
					switch(window->curr_track){
						case TEMPOTRACK:
							RemoveTemposCurrPos(window);
							break;
						case LPBTRACK:
							RemoveLPBsCurrPos(window);
							break;
						case TEMPONODETRACK:
							RemoveAllTempoNodesOnReallineCurrPos(window);
							break;
						default:
							if(window->curr_track_sub>=0) StopVelocityCurrPos(window,0);
							else RemoveNoteCurrPos(window);
					}
					break;
				case EVENT_RETURN:
					if(LeftShift(a)){
						InsertLines_CurrPos(window);
					}else{
						switch(window->curr_track){
							case TEMPOTRACK:
								SetTempoCurrPos(window);
								break;
							case LPBTRACK:
								SetLPBCurrPos(window);
								break;
							case TEMPONODETRACK:
								AddTempoNodeCurrPos(window,(float) -0.0f);
								break;
							default:
								if(window->curr_track>=0 && window->curr_track_sub>=0) AddVelocityCurrPos(window);
								break;
						}
					}
					break;
				case EVENT_HELP:
					SelectPatch(window,window->wblock->wtrack->track);
					break;
				case EVENT_F6:
					ScrollEditorToPercentLine_CurrPos(window,0);
					break;
				case EVENT_F7:
					ScrollEditorToPercentLine_CurrPos(window,25);
					break;
				case EVENT_F8:
					ScrollEditorToPercentLine_CurrPos(window,50);
					break;
				case EVENT_F9:
					ScrollEditorToPercentLine_CurrPos(window,75);
					break;
				case EVENT_F10:
					ScrollEditorToPercentLine_CurrPos(window,100);
					break;
				case EVENT_SPACE:
					if(RightExtra(a)){
						PlayBlockFromStart(window);
					}else{
						if(RightAlt(a)){
							if(RightShift(a)){
								PlaySongFromStart(window);
							}else{
								PlayBlockCurrPos(window);
							}
						}else{
							if(RightShift(a)){
								PlaySongCurrPos(window);
							}else{
								PlayStop();
							}
						}
					}
					break;
				case EVENT_A:
					if(RightShift(a)){
						SelectPrevPlaylistWBlock(window);
					}else{
						SelectPrevWBlock(window);
					}
					break;
				case EVENT_F:
					if(RightShift(a)){
						SelectNextPlaylistWBlock(window);
					}else{
						if(LeftCtrl(a)){
							AppendWBlock(window);
						}else{
							SelectNextWBlock(window);
						}
					}
					break;
				case EVENT_Z:
					if(LeftExtra(a)){
						CancelRange_CurrPos(window);
					}else{
					}
					break;
				case EVENT_S:
					if(LeftAlt(a)){
						SwapTrack_CurrPos(window);
						break;
					}
					break;
				case EVENT_X:
					if(LeftExtra(a)){
						CutRange_CurrPos(window);
					}else{
						if(LeftAlt(a)){
							CB_CutTrack_CurrPos(window);
						}else{
						}
					}
					break;
				case EVENT_D:
					if(LeftAlt(a)){
						if(LeftShift(a)){
							TransposeTrack_CurrPos(window,-12);
						}else{
							TransposeTrack_CurrPos(window,-1);
						}
						break;
					}
					if(LeftExtra(a)){
						if(LeftShift(a)){
							TransposeRange_CurrPos(window,-12);
						}else{
							TransposeRange_CurrPos(window,-1);
						}
						break;
					}
					if(LeftCtrl(a)){
						if(LeftShift(a)){
							TransposeBlock_CurrPos(window,-12);
						}else{
							TransposeBlock_CurrPos(window,-1);
						}
						break;
					}
					break;
				case EVENT_C:
					if(LeftExtra(a)){
						CopyRange_CurrPos(window);
					}else{
						if(LeftAlt(a)){
							CB_CopyTrack_CurrPos(window);
						}else{
							if(LeftCtrl(a)){
								CB_CopyBlock_CurrPos(window);
							}else{
								if(RightExtra(a)){
									if(RightShift(a)){
#ifdef AMIAROS
										Amiga_ConfigColors();
#endif
									}else{
										Window_config(window);
									}
								}else{
								}
							}
						}
					}
					break;
				case EVENT_V:
					if(LeftExtra(a)){
						PasteRange_CurrPos(window);
					}else{
						if(LeftAlt(a)){
							CB_PasteTrack_CurrPos(window);
						}else{
							if(LeftCtrl(a)){
								CB_PasteBlock_CurrPos(window);
							}
						}
					}
					break;
				case EVENT_G:
					break;
				case EVENT_B:
					if(LeftExtra(a)){
						MarkRange_CurrPos(window);
					}else{
						if(RightExtra(a)){
							Block_Properties_CurrPos(window);
						}
					}
					break;
				case EVENT_H:
					break;
				case EVENT_N:
					if(LeftAlt(a)){
						ChangeNoteLength_CurrPos(window);
						break;
					}
					if(LeftCtrl(a)){
						ChangeNoteLength_Block_CurrPos(window);
						break;
					}
					break;
				case EVENT_J:
					break;
				case EVENT_M:
					if(LeftAlt(a)){
						MinimizeTrack_CurrPos(window);
						break;
					}
					if(LeftCtrl(a)){
						MinimizeBlock_CurrPos(window);
						break;
					}
					break;
				case EVENT_MR1:
					if(LeftExtra(a)){
						if(LeftShift(a)){
							IncFontSize_CurrPos(window,-4);
						}else{
							IncFontSize_CurrPos(window,-1);
						}
					}
					break;
				case EVENT_L:
					break;
				case EVENT_MR2:
					if(LeftExtra(a)){
						if(LeftShift(a)){
							IncFontSize_CurrPos(window,4);
						}else{
							IncFontSize_CurrPos(window,1);
						}
					}
					break;
				case EVENT_LR1:
					break;
				case EVENT_MR3:
					if(NoSwitch(a)){
					}else{
						SetFontSizeNormal_CurrPos(window);
					}
					break;
				case EVENT_LR3:
					break;
				case EVENT_Q:
					if(LeftAlt(a)){
						Quantitize_track_CurrPos(window);
						break;
					}
					if(LeftExtra(a)){
						Quantitize_range_CurrPos(window);
						break;
					}
					if(LeftCtrl(a)){
						Quantitize_block_CurrPos(window);
						break;
					}
					if(RightExtra(a)){
						if(!Quit(window)) break;
						return 1;
					}
					break;
				case EVENT_2:
					break;
				case EVENT_W:
					if(RightExtra(a)){
						if(RightShift(a)){
							SaveAs(root);
						}else{
							Save(root);
						}
					}
					break;
				case EVENT_3:
					break;
				case EVENT_E:
					break;
				case EVENT_R:
					if(LeftAlt(a)){
						InsertTracks_CurrPos(window,-1);
					}else{
						if(LeftCtrl(a)){
							DeleteBlock_CurrPos(window);
						}
					}
					break;
				case EVENT_5:
					break;
				case EVENT_T:
					break;
				case EVENT_6:
					break;
				case EVENT_Y:
					break;
				case EVENT_7:
					break;
				case EVENT_U:
					if(LeftAlt(a)){
						if(LeftShift(a)){
							TransposeTrack_CurrPos(window,12);
						}else{
							TransposeTrack_CurrPos(window,1);
						}
					}else{
						if(LeftExtra(a)){
							if(LeftShift(a)){
								TransposeRange_CurrPos(window,12);
							}else{
								TransposeRange_CurrPos(window,1);
							}
						}else{
							if(LeftCtrl(a)){
								if(LeftShift(a)){
									TransposeBlock_CurrPos(window,12);
								}else{
									TransposeBlock_CurrPos(window,1);
								}
							}else{
								if(RightExtra(a)){
									if(RightAlt(a)){
										SetMaxUndos(window);
									}else{
										if(RightShift(a)){
											Redo();
										}else{
											Undo();
										}
									}
								}
							}
						}
					}
					break;
				case EVENT_I:
					if(LeftAlt(a)){
						InsertTracks_CurrPos(window,1);
					}else{
						if(LeftCtrl(a)){
							InsertBlock_CurrPos(window);
						}
					}
					break;
				case EVENT_8:
					if(LeftAlt(a)){
						InvertTrack_CurrPos(window);
						break;
					}
					if(LeftExtra(a)){
						InvertRange_CurrPos(window);
						break;
					}
					if(LeftCtrl(a)){
						InvertBlock_CurrPos(window);
						break;
					}
					break;
				case EVENT_9:
					if(LeftAlt(a)){
						BackWardsTrack_CurrPos(window);
						break;
					}
					if(LeftExtra(a)){
						BackWardsRange_CurrPos(window);
						break;
					}
					if(LeftCtrl(a)){
						BackWardsBlock_CurrPos(window);
						break;
					}
					break;
				case EVENT_O:
					if(RightExtra(a)){
						if( Load_CurrPos(window)){
							return 2;
						}else{
							break;
						}
					}
					break;
				case EVENT_0:
					break;
				case EVENT_P:
					break;
				case EVENT_PR1:
					break;
				case EVENT_0R2:
					break;
				case EVENT_PR2:
					break;
				case EVENT_0R3:
					break;
				case EVENT_KP_ENTER:
					if(LeftShift(a)){
						SoloTrack_CurrPos(window);
					}else{
						if(LeftCtrl(a)){
							AllTracksOn_CurrPos(window);
						}else{
							SwitchTrackOnOff_CurrPos(window);
						}
					}
					break;

#define KPSTUFF(b) \
				case EVENT_KP_##b: \
					if(LeftCtrl(a)){ \
						temp=1##b; \
					}else{ \
						temp=b; \
					} \
					if(CapsLock(a)){ \
						temp+=20; \
					} \
					if(LeftShift(a)){ \
						TRACK_OF_solo_spesified_CurrPos(window,(NInt)temp); \
					}else{ \
						if(LeftAlt(a)){ \
							TRACK_OF_switch_spesified_CurrPos(window,(NInt)temp); \
						}else{ \
							SetCursorPosConcrete_CurrPos(window,(NInt)temp); \
						} \
					} \
				break;

				KPSTUFF(0)
				KPSTUFF(1)
				KPSTUFF(2)
				KPSTUFF(3)
				KPSTUFF(4)
				KPSTUFF(5)
				KPSTUFF(6)
				KPSTUFF(7)
				KPSTUFF(8)
				KPSTUFF(9)

				default:
					if(in_tevent->SubID<0 || in_tevent->SubID>EVENT_MAX)
						RError("Warning, Unknown TR_KEYBOARD; TR_SUB event: %d. \n",in_tevent->SubID);
					break;
			}
			break;
		case TR_MOUSEMOVE:
			MouseMove(window,in_tevent->x,in_tevent->y);
			break;
		case TR_LEFTMOUSEDOWN:
			LeftMouseDown(window,in_tevent->x,in_tevent->y);
			break;
		case TR_LEFTMOUSEUP:
			return LeftMouseUp(window,in_tevent->x,in_tevent->y);
		case TR_RIGHTMOUSEUP:
			AddFXNodeLineCurrPos(window);
			break;
	}
	return 0;
}

uint32_t CanITreatThisEvent_questionmark(int ID,struct Tracker_Windows *window){
	return (window->event_treat&(1<<ID));
}

int EventReciever(struct TEvent *in_tevent, struct Tracker_Windows *window){
	struct TEventFIFO *element;
	int ret;

	if(window->dontbuffer==1) return 0;

	if(CanITreatThisEvent_questionmark(in_tevent->ID,window)==0){
	  Blt_markVisible(window);
	  ret=EventTreater(in_tevent,window);
	  Blt_clearNotUsedVisible(window);
	  Blt_blt(window);
	  return ret;
	}

	if(window->TELlast==NULL){
		element=window->TELroot=window->TELlast=talloc(sizeof(struct TEventFIFO));
	}else{
		element=window->TELlast->next=talloc(sizeof(struct TEventFIFO));
	}

	element->t.ID=in_tevent->ID;
	element->t.SubID=in_tevent->SubID;
	element->t.keyswitch=in_tevent->keyswitch;
	element->t.x=in_tevent->x;
	element->t.y=in_tevent->y;

	window->TELlast=element;

	return 0;
}

int DoTreatAllEvents(struct Tracker_Windows *window){
	struct TEventFIFO *element,*prev=NULL;
	int ret=0,nowret;
	element=window->TELroot;
	while(element!=NULL){
		if(CanITreatThisEvent_questionmark(element->t.ID,window)==0){
			nowret=EventTreater(&element->t,window);
			ret=max(ret,nowret);
			if(prev!=NULL){
				prev->next=element->next;
			}else{
				window->TELroot=element->next;
			}
		}
		prev=element;
		element=element->next;
	}

	element=window->TELroot;
	if(element==NULL){
		window->TELlast=NULL;
	}else{
		while(element!=NULL){
			if(element->next==NULL) window->TELlast=element;
			element=element->next;
		}
	}

	return ret;
}

void DontTreatAnyEvents(struct Tracker_Windows *window){
	window->event_treat=(uint32_t)(~0);
}

void DontTreatAnyEvents_AndDontBuffer(struct Tracker_Windows *window){
	window->event_treat=(uint32_t)(~0);
	window->dontbuffer=1;
}

int TreatAllEvents(struct Tracker_Windows *window){
	window->event_treat=0;
	window->dontbuffer=0;
	return DoTreatAllEvents(window);
}

int TreatEvents(int ID,struct Tracker_Windows *window){
	window->event_treat&=~(1<<ID);
	return DoTreatAllEvents(window);
}

void DontTreatEvents(int ID,struct Tracker_Windows *window){
	window->event_treat|=(1<<ID);
}









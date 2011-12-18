



#include "nsmtracker.h"
#include "disk.h"

#include "disk_load_proc.h"

#include "disk_root_proc.h"
#include "disk_save_proc.h"
#include "trackreallineelements_proc.h"
#include "visual_proc.h"
#include "wblocks_proc.h"
#include "windows_proc.h"

#ifdef AMIAROS
#include "Amiga_bs.h"
#include "instrprop/Amiga_instrprop.h"
#include "plug-ins/disk_camd_mymidilinks_proc.h"
#endif

#include "control_proc.h"
#include "OS_disk_proc.h"
#include "undo.h"
#include "player_proc.h"

#ifdef AMIAROS
#include "../mmd2loader/mmd2load_proc.h"
#endif

#include "disk_load_proc.h"


extern struct Root *root;

#ifdef AMIAROS
extern NInt currpatch;
#endif

struct Instruments *def_instrument;

bool Load(char *filename){
	float version;
	struct Root *newroot;

	dc.file=fopen(filename,"r");
	if(dc.file==NULL){
		RError("Could not open %s for loading\n",filename);
		return false;
	}

	dc.ls=talloc(BUFFERLENGTH+1);
	DC_fgets();

	if(strcmp("RADIUM SONG",dc.ls)){
		RError("First line in song was not 'RADIUM SONG', but '%s'\n",dc.ls);
		fclose(dc.file);
		return false;
	}

	version=DC_LoadF();

	if(version>0.4201 && version<0.50){
		version=0.51;
		dc.colorize=true;
		dc.startcolor=5;
	}else{
		dc.colorize=false;
	}

	if(version>DISKVERSION+0.0001){
		RError("You are trying to load a %f version, while this program is only %f.\n",version,DISKVERSION);
		return false;
	}else{
		printf("Song diskVersion: %f\n",version);
	}

	dc.filename=filename;

	dc.playlist=NULL;
	dc.success=true;

	DC_Next();
	if(strcmp(dc.ls,"OSSTUFF")){
		fprintf(stderr,"OSSTUFF not found, but: '%s'. File: '%s'\n",dc.ls,filename);
		fclose(dc.file);
		EndProgram();
		exit(4);
	}

	LoadOsStuff();

	if(strcmp(dc.ls,"ROOT")){
		fprintf(stderr,"ROOT not found\n");
		fclose(dc.file);
		EndProgram();
		exit(5);
	}

		newroot=LoadRoot();

	fclose(dc.file);

	if(!dc.success){
		printf("no\n");
		EndProgram();
		exit(6);
	}

	printf("jess\n");

	ResetUndo();

	def_instrument=root->def_instrument;

#ifdef AMIAROS
	CloseHelpWindow();
	CloseCPPWindowWindow();
	CloseBlockSelectWindow();
#endif
	CloseAllTrackerWindows();

	root=newroot;		//BANG!

	DLoadRoot(newroot);



#ifdef AMIAROS
	currpatch=-1;
#endif

	return true;

}

extern int num_undos;
extern int max_num_undos;

#ifdef AMIAROS
extern char *mmp2filename;
#endif

bool Load_CurrPos(struct Tracker_Windows *window){
	char *filename;
	char *ret=NULL;
	char temp[200];

	PlayStop();

	if(num_undos>0){
//		sprintf(temp,"%s%d changes has been made to file. Are you shure? (yes/no)",num_undos>=max_num_undos-1?"At least":"",num_undos);
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

	filename=GFX_GetLoadFileName(window,(ReqType)0,"Select file to load","work:radiumsongs");

	if(filename==NULL) return false;

	if(strlen(filename)>4){
		if(
			!strcmp(filename+strlen(filename)-5,".mmd2") ||
			!strcmp(filename+strlen(filename)-5,".mmd3") ||
			!strcmp(filename+strlen(filename)-4,".mmd")
		){
#ifdef AMIAROS
			mmp2filename=filename;
#endif
			return Load("radium:Init.rad");
		}
	}

	return Load(filename);

}






#include <unistd.h>

#include "nsmtracker.h"
#include "disk.h"
#include "disk_root_proc.h"
#include "OS_disk_proc.h"
#include "disk_save_proc.h"
#include "visual_proc.h"
#include "player_proc.h"


void Save_Clean(char *filename,struct Root *theroot){
	int length1,length2;

	dc.success=true;

	dc.file=fopen(filename,"w");
	if(dc.file==NULL){
		RError("Could not open file for writing.\n");
		return;
	}

	length1=fprintf(dc.file,"RADIUM SONG\n");
	length2=fprintf(dc.file,"%f\n",DISKVERSION);

	if(length1<0 || length2<0){
		RError("Could not write to file.\n");
		fclose(dc.file);
		return;
	}

	DC_start("OSSTUFF");
		SaveOsStuff();
	DC_end();

	SaveRoot(theroot);

	if( ! dc.success){
		RError("Problems writing to file.\n");
	}

	fclose(dc.file);
}

void SaveAs(struct Root *theroot){
	char *filename;
	char *ret=NULL;

	PlayStop();

	filename=GFX_GetSaveFileName(theroot->song->tracker_windows,(ReqType)0,"Select file to save","work:radiumsongs");

	if( ! access(filename,F_OK)){
		while(
			ret==NULL || (
				! strcmp("yes",ret) &&
				! strcmp("no",ret)
			)
		){
			ret=GFX_GetString(
				theroot->song->tracker_windows,
				(ReqType)0,
				"File already exists, are you sure? (yes/no)"
			);
		}
		if(!strcmp("no",ret)) return;
	}

	Save_Clean(filename,theroot);

}

void Save(struct Root *theroot){

	PlayStop();

	if(dc.filename==NULL){
		SaveAs(theroot);
	}else{
		Save_Clean(dc.filename,theroot);
	}
}


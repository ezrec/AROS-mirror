/*
 * DEBUGGING.C
 *
*/

#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dostags.h>

#if defined(__AROS__)
#include <proto/exec.h>
#include <proto/dos.h>
#endif
#if defined(__MORPHOS__) 
#include <ppcinline/dos.h>
extern struct DOSBase *DOSBase;
#endif

#include <caf/caf_types.h>
#include <caf/caf_debugging.h>
#include <caf/thread_helpers.h> // CAF debugging has to be thread safe

char caf_debug_fname[1024];
int  caf_debug_mode;
BPTR caf_debug_file_handle;
bool caf_debug_inited;

void CafDebugInit(int in_mode)
{
	BPTR caf_file;
	int mode = in_mode;
	
	caf_debug_file_handle = (BPTR) 0;
	caf_debug_inited = false;
	caf_debug_mode = CAF_DEBUG_NONE;
	
	if (mode & CAF_DEBUG_STDOUT) {
	}
	
	if (mode & CAF_DEBUG_FILE) {
		caf_file = Open(caf_debug_fname, MODE_NEWFILE);
		if (!caf_file)
			mode -= CAF_DEBUG_FILE;
	}
	
	if (mode != 0) {
		caf_debug_mode = mode;
		if (mode & CAF_DEBUG_FILE)
			caf_debug_file_handle = caf_file;
		
		caf_debug_inited = true;
		DebugOutput("C.A.F. debugging initialized\n");
	}
}

void CafDebugCleanup(void)
{
	if (caf_debug_inited == false)
		return;
	
	if ((caf_debug_mode & CAF_DEBUG_FILE) && (caf_debug_file_handle != 0)) {
		Close(caf_debug_file_handle);
		caf_debug_file_handle = (BPTR) 0;
	}
	
	caf_debug_inited = false;
	caf_debug_mode = CAF_DEBUG_NONE;	
}

void SetDebugMode(int mode)
{
	caf_debug_mode = mode;
}

void SetDebugFile(char *fname)
{
	caf_strncpy(caf_debug_fname, fname, 1024);
}

void DebugOutputNL(char *string, bool b_addnl)
{
	int slen;
	
	if (caf_debug_inited == false)
		return;
	
	slen = caf_strlen(string);
	if (!slen)
		return;
	
	if (caf_debug_mode & CAF_DEBUG_FILE) {
		Write(caf_debug_file_handle, string, slen);
		if (b_addnl)
			Write(caf_debug_file_handle, "\n", 1);
	}

#if !defined(__AROS__)
	if (caf_debug_mode & CAF_DEBUG_STDOUT) {
		write(0, string, slen);
		if (b_addnl)
			write(0, "\n", 1);
	}
#endif
}

void DebugOutput(char *string)
{
	if (caf_debug_inited == false)
		return;
	
	if (caf_strlen(string) == 0)
		return;
	
	DebugOutputNL(string, false);
}


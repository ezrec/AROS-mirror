/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/


#include <exec/types.h>
#include <libraries/dos.h>
#include <exec/resident.h>
#include <exec/memory.h>
#include <exec/libraries.h>


/*Our Library Structure */


struct MusicBase {
    struct Library MLib;
    BPTR    ml_SegList;
    APTR    ml_Module;
    LONG    ml_Len;
    WORD    ml_Playing,
            ml_ModType,
            ml_Volume,
            ml_Tempo,
            ml_Tracks,
            ml_SongPos;
    struct Library *ml_DOSBase, *ml_PPBase;
    char   *ml_SongName;
};


/* Module types currently supported */
#define MOD_STNTPT   1               /* Sound/Noise/ProTracker */
#define MOD_MED      2               /* MED Module (MMD0) */
#define MOD_OKTA     3               /* Oktalyzer mod */
#define MOD_OLDST    4               /* 15 Inst SoundTracker */

/* Error codes returned by PlayModule

**********************NOTE********************************
 if err code is < 0, then the failure code is a PowerPacker.Library fail

*/


#define ML_NOMEM        101       /* not enuff memory to load module */
#define ML_BADMOD       102       /* garbled or not-supported */
#define ML_NOMOD        103       /* DOS Open failed */
#define ML_NOPLAYER     104       /* CIA Player Allocations Failed */
#define ML_LIBLOCKED    105       /* LIBBASE is Locked for processing */
#define ML_INVALIDFNAME 106       /* Bad file name */
#define ML_NOAUDIO      107       /* Couldn't lock Audio channels */

WORD PlayModule(char *,BOOL);
VOID StopModule(void);
WORD IsModule(char *);
VOID FlushModule(void);
VOID ContModule(void);
VOID SetVolume(WORD);
VOID PlayFaster(void);
VOID PlaySlower(void);
VOID TempoReset(void);

/* AROS: We don't use pragmas

#pragma libcall MUSICBase PlayModule   1e 0802
#pragma libcall MUSICBase StopModule   24 0
#pragma libcall MUSICBase IsModule     2a 801
#pragma libcall MUSICBase FlushModule  30 0
#pragma libcall MUSICBase ContModule   36 0
#pragma libcall MUSICBase SetVolume    3c 801
#pragma libcall MUSICBase PlayFaster   42 0
#pragma libcall MUSICBase PlaySlower   48 0
#pragma libcall MUSICBase TempoReset   4e 0


*/


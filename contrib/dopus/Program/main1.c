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

#include "dopus.h"

int readsetup(file)
char *file;
{
	struct ConfigStuff cstuff;
	char *s;
	int a;

	strcpy(str_config_file,file);
	strcpy(str_config_basename,file);
	if ((s=strstri(BaseName(str_config_basename),".CFG"))) *s=0;

	fixcstuff(&cstuff);
	if ((a=ReadConfig(file,&cstuff))!=1) DefaultConfig(&cstuff);
	cstufffix(&cstuff);

	if (a==1) {
		dopus_curgadbank=dopus_firstgadbank;
		data_gadgetrow_offset=data_drive_offset=0;
		CheckConfig(&cstuff);
	}
	config_changed=0;
	return(a);
}

int savesetup()
{
	struct ConfigStuff cstuff;
	int a;

	fixcstuff(&cstuff);
	if (!status_iconified && status_publicscreen && Window) {
		if (MainScreen) {
			config->scr_winx=Window->LeftEdge;
			config->scr_winy=Window->TopEdge;
		}
		else {
			config->wbwinx=Window->LeftEdge;
			config->wbwiny=Window->TopEdge;
		}
	}
	a=SaveConfig(str_config_file,&cstuff);
	cstufffix(&cstuff);
	if (a) config_changed=0;
	return(a);
}

void getdefaultconfig()
{
	struct ConfigStuff cstuff;

	fixcstuff(&cstuff);
	DefaultConfig(&cstuff);
	cstufffix(&cstuff);
	config_changed=0;
}

void fixcstuff(cstuff)
struct ConfigStuff *cstuff;
{
	cstuff->config=config;
	cstuff->typekey=filetype_key;
	cstuff->firsttype=dopus_firsttype;
	cstuff->firstbank=dopus_firstgadbank;
	cstuff->curbank=dopus_curgadbank;
	cstuff->firsthotkey=dopus_firsthotkey;
}

void cstufffix(cstuff)
struct ConfigStuff *cstuff;
{
	config=cstuff->config;
	filetype_key=cstuff->typekey;
	dopus_firsttype=cstuff->firsttype;
	dopus_firstgadbank=cstuff->firstbank;
	dopus_curgadbank=cstuff->curbank;
	dopus_firsthotkey=cstuff->firsthotkey;
}

// test.c
// 17 Jun 2001 15:52:49

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iconobject.h>
#include <clib/iconobject_protos.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <stdio.h>
#include <stdlib.h>


struct Library *IconobjectBase;

int main(int argc, char *argv[])
{
	if( IconobjectBase = (APTR) OpenLibrary( "iconobject.library", 0L ) )
		{
		Object *iobj;
		char *IconName = "SYS:WBStartup/ASyncWB";

		if (2 == argc)
			IconName = argv[1];

		printf("OK, Version %ld.%ld\n", IconobjectBase->lib_Version, IconobjectBase->lib_Revision);

		iobj = NewIconObjectTags(IconName,
				TAG_END);

		printf("IconObject = %p\n", iobj);

		if (iobj)
			{
			DisposeIconObject(iobj);
			}

		CloseLibrary( IconobjectBase );
		}

	return 0;
}


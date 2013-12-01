#include <exec/types.h>
#include <proto/exec.h>
#include "../plugin_api.h"

#include <exec/libraries.h>
#include <stdio.h>

int main(void)
{
	struct Library  *PluginBase;

	if(NULL != (PluginBase = OpenLibrary("title_clock.plugin", 40)) )
	{
		printf("plugin poened\n");
		tcpl_ReReadPrefs();
		CloseLibrary(PluginBase);
	}
	return(0);
}


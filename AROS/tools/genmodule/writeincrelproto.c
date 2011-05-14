/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.

    Desc: Function to write proto/modulename_rel.h. Part of genmodule.
*/
#include "genmodule.h"

void writeincrelproto(struct config *cfg)
{
    FILE *out;
    char line[256];
    struct linelist *linelistit;
    
    snprintf(line, 255, "%s/proto/%s_rel.h", cfg->gendir, cfg->modulename);
    out = fopen(line, "w");

    if (out == NULL)
    {
        perror(line);
    	exit(20);
    }

    fprintf(out,
	    "#ifndef PROTO_%s_REL_H\n"
	    "#define PROTO_%s_REL_H\n"
	    "\n"
            "%s"
	    "\n"
	    "#include <exec/types.h>\n"
	    "#include <aros/system.h>\n"
	    "\n"
	    "#include <clib/%s_protos.h>\n"
	    "\n"
	    "#if !defined(%s) && !defined(__NOLIBBASE__) && !defined(__%s_NOLIBBASE__)\n"
            " extern IPTR %s_offset;\n"
	    " #ifdef __%s_STDLIBBASE__\n"
	    "  #define %s ((struct Library *)((char *)AROS_GET_LIBBASE+%s_offset))\n"
	    " #else\n"
	    "  #define %s ((%s)((char *)AROS_GET_LIBBASE+%s_offset))\n"
	    " #endif\n"
	    "#endif\n"
	    "\n",
	    cfg->modulenameupper, cfg->modulenameupper, getBanner(cfg),
	    cfg->modulename,
	    cfg->libbase, cfg->modulenameupper,
            cfg->libbase,
	    cfg->modulenameupper,
	    cfg->libbase, cfg->libbase,
	    cfg->libbase, cfg->libbasetypeptrextern, cfg->libbase
    );
    
    fprintf(out,
	    "#if !defined(NOLIBDEFINES) && !defined(%s_NOLIBDEFINES)\n"
	    "#   include <defines/%s.h>\n"
	    "#endif\n"
	    "\n"
	    "#endif /* PROTO_%s_REL_H */\n",
	    cfg->modulenameupper, cfg->modulename, cfg->modulenameupper
    );
    fclose(out);
}

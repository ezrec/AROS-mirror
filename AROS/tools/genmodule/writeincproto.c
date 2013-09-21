/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.

    Desc: Function to write proto/modulename(_rel).h. Part of genmodule.
*/
#include "genmodule.h"

void writeincproto(struct config *cfg)
{
    FILE *out;
    char line[256], define[256], *banner;
    struct linelist *linelistit;
    
    snprintf(line, 255, "%s/proto/%s.h",
             cfg->gendir, cfg->incname
    );
    out = fopen(line, "w");

    if (out == NULL)
    {
        perror(line);
    	exit(20);
    }

    banner = getBanner(cfg);
    fprintf(out,
	    "#ifndef PROTO_%s_H\n"
	    "#define PROTO_%s_H\n"
	    "\n"
            "%s"
	    "\n"
	    , cfg->incnameupper
            , cfg->incnameupper
            , banner
    );
    fprintf(out,
	    "#include <exec/types.h>\n"
	    "%s"
	    "#include <aros/system.h>\n"
	    "\n"
	    "#include <clib/%s_protos.h>\n"
	    "\n",
	    (cfg->modtype == DEVICE) ? "#include <exec/devices.h>\n" : "",
	    cfg->incname
    );
    freeBanner(banner);
    fprintf(out,
            "#ifndef __%s_RELLIBBASE__\n"
            " #if !defined(__NOLIBBASE__) && !defined(__%s_NOLIBBASE__)\n"
            "  #if !defined(%s)\n"
            "   #ifdef __%s_STDLIBBASE__\n"
            "    extern struct Library *%s;\n"
            "   #else\n"
            "    extern %s%s;\n"
            "   #endif\n"
            "  #endif\n"
            " #endif\n"
            " #ifndef __aros_getbase_%s\n"
            "  #define __aros_getbase_%s() (%s)\n"
            " #endif\n"
            "#else /* __%s_RELLIBASE__ */\n"
            " extern const IPTR __aros_rellib_offset_%s;\n"
            " #define AROS_RELLIB_OFFSET_%s __aros_rellib_offset_%s\n"
            " #define AROS_RELLIB_BASE_%s __aros_rellib_base_%s\n"
            " #ifndef __aros_getbase_%s\n"
            "  #ifndef __aros_getoffsettable\n"
            "   char *__aros_getoffsettable(void);\n"
            "  #endif\n"
            "  #define __aros_getbase_%s() (*(%s*)(__aros_getoffsettable()+__aros_rellib_offset_%s))\n"
            " #endif\n"
            "#endif\n"
            "\n",
            cfg->modulenameupper,
            cfg->modulenameupper,
            cfg->libbase,
            cfg->modulenameupper,
            cfg->libbase,
            cfg->libbasetypeptrextern, cfg->libbase,
            cfg->libbase,
            cfg->libbase, cfg->libbase,
            cfg->modulenameupper,
            cfg->libbase,
            cfg->modulenameupper, cfg->libbase,
            cfg->modulenameupper, cfg->libbase,
            cfg->libbase,
            cfg->libbase, cfg->libbasetypeptrextern, cfg->libbase
    );


    // define name must not start with a digit
    // this solves a problem with proto/8svx.h
    if (isdigit(cfg->modulenameupper[0]))
    {
	snprintf(define, sizeof define, "X%s", cfg->modulenameupper);
    }
    else
    {
	strncpy(define, cfg->modulenameupper, sizeof define);
    }

    fprintf(out,
            "#if !defined(NOLIBINLINE) && !defined(%s_NOLIBINLINE) && !defined(__%s_RELLIBBASE__)\n"
            "#   include <inline/%s.h>\n"
	    "#elif !defined(NOLIBDEFINES) && !defined(%s_NOLIBDEFINES)\n"
	    "#   include <defines/%s.h>\n"
	    "#endif\n"
	    "\n"
	    "#endif /* PROTO_%s_H */\n",
            define, cfg->modulenameupper,
            cfg->incname,
	    define,
            cfg->incname,
            cfg->incnameupper
    );

    fclose(out);
}

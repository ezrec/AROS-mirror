#ifndef CONFIG_ERR_H
#define CONFIG_ERR_H

static char *ERR_EOF 				  = "Unexpected end of file";
static char *ERR_MEM					  = "Out of memory";
static char *ERR_BADVALUE 			  = "Bad value";
static char *ERR_UNKNOWNSET		  = "Unknown SET variable \"%s\"";
static char *ERR_MISSINGISANDVALUE = "Missing \"=\" and value";
static char *ERR_SET					  = "To set a configuration variable you must write \"variablename = value\"";
static char *ERR_MISSINGVALUE		  = "Missing value";
static char *ERR_NESTING 			  = "Too many levels of nesting";
static char *ERR_MISSINGCONDITION  = "Missing condition";
static char *ERR_UNKNOWNGET		  = "Unknown GET variable \"%s\"";
static char *ERR_UNKNOWNOP			  = "Unknown operation \"%s\"";
static char *ERR_ENDIFWOIF			  = "ENDIF without IF";
static char *ERR_ELSEWOIF			  = "ELSE without IF";

#endif


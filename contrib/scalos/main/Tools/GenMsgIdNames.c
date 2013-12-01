// GenMsgIdNames.c
// $Date$
// $Revision$


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <exec/types.h>
#include <dos/dos.h>

#include <defs.h> // +jmc+
#include <Year.h> // +jmc+

static LONG GenerateOutput(FILE *fdIn, FILE *fdOut);
static void StripLF(char *Line);

CONST_STRPTR Ver = "\0$VER: GenMsgIdNames 1.0 (30.12.2002) "
	COMPILER_STRING
	"©2002" CURRENTYEAR " The Scalos Team";

int main(int argc, char *argv[])
{
	FILE *fdIn, *fdOut;

	if (argc != 3)
		{
		fprintf(stderr, "Usage: %s xxxlocale.h msgnames.h\n", argv[0]);
		return RETURN_WARN;
		}

	fdIn = fopen(argv[1], "r");
	if (NULL == fdIn)
		{
		fprintf(stderr, "Failed to open input file <%s>\n", argv[1]);
		return RETURN_ERROR;
		}
	fdOut = fopen(argv[2], "w");
	if (NULL == fdOut)
		{
		fclose(fdIn);
		fprintf(stderr, "Failed to open output file <%s>\n", argv[2]);
		return RETURN_ERROR;
		}

	fprintf(fdOut, "// %s\n", argv[2]);

	GenerateOutput(fdIn, fdOut);

	fclose(fdIn);
	fclose(fdOut);

	return 0;
}


static LONG GenerateOutput(FILE *fdIn, FILE *fdOut)
{
	time_t t;
	struct tm *myTm;
	char Line[512];
	char TimeString[40];
	BOOL Start = FALSE;
	BOOL Finished = FALSE;

	time(&t);
	myTm = localtime(&t);

	strftime(TimeString, sizeof(TimeString), "%d %b %Y %H:%M:%S", myTm);
	fprintf(fdOut, "// %s\n\n", TimeString);

	fprintf(fdOut, "#ifndef MSGIDNAMES_H_INCLUDED\n");
	fprintf(fdOut, "#define MSGIDNAMES_H_INCLUDED\n\n");
	fprintf(fdOut, "/****************************************************************************/\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "/* This file was created automatically.\n");
	fprintf(fdOut, " * Do NOT edit by hand!\n");
	fprintf(fdOut, " */\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "#include \"locale.h\"\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "#define	UNKNOWN_MSG	((LONG) ~0)\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "struct MsgIdNameDef\n");
	fprintf(fdOut, "	{\n");
	fprintf(fdOut, "	ULONG mnd_MsgId;\n");
	fprintf(fdOut, "	CONST_STRPTR mnd_MsgName;\n");
	fprintf(fdOut, "	};\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "static const struct MsgIdNameDef MsgIdNames[] =\n");
	fprintf(fdOut, "{\n");

	while (!feof(fdIn) && !Finished)
		{
		if (fgets(Line, sizeof(Line)-1, fdIn))
			{
			StripLF(Line);

			if (0 == strlen(Line))
				continue;

			if (Start)
				{
				if (0 == stricmp(Line, "#endif /* CATCOMP_NUMBERS */"))
					{
					Start = FALSE;
					Finished = TRUE;
					}
				else if (0 == strnicmp(Line, "#define ", 8))
					{
					const char *lp = Line + 8;
					char IdName[255];
					char *dlp = IdName;
					size_t MaxLen = sizeof(IdName);

					while (*lp && !isspace((int) *lp) && MaxLen > 1)
						{
						*dlp++ = *lp++;
						MaxLen--;
						}
					*dlp = '\0';

					fprintf(fdOut, "\t{ %s,\t \"%s\"\t},\n", IdName, IdName); 
					}
				}
			else
				{
				if (0 == stricmp(Line, "#ifdef CATCOMP_NUMBERS"))
					Start = TRUE;
				}
			}
		}


	fprintf(fdOut, "	{ 0,						NULL }\n");
	fprintf(fdOut, "};\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "/****************************************************************************/\n");
	fprintf(fdOut, "\n");
	fprintf(fdOut, "#endif	/* MSGIDNAMES_H_INCLUDED */\n");

	return RETURN_OK;
}


static void StripLF(char *Line)
{
	size_t Len = strlen(Line);

	Line += Len - 1;

	if ('\n' == *Line)
		*Line = '\0';
}


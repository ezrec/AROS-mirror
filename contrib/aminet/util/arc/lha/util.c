/*
 * util.c - part of LHa for UNIX
 *    Feb 26 1992 modified by Masaru Oki
 *    Mar  4 1992 modified by Masaru Oki
 *        #ifndef USESTRCASECMP added.
 *    Mar 31 1992 modified by Masaru Oki
 *        #ifdef NOMEMSET added.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "lharc.h"

#define BUFFERSIZE 2048

extern unsigned short crc;
extern int quiet;

/* crc_flag == 0: no crc, 1: crc check, 2: extract, 3: append */
/* return: size of source file */
long copyfile(FILE *f1, FILE *f2, long size, int crc_flg)
{
    unsigned short xsize;
    char *buf;
    long rsize = 0;

    if ((buf = (char *)malloc(BUFFERSIZE)) == NULL)
        fatal_error("virtual memory exhausted.\n");
    crc = 0;
    if ((crc_flg == 2 || crc_flg) && text_mode)
        init_code_cache();
    while (size > 0)
    {
        /* read */
        if (crc_flg == 3 && text_mode)
	{
            xsize = fread_txt(buf, BUFFERSIZE, f1);
            if (xsize == 0)
                break;
            if (ferror(f1))
	    {
                fatal_error("file read error\n");
            }
        }
        else
	{
            xsize = (size > BUFFERSIZE) ? BUFFERSIZE : size;
            if (fread(buf, 1, xsize, f1) != xsize)
	    {
                fatal_error("file read error\n");
            }
        }
        /* write */
        if (f2)
	{
            if (crc_flg == 2 && text_mode)
	    {
                if (fwrite_txt(buf, xsize, f2))
		{
                    fatal_error("file write error\n");
                }
            }
            else
	    {
                if (fwrite(buf, 1, xsize, f2) != xsize)
		{
                    fatal_error("file write error\n");
                }
            }
        }
        /* calculate crc */
        if (crc_flg)
	{
            calccrc(buf, xsize);
        }
        rsize += xsize;
        if (crc_flg != 3 || !text_mode)
            size -= xsize;
    }
    free(buf);
    return rsize;
}

int encode_stored_crc(FILE *ifp, FILE *ofp, long size,
		      long *original_size_var, long *write_size_var)
{
    int save_quiet;

    save_quiet = quiet;
    quiet = 1;
    size = copyfile (ifp,ofp,size,3);
    *original_size_var = *write_size_var = size;
    quiet = save_quiet;
    return crc;
}

/***************************************
    convert path delimiter
****************************************
    returns *filename
***************************************/
unsigned char *convdelim(unsigned char *path, unsigned char delim)
{
    unsigned char c;
    unsigned char *p;
#ifdef MULTIBYTE_CHAR
    int kflg;

    kflg = 0;
#endif
    for (p = path; (c = *p) != 0; p++)
    {
#ifdef MULTIBYTE_CHAR
        if (kflg)
        {
            kflg = 0;
        }
        else if (iskanji(c))
        {
            kflg = 1;
        }
        else
#endif
        if (c == '\\' || c == DELIM || c == DELIM2)
        {
            *p = delim;
            path = p + 1;
        }
    }
    return path;
}

/* If TRUE, archive file name is msdos SFX file name. */
boolean archive_is_msdos_sfx1(char *name)
{
    int len = strlen (name);

    return ((len >= 4) &&
            (stricmp (".COM",name + len - 4) == 0 ||
            stricmp (".EXE",name + len - 4) == 0)) ||
            ((len >= 2) &&
            (stricmp (".x",name + len - 2) == 0));
}

/* skip SFX header */
boolean skip_msdos_sfx1_code(FILE *fp)
{
    unsigned char buffer[4096];
    unsigned char *p, *q;
    int n;

    n = fread (buffer, sizeof (char), 4096, fp);

    for (p = buffer + 2, q = buffer + n - 5; p < q; p ++)
    {
        /* found "-l??-" keyword (as METHOD type string) */
        if (p[0] == '-' && p[1] == 'l' && p[4] == '-')
        {
            /* size and checksum validate check */
            if (p[-2] > 20 && p[-1] == calc_sum (p, p[-2]))
            {
                fseek (fp, ((p - 2) - buffer) - n, SEEK_CUR);
                return TRUE;
            }
        }
    }

    fseek (fp, -n, SEEK_CUR);
    return FALSE;
}

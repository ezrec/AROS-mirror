/*Generic source code to un-HQX a file ... Unix */

#ifndef lint
static char version[] = "xbin.c Version 2.3 09/30/85";
#endif lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

#ifdef MAXNAMLEN        /* 4.2 BSD */
#define FNAMELEN MAXNAMLEN
#else
#define FNAMELEN DIRSIZ
#endif

#include <sys/time.h>
long timezone;
#define search_last strrchr
extern char *strrchr();

/* Mac time of 00:00:00 GMT, Jan 1, 1970 */
#define TIMEDIFF 0x7c25b080

#define DATABYTES 128

#define BYTEMASK 0xff
#define BYTEBIT 0x100
#define WORDMASK 0xffff
#define WORDBIT 0x10000

#define NAMEBYTES 63
#define H_NLENOFF 1
#define H_NAMEOFF 2

/* 65 <-> 80 is the FInfo structure */
#define H_TYPEOFF 65
#define H_AUTHOFF 69
#define H_FLAGOFF 73

#define H_LOCKOFF 81
#define H_DLENOFF 83
#define H_RLENOFF 87
#define H_CTIMOFF 91
#define H_MTIMOFF 95

#define H_OLD_DLENOFF 81
#define H_OLD_RLENOFF 85

#define F_BUNDLE 0x2000
#define F_LOCKED 0x8000

struct macheader {
        char m_name[NAMEBYTES+1];
        char m_type[4];
        char m_author[4];
        short m_flags;
        long m_datalen;
        long m_rsrclen;
        long m_createtime;
        long m_modifytime;
} mh;

struct filenames {
        char f_info[256];
        char f_data[256];
        char f_rsrc[256];
} files;

int pre_beta;   /* options */
int listmode;
int verbose;

int compressed; /* state variables */
int qformat;
FILE *ifp;

/*
 * xbin -- unpack BinHex format file into suitable
 * format for downloading with macput
 * Dave Johnson, Brown University Computer Science
 *
 * (c) 1984 Brown University
 * may be used but not sold without permission
 *
 * created ddj 12/16/84
 * revised ddj 03/10/85 -- version 4.0 compatibility, other minor mods
 * revised ddj 03/11/85 -- strip LOCKED bit from m_flags
 * revised ahm 03/12/85 -- System V compatibility
 * revised dba 03/16/85 -- (Darin Adler, TMQ Software)  4.0 EOF fixed,
 *                         4.0 checksum added
 * revised ddj 03/17/85 -- extend new features to older formats: -l, stdin
 * revised ddj 03/24/85 -- check for filename truncation, allow multiple files
 * revised ddj 03/26/85 -- fixed USG botches, many problems w/multiple files
 * revised jcb 03/30/85 -- (Jim Budler, amdcad!jimb), revised for compatibility
 *                         with 16-bit int machines
 * revised dl  06/16/85 -- (Dan LaLiberte, liberte@uiucdcs) character
 *                         translation speedup
 * revised ddj 09/30/85 -- fixed problem with run of RUNCHAR
 */
char usage[] = "usage: \"xbin [-v] [-l] [-o] [-n name] [-] filename\"\n";

main(ac, av)
char **av;
{
        char *filename, *macname;

        filename = ""; macname = "";
        ac--; av++;
        while (ac) {
                if (av[0][0] == '-') {
                        switch (av[0][1]) {
                        case '\0':
                                filename = "-";
                                break;
                        case 'v':
                                verbose++;
                                break;
                        case 'l':
                                listmode++;
                                break;
                        case 'o':
                                pre_beta++;
                                break;
                        case 'n':
                                if (ac > 1) {
                                        ac--; av++;
                                        macname = av[0];
                                        filename = "";
                                        break;
                                }
                                else
                                        goto bad_usage;
                        default:
                                goto bad_usage;
                        }
                }
                else
                        filename = av[0];
                if (filename[0] != '\0') {
                        setup_files(filename, macname);
                        if (listmode) {
                                print_header();
                        }
                        else {
                                process_forks();
                                /* now that we know the size of the forks */
                                forge_info();
                        }
                        if (ifp != stdin)
                                fclose(ifp);
                        macname = "";
                        ifp = NULL;             /* reset state */
                        qformat = 0;
                        compressed = 0;
                }
                ac--; av++;
        }
        if (*filename == '\0') {
bad_usage:
                fprintf(stderr, usage);
                exit(1);
        }
}

static char *extensions[] = {
        ".hqx",
        ".hcx",
        ".hex",
        "",
        NULL
};

setup_files(filename, macname)
char *filename;         /* input file name -- extension optional */
char *macname;          /* name to use on the mac side of things */
{
        char namebuf[256], *np;
        char **ep;
        int n;
        struct stat stbuf;
        long curtime;

        if (filename[0] == '-') {
                ifp = stdin;
                filename = "stdin";
        }
        else {
                /* find input file and open it */
                for (ep = extensions; *ep != NULL; ep++) {
                        sprintf(namebuf, "%s%s", filename, *ep);
                        if (stat(namebuf, &stbuf) == 0)
                                break;
                }
                if (*ep == NULL) {
                        perror(namebuf);
                        exit(-1);
                }
                ifp = fopen(namebuf, "r");
                if (ifp == NULL) {
                        perror(namebuf);
                        exit(-1);
                }
        }
        if (ifp == stdin) {
                curtime = time(0);
                mh.m_createtime = curtime;
                mh.m_modifytime = curtime;
        }
        else {
                mh.m_createtime = stbuf.st_mtime;
                mh.m_modifytime = stbuf.st_mtime;
        }
        if (listmode || verbose) {
                fprintf(stderr, "%s %s%s",
                        listmode ? "\nListing" : "Converting",
                        namebuf, listmode ? ":\n" : " ");
        }

        qformat = find_header(); /* eat mailer header &cetera, intuit format */

        if (qformat)
                do_q_header(macname);
        else
                do_o_header(macname, filename);

        /* make sure host file name doesn't get truncated beyond recognition */
        n = strlen(mh.m_name);
        if (n > FNAMELEN - 2)
                n = FNAMELEN - 2;
        strncpy(namebuf, mh.m_name, n);
        namebuf[n] = '\0';

        /* get rid of troublesome characters */
        for (np = namebuf; *np; np++)
                if (*np == ' ' || *np == '/')
                        *np = '_';

        sprintf(files.f_data, "%s.data", namebuf);
        sprintf(files.f_rsrc, "%s.rsrc", namebuf);
        sprintf(files.f_info, "%s.info", namebuf);
        if (verbose)
                fprintf(stderr, "==> %s.{info,data,rsrc}\n", namebuf);
}

/* print out header information in human-readable format */
print_header()
{
        char *ctime();

        printf("macname: %s\n", mh.m_name);
        printf("filetype: %.4s, ", mh.m_type);
        printf("author: %.4s, ", mh.m_author);
        printf("flags: 0x%x\n", mh.m_flags);
        if (qformat) {
                printf("data length: %ld, ", mh.m_datalen);
                printf("rsrc length: %ld\n", mh.m_rsrclen);
        }
        if (!pre_beta) {
                printf("create time: %s", ctime(&mh.m_createtime));
        }
}

process_forks()
{
        if (qformat) {
                /* read data and resource forks of .hqx file */
                do_q_fork(files.f_data, mh.m_datalen);
                do_q_fork(files.f_rsrc, mh.m_rsrclen);
        }
        else
                do_o_forks();
}

/* write out .info file from information in the mh structure */
forge_info()
{
        static char buf[DATABYTES];
        char *np;
        FILE *fp;
        int n;
        long tdiff;
        struct tm *tp;
#ifdef BSD
        struct timeb tbuf;
#else
        long bs;
#endif

        for (np = mh.m_name; *np; np++)
                if (*np == '_') *np = ' ';

        buf[H_NLENOFF] = n = np - mh.m_name;
        strncpy(buf + H_NAMEOFF, mh.m_name, n);
        strncpy(buf + H_TYPEOFF, mh.m_type, 4);
        strncpy(buf + H_AUTHOFF, mh.m_author, 4);
        put2(buf + H_FLAGOFF, mh.m_flags & ~F_LOCKED);
        if (pre_beta) {
                put4(buf + H_OLD_DLENOFF, mh.m_datalen);
                put4(buf + H_OLD_RLENOFF, mh.m_rsrclen);
        }
        else {
                put4(buf + H_DLENOFF, mh.m_datalen);
                put4(buf + H_RLENOFF, mh.m_rsrclen);

                /* convert unix file time to mac time format */
#ifdef BSD
                ftime(&tbuf);
                tp = localtime(&tbuf.time);
                tdiff = TIMEDIFF - tbuf.timezone * 60;
                if (tp->tm_isdst)
                        tdiff += 60 * 60;
#else
                /* I hope this is right! -andy */
                time(&bs);
                tp = localtime(&bs);
                tdiff = TIMEDIFF - timezone;
                if (tp->tm_isdst)
                        tdiff += 60 * 60;
#endif
                put4(buf + H_CTIMOFF, mh.m_createtime + tdiff);
                put4(buf + H_MTIMOFF, mh.m_modifytime + tdiff);
        }
        fp = fopen(files.f_info, "w");
        if (fp == NULL) {
                perror("info file");
                exit(-1);
        }
        fwrite(buf, 1, DATABYTES, fp);
        fclose(fp);
}

/* eat characters until header detected, return which format */
find_header()
{
        int c, at_bol;
        char ibuf[BUFSIZ];

        /* look for "(This file ...)" line */
        while (fgets(ibuf, BUFSIZ, ifp) != NULL) {
                if (strncmp(ibuf, "(This file", 10) == 0)
                        break;
        }
        at_bol = 1;
        while ((c = getc(ifp)) != EOF) {
                switch (c) {
                case '\n':
                case '\r':
                        at_bol = 1;
                        break;
                case ':':
                        if (at_bol)     /* q format */
                                return 1;
                        break;
                case '#':
                        if (at_bol) {   /* old format */
                                ungetc(c, ifp);
                                return 0;
                        }
                        break;
                default:
                        at_bol = 0;
                        break;
                }
        }

        fprintf(stderr, "unexpected EOF\n");
        exit(2);
        /* NOTREACHED */
}

static unsigned int crc;

short get2q();
long get4q();

/* read header of .hqx file */
do_q_header(macname)
char *macname;
{
        char namebuf[256];              /* big enough for both att & bsd */
        int n;
        unsigned int calc_crc, file_crc;

        crc = 0;                        /* compute a crc for the header */
        q_init();                       /* reset static variables */

        n = getq();                     /* namelength */
        n++;                            /* must read trailing null also */
        getqbuf(namebuf, n);            /* read name */
        if (macname[0] == '\0')
                macname = namebuf;

        n = strlen(macname);
        if (n > NAMEBYTES)
                n = NAMEBYTES;
        strncpy(mh.m_name, macname, n);
        mh.m_name[n] = '\0';

        getqbuf(mh.m_type, 4);
        getqbuf(mh.m_author, 4);
        mh.m_flags = get2q();
        mh.m_datalen = get4q();
        mh.m_rsrclen = get4q();

        comp_q_crc(0);
        comp_q_crc(0);
        calc_crc = crc;
        file_crc = get2q();
        verify_crc(calc_crc, file_crc);
}

do_q_fork(fname, len)
char *fname;
register long len;
{
        FILE *outf;
        register int c, i;
        unsigned int calc_crc, file_crc;

        outf = fopen(fname, "w");
        if (outf == NULL) {
                perror(fname);
                exit(-1);
        }

        crc = 0;        /* compute a crc for a fork */

        if (len)
                for (i = 0; i < len; i++) {
                        if ((c = getq()) == EOF) {
                                fprintf(stderr, "unexpected EOF\n");
                                exit(2);
                        }
                        putc(c, outf);
                }

        comp_q_crc(0);
        comp_q_crc(0);
        calc_crc = crc;
        file_crc = get2q();
        verify_crc(calc_crc, file_crc);
        fclose(outf);
}

/* verify_crc(); -- check if crc's check out */
verify_crc(calc_crc, file_crc)
unsigned int calc_crc, file_crc;
{
        calc_crc &= WORDMASK;
        file_crc &= WORDMASK;

        if (calc_crc != file_crc) {
                fprintf(stderr, "CRC error\n---------\n");
                fprintf(stderr, "CRC in file:\t0x%x\n", file_crc);
                fprintf(stderr, "calculated CRC:\t0x%x\n", calc_crc);
                exit(3);
        }
}

static int eof;
static char obuf[3];
static char *op, *oend;

/* initialize static variables for q format input */
q_init()
{
        eof = 0;
        op = obuf;
        oend = obuf + sizeof obuf;
}

/* get2q(); q format -- read 2 bytes from input, return short */
short
get2q()
{
        register int c;
        short value = 0;

        c = getq();
        value = (c & BYTEMASK) << 8;
        c = getq();
        value |= (c & BYTEMASK);

        return value;
}

/* get4q(); q format -- read 4 bytes from input, return long */
long
get4q()
{
        register int c, i;
        long value = 0L;

        for (i = 0; i < 4; i++) {
                c = getq();
                value <<= 8;
                value |= (c & BYTEMASK);
        }
        return value;
}

/* getqbuf(); q format -- read n characters from input into buf */
/*              All or nothing -- no partial buffer allowed */
getqbuf(buf, n)
register char *buf;
register int n;
{
        register int c, i;

        for (i = 0; i < n; i++) {
                if ((c = getq()) == EOF)
                        return EOF;
                *buf++ = c;
        }
        return 0;
}

#define RUNCHAR 0x90

/* q format -- return one byte per call, keeping track of run codes */
getq()
{
        register int c;

        if ((c = getq_nocrc()) == EOF)
                return EOF;
        comp_q_crc((unsigned)c);
        return c;
}

getq_nocrc()
{
        static int rep, lastc;
        int c;

        if (rep) {
                rep--;
                return lastc;
        }
        if ((c = getq_raw()) == EOF) {
                return EOF;
        }
        if (c == RUNCHAR) {
                if ((rep = getq_raw()) == EOF)
                        return EOF;
                if (rep != 0) {
                        /* already returned one, about to return another */
                        rep -= 2;
                        return lastc;
                }
                else {
                        lastc = RUNCHAR;
                        return RUNCHAR;
                }
        }
        else {
                lastc = c;
                return c;
        }
}

/* q format -- return next 8 bits from file without interpreting run codes */
getq_raw()
{
        char ibuf[4];
        register char *ip = ibuf, *iend = ibuf + sizeof ibuf;
        int c;

        if (op == obuf) {
                for (ip = ibuf; ip < iend; ip++) {
                        if ((c = get6bits()) == EOF)
                                if (ip <= &ibuf[1])
                                        return EOF;
                                else if (ip == &ibuf[2])
                                        eof = 1;
                                else
                                        eof = 2;
                        *ip = c;
                }
                obuf[0] = (ibuf[0] << 2 | ibuf[1] >> 4);
                obuf[1] = (ibuf[1] << 4 | ibuf[2] >> 2);
                obuf[2] = (ibuf[2] << 6 | ibuf[3]);
        }
        if ((eof) & (op >= &obuf[eof]))
                return EOF;
        c = *op++;
        if (op >= oend)
                op = obuf;
        return (c & BYTEMASK);
}

/*
char tr[] = "!\"#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";
             0 123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
             0                1               2               3 
trlookup is used to translate by direct lookup.  The input character
is an index into trlookup.  If the result is 0xFF, a bad char has been read.
Added by:  Dan LaLiberte, liberte@uiucdcs.Uiuc.ARPA, ihnp4!uiucdcs!liberte
*/
char trlookup[83] = {   0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFF, 0xFF,
                        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0xFF,
                        0x14, 0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                        0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
                        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0xFF,
                        0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0xFF,
                        0x2C, 0x2D, 0x2E, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF,
                        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xFF,
                        0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0xFF, 0xFF,
                        0x3D, 0x3E, 0x3F };

/* q format -- decode one byte into 6 bit binary */
get6bits()
{
        register int c;
        register int tc;

        while (1) {
                c = getc(ifp);
                switch (c) {
                case '\n':
                case '\r':
                        continue;
                case ':':
                case EOF:
                        return EOF;
                default:
                        tc = ((c-' ') < 83) ? trlookup[c-' '] : 0xff;
/*                      fprintf(stderr, "c = '%c'  tc = %4x\n", c, tc); */
                        if (tc != 0xff)
                                return (tc);
                        fprintf(stderr, "bad char: '%c'\n", c);
                        return EOF;
                }
        }
}


#define CRCCONSTANT 0x1021

comp_q_crc(c)
register unsigned int c;
{
        register int i;
        register unsigned long temp = crc;

        for (i=0; i<8; i++) {
                c <<= 1;
                if ((temp <<= 1) & WORDBIT)
                        temp = (temp & WORDMASK) ^ CRCCONSTANT;
                temp ^= (c >> 8);
                c &= BYTEMASK;
        }
        crc = temp;
}

/* old format -- process .hex and .hcx files */
do_o_header(macname, filename)
char *macname, *filename;
{
        char namebuf[256];              /* big enough for both att & bsd */
        char ibuf[BUFSIZ];
        int n;

        /* set up name for output files */
        if (macname[0] == '\0') {
                strcpy(namebuf, filename);

                /* strip directories */
                macname = search_last(namebuf, '/');
                if (macname == NULL)
                        macname = namebuf;
                else
                        macname++;

                /* strip extension */
                n = strlen(macname);
                if (n > 4) {
                    n -= 4;
                    if (macname[n] == '.' && macname[n+1] == 'h'
                                            && macname[n+3] == 'x')
                            macname[n] = '\0';
                }
        }
        n = strlen(macname);
        if (n > NAMEBYTES)
                n = NAMEBYTES;
        strncpy(mh.m_name, macname, n);
        mh.m_name[n] = '\0';

        /* read "#TYPEAUTH$flag"  line */
        if (fgets(ibuf, BUFSIZ, ifp) == NULL) {
                fprintf(stderr, "unexpected EOF\n");
                exit(2);
        }
        n = strlen(ibuf);
        if (n >= 7 && ibuf[0] == '#' && ibuf[n-6] == '$') {
                if (n >= 11)
                        strncpy(mh.m_type, &ibuf[1], 4);
                if (n >= 15)
                        strncpy(mh.m_author, &ibuf[5], 4);
                sscanf(&ibuf[n-5], "%4hx", &mh.m_flags);
        }
}

do_o_forks()
{
        char ibuf[BUFSIZ];
        int forks = 0, found_crc = 0;
        unsigned int calc_crc, file_crc;
        extern long make_file();


        crc = 0;        /* calculate a crc for both forks */

        /* create empty files ahead of time */
        close(creat(files.f_data, 0666));
        close(creat(files.f_rsrc, 0666));

        while (!found_crc && fgets(ibuf, BUFSIZ, ifp) != NULL) {
                if (forks == 0 && strncmp(ibuf, "***COMPRESSED", 13) == 0) {
                        compressed++;
                        continue;
                }
                if (strncmp(ibuf, "***DATA", 7) == 0) {
                        mh.m_datalen = make_file(files.f_data, compressed);
                        forks++;
                        continue;
                }
                if (strncmp(ibuf, "***RESOURCE", 11) == 0) {
                        mh.m_rsrclen = make_file(files.f_rsrc, compressed);
                        forks++;
                        continue;
                }
                if (compressed && strncmp(ibuf, "***CRC:", 7) == 0) {
                        found_crc++;
                        calc_crc = crc;
                        sscanf(&ibuf[7], "%x", &file_crc);
                        break;
                }
                if (!compressed && strncmp(ibuf, "***CHECKSUM:", 12) == 0) {
                        found_crc++;
                        calc_crc = crc & BYTEMASK;
                        sscanf(&ibuf[12], "%x", &file_crc);
                        file_crc &= BYTEMASK;
                        break;
                }
        }

        if (found_crc)
                verify_crc(calc_crc, file_crc);
        else {
                fprintf(stderr, "missing CRC\n");
                exit(3);
        }
}

long
make_file(fname, compressed)
char *fname;
int compressed;
{
        char ibuf[BUFSIZ];
        FILE *outf;
        register long nbytes = 0L;

        outf = fopen(fname, "w");
        if (outf == NULL) {
                perror(fname);
                exit(-1);
        }

        while (fgets(ibuf, BUFSIZ, ifp) != NULL) {
                if (strncmp(ibuf, "***END", 6) == 0)
                        break;
                if (compressed)
                        nbytes += comp_to_bin(ibuf, outf);
                else
                        nbytes += hex_to_bin(ibuf, outf);
        }

        fclose(outf);
        return nbytes;
}

comp_c_crc(c)
unsigned char c;
{
        crc = (crc + c) & WORDMASK;
        crc = ((crc << 3) & WORDMASK) | (crc >> 13);
}

comp_e_crc(c)
unsigned char c;
{
        crc += c;
}

#define SIXB(c) (((c)-0x20) & 0x3f)

comp_to_bin(ibuf, outf)
char ibuf[];
FILE *outf;
{
        char obuf[BUFSIZ];
        register char *ip = ibuf;
        register char *op = obuf;
        register int n, outcount;
        int numread, incount;

        numread = strlen(ibuf);
        ip[numread-1] = ' ';            /* zap out the newline */
        outcount = (SIXB(ip[0]) << 2) | (SIXB(ip[1]) >> 4);
        incount = ((outcount / 3) + 1) * 4;
        for (n = numread; n < incount; n++)     /* restore lost spaces */
                ibuf[n] = ' ';

        n = 0;
        while (n <= outcount) {
                *op++ = SIXB(ip[0]) << 2 | SIXB(ip[1]) >> 4;
                *op++ = SIXB(ip[1]) << 4 | SIXB(ip[2]) >> 2;
                *op++ = SIXB(ip[2]) << 6 | SIXB(ip[3]);
                ip += 4;
                n += 3;
        }

        for (n=1; n <= outcount; n++)
                comp_c_crc((unsigned)obuf[n]);

        fwrite(obuf+1, 1, outcount, outf);
        return outcount;
}

hex_to_bin(ibuf, outf)
char ibuf[];
FILE *outf;
{
        register char *ip = ibuf;
        register int n, outcount;
        int c;

        n = strlen(ibuf) - 1;
        outcount = n / 2;
        for (n = 0; n < outcount; n++) {
                c = hexit(*ip++);
                comp_e_crc((unsigned)(c = (c << 4) | hexit(*ip++)));
                fputc(c, outf);
        }
        return outcount;
}

hexit(c)
int c;
{
        if ('0' <= c && c <= '9')
                return c - '0';
        if ('A' <= c && c <= 'F')
                return c - 'A' + 10;

        fprintf(stderr, "illegal hex digit: %c", c);
        exit(4);
        /* NOTREACHED */
}

put2(bp, value)
char *bp;
short value;
{
        *bp++ = (value >> 8) & BYTEMASK;
        *bp++ = value & BYTEMASK;
}

put4(bp, value)
char *bp;
long value;
{
        register int i, c;

        for (i = 0; i < 4; i++) {
                c = (value >> 24) & BYTEMASK;
                value <<= 8;
                *bp++ = c;
        }
}


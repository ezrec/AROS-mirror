/* Un-TAR a Tape ARchive file */
/* By Andrew Church           */
/* 95ACHURCH@vax.mbhs.edu     */
/*                            */
/* GUI added by Henning Kiel  */
/* hkiel@aros.org             */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <zlib.h>
#include <sys/stat.h>

typedef struct
{
    int  type;
    long size;
    char name[100];
} FileHeader;


#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <fcntl.h>
#include <unistd.h>

extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;
int archlen = 0;
int use_gui = 0;
struct Window *win;
struct Screen *scr;
struct RastPort *rp;
char *title;
#define G_WIDTH 150
#define G_HEIGHT 16

void close_gui( )
{
    if (scr)   UnlockPubScreen(NULL, scr);
    if (win)   CloseWindow(win);
    if (title) free(title);
}

int open_gui( char *file )
{
    char ttitle[256];

    sprintf(ttitle, "Decompressing %s...", file);

    title = strdup(ttitle);

    if (title)
    {
        scr = LockPubScreen(NULL);

        if (scr)
        {
            win = OpenWindowTags(NULL,
	        WA_Title, title,
	        WA_InnerWidth, G_WIDTH,
	        WA_InnerHeight, G_HEIGHT,
	        WA_Top, scr->Height/2-G_HEIGHT/2,
	        WA_Left, scr->Width/2-G_WIDTH/2,
	        WA_GimmeZeroZero, TRUE,
	        WA_Activate, TRUE,
	        WA_DragBar, TRUE,
	        TAG_END
            );

	    if (win)
	    {
	        rp = win->RPort;

                SetAPen(rp, 3);
    	        return 1;
	    }
        }
    }

    close_gui();

    return 0;
}
void update_gui( int pos )
{
    static int oldpos = 0;

    if(pos>oldpos)
    {
	RectFill( rp, 0, 0, (pos*G_WIDTH)/archlen, G_HEIGHT );
	oldpos = pos;
    }
}

int CopyFile(gzFile *tgz, FILE *to, long size)
{
    char buf[512];

    while(size>512)
    {
	if
	(
	    gzread(tgz, buf, 512)    < 0 ||
 	    fwrite(buf, 512, 1, to) == 0
	)
	return 0;

 	size-=512;
    }

    if
    (
        gzread(tgz, buf, 512)     < 0 ||
        fwrite(buf, size, 1, to) == 0
    )
    return 0;

    return 1;
}

long OctalToDec(char *s)
{
    int i;
    long n=0;

    for(i=0;i<strlen(s);i++)
    {
        if(s[i]!=' ')
	{
            n<<=3;
	    n+=s[i]-'0';
	}
    }

    return n;
}


int ReadHeader(gzFile *tgz, FileHeader *hdr)
{
    char buf[512];

    if (gzread(tgz, buf, 512) < 0) return 0;

    memcpy(hdr->name, buf, 100);

    buf[100+8] = 0;
    hdr->type=OctalToDec(buf+100);

    buf[100+8+16+12] = 0;
    hdr->size=OctalToDec(buf+100+8+16);

    return 1;
}

int mkdirhier(char *path, int mode)
{
    char *endpath   = path;
    int   err = 0;
    int   putback = 0;

    while (*endpath != '\0')
    {
	while (*endpath != '\0' && *endpath != '/')
            endpath++;

        if (*endpath == '/')
        {
            *endpath = '\0';
	    putback = 1;
	}

	err = mkdir(path, mode);

	if (err)
	{
	    if (errno != EEXIST)
	        return err;
	}
	else
	{
	    if(use_gui==1)
	    {
	    }
	    else
	    {
		printf("Created directory %s\n", path);
	    }
	}

	if (putback)
	{
	    putback = 0;
	    *(endpath++) = '/';
	}
    }

    return 0;
}

int main(int argc, char **argv)
{
    int fd;
    gzFile *tgz;
    FileHeader hdr;
    struct stat statbuf;

    if(argc==1)
    {
        puts("Required argument missing");
        exit(20);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd==-1 || !(tgz = gzdopen(fd, "rb")))
    {
        printf("Couldn't open %s\n",argv[1]);
        return 20;
    }

    if(argc==3)
    {
	if(strcasecmp("GUI",argv[2])==0)
	{
	    use_gui = 1;
	}
    }

    /* open gui */
    if(use_gui==1)
    {
        fstat(fd, &statbuf);
        archlen = statbuf.st_size;
        if (!open_gui(argv[1]))
	{
	    printf("Couldn't open the gui\n");
	    gzclose(tgz);
	    return 20;
	}
    }
    else
    {
        printf("Extracting files from archive: %s\n",argv[1]);
    }

    while(ReadHeader(tgz, &hdr) && hdr.name[0])
    {
	char *fp = rindex(hdr.name, '/');

	if(use_gui==1)
	{
	    update_gui(lseek(fd,0L,SEEK_CUR));
	}
	if (fp)
	{
	    *fp = '\0';  /* remove trailing '/' */
	    mkdirhier(hdr.name,0);
	    *fp = '/';   /* put it back */
        }

	if (!fp || fp[1] != '\0')
	{
	    FILE *f;

	    if(use_gui==1)
	    {
	    }
	    else
	    {
		printf("Extracting (%8ld bytes) %s\n",hdr.size,hdr.name);
	    }
	    if((f=fopen(hdr.name,"wb")))
	    {
                CopyFile(tgz, f, hdr.size);
                fclose(f);
	    }
	    else
	    {
		perror("Couldn't create file");
	    }
	}
    }

    gzclose(tgz);

    if(use_gui==1)
    {
        close_gui();
    }

    return 0;
}

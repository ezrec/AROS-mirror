/* Un-TAR a Tape ARchive file */
/* By Andrew Church           */
/* 95ACHURCH@vax.mbhs.edu     */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <zlib.h>

typedef struct
{
    int  type;
    long size;
    char name[100];
} FileHeader;


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
    char s[16];

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
	    printf("Created directory %s\n", path);

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
    gzFile *tgz;
    FileHeader hdr;

    if(argc==1)
    {
        puts("Required argument missing");
        exit(20);
    }

    tgz = gzopen(argv[1], "rb");
    if (!tgz)
    {
        printf("Couldn't open %s",argv[1]);
        return 20;
    }

    printf("Extracting files from archive: %s\n",argv[1]);

    while(ReadHeader(tgz, &hdr) && hdr.name[0])
    {
	char *fp = rindex(hdr.name, '/');

	if (fp)
	{
	    *fp = '\0';  /* remove trailing '/' */
	    mkdirhier(hdr.name,0);
	    *fp = '/';   /* put it back */
        }

	if (!fp || fp[1] != '\0')
	{
	    FILE *f;

	    printf("Extracting (%8ld bytes) %s\n",hdr.size,hdr.name);
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

    return 0;
}

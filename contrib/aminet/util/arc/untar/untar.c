/* Un-TAR a Tape ARchive file */
/* By Andrew Church           */
/* 95ACHURCH@vax.mbhs.edu     */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

typedef struct
{
    int  type;
    long size;
    char name[100];
} FileHeader;


int CopyFile(FILE *tar, FILE *to, long size)
{
    char buf[512];

    while(size>512)
    {
	if
	(
	    fread(buf, 512, 1, tar)  == 0 ||
 	    fwrite(buf, 512, 1, to) == 0
	)
	return 0;

 	size-=512;
    }

    if
    (
        fread(buf, 512, 1, tar)  == 0 ||
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


int ReadHeader(FILE *tar, FileHeader *hdr)
{
    char buf[512];
    char s[16];

    if (fread(buf, 512, 1, tar) == 0) return 0;

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
    FILE *tar;
    FileHeader hdr;

    if(argc==1)
    {
        puts("Required argument missing");
        exit(20);
    }

    tar = fopen(argv[1], "r");
    if (!tar)
    {
        printf("File not found: %s\n",argv[1]);
        return 20;
    }

    printf("Extracting files from archive: %s\n",argv[1]);

    while(ReadHeader(tar, &hdr) && hdr.name[0])
    {
	char *fp = rindex(hdr.name, '/');

	if (fp)
	{
	    *fp = '\0';  /* remove trailing '/' */
	    mkdirhier(hdr.name,0);
	    *fp = '/';   /* put it back */
        }

	if (hdr.size && (!fp || fp[1] != '\0'))
	{
	    FILE *f;

	    printf("Extracting (%8ld bytes) %s\n",hdr.size,hdr.name);
	    if((f=fopen(hdr.name,"w")))
	    {
                CopyFile(tar, f, hdr.size);
                fclose(f);
	    }
	    else
	    {
		perror("Couldn't create file");
	    }
	}
    }

    fclose(tar);

    return 0;
}

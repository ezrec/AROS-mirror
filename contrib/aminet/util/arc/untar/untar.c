/* Un-TAR a Tape ARchive file */
/* By Andrew Church           */
/* 95ACHURCH@vax.mbhs.edu     */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <proto/dos.h>

struct FileHeader {
  int  type;
  long size;
  char name[100];
};

FILE *tar,*f;
long pos;
struct FileHeader hdr;

void AbsSeek(f,pos)
FILE *f;
long pos;
{
  fsetpos(f,&pos);
}

void RelSeek(f,pos)
FILE *f;
long pos;
{
  long a;

  fgetpos(f,&a); a+=pos;
  fsetpos(f,&a);
}

void CopyData(f1,f2,size)
FILE *f1,*f2;
long size;
{
  char buf[512];

  while(size>512) {
    fread(buf,512,1,f1);
    fwrite(buf,512,1,f2);
    size-=512;
  }
  fread(buf,size,1,f1);
  fwrite(buf,size,1,f2);
}

long OctalToDec(s)
char *s;
{
  int i;
  long n=0;

  for(i=0;i<strlen(s);i++)
    if(s[i]!=' ') {
      n<<=3; n+=s[i]-'0';
    }
  return n;
}

void ReadHeader(hdr)
struct FileHeader *hdr;
{
  char s[16];

  fread(hdr->name,1,100,tar);
  fread(s,1,8,tar); s[8]=0; hdr->type=OctalToDec(s);
  RelSeek(tar,16);
  fread(s,1,12,tar); s[12]=0; hdr->size=OctalToDec(s);
  RelSeek(tar,376);
  pos+=512;
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

int main(argc,argv)
int argc;
char **argv;
{
    if(argc==1)
    {
        fputs(stderr, "Required argument missing");
        exit(20);
    }

    if((tar=fopen(argv[1],"r")))
    {
        printf("Extracting files from archive: %s\n",argv[1]);
        pos=0;
        ReadHeader(&hdr);

	while(ftell(tar)!=EOF && hdr.name[0])
	{
	    char *fp = PathPart(hdr.name);

	    if (*fp == '/')
	    {
		*fp = '\0';  /* remove trailing '/' */
        	mkdirhier(hdr.name,0);
		*fp = '/';   /* put it back */
      	    }

	    if (fp != hdr.name + strlen(hdr.name) - 1)
	    {
	        printf("Extracting (%8d bytes) %s\n",hdr.size,hdr.name);
	        if((f=fopen(hdr.name,"w")))
	        {
                    CopyData(tar,f,hdr.size);
          	    fclose(f);
	        }
	        else
	        {
		    perror("Couldn't create file");
	        }
	    }

	    pos+=(hdr.size+511) & -512;
            AbsSeek(tar,pos);

	    ReadHeader(&hdr);
	 }

	 fclose(tar);

	 exit(0);

    }
    printf("File not found: %s\n",argv[1]);

    exit(20);
}

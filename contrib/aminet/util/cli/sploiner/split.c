#include "common.h"
#include "split.h"
#include "getbuff.h"

static void Breakup(FILE *fp, char *name, int filesize, int noshadow);

void split(int argc, char *argv[])
{
  FILE *fp;

  extern char *optarg;
  extern int optind;
  extern int opterr;
  int ch;
  int filesize=DISKSIZE;
  int noshadow=FALSE;
  char outname[256];
  strncpy(outname,"Part",256);
  opterr=0;

  if (argc==0) usage_split();
	  
  if ((fp = fopen(*argv,"rb")) == NULL)
    {
      printf("Can't open %s\n",*argv);
      usage_split();
    }
  else
    {
      while ((ch = getopt(argc, argv, "s:no:")) != EOF)
	switch(ch) 
	  {
	  case 's':
	    if ((filesize=atoi(optarg))<10000) 
	      {
		printf("Disksize < 10 Kb??? Go buy som bigger disks.\n");
		exit(1);
	      }
	    break;
	  case 'n':
	    noshadow=TRUE;
	    break;
	  case 'o':
	    strncpy(outname,optarg,256); /* I don't know if optarg will be */
	                                 /* free'ed, but i take no cances */
	    outname[255] = '\0';/*Just in case optarg was more than 256 chars*/
	    break;
	  default:
	    usage_split();
	  }
      argc -= optind;
      argv += optind;
      
      Breakup(fp, outname, filesize, noshadow);
      fclose(fp);
    }
}

static void Breakup(FILE *fp, char *name, int filesize, int noshadow)
{
  FILE *ofp; 
  int ext, c, insize;
  register countc, reached = 0;
  int part=0;
  char outfile[105];
  char *shadow=NULL;
  void *inbuffer, *outbuffer;

  insize = FileSize(fp);
  if (filesize > insize) filesize = insize;

  OutputName(name, outfile);

  if (noshadow==FALSE) 
    {
      shadow = (char *) malloc(filesize);
      if (shadow==NULL) 
	{ 
	  noshadow=TRUE; 
	  fprintf(stderr, "Not enough memory for shadowfile - running with\n");
	  fprintf(stderr, "noshadow (-n) option on.\n");
	}
      else for(countc=0 ; countc<filesize ; countc++) shadow[countc]=0;
    }

  printf("Filesize: %d bytes\n", filesize);
  ext=strlen(outfile)-4;
   
  outfile[ext+1] = ((part/100) + 48);
  outfile[ext+2] = ((part/10) + 48);
  outfile[ext+3] = ((part%10) + 48);
  if ((ofp = fopen(outfile, "wb")) == NULL) 
    {printf("Couldn't open %s for writing.\n",outfile); exit(1);}

  outbuffer=buffer_init(ofp, 10000);
  
  inbuffer=buffer_init(fp, 15000);

  for(countc=0 ; (c = getcbuf(inbuffer)) != EOF ; countc++)
    {
      if (countc == filesize)
	{
	  flush(outbuffer);
	  discard(outbuffer);
	  fclose(ofp);
	  part++;
	  outfile[ext+1] = ((part/100) + 48);
	  outfile[ext+2] = ((part/10) + 48);
	  outfile[ext+3] = ((part%10) + 48);
	  countc = 0;
	  if ((ofp = fopen(outfile, "wb")) == NULL) 
	    {printf("Couldn't open %s for writing.\n",outfile); exit(1);}
	  outbuffer=buffer_init(ofp, 10000);
	}
      putcbuf(c, outbuffer);
      if (((++reached)%32768) == 0)
	{
	  fprintf(stderr, "\rWriting %s: ( %-8d / %-8d )", outfile ,reached, insize);
	  fflush(stderr);
	}
      if (!noshadow) shadow[countc]=shadow[countc] ^ c;
    }
  noshadow ? printf("\nDone!                    \n"):printf("\nDone! Writing Shadowfile.\n");
  discard(inbuffer);
  flush(outbuffer);
  discard(outbuffer);
  fclose(ofp);

  if (!noshadow)
    {
      outfile[ext+1] = 'S';
      outfile[ext+2] = 'H';
      outfile[ext+3] = 'A';
      if ((ofp=fopen(outfile, "wb")) == NULL) 
	{printf("Couldn't open %s for writing.\n",outfile); exit(1);}
      if(fwrite(shadow, 1, filesize, ofp) != filesize)
	printf("Error: Writing to <%s> failed.\n",outfile);
      fclose(ofp);
      part++;
    }
  printf("Output: %d files\n",part+1);
}




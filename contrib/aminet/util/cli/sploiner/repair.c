#include "common.h"

void repair(int argc, char *argv[])
{
  FILE *fp;

  size_t maxfilesize=0, filesize, i;
  char *magic = (char *) malloc(maxfilesize);/*allocating 0 bytes can't fail*/

  for(;(argc-- > 0) && strcmp(*argv,"as") ; argv++)
    {
      if ((fp = fopen(*argv,"rb")) == NULL)
	{
	  printf("Can't open %s\n",*argv);
	  usage_repair();
	}
      else
	{
	  if ((filesize=FileSize(fp)) > maxfilesize)
	    {
	      magic = (char *) realloc(magic,filesize);
	      if(magic == NULL)
		{
		  printf("Error: Failed to allocate %lu bytes for buffer.\n"
			 ,(long unsigned int) filesize);
		  fclose(fp);
		  exit(1);
		}
	      memset((&magic[maxfilesize]), 0, filesize-maxfilesize);
	      maxfilesize = filesize;
	    }
	  fprintf(stderr, "Reading %s\n",*argv);
	  for(i=0 ; i < filesize ; i++)
	    magic[i] = magic[i] ^ getc(fp);
	  fclose(fp);
	}
    }
  if ((argc < 0) || strcmp(*argv,"as")!=0) usage_repair();
  if ((fp = fopen(*++argv,"wb")) == NULL)
    {
      printf("Can't open %s\n",*argv);
      usage_repair();
    }
  else
    {
      fprintf(stderr, "Writing %s\n",*argv);
      if(fwrite(magic, 1, maxfilesize, fp) != maxfilesize)
	printf("Error: Write to <%s> failed.\n",*argv);
      fclose(fp);
      free(magic);
    }
}

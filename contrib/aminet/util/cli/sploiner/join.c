#include "common.h"

void join(int argc, char *argv[])
{
  FILE *fp_in;
  FILE *fp_out;
  char infiles[105];
  char *outfile;
  int ext, part = 0;

  if (argc != 3) usage_join();
  OutputName(*argv, infiles);
  if (strcmp(*++argv, "as") != 0) usage_join();
  outfile = *++argv;
  ext=strlen(infiles)-4;
  
  if ((fp_out = fopen(outfile, "wb")) == NULL) 
    printf("Can't open %s for writing.\n",outfile);

  while((fp_in = fopen(infiles, "rb")) != NULL)
    {
      int i;
      char *buff=(char *) malloc(20000);
      if (buff == NULL)
	{
	  fprintf(stderr, "Captain, sensors reveal a distortion in the\n");
	  fprintf(stderr, "space/time continuum. We cannot find space\n");
	  fprintf(stderr, "for our %d bytes buffer at this time\n", 20000);
	  fprintf(stderr, "Starfleet is aborting our mission.\n");
	  exit(1);
	}
      fprintf(stderr, "Reading %s\n",infiles);
      while(((i=fread(buff, 1, 20000, fp_in))!=0) && !ferror(fp_in))
	if(fwrite(buff, 1, i, fp_out) != i)
	  {
	    fprintf(stderr, "Error: Writing to <%s> failed.\n",outfile);
	    fclose(fp_in);
	    fclose(fp_out);
	    exit(1);
	  }
      free(buff);
      fclose(fp_in);
      if(ferror(fp_in))
	{
	  fprintf(stderr, "Error: Reading from <%s> failed.\n",outfile);
	  break; /* goto "fclose(fp_out);" and then return. */
	}
      part++;
      infiles[ext+1] = ((part/100) + 48);
      infiles[ext+2] = (((part/10)%10) + 48);
      infiles[ext+3] = ((part%10) + 48);
    }
  if(infiles[ext+1] == '0' &&
     infiles[ext+2] == '0' && 
     infiles[ext+3] == '0')
    fprintf(stderr, "Couldn't find %s for reading - nothing read.\n", infiles);
  else
    fprintf(stderr, "Done writing %s\n", outfile);
  fclose(fp_out);
}


#include <stdio.h>
#include <stdlib.h>
#include "getbuff.h"

typedef struct 
{
  int buf_size;
  int current;
  int top;
  char *data;
  FILE *fp;
} buffer;

void *buffer_init(FILE *fp, size_t buf_size)
{
  buffer *buf=(buffer *) malloc(sizeof(buffer));
  if(buf != NULL)
    {
      buf->buf_size=buf_size;
      buf->current=0;
      buf->top=0;
      buf->data=(char *) malloc(buf_size);
    }
  if (buf == NULL || buf->data==NULL)
    {
      fprintf(stderr, "Captain, sensors reveal a distortion in the\n");
      fprintf(stderr, "space/time continuum. We cannot find space\n");
      fprintf(stderr, "for our %lu bytes buffer at this time\n"
	      ,(long unsigned int) buf_size);
      fprintf(stderr, "Starfleet is aborting our mission.\n");
      exit(1);
    }
  buf->fp=fp;
  return buf;
}

int getcbuf(void *buff)
{
  buffer *buf = (buffer *) buff;
  if (buf->current==buf->top)
    {
      int i;
      i=fread(buf->data, 1, buf->buf_size, buf->fp);
      buf->top=i;
      buf->current=0;
      if (i==0) return EOF;
    }
  return(((buf->data)[buf->current++]) & 0377); /* Return the lower 8 bits */
}

int putcbuf(char c, void *buff)
{
  buffer *buf = (buffer *) buff;
  if (buf->current==buf->buf_size)
    {
      int i;
      i=fwrite(buf->data, 1, buf->buf_size, buf->fp);
      buf->current=0;
      if (i!=buf->buf_size) 
	{
	  fprintf(stderr,"Write failed - aborting.\n");
	  fclose(buf->fp);
	  exit(1);
	}
    }
  (buf->data)[buf->current++]=c;
  return 0;
}

int flush(void *buff)
{
  buffer *buf = (buffer *) buff;
  int i;
  i = fwrite(buf->data, 1, buf->current, buf->fp);
  if (i!=buf->current) 
    {
      fprintf(stderr,"Write failed - aborting.\n");
      fclose(buf->fp);
      exit(1);
    }
  buf->current=0;
  return i;
}

void discard(void *buff)
{
  buffer *buf = (buffer *) buff;
  free(buf->data);
  free(buf);
}

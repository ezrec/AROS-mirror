#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>

static void convert(char *name)
{
    FILE 		*f = NULL;
    unsigned char 	last_c = '\0', *convertedbuffer, *buffer = NULL;
    int 		i, outfilelen = 0, filelen = 0;
    int			line = 1;
    
    f = fopen(name, "rb");
    if (!f)
    {
        fprintf(stderr, "Cannot open file \"%s\" for read!\n", name);
	goto done;
    }
    
    fseek(f, 0, SEEK_END);
    filelen = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (filelen < 1)
    {
        fprintf(stderr, "Error obtaining file size of \"%s\"!", name);
	goto done;
    }
    
    buffer = malloc(filelen * 2);
    if (!buffer)
    {
        fprintf(stderr, "Out of memory! Allocation of %i Bytes failed!", filelen * 2);
        goto done;
    }
    
    if (fread(buffer, 1, filelen, f) != filelen)
    {
        fprintf(stderr, "Error reading from \"%s\"!", name);
	goto done;
    }
    
    fclose(f);
    f = NULL;
    
    convertedbuffer = buffer + filelen;
    
    for(i = 0; i < filelen; i++)
    {
        unsigned char c = buffer[i];
	
	if ((c < 32) && (c != '\n') && (c != '\r') && (c != '\t'))
	{
	    printf("Warning: file: \"%s\": char < 32 (%i) !! in line %i!\n", name, c, line);
	}
	
	if (c != '\r')
	{
	    convertedbuffer[outfilelen++] = c;
	}
	
	if (c == '\n') line++;
	
	if ((c == '\\') && (last_c == '\\'))
	{
	    printf("Warning: file: \"%s\": double backslash in line %i!\n", name, line);
	}
	
	last_c = c;
    }
    
    f = fopen(name, "wb");
    if (!f)
    {
        fprintf(stderr, "Cannot open file \"%s\" for write!\n", name);
	goto done;
    }
    
    if (fwrite(convertedbuffer, 1, outfilelen, f) != outfilelen)
    {
        fprintf(stderr, "Error writing to \"%s\"!", name);
	goto done;
    }
       
done:
    if (f) fclose(f);
    if (buffer) free(buffer);
}

int main(int argc, char **argv)
{
    int i;
    
    for(i = 1;i < argc;i++)
    {
        printf("%s %s\n", argv[0], argv[i]);
        convert(argv[i]);
    }
    
    return 0;
}

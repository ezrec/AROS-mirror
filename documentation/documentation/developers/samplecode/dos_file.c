/*
    DOS read/write example
*/

#include <proto/dos.h>

#include <stdlib.h>

TEXT buffer[100];

int main(void)
{
    BPTR infile = 0;
    BPTR outfile = 0;
    
    if ( ! (infile = Open("s:startup-sequence", MODE_OLDFILE)))
    {
        goto cleanup;
    }
    
    if ( ! (outfile = Open("ram:startup-copy", MODE_NEWFILE)))
    {
        goto cleanup;
    }

    while (FGets(infile, buffer, sizeof(buffer)))
    {
        if (FPuts(outfile, buffer)) // FPuts returns 0 on error
        {
            goto cleanup;
        }
    }
    
cleanup:
    /*
        Some may argue that "goto" is bad programming style,
        but for function cleanup it still makes sense.
    */
    PrintFault(IoErr(), "Error"); // Does nothing when error code is 0
    if (infile) Close(infile);
    if (outfile) Close(outfile);
    
    return 0;
}

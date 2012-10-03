/*
    Example of an Easyrequester
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <stdlib.h>
#include <stdio.h>


int main(void)
{
    LONG result;
    
    struct EasyStruct es_simple = {
        sizeof(struct EasyStruct),      // es_StructSize
        0,                              // es_Flags
        "Window Title",                 // es_Title
        "Example for EasyRequest.\n"
        "Lines can be split with \\n.", // es_TextFormat
        "Yes|No|Cancel"                 // es_GadgetFormat
    };

    /*
        The return value is the number of the clicked button. Note to order:
        1,2,3,...,N,0. (This was done for compatibility with AutoRequest().)
    */
    result = EasyRequest(NULL, &es_simple, NULL);
    printf("Result of EasyRequest %d\n", result);

    /*
        EasyRequest allows RawDoFmt()-like formatting. RawDoFmt() expects WORD
        alignment but the C compiler aligns to LONG. Thus we have to use for
        integer variables %ld or %lu and convert to LONG or ULONG.
    */
    
    STRPTR string = "Inserted string";
    LONG longnumber = 2147483647;
    WORD wordnumber = 32767;
    
    struct EasyStruct es_rawdofmt = {
        sizeof(struct EasyStruct),
        0,
        "WindowTitle",
        "Test for RawDoFmt()-like formatting.\n\n"
        "String: %s\n"
        "WORD: %ld\n"
        "LONG: %ld\n",
        "WORD: %ld"
    };

    /*
        The arguments beginning with the 4th are used for the placeholders. First
        for es_TextFormat then es_GadgetFormat.
    */
    EasyRequest(NULL, &es_rawdofmt, NULL, string, (LONG)wordnumber, longnumber, (LONG)wordnumber);
    
    return 0;
}

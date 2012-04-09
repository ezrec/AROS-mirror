
//************************
//
// Name : Do_Trash.h
//
// Ver 2.3
//
//************************

#ifndef DO_TRASH_H
#define DO_TRASH_H 1

//************************ FORWARD DECLARACTIONS

void FindSlot(char *buffer, char *fname, BPTR trash);
BOOL ExistsFile(char *file);
BOOL ExistsInfo(char *file);

BPTR FigureTrash(char *dest);
BPTR AttemptCD(char *dir);
BPTR AttemptND(char *dir);

char *tok(char *dest, char *sour);

//************************ END OF FILE
#endif

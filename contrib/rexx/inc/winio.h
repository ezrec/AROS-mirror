/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1999/11/29 14:58:00  bnv
 * Initial revision
 *
 */

#ifndef __WINIO_H__
#define __WINIO_H__

#ifndef	SIG_IGN
#	define	SIG_IGN		NULL
#	define	SIGINT		0
#endif

void	WSetTitle(const char *title);
void	WGotoXY(int X, int Y);
int	WWhereX(void);
int	WWhereY(void);
void	WClrscr(void);
void	WClreol(void);

void	WSignal(int sig, void (*func)(int sig));

void	WWriteBuf(LPTSTR Buffer, WORD Count);
void	WWriteChar(TCHAR Ch);
BOOL	WKeyPressed(void);
int	WReadKey(void);
WORD	WReadBuf(char *Buffer, WORD Count);

void	WInitWinIO(HINSTANCE,HINSTANCE,int);
void	WExitWinIO(void);

#endif

/*
 *	Menunumbers for MemSnap's menu.
 */

#define PROJECT 0
	#define SMALL	   0
	#define LARGE	   1
	#define ABOUT	   2
	#define QUIT	   3

struct Menu *AllocMemSnapMenu(struct Window *);
void FreeMemSnapMenu(void);

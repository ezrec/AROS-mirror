#include <intuition/intuition.h>
#include <graphics/rastport.h>

void MyWrite(char * letter);
void feed(char *process);
void orient(int who);
void Keywait(void);
void rotate(int side, int direction);
void DoMenu(ULONG item);
void CloseSelect(void);
void ProcMouse(SHORT mousex, SHORT mousey);
void procgads(struct Gadget *add);
void update(int gadget);

struct GfxBase * GfxBase;
struct RastPort * rp2;
int single;
int total;
int wait;
int buffer;
int best;
int yp;
struct Window * Wdw;
struct IntuiMessage *msg;

#define BUFSIZE 100
char key[BUFSIZE];

typedef enum { FITBEZIER, DRAWPOLY, NOTACTIVE } CURMODE;

void SetFitBez();
void SetPolyDraw();

extern CURMODE CurMode;
extern void EditControl0();
extern void EditControl1();

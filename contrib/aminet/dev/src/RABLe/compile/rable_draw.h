/* Function prototypes */

void draw_circle(struct Window *, int, struct IntuiMessage *);
void draw_oval(struct Window *, int, struct IntuiMessage *);
void draw_square(struct Window *, int, struct IntuiMessage *);
void draw_line(struct Window *, struct IntuiMessage *);
void draw_free(struct Window *, struct IntuiMessage *);
void draw_rectangle(struct Window *, int, struct IntuiMessage *);
void draw_triangle(struct Window *, int, struct IntuiMessage *);

/* Routines used for geometric construction */

void make_square(struct RastPort *rp, WORD xcurrent, WORD ycurrent);
void make_fsquare(struct RastPort *rp, WORD xcurrent, WORD ycurrent);
void make_rectangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent);
void make_frectangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent);
void make_triangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent);
void make_ftriangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent);


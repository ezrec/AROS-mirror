/*
 * xhextris Copyright 1990 David Markley, dm3e@+andrew.cmu.edu, dam@cs.cmu.edu
 *
 * Permission to use, copy, modify, and distribute, this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders be used in
 * advertising or publicity pertaining to distribution of the software with
 * specific, written prior permission, and that no fee is charged for further
 * distribution of this software, or any modifications thereof.  The copyright
 * holder make no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDER DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA, PROFITS, QPA OR GPA, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* This file contains the X I/O handling routines for hextris.
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/time.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <devices/timer.h>
#include <devices/rawkeycodes.h>

#include "header.h"

struct Screen *scr;
struct Window *win;
struct ColorMap *cm;
struct RastPort *rp, *hex_rp;

void end_game();
void set_up_display(int inverse);
void set_up_window(int *width,int *height, char *argv[], int argc);
void init_piece(piece_t * piece, int upos);
int update_drop(position_t grid[MAXROW][MAXCOLUMN],
    piece_t *npiece, piece_t *piece,int *score, int* rows);
int read_high_scores(high_score_t high_scores[MAXHIGHSCORES]);
int is_high_score(char name[MAXNAMELENGTH],char userid[MAXUSERIDLENGTH],
    	    	  int score,int rows,high_score_t high_scores[MAXHIGHSCORES]);
int write_high_scores(high_score_t high_scores[MAXHIGHSCORES], char* uniqueid);
void redraw_game(position_t grid[MAXROW][MAXCOLUMN],piece_t *npiece,
    	    	 piece_t *piece, int *score, int *rows, int game_view,
		 high_score_t high_scores[MAXHIGHSCORES]);
void do_choice(char *choice,position_t grid[MAXROW][MAXCOLUMN],
    	    	piece_t *npiece,piece_t *piece,int *score,int *rows, int *game_over, int *game_view,
    	    	high_score_t high_scores[MAXHIGHSCORES]);

LONG Orange1, Red1, Blue1, Green1, Yellow1, Chocolate1, Purple1, 
  SteelBlue1, Black, White, Plum1, Maroon1, Pink1, Wheat;
LONG Orange4, Red4, Blue4, Green4, Yellow4, Chocolate4, Purple4, SteelBlue4,
  Plum4, Maroon4, Pink4, DarkSlateGrey;

struct timerequest  *TimerIO;
struct MsgPort	    *TimerMP;
ULONG	    	     TimerMask;
ULONG	    	     WinMask;

#define Orange1_R 255
#define Orange1_G 165
#define Orange1_B 0

#define Red1_R 255
#define Red1_G 0
#define Red1_B 0

#define Blue1_R 0
#define Blue1_G 0
#define Blue1_B 255

#define Green1_R 0
#define Green1_G 255
#define Green1_B 0

#define Yellow1_R 255
#define Yellow1_G 255
#define Yellow1_B 0

#define Chocolate1_R 255
#define Chocolate1_G 127
#define Chocolate1_B 36

#define Purple1_R 155
#define Purple1_G 48
#define Purple1_B 255

#define SteelBlue1_R 99
#define SteelBlue1_G 184
#define SteelBlue1_B 255

#define Black_R 0
#define Black_G 0
#define Black_B 0

#define White_R 255
#define White_G 255
#define White_B 255

#define Plum1_R 255
#define Plum1_G 187
#define Plum1_B 255

#define Maroon1_R 255
#define Maroon1_G 52
#define Maroon1_B 179

#define Pink1_R 255
#define Pink1_G 181
#define Pink1_B 197

#define Wheat_R 245
#define Wheat_G 222
#define Wheat_B 179

#define Orange4_R 139
#define Orange4_G 90
#define Orange4_B 0

#define Red4_R 139
#define Red4_G 0
#define Red4_B 0

#define Blue4_R 0
#define Blue4_G 0
#define Blue4_B 139

#define Green4_R 0
#define Green4_G 139
#define Green4_B 0

#define Yellow4_R 139
#define Yellow4_G 139
#define Yellow4_B 0

#define Chocolate4_R 139
#define Chocolate4_G 69
#define Chocolate4_B 19

#define Purple4_R 85
#define Purple4_G 26
#define Purple4_B 139

#define SteelBlue4_R 54
#define SteelBlue4_G 100
#define SteelBlue4_B 139

#define Plum4_R 139
#define Plum4_G 102
#define Plum4_B 139

#define Maroon4_R 139
#define Maroon4_G 28
#define Maroon4_B 98

#define Pink4_R 139
#define Pink4_G 99
#define Pink4_B 108

#define DarkSlateGrey_R 47
#define DarkSlateGrey_G 79
#define DarkSlateGrey_B 79

#define itoa(i,s) sprintf(s, "%d", i)

#include <aros/debug.h>

/* This is the big, ugly main X procedure...
 */

    high_score_t high_scores[MAXHIGHSCORES];
    position_t grid[MAXROW][MAXCOLUMN];
    char buffer[512];

void main(argc, argv)
int argc;
char **argv;
{
    int width, height, i, bufsize=20, inverse=0, pleasure=0, window_size = 0;
    struct timeval tp, ltp;
    struct timezone tzp;
    double intvl = 0, newintvl;
    fd_set fdst;
    struct passwd  *pwent;
/* The following variables are required by hextris */
    int score = 0, rows = 0, game_over = 1, game_view = 1, oldscore = 0;

    piece_t npiece, piece;
    char *name, *log_name;
    
    if ((log_name = (char *)getenv("USER")) == NULL)
	log_name = "AROS User";
    for (i = 1; i < argc; i++) {
	if (! strcmp(argv[i],"-rv")) {
	    inverse = 1;
	    continue;
	}
	if (! strcmp(argv[i],"-p"))
	  pleasure = 1;
    }
    if ((name = (char *)getenv("XHEXNAME")) == NULL)
      name = log_name;

    printf("\nWelcome, %s...\n",name);

    gettimeofday(&tp, &tzp);
    srandom((int)(tp.tv_usec));
#ifdef LOG
    strcpy(log_message,log_name);
    strcat(log_message,"\t");
    strcat(log_message,SYS_NAME);
    strcat(log_message,"\t1.00");
#endif
    set_up_display(inverse);
    set_up_window(&width,&height,argv,argc);
 
    redraw_game(grid,&npiece,&piece,&score,&rows,game_view,
	    	high_scores);

    while(1) {
	if (pleasure) {
	    score = 0;
	    intvl = 400000;
	} else
	  intvl = 100000+(200000-((rows > 40) ? 20 : (rows/2))*10000);
	oldscore = score;
	if (! game_over)
	  if ((game_over = update_drop(grid,&npiece,&piece,&score,&rows))) {
	      read_high_scores(high_scores);
	      if (is_high_score(name, log_name, score, rows, high_scores))
		write_high_scores(high_scores,log_name);
	      read_high_scores(high_scores);
	  }
	if (score != oldscore) {
	    intvl = 400000;
	}
	gettimeofday(&ltp, NULL);
	while (1)
	{
	    struct IntuiMessage *msg;
	    
	    gettimeofday(&tp, NULL);
	    newintvl = intvl - (((tp.tv_sec - ltp.tv_sec)*1000000)+
	      (tp.tv_usec - ltp.tv_usec));
	    if (newintvl <= 0)
	      break;
	    tp.tv_sec = 0;
	    tp.tv_usec = newintvl;

    	    {
	    	struct Message *m;
		
	    	ULONG sigs;
		
	    	TimerIO->tr_time.tv_sec = tp.tv_sec;
		TimerIO->tr_time.tv_micro = tp.tv_usec;

		SetSignal(0, TimerMask);
		
		SendIO(&TimerIO->tr_node);
		sigs = Wait(TimerMask | WinMask);
		if (!(CheckIO(&TimerIO->tr_node))) AbortIO(&TimerIO->tr_node);
		WaitIO(&TimerIO->tr_node);
		
		while((m = GetMsg(TimerMP))) ReplyMsg(m);
	    }

	    while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
	    {
	    	UWORD code = msg->Code;
		
	    	switch(msg->Class)
		{			
		    case IDCMP_REFRESHWINDOW:
		    	BeginRefresh(win);
		    	redraw_game(grid,&npiece,&piece,&score,&rows,game_view,
				high_scores);
			EndRefresh(win, TRUE);
			break;

		    case IDCMP_CLOSEWINDOW:
		    	code = RAWKEY_ESCAPE;
			/* fall through */
			
		    case IDCMP_RAWKEY:
		    	switch(code)
			{
			    case RAWKEY_LEFT:
			    	code = '4';
				break;
				
			    case RAWKEY_RIGHT:	
			    	code = '6';
			    	break;
				
			    case RAWKEY_UP:
			    	code = '8';
				break;
				
			    case RAWKEY_DOWN:
			    	code = '2';
				break;
				
			    case RAWKEY_ESCAPE:
			    	code = 'q';
				break;
				
			    default:
			    	code = 0xFFFF;
				break;
			}
			if (code == 0xFFFF) break;
			
			/* fall through */
				
		    case IDCMP_VANILLAKEY:
		    	if (code == 27) code = 'q';
		    	buffer[0] = code;
			oldscore = score;
			do_choice(buffer,grid,&npiece,&piece,&score,&rows,
				  &game_over, &game_view, high_scores);
			if ((score != oldscore) || (! score))
			{
			    intvl = 400000;
			    gettimeofday(&ltp, NULL);
			}
			break;
		    default:
			break;
		}
		
		ReplyMsg((struct Message *)msg);
		
	    }
	    
	}
    }
    
    
}

/* This sets up the basic connections to the X server, the fonts, and
 * which colors are to be foreground and background.
 */
void set_up_display(inverse)
int inverse;
{
    TimerMP = CreateMsgPort();
    if (!TimerMP)
    {
    	fprintf(stderr, "xhextris: cannot create msgport.\n");
	end_game();
    }
    
    TimerIO = (struct timerequest *)CreateIORequest(TimerMP, sizeof(*TimerIO));
    if (!TimerIO)
    {
    	fprintf(stderr, "xhextris: cannot create iorequest.\n");
	end_game();
    }
    
    if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)TimerIO, 0))
    {
    	fprintf(stderr, "xhextris: cannot open timer.device.\n");
	end_game();
    }
    
    TimerMask = 1L << TimerMP->mp_SigBit;
    
    if (!(scr = LockPubScreen(NULL)))
    {
    	fprintf(stderr, "xhextris: cannot lock public screen.\n");
	end_game();
    }
    
    cm = scr->ViewPort.ColorMap;
    
    #define OBTAIN_COL(var) \
    	var = ObtainBestPen(cm, ((ULONG)var##_R) * 0x01010101, \
	    	    	    	((ULONG)var##_G) * 0x01010101, \
				((ULONG)var##_B) * 0x01010101, \
				OBP_FailIfBad, FALSE, \
				OBP_Precision, PRECISION_IMAGE, \
				TAG_DONE)
		
    OBTAIN_COL(Red4);
    OBTAIN_COL(Green4);
    OBTAIN_COL(Black);
    OBTAIN_COL(White);
    OBTAIN_COL(Orange4);
    OBTAIN_COL(Blue4);
    OBTAIN_COL(Yellow4);
    OBTAIN_COL(Chocolate4);
    OBTAIN_COL(Purple4);
    OBTAIN_COL(SteelBlue4);
    OBTAIN_COL(Plum4);
    OBTAIN_COL(Maroon4);
    OBTAIN_COL(Pink4);
    OBTAIN_COL(Wheat);
    OBTAIN_COL(DarkSlateGrey);
    OBTAIN_COL(Red1);
    OBTAIN_COL(Green1);
    OBTAIN_COL(Blue1);
    OBTAIN_COL(Orange1);
    OBTAIN_COL(Yellow1);
    OBTAIN_COL(Chocolate1);
    OBTAIN_COL(Purple1);
    OBTAIN_COL(SteelBlue1);
    OBTAIN_COL(Plum1);
    OBTAIN_COL(Maroon1);
    OBTAIN_COL(Pink1);
    		
}

/* This sets up the window position, size, fonts, and gcs.
 */
void set_up_window(width,height,argv,argc)
int *width, *height;
char *argv[];
int argc;
{
    *width = MAXCOLUMN*40;
    *height = (MAXROW+4)*20;
    
    win = OpenWindowTags(NULL, WA_PubScreen, (IPTR)scr,
    	    	    	       WA_InnerWidth, *width + 20,
			       WA_InnerHeight, *height,
			       WA_CloseGadget, TRUE,
			       WA_DragBar, TRUE,
			       WA_DepthGadget, TRUE,
			       WA_Activate, TRUE,
			       WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY | IDCMP_VANILLAKEY,
			       WA_Title, (IPTR)WINDOWNAME,
			       TAG_DONE);
    if (!win)
    {
    	fprintf(stderr, "xhextris: cannot open window.\n");
	end_game();
    }
    
    rp = win->RPort;
    hex_rp = CloneRastPort(rp);
    if (!hex_rp)
    {
    	fprintf(stderr, "xhextris: cannot clone window rastport.\n");
	end_game();
    }
    
    WinMask = 1L << win->UserPort->mp_SigBit;
    
    SetAPen(rp, Wheat);
    SetBPen(rp, Black);
    RectFill(rp, win->BorderLeft, win->BorderTop,
    	    	 win->Width - 1 - win->BorderRight,
		 win->Height - 1 - win->BorderBottom);
    SetAPen(hex_rp, Black);
    SetBPen(hex_rp, DarkSlateGrey);	 	    
}

/* This is required by hextris!
 *
 * This clears the window.
 */
void clear_display()
{
    LONG oldpen = GetAPen(rp);
    
    SetAPen(rp, Black);
    RectFill(rp, win->BorderLeft, win->BorderTop,
    	    	 win->Width - 1 - win->BorderRight,
		 win->Height - 1 - win->BorderBottom);
    SetAPen(rp, oldpen);
}

/* This is required by hextris!
 *
 * This displays the current score and rows completed.
 */
void display_scores(score,rows)
int *score, *rows;
{
    int y_offset, x_offset;
    char scores[40];

    sprintf(scores,"Score: %6d", *score);
    y_offset = 160;
    x_offset = (MAXCOLUMN + 1) * 20;

    LONG oldpen = GetAPen(rp);
    SetAPen(rp, Black);
    RectFill(rp, win->BorderLeft + x_offset,
    	    	 win->BorderTop + y_offset,
		 win->BorderLeft + x_offset + MAXCOLUMN*20 - 1,
		 win->BorderTop + y_offset + 50 -1);
    SetAPen(rp, oldpen);
    
    Move(rp, win->BorderLeft + x_offset, win->BorderTop + y_offset);
    Text(rp, scores, strlen(scores));
    
    sprintf(scores,"Rows: %3d", *rows);
    y_offset += 20;

    Move(rp, win->BorderLeft + x_offset, win->BorderTop + y_offset);
    Text(rp, scores, strlen(scores));
   
}

/* This is required by hextris!
 *
 * This displays the help information.
 */
void display_help()
{
    int y_offset, x_offset, i;
    static char *message[] = { "The keys to press are:",
				 "J,j,4 - move left.",
				 "L,l,6 - move right.",
				 "K,k,5 - rotate ccw.",
				 "I,i,8 - rotate cw.",
				 "space,0 - drop.",
				 "N,n - new game.",
				 "P,p - pause game.",
				 "U,u - unpause game.",
				 "R,r - redisplay game.",
				 "H,h - show high scores.",
				 "G,g - show game.",
				 "Q,q - quit game.",
				 " ",
				 "--------------------",
				 "Created By:",
				 "  David Markley",
				 "Font By:",
				 "  Jon Slenk" };


    y_offset = 200;
    x_offset = (MAXCOLUMN + 1) * 20;
    for (i = 0; i < 19; i++)
    {
    	Move(rp, win->BorderLeft + x_offset, win->BorderTop + y_offset +(i * 17));
	Text(rp, message[i], strlen(message[i]));
    }
}

void display_help_score()
{
    int y_offset, x_offset, i;
    static char *message[] = { "Keys:",
				 "N,n - new game.",
				 "G,g - show game.",
				 "Q,q - quit game."};

    y_offset = 200;
    x_offset = (MAXCOLUMN + 1) * 26;
    for (i = 0; i < 4; i++)
    {
    	Move(rp, win->BorderLeft + x_offset, win->BorderTop + y_offset+(i * 17));
	Text(rp, message[i], strlen(message[i]));
    }
    
}

/* This is required by hextris!
 *
 * This displays the high score list.
 */
void display_high_scores(high_scores)
high_score_t high_scores[MAXHIGHSCORES];
{
    int y_offset, i;
    static int x_offset[5] = {5,30,150,200,300};
    static char *header[] = {"#","Name","UID","Score","Rows"};
    char message[40] = "";

    clear_display();
    y_offset = 40;
    for (i = 0; i < 5; i++)
    {
    	Move(rp, win->BorderLeft + x_offset[i], win->BorderTop + y_offset);
	Text(rp, header[i], strlen(header[i]));
    }
    
    y_offset = 60;
    for (i = 0; i < ((MAXHIGHSCORES > 40) ? 30 : MAXHIGHSCORES); i++) {
	itoa(i+1,message);
	
	Move(rp, win->BorderLeft + x_offset[0], win->BorderTop + y_offset+(i*17));
	Text(rp, message, strlen(message));

	Move(rp, win->BorderLeft + x_offset[1], win->BorderTop + y_offset+(i*17));
	Text(rp, high_scores[i].name, strlen(high_scores[i].name));
	
	strncpy(message, high_scores[i].userid, 5);
	
	Move(rp, win->BorderLeft + x_offset[2], win->BorderTop + y_offset+(i*17));
	Text(rp, message, strlen(message));
	
	itoa(high_scores[i].score,message);
	Move(rp, win->BorderLeft + x_offset[3], win->BorderTop + y_offset+(i*17));
	Text(rp, message, strlen(message));

	itoa(high_scores[i].rows,message);
	Move(rp, win->BorderLeft + x_offset[4], win->BorderTop + y_offset+(i*17));
	Text(rp, message, strlen(message));
    }

}

/* This is required by hextris!
 *
 * This displays the next piece to be dropped.
 */
void show_next_piece(npiece)
piece_t *npiece;
{
    LONG oldpen;
    
    piece_t tpiece;

    tpiece.type = npiece->type;
    tpiece.rotation = npiece->rotation;
    tpiece.row = 1;
    tpiece.column = MAXCOLUMN+6;
    
    oldpen = GetAPen(rp);
    SetAPen(rp, Black);
    RectFill(rp, win->BorderLeft + (MAXCOLUMN+3)*18,
    	    	 win->BorderTop + 0,
		 win->BorderLeft + (MAXCOLUMN+3)*18 + 150 - 1,
		 win->BorderTop + 0 + 140 - 1);
    SetAPen(rp, oldpen);
    init_piece(&tpiece, 0);
}

/* This is required by hextris!
 *
 * This draws one hex at the specified row and column specified.
 */
 
void draw_hex_shape(int x, int y, int fill)
{
    y -= 19; /* stegerg: cludge, as window does not quite fit into 800x600 screen */
    
    if (fill)
    {
    	y++;
	RectFill(hex_rp, x + 4, y, x + 15, y + 17);
	RectFill(hex_rp, x + 3, y + 2, x + 3, y + 15);
	RectFill(hex_rp, x + 2, y + 4, x + 2, y + 13);
	RectFill(hex_rp, x + 1, y + 6, x + 1, y + 11);
	RectFill(hex_rp, x, y + 8, x, y + 9);
	RectFill(hex_rp, x + 16, y + 2, x + 16, y + 15);
	RectFill(hex_rp, x + 17, y + 4, x + 17, y + 13);
	RectFill(hex_rp, x + 18, y + 6, x + 18, y + 11);
	RectFill(hex_rp, x + 19, y + 8, x + 19, y + 9);
    }
    else
    {
    	RectFill(hex_rp, x + 3, y, x + 16, y + 19);
	RectFill(hex_rp, x + 2, y + 3, x + 2, y + 16);
	RectFill(hex_rp, x + 1, y + 5, x + 1, y + 14);
	RectFill(hex_rp, x, y + 7, x, y + 12);
	RectFill(hex_rp, x + 17, y + 3, x + 17, y + 16);
	RectFill(hex_rp, x + 18, y + 5, x + 18, y + 14);
	RectFill(hex_rp, x + 19, y + 7, x + 19, y + 12);
    }
    
}
 
void draw_hex(row,column,fill,type)
int row,column,fill,type;
{
    int y_offset, x_offset;
    char hex[2];

    x_offset = 20 + column * 16;
    y_offset = 20 + row * 19 + (column & 1) * 9;
    if (fill) {
      strcpy(hex, "|");
      switch (type) {
      case 0:
      	SetAPen(hex_rp, Orange1);
	SetBPen(hex_rp, Orange4);
	break;
      case 1:
      	SetAPen(hex_rp, Red1);
	SetBPen(hex_rp, Red4);
	break;
      case 2:
      	SetAPen(hex_rp, Blue1);
	SetBPen(hex_rp, Blue4);
	break;
      case 3:
      	SetAPen(hex_rp, Green1);
	SetBPen(hex_rp, Green4);
	break;
      case 4:
      	SetAPen(hex_rp, Yellow1);
	SetBPen(hex_rp, Yellow4);
	break;
      case 5:
      	SetAPen(hex_rp, Chocolate1);
	SetBPen(hex_rp, Chocolate4);
	break;
      case 6:
      	SetAPen(hex_rp, Purple1);
	SetBPen(hex_rp, Purple4);
	break;
      case 7:
      	SetAPen(hex_rp, SteelBlue1);
	SetBPen(hex_rp, SteelBlue4);
	break;
      case 8:
      	SetAPen(hex_rp, Plum1);
	SetBPen(hex_rp, Plum4);
	break;
      case 9:
      	SetAPen(hex_rp, Maroon1);
	SetBPen(hex_rp, Maroon4);
	break;
      case 10:
      	SetAPen(hex_rp, Pink1);
	SetBPen(hex_rp, Pink4);
	break;
      }	
    } else {
      	SetAPen(hex_rp, DarkSlateGrey);
	SetBPen(hex_rp, DarkSlateGrey);
	strcpy(hex,"}");
    }
    
    draw_hex_shape(win->BorderLeft + x_offset, win->BorderTop  + y_offset, fill);

}

void draw_pos(column,fill,type)
int column,fill,type;
{
    int y_offset, x_offset;
    char hex[2];

    x_offset = 20 + column * 16;
    y_offset = 20 + (MAXROW + 2) * 19 + (column & 1) * 9;
    if (fill) {
      strcpy(hex, "|");
      switch (type) {
      case 0:
      	SetAPen(hex_rp, Orange1);
	SetBPen(hex_rp, Orange4);
	break;
      case 1:
      	SetAPen(hex_rp, Red1);
	SetBPen(hex_rp, Red4);
	break;
      case 2:
      	SetAPen(hex_rp, Blue1);
	SetBPen(hex_rp, Blue4);
	break;
      case 3:
      	SetAPen(hex_rp, Green1);
	SetBPen(hex_rp, Green4);
	break;
      case 4:
      	SetAPen(hex_rp, Yellow1);
	SetBPen(hex_rp, Yellow4);
	break;
      case 5:
      	SetAPen(hex_rp, Chocolate1);
	SetBPen(hex_rp, Chocolate4);
	break;
      case 6:
      	SetAPen(hex_rp, Purple1);
	SetBPen(hex_rp, Purple4);
	break;
      case 7:
      	SetAPen(hex_rp, SteelBlue1);
	SetBPen(hex_rp, SteelBlue4);
	break;
      case 8:
      	SetAPen(hex_rp, Plum1);
	SetBPen(hex_rp, Plum4);
	break;
      case 9:
      	SetAPen(hex_rp, Maroon1);
	SetBPen(hex_rp, Maroon4);
	break;
      case 10:
      	SetAPen(hex_rp, Pink1);
	SetBPen(hex_rp, Pink4);
	break;
      }	
    } else {
      	SetAPen(hex_rp, DarkSlateGrey);
	SetBPen(hex_rp, DarkSlateGrey);
	strcpy(hex,"}");
    }
    draw_hex_shape(win->BorderLeft + x_offset, win->BorderTop  + y_offset, fill);

}

/* This is required by hextris!
 *
 * This ends the game by closing everything down and exiting.
 */
void end_game()
{
    if (win) CloseWindow(win);
    
    if (hex_rp) FreeRastPort(hex_rp);
    
    #define RELEASE_COL(var) \
    	if (cm && (var != -1)) ReleasePen(cm, var)
	
    RELEASE_COL(Red4);
    RELEASE_COL(Green4);
    RELEASE_COL(Black);
    RELEASE_COL(White);
    RELEASE_COL(Orange4);
    RELEASE_COL(Blue4);
    RELEASE_COL(Yellow4);
    RELEASE_COL(Chocolate4);
    RELEASE_COL(Purple4);
    RELEASE_COL(SteelBlue4);
    RELEASE_COL(Plum4);
    RELEASE_COL(Maroon4);
    RELEASE_COL(Pink4);
    RELEASE_COL(Wheat);
    RELEASE_COL(DarkSlateGrey);
    RELEASE_COL(Red1);
    RELEASE_COL(Green1);
    RELEASE_COL(Orange1);
    RELEASE_COL(Blue1);
    RELEASE_COL(Yellow1);
    RELEASE_COL(Chocolate1);
    RELEASE_COL(Purple1);
    RELEASE_COL(SteelBlue1);
    RELEASE_COL(Plum1);
    RELEASE_COL(Maroon1);
    RELEASE_COL(Pink1);        
  
    if (scr) UnlockPubScreen(NULL, scr);
    
    if (TimerIO)
    {
    	CloseDevice(&TimerIO->tr_node);
	DeleteIORequest(&TimerIO->tr_node);
    }
    
    if (TimerMP)
    {
    	DeleteMsgPort(TimerMP);
    }
    
    exit(0);
}

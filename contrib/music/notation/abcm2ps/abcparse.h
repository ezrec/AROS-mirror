/*++
 * Declarations for abcparse.c.
 *
 *-*/

#define MAXVOICE 32	/* max number of voices */

#define MAXHD	8	/* max heads on one stem */
#define MAXDC	7	/* max decorations */

#define BASE_LEN 1536	/* basic note length (semibreve or whole note - same as MIDI) */

/* accidentals */
enum accidentals {
	A_NULL,		/* none */
	A_SH,		/* sharp */
	A_NT,		/* natural */
	A_FT,		/* flat */
	A_DS,		/* double sharp */
	A_DF		/* double flat */
};

/* bar types */
enum bar_type {			/* codes for different types of bars */
	B_INVIS,		/* invisible; for endings without bars and [] */
	B_SINGLE,		/* |	single bar */
	B_DOUBLE,		/* ||	thin double bar */
	B_THIN_THICK,		/* |]   thick at section end  */
	B_THICK_THIN,		/* [|   thick at section start */
	B_LREP,			/* |:	left repeat bar */
	B_RREP,			/* :|	right repeat bar */
	B_DREP,			/* ::	double repeat bar */
	B_DASH			/* :    dashed bar */
};

/* note structure */
struct deco {			/* describes decorations */
	char n;			/* number of decorations */
	unsigned char t[MAXDC];	/* type of deco */
};

struct note {		/* note or rest */
	signed char pits[MAXHD]; /* pitches for notes */
	short lens[MAXHD];	/* note lengths as multiple of BASE */
	unsigned char accs[MAXHD]; /* code for accidentals */
	char sl1[MAXHD];	/* which slur starts on this head */
	char sl2[MAXHD];	/* which slur ends on this head */
	char ti1[MAXHD];	/* flag to start tie here */
	char ti2[MAXHD];	/* flag to end tie here */
	unsigned invis:1;	/* invisible rest */
	unsigned word_end:1;	/* 1 if word ends here */
	unsigned stemless:1;	/* note with no stem */
	unsigned lyric_start:1;	/* may start a lyric here */
	char nhd;		/* number of notes in chord - 1 */
	char p_plet, q_plet, r_plet; /* data for n-plets */
	char slur_st; 		/* how many slurs start here */
	char slur_end;		/* how many slurs end here */
	struct deco dc;		/* decorations */
};

/* symbol definition */
struct abctune;
struct abcsym {
	struct abctune *tune;	/* tune */
	struct abcsym *next;	/* next symbol */
	struct abcsym *prev;	/* previous symbol */
	char type;		/* symbol type */
#define ABC_T_NULL 0
#define ABC_T_INFO 1		/* (text[0] gives the info type) */
#define ABC_T_PSCOM 2
#define ABC_T_CLEF 3
#define ABC_T_NOTE 4
#define ABC_T_REST 5
#define ABC_T_BAR 6
#define ABC_T_EOLN 7
#define ABC_T_INFO2 8		/* (info without header - H:) */
#define ABC_T_MREST 9		/* multi-measure rest */
#define ABC_T_MREP 10		/* measure repeat */
#define ABC_T_GRACE_START 11	/* start of grace note sequence */
#define ABC_T_GRACE_END 12	/* end of grace note sequence */
#define ABC_T_V_OVER 13		/* voice overlay */
	char state;		/* symbol state in file/tune */
#define ABC_S_GLOBAL 0			/* global definition */
#define ABC_S_HEAD 1			/* header definition (after X:) */
#define ABC_S_TUNE 2			/* in tune definition (after K:) */
#define ABC_S_EMBED 3			/* embedded header (between [..]) */
	short linenum;		/* line number / ABC file */
	char *text;		/* main text (INFO, PSCOM),
				 * guitar chord (NOTE, REST, BAR) */
	char *comment;		/* comment part (when keep_comment) */
	union {			/* type dependent part */
		struct {		/* K: info */
			signed char sf;		/* sharp (> 0) flats (< 0) */
			char bagpipe;
			char minor;		/* major (0) / minor (1) */
			char empty;		/* clef alone if 1 */
		} key;
		struct {		/* L: info */
			int base_length;	/* basic note length */
		} length;
		struct meter_s {	/* M: info */
			short wmeasure;		/* duration of a measure */
			short nmeter;		/* number of meter elements */
#define MAX_MEASURE 6
			struct {
				char top[8];	/* top value */
				char bot[2];	/* bottom value */
			} meter[MAX_MEASURE];
		} meter;
		struct {		/* Q: info */
			char *str;
			int length;
			int value;		/* (0: no tempo) */
		} tempo;
		struct {		/* V: info */
			char *name;		/* name */
			char *fname;		/* full name */
			char *nname;		/* nick name */
			char voice;		/* voice number */
			char merge;		/* merge with previous voice */
			signed char stem;	/* have all stems up or down */
		} voice;
		struct {		/* bar, mrest or mrep */
			struct deco dc;		/* decorations */
			enum bar_type type;
			char repeat_bar;
			char len;		/* len if mrest or mrep */
		} bar;
		struct clef_s {		/* clef */
			char type;
#define TREBLE 0
#define ALTO 1
#define BASS 2
			char line;
			signed char octave;
			signed char transpose;
		} clef;
		struct note note;	/* note, rest */
		struct {		/* user defined accent */
			unsigned char symbol;
			unsigned char value;
		} user;
		struct staff_s {	/* %%staves */
			char voice;
			char flags;
#define OPEN_BRACE 0x01
#define CLOSE_BRACE 0x02
#define OPEN_BRACKET 0x04
#define CLOSE_BRACKET 0x08
#define OPEN_PARENTH 0x10
#define CLOSE_PARENTH 0x20
#define STOP_BAR 0x40
			char *name;
		} staves[MAXVOICE];
		struct {		/* start of grace note sequence */
			char sappo;		/* short appoggiatura */
		} grace_start;
		struct {		/* voice overlay */
			char type;
#define V_OVER_S 0				/* single & */
#define V_OVER_D 1				/* && */
#define V_OVER_SS 2				/* (& */
#define V_OVER_SD 3				/* (&& */
#define V_OVER_E 4				/* )& */
		} v_over;
	} u;
};

/* tune definition */
struct abctune {
	struct abctune *next;	/* next tune */
	struct abctune *prev;	/* previous tune */
	struct abcsym *first_sym; /* first symbol */
	struct abcsym *last_sym; /* last symbol */
	int client_data;	/* client data */
};

#ifdef WIN32
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

#if defined(__cplusplus)
extern "C" {
#endif
extern char *deco_tb[];
extern int severity;
void abc_delete(struct abcsym *as);
void abc_free(struct abctune *first_tune);
void abc_init(void *alloc_f_api(int size),
	      void free_f_api(void *ptr),
	      void level_f_api(int level),
	      int client_sz_api,
	      int keep_comment_api);
void abc_insert(char *file_api,
		struct abcsym *s);
struct abcsym *abc_new(struct abctune *t,
		       unsigned char *p,
		       unsigned char *comment);
struct abctune *abc_parse(char *file_api);
char *get_str(unsigned char *d,
	      unsigned char *s,
	      int maxlen);
void note_sort(struct abcsym *s);
unsigned char *parse_deco(unsigned char *p,
			  struct deco *deco);
#if defined(__cplusplus)
}
#endif

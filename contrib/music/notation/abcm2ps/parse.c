/*
 * This file is part of abcm2ps.
 * Copyright (C) 1998-2002 Jean-François Moine
 * Adapted from abc2ps, Copyright (C) 1996,1997 Michael Methfessel
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "abcparse.h"
#include "abc2ps.h" 

struct STAFF staff_tb[MAXSTAFF];	/* staff table */
int nstaff;				/* (0..MAXSTAFF-1) */

struct lyric_fonts_s lyric_fonts[8];
int nlyric_font;

struct VOICE_S voice_tb[MAXVOICE];	/* voice table */
int nvoice;				/* (0..MAXVOICE-1) */
static struct VOICE_S *curvoice;	/* current voice while parsing */
struct VOICE_S *first_voice;		/* first voice */

static struct FORMAT dfmt;		/* format at start of tune */

static int lyric_nb;			/* current number of lyric lines */
static struct SYMBOL *lyric_start;	/* 1st note of the line for w: */
static struct SYMBOL *lyric_cont;	/* current symbol when w: continuation */

static struct SYMBOL *grace_head, *grace_tail;

static int bar_number;			/* (for %%setbarnb) */

static float multicol_start, multicol_max;	/* (for multicol) */
static float lmarg, rmarg;

#define SQ_ANY 0x00
#define SQ_CLEF 0x10
#define SQ_SIG 0x20
#define SQ_GRACE 0x30
#define SQ_BAR 0x40
#define SQ_EXTRA 0x50
#define SQ_NOTE 0x70
static unsigned char seq_tb[15] = {	/* sequence # indexed by symbol type */
	SQ_EXTRA, SQ_EXTRA, SQ_NOTE, SQ_NOTE, SQ_BAR,
	SQ_CLEF, SQ_SIG, SQ_SIG, SQ_ANY, SQ_ANY,
	SQ_NOTE, SQ_ANY, SQ_NOTE, SQ_GRACE, SQ_ANY
};

static void get_clef(struct SYMBOL *s);
static void get_key(struct SYMBOL *s);
static void get_meter(struct SYMBOL *s);
static void get_voice(struct SYMBOL *s);
static void get_note(struct SYMBOL *s);
static struct abcsym *process_pscomment(struct abcsym *as);
static void set_nplet(struct SYMBOL *s);
static void sym_link(struct SYMBOL *s);

/*  subroutines connected with parsing the input file  */

/* -- returns a new symbol at end of list -- */
struct SYMBOL *add_sym(struct VOICE_S *p_voice,
		       int type)
{
	struct SYMBOL *s;

	s = (struct SYMBOL *) getarena(sizeof *s);
	memset(s, 0, sizeof *s);
	if (p_voice->sym != 0) {
		p_voice->last_symbol->next = s;
		s->prev = p_voice->last_symbol;
	} else	p_voice->sym = s;
	p_voice->last_symbol = s;

	s->type = type;
	s->seq = seq_tb[type];
	s->voice = p_voice - voice_tb;
	s->staff = p_voice->staff;
	return s;
}

/* -- insert a symbol after a reference one -- */
struct SYMBOL *ins_sym(int type,
		       struct SYMBOL *s)	/* previous symbol */
{
	struct VOICE_S *p_voice;
	struct SYMBOL *new_s, *next;

	curvoice = p_voice = &voice_tb[s->voice];
	p_voice->last_symbol = s;
	next = s->next;
	new_s = add_sym(p_voice, type);
	if ((new_s->next = next) != 0)
		next->prev = new_s;
	return new_s;
}

/* -- duplicate the symbols of the voices appearing in many staves -- */
void voice_dup(void)
{
	struct VOICE_S *p_voice, *p_voice1;
	struct SYMBOL *s, *s2;

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		int voice;

		if (p_voice->clone < 0)
			continue;
		voice = p_voice - voice_tb;
		p_voice1 = &voice_tb[(unsigned) p_voice->clone];
		p_voice->name = p_voice1->name;
		for (s = p_voice1->sym;
		     s != 0;
		     s = s->next) {
			s2 = (struct SYMBOL *) getarena(sizeof *s2);
			memcpy(s2, s, sizeof *s2);
			if (p_voice->sym != 0) {
				p_voice->last_symbol->next = s2;
				s2->prev = p_voice->last_symbol;
			} else	p_voice->sym = s2;
			p_voice->last_symbol = s2;

			s2->voice = voice;
			s2->staff = p_voice->staff;
			s2->ly = 0;
		}
	}
}

/* -- change the accidentals in the guitar chords -- */
static void gchord_adjust(struct SYMBOL *s)
{
	char *p;
	int l;

	p = s->as.text;
	while (*p != '\0') {
		switch (*p) {
		case '#':
			if (!cfmt.freegchord)
				*p = '\201';
			break;
		case 'b':
			if (!cfmt.freegchord)
				*p = '\202';
			break;
		case '=':
			if (!cfmt.freegchord)
				*p = '\203';
			break;
		case '\\':
			p++;
			switch (*p) {
			case '\0':
				return;
			case '#':
				p[-1] = '\201';
				goto move;
			case 'b':
				p[-1] = '\202';
				goto move;
			case '=':
				p[-1] = '\203';
			move:
				l = strlen(p);
				memmove(p, p + 1, l);
				p--;
				break;
			}
			break;
		}
		p++;
	}
}

/* -- parse a lyric (vocal) definition -- */
static char *get_lyric(unsigned char *p)
{
	struct SYMBOL *s;
	char word[81], *w;
	int ln;
	int curfont;

	/* search/create the lyric font */
	for (curfont = 0; curfont < nlyric_font; curfont++) {
		if (lyric_fonts[curfont].font == cfmt.vocalfont.fnum
		    && lyric_fonts[curfont].size == cfmt.vocalfont.size)
			break;
	}
	if (curfont >= nlyric_font) {
		if (curfont >= sizeof lyric_fonts / sizeof lyric_fonts[0])
			return "Too many lyric fonts";
		lyric_fonts[curfont].font = cfmt.vocalfont.fnum;
		lyric_fonts[curfont].size = cfmt.vocalfont.size;
		nlyric_font++;
	}

	if ((s = lyric_cont) == 0) {
		if (lyric_nb >= MAXLY)
			return "Too many lyric lines";
		ln = lyric_nb++;
		s = lyric_start;
	} else	{
		lyric_cont = 0;
		ln = lyric_nb - 1;
	}

	/* scan the lyric line */
	while (*p != '\0') {
		while (isspace(*p))
			p++;
		if (*p == '\0')
			break;
		switch (*p) {
		case '|':
			while (s != 0 && (s->type != BAR
					   || s->as.u.bar.type == B_INVIS))
				s = s->next;
			if (s == 0)
				return "Not enough bar lines for lyric line";
			s = s->next;
			p++;
			continue;
		case '-':
			word[0] = '\x02';
			word[1] = '\0';
			p++;
			break;
		case '_':
			word[0] = '\x03';
			word[1] = '\0';
			p++;
			break;
		case '*':
			word[0] = *p++;
			word[1] = '\0';
			break;
		case '\\':
			if (p[1] == '\0') {
				lyric_cont = s;
				return 0;
			}
			/* fall thru */
		default:
			w = word;
			for (;;) {
				unsigned char c;

				c = *p;
				switch (c) {
				case '\0':
				case ' ':
				case '\t':
				case '_':
				case '*':
				case '|':
					break;
				case '~':
					c = ' ';
					goto addch;
				case '-':
					c = '\x02';
					goto addch;
				case '\\':
					if (p[1] == '\0')
						break;
					switch (p[1]) {
					case '~':
					case '_':
					case '*':
					case '|':
					case '-':
						c = p[1];
						p++;
						break;
					}
					/* fall thru */
				default:
				addch:
					if (w < &word[sizeof word - 1])
						*w++ = c;
					p++;
					if (c == '\x02')
						break;
					continue;
				}
				break;
			}
			*w = '\0';
			break;
		}

		/* store word in next note */
		while (s != 0 && s->type != NOTE)
			s = s->next;
		if (s == 0)
			return "Not enough notes for lyric line";
		if (word[0] != '*') {
			int l;

			if (s->ly == 0) {
				s->ly = (struct lyrics *) getarena(sizeof (struct lyrics));
				memset(s->ly, 0, sizeof (struct lyrics));
			}
			l = strlen(word) + 1 + 1;
			w = getarena(l);
			s->ly->w[ln] = w;
			w[0] = curfont;		/* the 1st char is the font index */
			strcpy(w + 1, word);
		}
		s = s->next;
	}
	while (s != 0 && s->type != NOTE)
		s = s->next;
	if (s != 0)
		return "Not enough words for lyric line";
	return 0;
}

/* -- get staves definition (%%staves) -- */
static void get_staves(struct SYMBOL *s)
{
	int i, staff, flags;
	struct staff_s *p_staff;
	struct VOICE_S *p_voice, *p_voice2;
	int dup_voice;

	/* clear, then link the voices */
	for (i = 0, p_voice = voice_tb;
	     i < MAXVOICE;
	     i++, p_voice++) {
		p_voice->clone = -1;
		p_voice->next = 0;
		p_voice->prev = 0;
		p_voice->second = 0;
		p_voice->floating = 0;
	}

	p_voice2 = 0;
	dup_voice = MAXVOICE;
	for (i = 0, p_staff = s->as.u.staves;
	     i < MAXVOICE && p_staff->name;
	     i++, p_staff++) {
		int voice;

		voice = p_staff->voice;
		p_voice = &voice_tb[voice];
		if (voice > nvoice)
			nvoice = voice;

		/* if voice already inserted, duplicate it */
		if (p_voice == p_voice2 || p_voice->next || p_voice->prev) {
			struct VOICE_S *p_voice3;

			dup_voice--;
			p_voice3 = &voice_tb[dup_voice];
			memcpy(p_voice3, p_voice, sizeof *p_voice3);
			p_voice3->clone = voice;
			p_voice3->next = 0;
			p_voice3->second = 0;
			p_voice3->floating = 0;
			p_voice = p_voice3;
			p_staff->voice = dup_voice;
		}

		/* link the voices */
		if ((p_voice->prev = p_voice2) == 0)
			first_voice = p_voice;
		else	p_voice2->next = p_voice;
		p_voice2 = p_voice;
	}

	/* define the staves */
	memset(staff_tb, 0, sizeof staff_tb);
	for (i = MAXSTAFF; --i >= 0; )
		staff_tb[i].clef.line = 2;	/* treble clef on 2nd line */
	staff = -1;
	for (i = 0, p_staff = s->as.u.staves;
	     i < MAXVOICE && p_staff->name;
	     i++, p_staff++) {
		int v;

		flags = p_staff->flags;
#if MAXSTAFF < MAXVOICE
		if (staff >= MAXSTAFF - 1) {
			ERROR(("line %d: Too many staves", s->as.linenum));
			exit(2);
		}
#endif
		staff++;

		p_voice = &voice_tb[(unsigned) p_staff->voice];

		p_voice->staff = staff;
		if (p_voice->forced_clef) {
			staff_tb[staff].forced_clef = 1;
			memcpy(&staff_tb[staff].clef, &p_voice->clef,
			       sizeof staff_tb[0].clef);
		}
		if (flags & STOP_BAR)
			staff_tb[staff].stop_bar = 1;
		if (flags & OPEN_BRACKET)
			staff_tb[staff].bracket = 1;
		if (flags & CLOSE_BRACKET)
			staff_tb[staff].bracket_end = 1;
		if (flags & OPEN_BRACE) {
			for (v = i + 1; v < MAXVOICE; v++)
				if (s->as.u.staves[v].flags & CLOSE_BRACE)
					break;
			switch (v - i) {
			case 1:				/* {a b} */
				if (flags & OPEN_PARENTH)
					goto err;
				break;
			case 2:				/* {a b c} */
				if (flags & OPEN_PARENTH
				    || (p_staff[1].flags & OPEN_PARENTH))
					break;
				i++;
				p_staff++;
				p_voice = &voice_tb[(unsigned) p_staff->voice];
				p_voice->second = 1;
				p_voice->floating = 1;
				p_voice->staff = staff;
				break;
			case 3:				/* {a b c d} */
				if (flags & OPEN_PARENTH
				    && (p_staff[2].flags & OPEN_PARENTH))
					break;
				if (flags & OPEN_PARENTH
				    || (p_staff[1].flags & OPEN_PARENTH)
				    || (p_staff[2].flags & OPEN_PARENTH))
					break;
				/* -> {(a b) (c d)} */
				p_staff->flags |= OPEN_PARENTH;
				flags |= OPEN_PARENTH;
				p_staff[1].flags |= CLOSE_PARENTH;
				p_staff[2].flags |= OPEN_PARENTH;
				p_staff[3].flags |= CLOSE_PARENTH;
				break;
			default:
				goto err;
			}
			staff_tb[staff].brace = 1;
		}
		if (flags & CLOSE_BRACE)
			staff_tb[staff].brace_end = 1;
		if (flags & OPEN_PARENTH) {
			while (i < MAXVOICE) {
				i++;
				p_staff++;
				p_voice = &voice_tb[(unsigned) p_staff->voice];
				p_voice->second = 1;
				p_voice->staff = staff;
				if (p_staff->flags & CLOSE_PARENTH)
					break;
			}
			if (p_staff->flags & STOP_BAR)
				staff_tb[staff].stop_bar = 1;
			if (p_staff->flags & CLOSE_BRACKET)
				staff_tb[staff].bracket_end = 1;
			if (p_staff->flags & CLOSE_BRACE) {
				staff_tb[staff].brace_end = 1;

				/* the lower voice must be main */
				if (p_voice->second) {
					p_voice->second = 0;
					do {
						p_voice--;
					} while (p_voice->second);
					p_voice->second = 1;
				}
			}
		}
	}
	nstaff = staff;
	return;

	/* when error, let one voice per staff */
err:
	ERROR(("line %d: %%%%staves error", s->as.linenum));
	for (p_voice = voice_tb, staff = 0;
	     p_voice != 0;
	     p_voice = p_voice->next, staff++)
		p_voice->staff = staff;
	nstaff = staff;
}

/* -- initialize the general tune characteristics of all potential voices -- */
static void voice_init(void)
{
	struct VOICE_S *p_voice;
	int	i;

	for (i = 0, p_voice = voice_tb;
	     i < MAXVOICE;
	     i++, p_voice++) {
		p_voice->sym = 0;
		p_voice->clone = -1;
		p_voice->bar_start = -1;
		p_voice->time = 0;
		p_voice->r_plet = 0;
	}
}

/* -- trim_title: move trailing "The" to front -- */
static void trim_title(unsigned char *p)
{
	unsigned char *q;
	int l;

	l = strlen(p);
	q = p + l - 3;
	if (strcmp(q, "The") != 0)
		return;
	q--;
	while (isspace(*q))
		q--;
	if (*q != ',')
		return;
	l = q - p;
	memmove(p + 4, p, l);
	memcpy(p, "The ", 4);
	p[l + 4] = '\0';
}

/* -- identify info line, store in proper place	-- */
static char *state_txt[4] = {
	"global", "header", "tune", "embedded"
};
static void get_info(struct SYMBOL *s,
		     int info_type,
		     unsigned char *p)
{
	struct ISTRUCT *inf;

	/* change global or local */
	inf = s->as.state == ABC_S_GLOBAL ? &default_info : &info;
	lvlarena(s->as.state != ABC_S_GLOBAL);

	while (isspace(*p))
	       p++;

	switch (info_type) {
	case 'A':
		inf->area = p;
		return;
	case 'B':
		inf->book = p;
		return;
	case 'C':
		if (inf->ncomp >= NCOMP)
			ERROR(("line %d: Too many composer lines",
			       s->as.linenum));
		else {
			inf->comp[inf->ncomp] = p;
			inf->ncomp++;
		}
		return;
	case 'D':
		add_text(p, TEXT_D);
		return;
	case 'd':
	case 'E':
	case 'F':
	case 'G':
		return;
	case 'H':
		add_text(p, TEXT_H);
		return;
	case 'I':
		return;
	case 'K':
		get_key(s);
		if (s->as.state != ABC_S_HEAD)
			return;
		tunenum++;
		PUT2("\n\n%% --- %s (%s) ---\n",
		     info.xref, info.title[0]);
		if (!epsf)
			bskip(cfmt.topspace);
		write_heading();
		reset_gen();
		nbar = cfmt.measurefirst;	/* measure numbering */
		curvoice = first_voice;		/* switch to the 1st voice */
		return;
	case 'L':
		return;
	case 'M':
		get_meter(s);
		return;
	case 'N':
		add_text(p, TEXT_N);
		return;
	case 'O':
		inf->orig = p;
		return;
	case 'P':
		switch (s->as.state) {
		case ABC_S_GLOBAL:
		case ABC_S_HEAD:
			inf->parts = p;
			break;
		default: {
			struct VOICE_S *old_voice;

			old_voice = curvoice;
			curvoice = first_voice;
			sym_link(s);
			s->type = PART;
			curvoice = old_voice;
			break;
		    }
		}
		return;
	case 'Q':
		if (curvoice != first_voice	/* tempo only for first voice */
		    || !cfmt.printtempo)
			return;
		switch (s->as.state) {
		case ABC_S_GLOBAL:
		case ABC_S_HEAD:
			inf->tempo = s;
			break;
		default:
			sym_link(s);
			s->type = TEMPO;
			break;
		}
		return;
	case 'R':
		inf->rhyth = p;
		return;
	case 'S':
		inf->src = p;
		return;
	case 'T':
		switch (s->as.state) {
		default:
			if (inf->ntitle >= 3) {
				ERROR(("line %d: Too many T:", s->as.linenum));
				return;
			}
			break;
		case ABC_S_GLOBAL:	/* new tune */
			if (!epsf)
				write_buffer(fout);
			dfmt = cfmt;
			memcpy(&info, &default_info, sizeof info);
			inf = &info;
			inf->xref = p;
			memcpy(&deco_tune, &deco_glob, sizeof deco_tune);
			voice_init();
			break;
		case ABC_S_TUNE:
			inf->ntitle = 0;
			break;
		}
		inf->title[inf->ntitle++] = p;
		trim_title(p);
		if (s->as.state != ABC_S_TUNE)
			return;
		output_music();
		write_inside_title();
		voice_init();
		reset_gen();		/* (display the time signature) */
		curvoice = first_voice;
		return;
	case 'U': {
		unsigned char *deco;

		deco = s->as.state == ABC_S_GLOBAL ? deco_glob : deco_tune;
		deco[s->as.u.user.symbol] = deco_intern(s->as.u.user.value);
		return;
	}
	case 'u':
		return;
	case 'V':
		get_voice(s);
		return;
	case 'w':
		if (cfmt.musiconly)
			return;
		if (s->as.state != ABC_S_TUNE
		    || lyric_start == 0)
			break;
		if ((p = get_lyric(p)) != 0)
			ERROR(("line %d: %s", s->as.linenum, p));
		return;
	case 'W':
		add_text(p, TEXT_W);
		return;
	case 'X':
		if (!epsf)
			write_buffer(fout);	/* flush stuff left from %% lines */
		dfmt = cfmt;			/* save the format at start of tune */
		memcpy(&info, &default_info, sizeof info);
		info.xref = p;
		memcpy(&deco_tune, &deco_glob, sizeof deco_tune);
		voice_init();			/* initialize all the voices */
		return;
	case 'Z':
		add_text(p, TEXT_Z);
		return;
	}
	ERROR(("line %d: %s info '%c:' not treated",
		s->as.linenum, state_txt[(int) s->as.state], info_type));
}

/* -- set head type, dots, flags for note -- */
void identify_note(struct SYMBOL *s,
		  int *p_head,
		  int *p_dots,
		  int *p_flags)
{
	int len, head, dots, flags;
	int base;

	switch (s->type) {
	case NOTE:
	case REST:
		len = s->as.u.note.lens[0];
		break;
	case TEMPO:
		len = s->as.u.tempo.length;
		break;
	default:
		bug("Unknown symbol type in identify_note", 0);
		return;
	}

	head = H_FULL;
	flags = 0;
	for (base = BREVE * 2; base > 0; base >>= 1) {
		if (len >= base)
			break;
	}
	if (len >= BREVE * 4) {
		ERROR(("line %d: Note too long",
		       s->as.linenum));
		len = base = BREVE * 2;
	}
	switch (base) {
	case BREVE * 2:
		head = H_SQUARE;
		break;
	case BREVE:
		head = cfmt.squarebreve ? H_SQUARE : H_OVAL;
		break;
	case SEMIBREVE:
		head = H_OVAL;
		break;
	case MINIM:
		head = H_EMPTY;
		break;
	case CROTCHET:
		break;
	case QUAVER:
		flags = 1;
		break;
	case SEMIQUAVER:
		flags = 2;
		break;
	case SEMIQUAVER / 2:
		flags = 3;
		break;
	case SEMIQUAVER / 4:
		flags = 4;
		break;
	default:
		ERROR(("line %d: Note too short",
		       s->as.linenum));
		len = base = SEMIQUAVER / 4;
		flags = 4;
		break;
	}

	dots = 0;
	if (len == base)
		;
	else if (2 * len == 3 * base)
		dots = 1;
	else if (4 * len == 7 * base)
		dots = 2;
	else if (8 * len == 15 * base)
		dots = 3;
	else	ERROR(("line %d: Note too much dotted",
		       s->as.linenum));

	*p_head = head;
	*p_dots = dots;
	*p_flags = flags;
}

/* -- bar ('|') -- */
static void get_bar(struct SYMBOL *s)
{
	/* remove the invisible repeat bars of the 1st voice */
	if (curvoice == first_voice
	    && s->as.u.bar.type == B_INVIS) {
		struct SYMBOL *s2;

		s2 = curvoice->last_symbol;
		if (s2->type == BAR
		    && s2->as.text == 0) {
			s2->as.text = s->as.text;
			if (s->sflags & S_EOLN)
				s2->sflags |= S_EOLN;
			s2->as.u.bar.repeat_bar = s->as.u.bar.repeat_bar;
			return;
		}
	}

	sym_link(s);
	s->type = BAR;

	if (bar_number != 0
	    && curvoice == first_voice) {
		s->u = bar_number;
		bar_number = 0;
	}

	/* the bar must be before a key signature */
	if (s->prev != 0
	    && s->prev->type == KEYSIG) {
		struct SYMBOL *s3;

		s3 = s->prev;
		curvoice->last_symbol = s3;
		s3->next = 0;
		s3->prev->next = s;
		s->prev = s3->prev;
		s->next = s3;
		s3->prev = s;
	}

	/* convert the decorations */
	if (s->as.u.bar.dc.n > 0)
		deco_cnv(&s->as.u.bar.dc);

	/* adjust the guitar chords */
	if (s->as.text != 0 && !s->as.u.bar.repeat_bar)
		gchord_adjust(s);
}

/* -- do a tune -- */
void do_tune(struct abctune *at,
	     int header_only)
{
	struct abcsym *as;
	int voice;

	/* initialize */
	memset(voice_tb, 0, sizeof voice_tb);
	voice_init();		/* initialize all the voices */
	if (!in_page)
		init_pdims();
	check_margin();
	clear_text();
	nvoice = 0;
	nstaff = 0;
	memset(staff_tb, 0, sizeof staff_tb);
	{
		int i;

		for (i = MAXVOICE; --i >= 0; ) {
			voice_tb[i].clef.line = 2;	/* treble clef on 2nd line */
			voice_tb[i].meter.nmeter = 1;
			voice_tb[i].meter.wmeasure = BASE_LEN;
			voice_tb[i].meter.meter[0].top[0] = '4';
			voice_tb[i].meter.meter[0].bot[0] = '4';
		}
		for (i = MAXSTAFF; --i >= 0; )
			staff_tb[i].clef.line = 2;
	}
	curvoice = first_voice = voice_tb;
#if 0
	clear_buffer();
#endif
	use_buffer = 1;

	/* scan the tune */
	voice = 0;
	grace_head = 0;
	for (as = at->first_sym; as != 0; as = as->next) {
		struct SYMBOL *s = (struct SYMBOL *) as;

		if (header_only
		    && as->state != ABC_S_GLOBAL)
			break;
		switch (as->type) {
		case ABC_T_INFO: {
			int info_type;
			unsigned char *p;

			if (header_only
			    && (as->text[0] == 'X'
				|| as->text[0] == 'T'))
				break;
			info_type = as->text[0];
			p = &as->text[2];
			for (;;) {
				get_info(s, info_type, p);
				if (as->next == 0
				    || as->next->type != ABC_T_INFO2)
					break;
				as = as->next;
				p = &as->text[0];
			}
			voice = curvoice - voice_tb;
			break;
		}
		case ABC_T_PSCOM:
			as = process_pscomment(as);
			voice = curvoice - voice_tb;
			break;
		case ABC_T_NOTE:
		case ABC_T_REST:
			get_note(s);
			break;
		case ABC_T_BAR:
			get_bar(s);
			break;
		case ABC_T_CLEF:
			get_clef(s);
			break;
		case ABC_T_EOLN:
			if (curvoice == first_voice
			    && curvoice->last_symbol != 0)
				curvoice->last_symbol->sflags |= S_EOLN;
			continue;
		case ABC_T_MREST:
		case ABC_T_MREP:
			sym_link(s);
			s->type = as->type == ABC_T_MREST ? MREST : MREP;
			s->len = voice_tb[s->voice].meter.wmeasure
				* s->as.u.bar.len;
			if (s->len > MAX_TIME) {
				ERROR(("line %d: Measure duration exceeds MAX_TIME",
				      s->as.linenum));
				s->len = MAX_TIME - 1;
			}
			break;
		case ABC_T_GRACE_START:
			sym_link(s);
			s->type = GRACE;
			grace_head = s;
			grace_tail = 0;
			break;
		case ABC_T_GRACE_END:
			grace_head = 0;
			continue;
		default:
			continue;
		}
		s->seq = seq_tb[s->type];
		s->time = curvoice->time;
		if (grace_head == 0)
			curvoice->time += s->len;
	}

	output_music();
	put_words();
	if (cfmt.writehistory)
		put_history();
	if (epsf && nbuf > 0) {
		write_eps();
		in_page = 0;
/*		init_pdims(); */
	} else {
		buffer_eob();
		write_buffer(fout);
	}
	if (info.xref != 0)
		cfmt = dfmt;	/* restore the format at start of tune */
}

/* -- get a clef definition (in K: or V:) -- */
static void get_clef(struct SYMBOL *s)
{
	struct VOICE_S *p_voice;

	if (s->as.prev->type == ABC_T_INFO
	    && s->as.prev->text[0] == 'V')	/* clef relative to a voice definition*/
		p_voice = &voice_tb[(int) s->as.prev->u.voice.voice];
	else	p_voice = curvoice;

	if (p_voice->sym == 0)
		memcpy(&staff_tb[(int) p_voice->staff].clef, &s->as.u.clef, /* initial clef */
		       sizeof s->as.u.clef);
	else {
		struct SYMBOL *s2;

#if 0		
		if (p_voice->clef.type != s->as.u.clef.type
		    || p_voice->clef.line != s->as.u.clef.line
		    || p_voice->clef.octave != s->as.u.clef.octave) {
#endif
			sym_link(s);
			s->type = CLEF;
			s->u = 1;	/* small clef */

			/* the clef change must be before a key signature */
			s2 = s->prev;
			if (s2->type == KEYSIG) {
				s2->next = 0;
				p_voice->last_symbol = s2;
				s->prev = s2->prev;
				if (s->prev != 0)
					s->prev->next = s;
				s->next = s2;
				s2->prev = s;
			}

			/* the clef change must be before a bar */
			s2 = s->prev;
			if (s2 != 0
			    && s2->type == BAR) {
				s2->next = s->next;
				if (s->next != 0)
					s->next->prev = s2;
				else	p_voice->last_symbol = s2;
				s->prev = s2->prev;
				if (s->prev != 0)
					s->prev->next = s;
				s->next = s2;
				s2->prev = s;
			}
/*		} */
	}
	memcpy(&p_voice->clef, &s->as.u.clef,	/* current clef */
	       sizeof p_voice->clef);
	p_voice->forced_clef = 1;		/* don't change */
	staff_tb[(int) p_voice->staff].forced_clef = 1;
}

/* -- get a key signature definition (K:) -- */
static void get_key(struct SYMBOL *s)
{
	struct VOICE_S *p_voice;
	int i;

	if (s->as.u.key.empty)
		return;			/* clef only */
	switch (s->as.state) {
	case ABC_S_GLOBAL:
		break;
	case ABC_S_HEAD: {
		for (i = MAXVOICE, p_voice = voice_tb;
		     --i >= 0;
		     p_voice++) {
			p_voice->sf = p_voice->sfp = s->as.u.key.sf;
			p_voice->bagpipe = s->as.u.key.bagpipe;
		}
		break;
	    }
	case ABC_S_TUNE:
		if (curvoice->sym == 0) {

			/* if first symbol of the first voice, change all voices */
			if (curvoice == first_voice) {
				for (i = MAXVOICE, p_voice = voice_tb;
				     --i >= 0;
				     p_voice++) {
					p_voice->sf = p_voice->sfp = s->as.u.key.sf;
					p_voice->bagpipe = s->as.u.key.bagpipe;
				}
			} else {
				curvoice->sf = curvoice->sfp = s->as.u.key.sf;
				curvoice->bagpipe = s->as.u.key.bagpipe;
			}
			break;
		}
		/* fall thru */
	case ABC_S_EMBED:
		sym_link(s);
		s->type = KEYSIG;
		s->u = curvoice->sfp;		/* old key signature */
		curvoice->sfp = s->as.u.key.sf;
		break;
	}
}

/* -- set meter from M: -- */
static void get_meter(struct SYMBOL *s)
{
	switch (s->as.state) {
	case ABC_S_GLOBAL:
		/*fixme: keep the values and apply to all tunes?? */
		break;
	case ABC_S_HEAD: {
		struct VOICE_S *p_voice;
		int i;

		for (i = MAXVOICE, p_voice = voice_tb;
		     --i >= 0;
		     p_voice++)
			memcpy(&p_voice->meter, &s->as.u.meter,
			       sizeof curvoice->meter);
		break;
	    }
	case ABC_S_TUNE:
		if (curvoice->sym == 0) {
			memcpy(&curvoice->meter, &s->as.u.meter,
			       sizeof curvoice->meter);
			reset_gen();	/* (display the time signature) */
			break;
		}
		/* fall thru */
	case ABC_S_EMBED:
		if (s->as.u.meter.nmeter == 0)
			break;		/* M:none */
		sym_link(s);
		s->type = TIMESIG;
		break;
	}
}

/* -- treat a 'V:' -- */
static void get_voice(struct SYMBOL *s)
{
	int voice;
	struct VOICE_S *p_voice;
	char t[64];

	voice = s->as.u.voice.voice;
	p_voice = &voice_tb[voice];
	if (voice > nvoice) {		/* new voice */
		struct VOICE_S *p_voice2;

		nvoice = voice;
		if (!s->as.u.voice.merge) {
#if MAXSTAFF < MAXVOICE
			if (nstaff >= MAXSTAFF - 1) {
				ERROR(("line %d: Too many staves",
					s->as.linenum));
				return;
			}
#endif
			nstaff++;
		} else	p_voice->second = 1;
		p_voice->staff = nstaff;
		for (p_voice2 = first_voice;
		     p_voice2->next != 0;
		     p_voice2 = p_voice2->next)
			;
		p_voice2->next = p_voice;
		p_voice->prev = p_voice2;
	}

	/* if in tune, switch to this voice */
	switch (s->as.state) {
	case ABC_S_TUNE:
	case ABC_S_EMBED:
		curvoice = p_voice;
		break;
	}

	/* if something has changed, update */
	if (s->as.u.voice.name != 0)
		p_voice->name = s->as.u.voice.name;
	if (s->as.u.voice.fname != 0) {
		tex_str(t, s->as.u.voice.fname, sizeof t,
			&voice_tb[voice].nmw);
		p_voice->nm = getarena(strlen(t) + 1);
		strcpy(p_voice->nm, t);
	}
	if (s->as.u.voice.nname != 0) {
		tex_str(t, s->as.u.voice.nname, sizeof t,
			&voice_tb[voice].snmw);
		p_voice->snm = getarena(strlen(t) + 1);
		strcpy(p_voice->snm, t);
	}
	if (s->as.u.voice.stem != 0)
		p_voice->stem = s->as.u.voice.stem;
}

/* -- note or rest -- */
static void get_note(struct SYMBOL *s)
{
	if (grace_head == 0) {
		sym_link(s);
		s->multi = curvoice->stem;
	} else {
		/* in a grace note sequence */
		if (grace_tail == 0)
			grace_head->grace = grace_tail = s;
		else {
			grace_tail->next = s;
			s->prev = grace_tail;
			grace_tail = s;
		}
	}
	if (s->as.type == ABC_T_NOTE)
		s->type = NOTE;
	else if (s->as.u.note.lens[0] != 0)
		s->type = REST;
	else {
		s->type = INVISIBLE;	/* 'y' = space */
		s->wl = s->wr = 3;
		s->pl = s->pr = 4;
		return;
	}

	/* set the note duration */
	if (grace_head == 0) {
		if (voice_tb[s->voice].r_plet == 0) {
			if (s->as.u.note.r_plet == 0)
				s->len = s->as.u.note.lens[0];
			else if (s->as.u.note.r_plet > 1)
				voice_tb[s->voice].r_plet = s->as.u.note.r_plet - 1;
			else {
/*fixme: should check that in abcparse*/
				ERROR(("line %d: bad 'r' value in a n-plet sequence",
				       s->as.linenum));
				s->len = s->as.u.note.lens[0];
			}
		} else {			/* in a n-plet sequence */
			if (s->as.u.note.r_plet != 0) {
				ERROR(("line %d: n-plet sequences on the same notes",
				       s->as.linenum));
			}
			if (--voice_tb[s->voice].r_plet == 0)
				set_nplet(s);
		}
	} else	s->len = s->as.u.note.lens[0];

	s->nhd = s->as.u.note.nhd;
	memcpy(s->pits, s->as.u.note.pits, sizeof s->pits);

	{
		int head, dots, nflags;

		identify_note(s, &head, &dots, &nflags);
		s->head = head;
		s->dots = dots;
		s->nflags = nflags;
	}

	if (s->as.u.note.lyric_start) {
		lyric_start = s;
		lyric_cont = 0;
		lyric_nb = 0;
	}

	/* convert the decorations */
	if (s->as.u.note.dc.n > 0)
		deco_cnv(&s->as.u.note.dc);

	/* adjust the guitar chords */
	if (s->as.text != 0)
		gchord_adjust(s);
}

/* -- process a pseudo-comment (%%) -- */
static struct abcsym *process_pscomment(struct abcsym *as)
{
	unsigned char *p;
	char *q;
	char w[32];
	float h1;
	struct SYMBOL *s = (struct SYMBOL *) as;

	p = as->text + 2;		/* skip '%%' */
	if (strncasecmp(p, "fmt ", 4) == 0)
		p += 4;			/* skip 'fmt' */

	q = p;
	p = get_str(w, p, sizeof w);
	switch (w[0]) {
	case 'b':
		if (strcmp(w, "begintext") == 0) {
			int job;

			if (epsf && as->state != ABC_S_HEAD)
				return as;
			job = OBEYLINES;
			if (*p == '\0'
			    || strncmp(p, "obeylines", 9) == 0)
				;
			else if (strncmp(p, "align", 5) == 0
				 || strncmp(p, "justify", 7) == 0)
				job = T_JUSTIFY;
			else if (strncmp(p, "skip", 4) == 0)
				job = SKIP;
			else if (strncmp(p, "ragged", 6) == 0
				 || strncmp(p, "fill", 4) == 0)
				job = T_FILL;
			else	{
				ERROR(("line %d: Bad argument for begintext: %s",
				       as->linenum, p));
			}
			output_music();
			buffer_eob();
			for (;;) {
				if (as->next == 0) {
					ERROR(("EOF found while scanning %%%%begintext"));
					return as;
				}
				as = as->next;
				p = as->text;
				if (*p == '%' && p[1] == '%') {
					p += 2;
					if (strncasecmp(p, "fmt ", 4) == 0)
						p += 4;
					if (strncmp(p, "endtext", 7) == 0) {
						if (job != SKIP)
							write_text_block(job, as->state);
						return as;
					}
				}
				if (job != SKIP)
					add_to_text_block(p, job);
			}
			/* not reached */
		}
		break;
	case 'm':
		if (strcmp(w, "multicol") == 0) {
			float bposy;

			output_music();
/*			buffer_eob();	*/
			if (strncmp(p, "start", 5) == 0) {
				multicol_start = get_bposy();
				multicol_max = multicol_start;
				lmarg = cfmt.leftmargin;
				rmarg = cfmt.rightmargin;
			} else if (strncmp(p, "new", 3) == 0) {
				bposy = get_bposy();
				bskip(bposy - multicol_start);
				if (bposy < multicol_max)
					multicol_max = bposy;
				cfmt.leftmargin = lmarg;
				cfmt.rightmargin = rmarg;
				check_margin();
			} else if (strncmp(p, "end", 3) == 0) {
				bposy = get_bposy();
				if (bposy > multicol_max)
					bskip(bposy - multicol_max);
				cfmt.leftmargin = lmarg;
				cfmt.rightmargin = rmarg;
				check_margin();
			} else {
				ERROR(("line %d: Unknown keyword '%s' in %%%%multicol",
					as->linenum, p));
			}
			return as;
		}
		break;
	case 's':
		if (strcmp(w, "setbarnb") == 0) {
			bar_number = atoi(p);
			return as;
		}
		if (strcmp(w, "sep") == 0) {
			float h2, len, lwidth;

			output_music();
			lwidth = (cfmt.landscape ? cfmt.pageheight : cfmt.pagewidth)
				- cfmt.leftmargin - cfmt.rightmargin;
			h1 = h2 = len = 0;
			if (*p != '\0') {
				h1 = scan_u(p);
				while (*p != '\0' && !isspace(*p))
					p++;
				while (isspace(*p))
					p++;
			}
			if (*p != '\0') {
				h2 = scan_u(p);
				while (*p != '\0' && !isspace(*p))
					p++;
				while (isspace(*p))
					p++;
			}
			if (*p != '\0')
				len = scan_u(p);
			if (h1 < 1)
				h1 = 0.5 * CM;
			if (h2 < 1)
				h2 = h1;
			if (len < 1)
				len = 3.0 * CM;
			bskip(h1);
			PUT2("%.1f %.1f sep0\n",
			     (lwidth - len) * 0.5,
			     (lwidth + len) * 0.5);
			bskip(h2);
			buffer_eob();
			return as;
		}
		if (strcmp(w, "staffbreak") == 0) {
			if (as->state != ABC_S_TUNE
			    && as->state != ABC_S_EMBED)
				return as;
			sym_link(s);
			s->type = FMTCHG;
			s->u = STBRK;
			if (*p != '\0')
				s->xmx = scan_u(p);
			else	s->xmx = 0.5 * CM;
			return as;
		}
		if (strcmp(w, "staves") == 0) {
			if (as->state == ABC_S_TUNE) {
				output_music();
				voice_init();
/*				reset_gen(); */
			}
			get_staves((struct SYMBOL *) as);
			curvoice = first_voice;
			return as;
		}
		break;
	case 'c':
	case 't':
		if (strcmp(w, "text") == 0 || strcmp(w, "center") == 0) {
			int job;

			if (epsf && as->state == ABC_S_GLOBAL)
				return as;
			job = w[0] == 't' ? OBEYLINES : OBEYCENTER;
			output_music();
			add_to_text_block(p, job);
			write_text_block(job, as->state);
			buffer_eob();
			return as;
		}
		break;
	case 'v':
		if (strcmp(w, "vskip") == 0) {
			output_music();
			h1 = scan_u(p);
			if (h1 < 1)
				h1 = 0.5 * CM;
			bskip(h1);
			buffer_eob();
			return as;
		}
		break;
	case 'n':
		if (strcmp(w, "newpage") == 0) {
			if (epsf)
				return as;
			output_music();
			write_buffer(fout);
			use_buffer = 0;
			write_pagebreak(fout);
			return as;
		}
		break;
	}
	if (interpret_format_line(q) == 0) {
		ops_into_fmt();
		if (strcmp(w, "leftmargin") == 0
		    || strcmp(w, "rightmargin") == 0) {
			if (as->state == ABC_S_TUNE
			    || as->state == ABC_S_EMBED) {
				struct VOICE_S *old_voice;

				old_voice = curvoice;
				curvoice = first_voice;
				sym_link(s);
				curvoice = old_voice;
				s->type = FMTCHG;
				if (w[0] == 'l') {
					s->u = LMARG;
					s->xmx = cfmt.leftmargin;
				} else {
					s->u = RMARG;
					s->xmx = cfmt.rightmargin;
				}
			} else {
				check_margin();
			}
		}
	}
	return as;
}

/* -- set the duration of notes/rests in a n-plet sequence -- */
static void set_nplet(struct SYMBOL *s)
{
	struct SYMBOL *t;
	int l, r, a, b, n, lplet;
	int time;

	l = 0;
	a = SEMIBREVE;
	for (;;) {
		if (s->type == NOTE
		    || s->type == REST) {
			l += s->as.u.note.lens[0];
			if (s->as.u.note.lens[0] < a)
				a = s->as.u.note.lens[0];
			if ((r = s->as.u.note.r_plet) != 0)
				break;
		}
		s = s->prev;
	}
	n = l / a;
	lplet = (l * s->as.u.note.q_plet) / s->as.u.note.p_plet;
	t = s;
	time = s->time;
	for (;;) {
		if (s->type == NOTE
		    || s->type == REST) {
			b = s->as.u.note.lens[0] / a;
			l = (lplet * b) / n;
			lplet -= l;
			s->len = l;
			if (--r == 0) {
				s->sflags |= S_NPLET_END;
				curvoice->time = time;
				break;
			}
			time += l;
			s->sflags |= (S_NPLET_ST|S_NPLET_END);
			n -= b;
		}
		s = s->next;
		s->time = time;
	}
	t->sflags &= ~S_NPLET_END;

	/* set the beam break on the last note */
	for (; s != 0; s = s->prev) {
		if (s->type == NOTE) {
			if (s->as.u.note.lens[0] < QUAVER)
				s->sflags |= S_BEAM_BREAK;
			break;
		}
	}
}

/* -- link a symbol in a voice -- */
static void sym_link(struct SYMBOL *s)
{
	struct VOICE_S *p_voice = curvoice;

	memset((&s->as) + 1, 0, sizeof (struct SYMBOL) - sizeof (struct abcsym));
	if (p_voice->sym != 0) {
		p_voice->last_symbol->next = s;
		s->prev = p_voice->last_symbol;
	} else	p_voice->sym = s;
	p_voice->last_symbol = s;

	s->voice = p_voice - voice_tb;
	s->staff = p_voice->staff;
}

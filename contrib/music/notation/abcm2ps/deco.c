/*++
 * Decoration handling.
 *
 * This file is part of abcm2ps.
 *
 * Copyright (C) 2000-2002, Jean-François Moine.
 *
 * Contact: mailto:moinejf@free.fr
 * Original site: http://moinejf.free.fr/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *--*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "abcparse.h"
#include "abc2ps.h"

int nbar;		/* current measure number */

static struct deco_elt {
	struct deco_elt *next;	/* next decoration */
	struct SYMBOL *s;	/* symbol */
	unsigned char t;	/* decoration index */
	unsigned char staff;	/* staff */
	char inv;		/* invert the glyph if 1 */
	char flags;
#define DE_VAL	0x01		/* put extra value if 1 */
#define DE_UP	0x02		/* above the staff */
#define DE_TREATED 0x04		/* (for !decox(! - ~decox)!) */
	float x, y;		/* x, y */
	float v;		/* extra value */
	char *str;		/* string / 0 */
} *deco_head, *deco_tail;

struct deco_def_s;
typedef void draw_f(struct deco_elt *de);
static draw_f d_arp, d_cresc, d_near, d_slide, d_upstaff,
	d_pf, d_trill;

/* decoration table */
/* !! don't change the order of the numbered items !! */
static struct deco_def_s {
	char *name;
	unsigned char func;	/* function index */
	signed char ps_func;	/* postscript function index */
	unsigned char h;	/* height */
	unsigned char wl;	/* width */
	unsigned char wr;
	unsigned char str;	/* string index - 255=deco name */
} deco_def_tb[128] = {
	{0, 0, 0, 0},			/* 0: unknown */
	{"dot", 0, 0, 4},		/* 1 */
	{"roll", 3, 10, 10},		/* 2 */
	{"fermata", 3, 6, 12},		/* 3 */
	{"emphasis", 3, 21, 6},		/* 4 */
	{"lowermordent", 3, 5, 10},	/* 5 */
	{"coda", 3, 16, 22},		/* 6 */
	{"uppermordent", 3, 4, 10},	/* 7 */
	{"segno", 3, 17, 20},		/* 8 */
	{"trill", 3, 7, 11},		/* 9 */
	{"upbow", 3, 8, 10},		/* 10 */
	{"downbow", 3, 9, 10},		/* 11 */
	{"gmark", 3, 3, 6},		/* 12 */
	{"slide", 1, 2, 3, 7},		/* 13 */
	{"tenuto", 0, 1, 4},		/* 14 */
	{"crescendo(", 7, -1, 20},	/* 15 */
	{"crescendo)", 7, 19, 20},	/* 16 */
	{"diminuendo(", 7, -1, 20},	/* 17 */
	{"diminuendo)", 7, 19, 20},	/* 18 */
	{"breath", 3, 18, 0},		/* 19 */
	{"longphrase", 3, 24, 0},	/* 20 */
	{"mediumphrase", 3, 25, 0},	/* 21 */
	{"shortphrase", 3, 26, 0},	/* 22 */
	{"invertedfermata", 3, 6, 12},	/* 23 */
	{"arpeggio", 2, 27, 0, 10},	/* 24 */
	{"invertedturn", 3, 22, 10},	/* 25 */
	{"invertedturnx", 3, 23, 10},	/* 26 */
	{"0", 3, 11, 8, 0, 0, 255},
	{"1", 3, 11, 8, 0, 0, 255},
	{"2", 3, 11, 8, 0, 0, 255},
	{"3", 3, 11, 8, 0, 0, 255},
	{"4", 3, 11, 8, 0, 0, 255},
	{"5", 3, 11, 8, 0, 0, 255},
	{"+", 3, 20, 7},
	{"accent", 3, 21, 8},
	{"D.C.", 3, 12, 12, 0, 0, 255},
	{"D.S.", 3, 12, 12, 0, 0, 255},
	{"emphasis", 3, 21, 8},
	{"fine", 3, 12, 12, 0, 0, 1},
	{"f", 6, 13, 20, 2, 2},
	{"ff", 6, 13, 20, 2, 5},
	{"fff", 6, 13, 20, 2, 8},
	{"ffff", 6, 13, 20, 2, 11},
	{"mf", 6, 13, 20, 2, 5},
	{"mordent", 3, 5, 10},
	{"open", 3, 30, 10},
	{"p", 6, 13, 20, 2, 2},
	{"pp", 6, 13, 20, 2, 5},
	{"ppp", 6, 13, 20, 2, 8},
	{"pppp", 6, 13, 20, 2, 11},
	{"pralltriller", 3, 4, 10},
	{"sfz", 6, 14, 20, 2, 8},
	{"turn", 3, 22, 10},
	{"wedge", 3, 29, 8},
	{"turnx", 3, 23, 10},
	{"trill(", 5, -1, 8},
	{"trill)", 5, 28, 8},
	{"snap", 3, 31, 14},
	{"thumb", 3, 15, 14},
};

/* c function table */
static draw_f *func_tb[] = {
	d_near,		/* 0 - near the note */
	d_slide,	/* 1 */
	d_arp,		/* 2 */
	d_upstaff,	/* 3 - tied to note */
	d_upstaff,	/* 4 (unused) */
	d_trill,	/* 5 */
	d_pf,		/* 6 - tied to staff */
	d_cresc,	/* 7 */
};

/* postscript function table */
static char *ps_func_tb[64] = {
	"stc",		/* 0: dot */
	"emb",		/* 1: tenuto */
	"sld",		/* 2: slide */
	"grm",		/* 3: gracing mark */
	"umrd",		/* 4: uppermordent */
	"lmrd",		/* 5: lowermordent */
	"hld",		/* 6: fermata */
	"trl",		/* 7: trill */
	"upb",		/* 8: upbow */
	"dnb",		/* 9: downbow */
	"cpu",		/* 10: roll */
	"fng",		/* 11: fingers */
	"dacs",		/* 12: D.C./ D.S. */
	"pf",		/* 13: p, f, pp, .. */
	"sfz",		/* 14: sfz */
	"thumb",	/* 15: thumb */
	"coda",		/* 16: coda */
	"sgno",		/* 17: segno */
	"brth",		/* 18: breath */
	"cresc",	/* 19: (de)crescendo */
	"dplus",	/* 20: plus */
	"accent",	/* 21: accent */
	"turn",		/* 22: turn */
	"turnx",	/* 23: turn with bar */
	"lphr",		/* 24: longphrase */
	"mphr",		/* 25: mediumphrase */
	"sphr",		/* 26: shortphrase */
	"arp",		/* 27: arpeggio */
	"ltr",		/* 28: long trill */
	"wedge",	/* 29: wedge */
	"opend",	/* 30: open */
	"snap",		/* 31: snap */
};

static char *str_tb[32] = {
	0,
	"FINE",		/* 1 */
};

static struct SYMBOL *first_note;	/* first note/rest of the line */

static void draw_gchord(struct SYMBOL *s, float gchy);

/* get the vertical offset of a long decoration */
static float get_y(struct SYMBOL *s,
		   int up,
		   float x,
		   float w,
		   float h)
{
	struct SYMBOL *s2, *s_start;
	int staff, start_seen;
	float y;

	s_start = s;
	start_seen = 0;
	staff = s->staff;
	y = up ? s->dc_top : s->dc_bot;
	while (s->ts_prev != 0 && s->ts_prev->x >= x)
		s = s->ts_prev;
	x += w;		/* right offset */
	if (up) {
		for (s2 = s; s2 != 0; s2 = s2->ts_next) {
			if (start_seen) {
				if (s2->x > x)
					break;
			} else if (s2 == s_start)
				start_seen = 1;
			if (s2->staff == staff) {
				if (y < s2->dc_top)
					y = s2->dc_top;
			}
		}
	} else {
		for (s2 = s; s2 != 0; s2 = s2->ts_next) {
			if (start_seen) {
				if (s2->x > x)
					break;
			} else if (s2 == s_start)
				start_seen = 1;
			if (s2->staff == staff) {
				if (y > s2->dc_bot - h)
					y = s2->dc_bot - h;
			}
		}
	}
	return y;
}

/* adjust the vertical offset of symbols under a long decoration */
static void set_y(struct SYMBOL *s,
		  int up,
		  float x,
		  float w,
		  float y)
{
	struct SYMBOL *s2, *s_start;
	int staff, start_seen;

	s_start = s;
	start_seen = 0;
	staff = s->staff;
	while (s->ts_prev != 0 && s->ts_prev->x >= x)
		s = s->ts_prev;
	x += w;		/* right offset */
	if (up) {
		for (s2 = s; s2 != 0; s2 = s2->ts_next) {
			if (start_seen) {
				if (s2->x > x)
					break;
			} else if (s2 == s_start)
				start_seen = 1;
			if (s2->staff == staff
			    && s2->dc_top < y)
				s2->dc_top = y;
		}
	} else {
		for (s2 = s; s2 != 0; s2 = s2->ts_next) {
			if (start_seen) {
				if (s2->x > x)
					break;
			} else if (s2 == s_start)
				start_seen = 1;
			if (s2->staff == staff
			    && s2->dc_bot > y)
				s2->dc_bot = y;
		}
	}
}

/* -- drawing functions -- */
/* special case for arpeggio */
static void d_arp(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	int m, h;
	float yc, xc;

	s = de->s;
	dd = &deco_def_tb[de->t];
	xc = 10;
	for (m = 0; m <= s->nhd; m++) {
		float dx;

		if (s->as.u.note.accs[m])
			dx = 8 - s->shhd[m] + s->shac[m];
		else {
			dx = 10 - s->shhd[m];
			switch (s->head) {
			case H_SQUARE:
			case H_OVAL:
				dx += 2.5;
				break;
			}
		}
		if (dx > xc)
			xc = dx;
	}
	h = s->ymx - s->ymn;
	if (h < 18)
		h = 18;
	yc = (float) (s->yav - h / 2 - 8);

	de->flags = DE_VAL;
	de->v = h;
	de->x = s->x - xc;
	de->y = yc;
}

/* special case for (de)crescendo */
static void d_cresc(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	int staff, voice, up;
	float x, y, dx;
	struct SYMBOL *s2;

	if (de->flags & DE_TREATED)
		return;
	s = de->s;
	dd = &deco_def_tb[de->t];
	voice = s->voice;

	if (dd->ps_func < 0) {		/* start of (de)crescendo */
		int t;

		/* !! works when '(de)crescendo)' is '(de)crescendo(' + 1 !! */
		t = de->t + 1;
		for (de = de->next; de != 0; de = de->next)
			if (de->t == t && de->s->voice == voice)
				break;
		if (de == 0) {		/* no end, insert one */
			de = (struct deco_elt *) getarena(sizeof *de);
			memset(de, 0, sizeof *de);
			deco_tail->next = de;
			deco_tail = de;
			de->s = s;
			de->t = t;
		}
		s2 = de->s;
		x = s->x + 4.;
		if (s->type == NOTE
		    && s->as.u.note.dc.n > 1)
			x += 6.;
	} else {			/* end without start */
		s2 = s;
		s = first_note;
		x = s->x - s->wl - 4.;
	}
	de->staff = staff = s2->staff;

	if (cfmt.exprabove
	    || (!cfmt.exprbelow
		&& staff_tb[staff].nvocal != 0))
		up = 1;
	else	up = 0;

	if (s2 == s) {			/* if no decoration end */
		dx = realwidth - x - 6.;
		if (dx < 20.) {		/* deco ends at start of line */
			x = realwidth - 20. - 6.;
			dx = 20.;
		}
	} else {
		dx = s2->x - x - 4.;
		if (s2->type == NOTE
		    && s2->as.u.note.dc.n > 1)
			dx -= 6.;
		if (dx < 20.)		/* deco ends at start of line */
			dx = 20.;
	}

	y = get_y(s, up, x, dx, dd->h);

	if (de->t == 16) {	/* 'crescendo)' */
		x += dx;
		dx = -dx;
	}

	de->flags = DE_VAL|DE_TREATED;
	if (up)
		de->flags |= DE_UP;
	de->v = dx;
	de->x = x;
	de->y = y;
	/* (set_y is done in draw_deco_staff) */
}

/* near the note (dot, tenuto) */
static void d_near(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	int y, sig;

	s = de->s;
	dd = &deco_def_tb[de->t];
	sig = s->stem > 0 ? -1 : 1;
	if (s->multi)
		sig = -sig;
	if (sig > 0)
		y = s->dc_top;
	else	y = s->dc_bot;
	y += 3 * sig;
	if (y > -3 && y < 24 + 3) {
		if (sig > 0)
			y++;
		y = (y + 5) / 6 * 6 - 3;	/* between lines */
	}
	if (s->dc_top < y + 2)
		s->dc_top = y + 2;
	if (s->dc_bot > y - 2)
		s->dc_bot = y - 2;

	de->x = s->x;
	de->y = y;
}

/* special case for piano/forte indications */
static void d_pf(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	float y, w;
	char *str;
	int up;

	s = de->s;
	dd = &deco_def_tb[de->t];
	if (cfmt.exprabove
	    || (!cfmt.exprbelow
		&& staff_tb[s->staff].nvocal != 0))
		up = 1;
	else	up = 0;

	str = dd->name;
	if (dd->str != 0 && dd->str != 255)
		str = str_tb[dd->str];
	w = dd->wl + dd->wr;
	y = get_y(s, up, s->x - dd->wl, w, dd->h);

	if (up)
		de->flags = DE_UP;
	de->v = w;
	de->x = s->x;
	de->y = y;
	de->str = str;
	/* (set_y is done in draw_deco_staff) */
}

/* special case for slide */
static void d_slide(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	int m;
	float yc, xc;

	s = de->s;
	dd = &deco_def_tb[de->t];
	yc = s->ymn;
	xc = 5;
	for (m = 0; m <= s->nhd; m++) {
		float dx, dy;

		if (s->as.u.note.accs[m])
			dx = 4 - s->shhd[m] + s->shac[m];
		else {
			dx = 5 - s->shhd[m];
			switch (s->head) {
			case H_SQUARE:
			case H_OVAL:
				dx += 2.5;
				break;
			}
		}
		dy = (float) (3 * (s->pits[m] - 18)) - yc;
		if (dy < 10 && dx > xc)
			xc = dx;
	}
	de->x = s->x - xc;
	de->y = yc;
}

/* special case for long trill */
static void d_trill(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	int staff, voice, up;
	float x, y, dx;
	struct SYMBOL *s2;

	if (de->flags & DE_TREATED)
		return;
	s = de->s;
	dd = &deco_def_tb[de->t];
	voice = s->voice;
	if (dd->ps_func < 0) {		/* start of trill */
		int t;

		/* !! works when 'trill)' is 'trill(' + 1 !! */
		t = de->t + 1;
		for (de = de->next; de != 0; de = de->next)
			if (de->t == t && de->s->voice == voice)
				break;
		if (de == 0) {		/* no end, insert one */
			de = (struct deco_elt *) getarena(sizeof *de);
			memset(de, 0, sizeof *de);
			deco_tail->next = de;
			deco_tail = de;
			de->s = s;
			de->t = t;
		}
		s2 = de->s;
		x = s->x;
		if (s->type == NOTE
		    && s->as.u.note.dc.n > 1)
			x += 6.;
	} else {			/* end without start */
		s2 = s;
		s = first_note;
		x = s->x - s->wl - 4.;
	}
	de->staff = staff = s2->staff;

	if (s2 == s) {			/* if no decoration end */
		dx = realwidth - x - 6.;
		up = !s->multi;
		if (dx < 20.) {		/* deco ends at start of line */
			x = realwidth - 20. - 6.;
			dx = 20.;
		}
	} else {
		dx = s2->x - x - 6.;
		up = !s2->multi;
		if (s2->type == NOTE)
			dx -= 6.;
		if (dx < 20.)		/* deco ends at start of line */
			dx = 20.;
	}

	y = get_y(s, up, x, dx, dd->h);
	if (up) {
		if (y < 24. + 2.)
			y = 24. + 2.;
	} else {
		if (y > -2.)
			y = -2.;
	}

	de->flags = DE_VAL|DE_TREATED;
	de->v = dx;
	de->x = x;
	de->y = y;

	if (up)
		y += dd->h;
	set_y(s, up, x, dx, y);
}

/* above the staff */
static void d_upstaff(struct deco_elt *de)
{
	struct SYMBOL *s;
	struct deco_def_s *dd;
	float x, yc;
	int inv;
	char *str;

	s = de->s;
	dd = &deco_def_tb[de->t];
	inv = 0;
	x = s->x;
	if (dd->str != 0)
		if (dd->str == 255)
			str = dd->name;
		else	str = str_tb[dd->str];
	else	str = 0;
	switch (de->t) {
	case 2:		/* roll */
		if (s->multi < 0
		    || (s->multi == 0 && s->stem > 0)) {
			yc = s->dc_bot;
			if (yc > -2.)
				yc = -2.;
			yc -= dd->h;
			s->dc_bot = yc;
			inv = 1;
		} else {
			yc = s->dc_top + 3;
			if (yc < 24. + 2.)
				yc = 24. + 2.;
			if (s->stem <= 0
			    && (s->dots == 0 || ((int) s->y % 6)))
				yc -= 2;
			s->dc_top = yc + dd->h;
		}
		break;
	case 19:	/* breath */
	case 20:	/* longphrase */
	case 21:	/* mediumphrase */
	case 22:	/* shortphrase */
		yc = 24. + 3.;
		if (s->next != 0)
			x += (s->next->x - x) * 0.4;
		else	x += 10;
		break;
	case 25:	/* invertedturn */
	case 26:	/* invertedturnx */
		inv = 1;
		/* fall thru */
	default:
		if (s->multi >= 0
		    && de->t != 23) {	/* invertedfermata */
			yc = s->dc_top;
			if (yc < 24. + 2.)
				yc = 24. + 2.;
			s->dc_top = yc + dd->h;
		} else {
			yc = s->dc_bot;
			if (yc > -2.)
				yc = -2.;
			yc -= dd->h;
			s->dc_bot = yc;
			switch (de->t) {
			case 3:		/* fermata */
			case 23:	/* invertedfermata */
				inv = 1;
				break;
			}
		}
		break;
	}
	if (inv)
		yc += dd->h;

	de->inv = inv;
	de->str = str;
	de->x = x;
	de->y = yc;
}

/* -- add a decoration - from %%deco -- */
/* syntax:
 *	%%deco <name> <c_func> <ps_func> <h> <wl> <wr> [<str>]
 */
void deco_add(char *text)
{
	struct deco_def_s *dd;
	int deco;
	char name[16];
	int c_func;
	char ps_func[16];
	int h, wl, wr, n;
	int ps_x, str_x;

	/* extract the arguments */
	if (sscanf(text, "%15s %d %15s %d %d %d%n",
		   name, &c_func, ps_func, &h, &wl, &wr, &n) != 6) {
		ERROR(("Invalid deco %s", text));
		return;
	}
	if (c_func < 0 || c_func >= sizeof func_tb / sizeof func_tb[0]) {
		ERROR(("%%%%deco: bad C function index (%s)", text));
		return;
	}
	if (h < 0 || wl < 0 || wr < 0) {
		ERROR(("%%%%deco: cannot have a negative value (%s)", text));
		return;
	}
	if (h > 30 || wl > 30 || wr > 30) {
		ERROR(("%%%%deco: abnormal h/wl/wr value (%s)", text));
		return;
	}
	text += n;
	while (isspace((unsigned char) *text))
		text++;

	/* search the decoration */
	for (deco = 1, dd = &deco_def_tb[1]; deco < 128; deco++, dd++) {
		if (dd->name == 0
		    || strcmp(dd->name, name) == 0)
			break;
	}
	if (deco == 128) {
		ERROR(("Too many decorations"));
		return;
	}

	/* search the postscript function */
	for (ps_x = 0; ps_x < sizeof ps_func_tb / sizeof ps_func_tb[0]; ps_x++) {
		if (ps_func_tb[ps_x] == 0
		    || strcmp(ps_func_tb[ps_x], ps_func) == 0)
			break;
	}
	if (ps_x == sizeof ps_func_tb / sizeof ps_func_tb[0]) {
		ERROR(("Too many postscript functions"));
		return;
	}

	/* have an index for the string */
	if (*text != '\0') {
		for (str_x = 1; str_x < sizeof str_tb / sizeof str_tb[0]; str_x++) {
			if (str_tb[str_x] == 0
			    || strcmp(str_tb[str_x], text) == 0)
				break;
		}
		if (str_x == sizeof str_tb / sizeof str_tb[0]) {
			ERROR(("Too many decoration strings"));
			return;
		}
	} else	str_x = 0;

	/* set the values */
	if (dd->name == 0)
		dd->name = strdup(name);	/* new decoration */
	dd->func = c_func;
	if (ps_func_tb[ps_x] == 0) {
		if (ps_func[0] == '-' && ps_func[1] == '\0')
			ps_x = -1;
		else	ps_func_tb[ps_x] = strdup(ps_func);
	}
	dd->ps_func = ps_x;
	dd->h = h;
	dd->wl = wl;
	dd->wr = wr;
	if (str_x != 0 && str_tb[str_x] == 0) {
		if (strcmp(text, name) == 0)
			str_x = 255;
		else	str_tb[str_x] = strdup(text);
	}
	dd->str = str_x;
}

/* -- convert the decorations -- */
void deco_cnv(struct deco *dc)
{
	int i;

	for (i = dc->n; --i >= 0; ) {
		unsigned char deco;

		deco = dc->t[i];
		if (deco < 128) {
			deco = deco_tune[deco];
			if (deco == 0
			    && dc->t[i] != 0)
				ERROR(("Notation '%c' not treated",
				       dc->t[i]));
		} else	deco = deco_intern(deco);
		dc->t[i] = deco;
	}
}

/* -- update the x position of a decoration -- */
void deco_update(struct SYMBOL *s, float dx)
{
	struct deco_elt *de;

	for (de = deco_head; de != 0; de = de->next) {
		if (de->s == s)
			de->x += dx;
	}
}

/* -- convert the external deco number to the internal one -- */
unsigned char deco_intern(unsigned char deco)
{
	char *name;

	name = deco_tb[deco - 128];
	for (deco = 1; deco < 128; deco++) {
		if (deco_def_tb[deco].name == 0)
			deco = 127;
		else if (strcmp(deco_def_tb[deco].name, name) == 0)
			break;
	}
	if (deco == 128) {
		ERROR(("Decoration %s not treated", name));
		deco = 0;
	}
	return deco;
}

/* -- adjust the symbol width -- */
float deco_width(struct SYMBOL *s)
{
	struct deco *dc;
	int i;
	float wl;

	wl = 0;
	if (s->type == BAR)
		dc = &s->as.u.bar.dc;
	else	dc = &s->as.u.note.dc;
	for (i = dc->n; --i >= 0; ) {
		struct deco_def_s *dd;

		dd =  &deco_def_tb[dc->t[i]];
		switch (dd->func) {
		case 1: wl += 7.; break;	/* slide */
		case 6: wl += 10.; break;	/* arpeggio */
		}
	}
	return wl;
}

/* -- draw the decorations -- */
/* (the staves are defined) */
void draw_all_deco(void)
{
	struct deco_elt *de;

	for (de = deco_head; de != 0; de = de->next) {
		struct deco_def_s *dd;
		int f;

		dd = &deco_def_tb[de->t];
		if ((f = dd->ps_func) < 0)
			continue;

		if (de->flags & DE_VAL)
			PUT1("%.1f ", de->v);
		if (de->str)
			PUT1("(%s) ", de->str);
		PUT2("%.1f %1.2f ",
		     de->x,
		     de->y + staff_tb[de->staff].y);
		if (de->inv)
			PUT1("gsave 1 -1 scale neg %s grestore\n", ps_func_tb[f]);
		else	PUT1("%s\n", ps_func_tb[f]);
	}
}

/* -- draw the decorations near the note (only) -- */
/* (the staves are not yet defined) */
/* (this function must be called first as it builds the deco element table) */
void draw_deco_near(void)
{
	struct SYMBOL *s;
	int k;
	struct deco *dc;
	unsigned char deco;
	struct deco_def_s *dd;
	struct deco_elt *de;
	struct SYMBOL *first;

	deco_head = deco_tail = 0;
	first = 0;
	for (s = first_voice->sym; s != 0; s = s->ts_next) {
		switch (s->type) {
		case BAR:
			if (s->as.u.bar.dc.n == 0)
				continue;
			dc = &s->as.u.bar.dc;
			break;
		case NOTE:
		case REST:
		case MREST:
			if (first == 0)
				first = s;
			if (s->as.u.note.dc.n == 0)
				continue;
			dc = &s->as.u.note.dc;
			break;
		default:
			continue;
		}

		for (k = dc->n; --k >= 0; ) {
			deco = dc->t[k];
			if (deco == 0)
				continue;
			dd = &deco_def_tb[deco];

			/* memorize the decorations */
			de = (struct deco_elt *) getarena(sizeof *de);
			memset(de, 0, sizeof *de);
			if (deco_head == 0)
				deco_head = de;
			else	deco_tail->next = de;
			deco_tail = de;
			de->s = s;
			de->t = deco;
			de->staff = s->staff;

			if (dd->func >= 3)	/* if not near the note */
				continue;
			if (s->type != NOTE) {
				ERROR(("Cannot have a %s on a rest or a bar",
				       dd->name));
				continue;
			}
			func_tb[dd->func](de);
		}
	}
	first_note = first;
}

/* -- draw more decorations tied to the note -- */
/* (the staves are not yet defined) */
void draw_deco_note(void)
{
	struct deco_elt *de;

	for (de = deco_head; de != 0; de = de->next) {
		struct deco_def_s *dd;
		int f;

		dd = &deco_def_tb[de->t];
		f = dd->func;
		if (f < 3 || f >= 6)
			continue;	/* not tied to the note */
		func_tb[f](de);
	}
}

/* -- draw the decorations tied to the staff -- */
/* (the staves are not yet defined) */
void draw_deco_staff(void)
{
	struct SYMBOL *s;
	struct VOICE_S *p_voice;
	float x, y;
	struct deco_elt *de;
	int voice;
	int some_gchord;
	struct {
		float ymin, ymax;
	} minmax[MAXSTAFF];

	/* set the top and bottom of all symbols out of the staves */
	for (s = first_voice->sym; s != 0; s = s->ts_next) {
		if (s->dc_top < 24. + 2.)
			s->dc_top = 24. + 2.;
		if (s->dc_bot > -2.)
			s->dc_bot = -2.;
	}

	/* search the vertical offset for the guitar chords */
	memset(minmax, 0, sizeof minmax);
	some_gchord = 0;
	for (s = first_voice->sym; s != 0; s = s->ts_next) {
		float w;
		char *p;

		if (s->as.text == 0)
			continue;
		switch (s->type) {
		case NOTE:
		case REST:
		case MREST:
			break;
		case BAR:
			if (!s->as.u.bar.repeat_bar)
				break;
		default:
			continue;
		}
		some_gchord = 1;
		w = cwid('a') * cfmt.gchordfont.size * 1.1;
		if ((p = strstr(s->as.text, "\\n")) != 0)
			w *= p - s->as.text;
		else	w *= strlen(s->as.text);
		y = get_y(s, 1, s->x, w, 0);
		if (y > minmax[s->staff].ymax)
			minmax[s->staff].ymax = y;
	}

	/* draw the guitar chords if any */
	if (some_gchord) {
		set_font(&cfmt.gchordfont);
		for (s = first_voice->sym; s != 0; s = s->ts_next) {
			if (s->as.text == 0)
				continue;
			switch (s->type) {
			case NOTE:
			case REST:
			case MREST:
				break;
			case BAR:
				if (!s->as.u.bar.repeat_bar)
					break;
			default:
				continue;
			}
			draw_gchord(s, minmax[s->staff].ymax + 4);
		}
	}

	/* compute the max number of lyrics */
	if (!cfmt.musiconly) {
		for (s = first_voice->sym; s != 0; s = s->ts_next) {
			struct lyrics *ly;
			int nlyric;

			if ((ly = s->ly) == 0)
				continue;
			for (nlyric = MAXLY; --nlyric >= 0; )
				if (ly->w[nlyric] != 0)
					break;
			nlyric++;
			voice = s->voice;
			if (voice_tb[voice].nvocal < nlyric)
				voice_tb[voice].nvocal = nlyric;
		}
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
			staff_tb[p_voice->staff].nvocal += p_voice->nvocal;
	}

	/* draw the repeat bars */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		if (p_voice->second)
			continue;
		for (s = p_voice->sym; s != 0; s = s->next) {
			struct SYMBOL *s1, *s2;
			int nmes;
			float x2, y2;
			int n;

			if (s->type != BAR
			    || !s->as.u.bar.repeat_bar)
				continue;		/* not a repeat bar */
			s1 = s;
			x = x2 = s->x;			/* (compiler warning) */
			n = 2;

			/* if 1st repeat, search the 2nd */
			if (s1->as.text[0] == '1') {
				nmes = -1;
				for (s = s->next; s != 0; s = s->next) {
					x = s->x;
					if (s->type == BAR) {
						nmes++;
						if (s->as.u.bar.repeat_bar)
							break;
					}
				}
				s2 = s;
				if (s != 0) {
					if (s->prev->type == BAR)
						x = s->prev->x;
					x -= 10.;
					n = 1;
				} else {
					x -= 8.;
					if (x > s1->x + 80.)
						x = s1->x + 80.;
					if (x < s1->x + 10.)	/* ?? */
						x = s1->x + 10.;
				}
	
				/* if 1st repeat bar, search the end of 2nd one */
				if (n == 1) {
					for (s = s->next; s != 0; s = s->next) {
						x2 = s->x;
						if (s->type == BAR
						    && --nmes < 0)
							break;
					}
				} else	{
					x2 = x;
				}
			} else {

				/* 2nd repeat without 1st one */
				x2 = x = s1->x + 80.;
				s2 = 0;
			}

			/* search the vertical offset */
			y = get_y(s1, 1, s1->x, x2 - s1->x, 0);
			if (y < 24. + 6. + 20.)
				y = 24. + 6. + 20.;

			/* have room for the repeat numbers */
			if (s1->next != 0) {
				y2 = s1->next->dc_top;
				if (s1->next->as.text != 0)	/* if guitar chord */
					y2 -= cfmt.gchordfont.size; /* got already */
				if (y2 > 24. + 6. + 4.)
					y = y2 + 10. + 4.;
			}
			if (n == 1 && s2->next != 0) {
				y2 = s2->next->dc_top;
				if (s2->next->as.text != 0)	/* if guitar chord */
					y2 -= cfmt.gchordfont.size; /* got already */
				if (y2 > 24. + 6. + 4.)
					y = y2 + 10. + 4.;
			}

			/* draw */
			PUT1("(%s) ", s1->as.text);
			PUT5("%.1f %.1f \x01%c%5.2f end%d\n",
			     x - s1->x, s1->x, '0' + s1->staff, y, n);
			if (n == 1) {
				PUT1("(%s) ", s2->as.text);
				PUT4("%.1f %.1f \x01%c%5.2f end2\n",
				     x2 - s2->x, s2->x, '0' + s1->staff, y);
			}

			set_y(s1, 1, s1->x, x2 - s1->x, y + 2.);

			if (s == 0)
				break;
		}
	}

	/* create the decorations tied to the staves */
	memset(minmax, 0, sizeof minmax);
	for (de = deco_head; de != 0; de = de->next) {
		struct deco_def_s *dd;

		dd = &deco_def_tb[de->t];
		if (dd->func < 6)		/* if not tied to the staff */
			continue;
		func_tb[dd->func](de);
		if (de->flags & DE_UP) {
			if (de->y > minmax[de->staff].ymax)
				minmax[de->staff].ymax = de->y;
		} else {
			if (de->y < minmax[de->staff].ymin)
				minmax[de->staff].ymin = de->y;
		}
	}

	/* and set them at a same vertical offset */
	for (de = deco_head; de != 0; de = de->next) {
		struct deco_def_s *dd;

		dd = &deco_def_tb[de->t];
		if (dd->ps_func < 0
		    || dd->func < 6)
			continue;
		x = de->x;
		if (de->flags & DE_UP)
			y = minmax[de->staff].ymax;
		else	y = minmax[de->staff].ymin;
		de->y = y;
		if (de->flags & DE_UP)
			y += dd->h;
		set_y(de->s, de->flags & DE_UP, x, de->v, y);
	}

	/* draw the measure numbers */
	if (cfmt.measurenb >= 0) {
		char *showm;
		int bar_time;
		int any_nb;
		float wmeasure;

		showm = cfmt.measurebox ? "showb" : "show";
		any_nb = 0;
		if (nbar > 1) {
			if (cfmt.measurenb == 0) {
				set_font(&cfmt.composerfont);
				any_nb = 1;
				PUT4(" 0 \x01%c%5.2f M (%d) %s",
				     '0', 24. + 14.,
				     nbar, showm);
			} else if (nbar % cfmt.measurenb == 0) {
				for (s = first_voice->sym;
				     s != 0;
				     s = s->next) {
					if (s->type != CLEF
					    && s->type != KEYSIG
					    && s->type != TIMESIG)
						break;
				}
				x = s->x - s->wl - 8.;
				set_font(&cfmt.composerfont);
				any_nb = 1;
/*fixme: set the vertical offset as below*/
				PUT5("%.1f \x01%c%5.2f M (%d) %s",
				     x, '0', 24. + 6.,
				     nbar, showm);
			}
		}

/*fixme: OK when no bar at the end of the previous line */
		wmeasure = first_voice->meter.wmeasure;
		bar_time = first_voice->sym->time
			+ wmeasure;
		for (s = first_voice->sym; s != 0; s = s->next) {
			switch (s->type) {
			case TIMESIG:
				wmeasure = s->as.u.meter.wmeasure;
				bar_time = s->time + wmeasure;
				continue;
			case MREST:
			case MREP:
				nbar += s->as.u.bar.len - 1;
			default:
				continue;
			case BAR:
				break;
			}
			if (s->u != 0)
				nbar = s->u;		/* (%%setbarnb) */
			if (s->time < bar_time		/* incomplete measure */
			    || s->as.u.bar.type == B_INVIS)
				continue;
			if (s->u == 0)
				nbar++;
			bar_time = s->time + wmeasure;
			if (s->next == 0
			    || cfmt.measurenb == 0
			    || (nbar % cfmt.measurenb) != 0)
				continue;
			if (!any_nb) {
				any_nb = 1;
				set_font(&cfmt.composerfont);
			}
/*fixme: compute the real width of the number */
			y = get_y(s, 1, s->x, 20., 0);
			if (y < s->dc_top + 5)
				y = s->dc_top + 5;
/* fixme: have the number just right below the top of the next symbol*/
			if (s->next != 0
			    && y < s->next->dc_top - 5)
				y = s->next->dc_top - 5;
			set_y(s, 1, s->x, 20., y + 6.);
			PUT5(" %.1f \x01%c%5.2f M (%d) %s",
			     s->x, '0', y,
			     nbar, showm);
		}
		if (any_nb)
			PUT0("\n");
	}
}

/* -- draw guitar chords -- */
/* (the staves are not yet defined) */
static void draw_gchord(struct SYMBOL *s,
			float gchy)
{
	float w, spc, yspc;
	char *p, *q, *r;
	char t[81];

	yspc = cfmt.gchordfont.size;
	p = t;
	tex_str(p, s->as.text, sizeof t, 0);

	/* ignore the guitar chord position (ABC draft) */
	switch (*p) {
	case '^':
	case '_':
	case '<':
	case '>':
	case '@':
		p++;
		break;
	}

	/* get where to print the first line */
	if (gchy < 34.)
		gchy = 34.;

	/* find the top line */
	q = p;
	while ((q = strstr(q, "\\n")) != 0) {
		gchy += yspc;
		q += 2;		/* skip "\n" */
	}
	s->dc_top = gchy + yspc;

	/* loop on each line */
	for (;;) {
		if ((q = strstr(p, "\\n")) != 0)
			*q = '\0';
		w = 0;
		for (r = p; *r != '\0'; r++)
			w += cwid(*r);
		spc = w * yspc * GCHPRE;
		if (spc > 8.0)
			spc = 8.0;
		PUT3("%.1f \x01%c%5.2f M ", s->x - spc,
		     '0' + s->staff, gchy);

		PUT1("(%s) gcshow ", p);
		if (q == 0)
			break;
		p = q + 2;	/* skip "\n" */
		gchy -= yspc;
	}
	PUT0("\n");
}

/* -- draw the parts and the tempo information -- */
/* (the staves are being defined) */
float draw_partempo(float top,
		    int any_part,
		    int any_tempo,
		    int any_vocal)
{
	struct SYMBOL *s;
	int head, dots, flags;
	float sc, dx, h, ht, w, ymin, nw, dy;
	char tmp[128];

	/* put the tempo indication at top */
	dy = 0;
	if (any_tempo) {
		ht = cfmt.tempofont.size + 2. + 2.;
		if (any_vocal)
			dy = ht;
		else {
			nw = 6. + cwid(' ') * cfmt.tempofont.size * 6;
			/*fixme:have tempo on other voices but the 1st?*/

			/* get the minimal y offset */
			ymin = 24. + 12.;
			for (s = first_voice->sym; s != 0; s = s->next) {
				float y;

				if (s->type != TEMPO)
					continue;

				tmp[0] = '\0';
				w = 0;
				if (s->as.u.tempo.str != 0) {
					tex_str(tmp, s->as.u.tempo.str, sizeof tmp, &w);
					w *= cfmt.tempofont.size;
				}
				w += nw;
				y = get_y(s, 1, s->x - 5., w + 30. + 10., 0) + 2.;
				if (y > ymin)
					ymin = y;
			}
			if (top < ymin + ht)
				dy = ymin + ht - top;
		}

		/* draw the tempo indications */
		set_font(&cfmt.tempofont);
		for (s = first_voice->sym; s != 0; s = s->next) {
			if (s->type != TEMPO)
				continue;

			/*fixme: cf left shift (-5.)*/
			PUT2("%.1f %.1f M ", s->x - 5., 2. - ht);
			if (s->as.u.tempo.str != 0) {
				tex_str(tmp, s->as.u.tempo.str, sizeof tmp, 0);

				/* draw the string */
				PUT1("(%s  ) show\n", tmp);
			}

			/* draw the tempo indication, if specified */
			if (s->as.u.tempo.value == 0)
				continue;
			identify_note(s, &head, &dots, &flags);

			/* draw the note */
			sc = 0.8 * cfmt.tempofont.size / 15.0;	/*fixme: 15.0 = original tempofont*/
			PUT1("gsave %.2f dup scale 15 3 rmoveto currentpoint\n",
			     sc);
			switch (head) {
			case H_OVAL:
				PUT0("HD");
				break;
			case H_EMPTY:
				PUT0("Hd");
				break;
			default:
				PUT0("hd");
				break;
			}
			dx = 4.0;
			if (dots) {
				float dotx;
				int i;

				dotx = 8;
				if (flags > 0)
					dotx += 4;
				switch (head) {
				case H_SQUARE:
				case H_OVAL:
					dotx += 2;
					break;
				case H_EMPTY:
					dotx += 1;
					break;
				}
				for (i = 0; i < dots; i++) {
					PUT1(" %.1f 0 dt", dotx);
					dx = dotx;
					dotx += 3.5;
				}
			}
			/* (16 is the stem height) */
			if (s->as.u.tempo.length < SEMIBREVE)
				PUT0(" 16 su");
			if (flags > 0) {
				PUT1(" 16 f%du", flags);
				if (dx < 6.0)
					dx = 6.0;
			}
			PUT2(" grestore %.2f 0 rmoveto ( = %d) show\n",
			     (dx + 18) * sc, s->as.u.tempo.value);
		}
	} else	ht = 0;

	/* then, put the parts */
	if (!any_part)
		return dy;

/*fixme: should reduce if parts don't overlap tempo...*/
	h = cfmt.partsfont.size + 2. + 2.;	/* + cfmt.partsspace; */
	
	if (any_vocal)
		dy += h;
	else {
		ymin = 24. + 14.;
		for (s = first_voice->sym; s != 0; s = s->next) {
			float y;

			if (s->type != PART)
				continue;
			tex_str(tmp, &s->as.text[2], sizeof tmp, &w);
			w *= cfmt.partsfont.size;
			y = get_y(s, 1, s->x - 10., w + 15., 0) + 5.;
			if (ymin < y)
				ymin = y;
		}
		if (top < ymin + h)
			dy += ymin + h - top;
	}

	set_font(&cfmt.partsfont);
	for (s = first_voice->sym; s != 0; s = s->next) {
		if (s->type != PART)
			continue;
		tex_str(tmp, &s->as.text[2], sizeof tmp, 0);
		PUT4("%.1f %.1f M (%s) show%s\n",
		     s->x - 10., 2. - ht - h, tmp, cfmt.partsbox ? "b" : "");
	}
	return dy;
}

/* -- initialize the default decorations -- */
void reset_deco(int deco_old)
{
	memset(&deco_glob, 0, sizeof deco_glob);

	/* standard */
	deco_glob['.'] = 1;
#ifdef DECO_IS_ROLL
	deco_glob['~'] = 2;
#endif
	deco_glob['H'] = 3;
	deco_glob['L'] = 4;
	deco_glob['M'] = 5;
	deco_glob['O'] = 6;
	deco_glob['P'] = 7;
	deco_glob['S'] = 8;
	deco_glob['T'] = 9;
	deco_glob['u'] = 10;
	deco_glob['v'] = 11;

	/* non-standard */
#ifndef DECO_IS_ROLL
	deco_glob['~'] = 12;
#endif
	deco_glob['J'] = 13;
	deco_glob['R'] = 2;

	/* abc2ps */
	if (deco_old) {
		deco_glob['M'] = 14;
	}
}

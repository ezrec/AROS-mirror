/*
 * Music generator.
 *
 * This file is part of abcm2ps.
 * Copyright (C) 1998-2002 Jean-François Moine
 * Adapted from abc2ps, Copyright (C) 1996,1997 Michael Methfessel
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
 */

/*fixme-todo:
 *	- correct beaming when > 2 voices/staff
 *	- have %%staves in rows
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "abcparse.h"
#include "abc2ps.h"

float realwidth;		/* real staff width while generating */

static int insert_meter;	/* flag to insert time signature */

static float alfa_last, beta_last;	/* for last short short line.. */

static struct SYMBOL *tssym;	/* time sorted list of symbols */
static struct SYMBOL *tsnext;	/* next line when cut */

#include "style.h"		/* globals to define layout style */
char *style = STYLE;

#define AT_LEAST(a,b)  do { float tmp = b; if(a<tmp) a=tmp; } while (0)

/*  subroutines connected with output of music	*/

/* -- Sets the prefered width for a note depending on the duration -- */
/* Return the horizontal space of a note/rest */
static float nwidth(int len)
{
#if 1
	return 24. * (float) len / (float) (BASE_LEN / 4) + 16.;
#else
	return 24. * (float) len / (float) (BASE_LEN / 4) + 12.;
#endif
}

/* -- decide whether to shift heads to other side of stem on chords -- */
/* also position accidentals to avoid too much overlap */
/* this routine is called only once per tune */
static void set_head_directions(struct SYMBOL *s)
{
	int i, n, sig, d, shift, nac;
	int i1, i2, m;
	float dx, xmn;

	n = s->as.u.note.nhd;
	dx = 9.0;
	switch (s->head) {
	case H_SQUARE:
	case H_OVAL:
		dx = 9.0 + 3.0;
		break;
	}
	for (i = 0; i <= n; i++)
		s->shac[i] = dx;
	if (n == 0)
		return;

	dx = 7.0;
	switch (s->head) {
	case H_SQUARE:
		dx = 13.0;
		break;
	case H_OVAL:
		dx = 10.0;
		break;
	case H_EMPTY:
		dx = 7.8;
		break;
	}
	i1 = 1;
	i2 = n + 1;
	sig = s->stem;
	if (sig < 0) {
		dx = -dx;
		i1 = n - 1;
		i2 = -1;
	}
	shift = 0;				/* shift heads */
	for (i = i1; i != i2; i += sig) {
		d = s->pits[i] - s->pits[i - sig];
		if (d < 0)
			d = -d;
		if (d > 3 || (d >= 2 && s->head < H_SQUARE))
			shift = 0;
		else {
			shift = !shift;
			if (shift) {
				s->shhd[i] = dx;
				if (dx > s->xmx)
					s->xmx = dx;
			}
		}
	}

	shift = 0;			/* shift accidentals */
	for (i = n; i >= 0; i--) {
		xmn = 0;		/* left-most pos of a close head */
		nac = 99;		/* relative pos of next acc above */
		for (m = 0; m <= n; m++) {
			float xx;
			int da;

			xx = s->shhd[m];
			d = s->pits[m] - s->pits[i];
			da = d > 0 ? d : -d;
			if (da <= 5 && xx < xmn)
				xmn = xx;
			if (d > 0 && da < nac && s->as.u.note.accs[m])
				nac = da;
		}
		s->shac[i] = 9. - xmn + s->shhd[i];	/* aligns accidentals in column */
		switch (s->head) {
		case H_SQUARE:
			s->shac[i] += 5.;
			break;
		case H_OVAL:
			s->shac[i] += 3.;
			break;
		case H_EMPTY:
			s->shac[i] += 1.;
			break;
		}
		if (s->as.u.note.accs[i]) {
			if (nac >= 6) {			/* no overlap */
				shift = 0;
				continue;
			}
			if (nac >= 4) {			/* weak overlap */
				if (shift != 0) {
					shift = 0;
					continue;
				}
				shift = 1;
			} else {			/* strong overlap */
				if (shift > 2) {
					shift = 0;
					continue;
				}
				shift += 2;
			}
			if (shift == 1)
				s->shac[i] += 4.5;
			else	s->shac[i] += 3.5 * shift;
		}
	}
}

/* -- insert a clef change -- */
static void insert_clef(struct SYMBOL *s,
			int clef_type)
{
	struct SYMBOL *new_s;
	int time, seq;

	/* create the symbol */
	new_s = ins_sym(CLEF, s->prev);
	new_s->as.u.clef.type = clef_type;
	if (clef_type == TREBLE)
		new_s->as.u.clef.line = 2;
	else {
		new_s->as.u.clef.line = 4;
		new_s->as.u.clef.transpose = 14;
	}
	new_s->u = 1;		/* small clef */

	/* link in time */
	time = s->time;
	seq = s->seq;
	do {
		s = s->ts_prev;
	} while (s->time == time
		 && s->seq == seq);
	new_s->ts_next = s->ts_next;
	new_s->ts_next->ts_prev = new_s;
	s->ts_next = new_s;
	new_s->ts_prev = s;
	new_s->time = time;
}

/* -- define the clef for a staff -- */
/* this function is called only once for the whole tune */
static void set_clef(int staff)
{
	struct SYMBOL *s;
	int min, max;
	struct SYMBOL *last_chg;
	int clef_type;

	min = max = 16;			/* 'C' */

	/* count the number of notes upper and lower than 'C' */
	for (s = tssym; s != 0; s = s->ts_next) {
		int xp;

		if (s->staff != staff
		    || s->type != NOTE)
			continue;
		xp = s->nhd;
		if (s->pits[xp] > max)
			max = s->pits[xp];
		else if (s->pits[0] < min)
			min = s->pits[0];
	}

	if (min >= 13) {		/* all upper than 'G,' --> treble clef */
#if 0 /* already done */
		staff_tb[staff].clef.type = TREBLE;
		staff_tb[staff].clef.line = 2;
		staff_tb[staff].clef.transpose = 0;
#endif
		return;
	}
	if (max <= 19) {		/* all lower than 'F' --> bass clef */
		staff_tb[staff].clef.type = BASS;
		staff_tb[staff].clef.line = 4;
		staff_tb[staff].clef.transpose = 14;
		return;
	}

	/* set clef changes */
	clef_type = TREBLE;
	last_chg = 0;
	for (s = tssym; s != 0; s = s->ts_next) {
		struct SYMBOL *s2, *s3;

		if (s->staff != staff
		    || s->type != NOTE)
			continue;

		/* check if a clef change must occur */
		if (clef_type == TREBLE) {
			if (s->pits[0] > 12		/* F, */
			    || s->pits[s->nhd] > 20)	/* G */
				continue;
			s2 = s->ts_prev;
			if (s2->time == s->time
			    && s2->staff == staff
			    && s2->pits[0] >= 19)	/* F */
				continue;
			s2 = s->ts_next;
			if (s2 != 0
			    && s2->staff == staff
			    && s2->time == s->time
			    && s2->pits[0] >= 19)	/* F */
				continue;
		} else {
			if (s->pits[0] < 12		/* F, */
			    || s->pits[s->nhd] < 20)	/* G */
				continue;
			s2 = s->ts_prev;
			if (s2->time == s->time
			    && s2->staff == staff
			    && s2->pits[0] <= 13)	/* G, */
				continue;
			s2 = s->ts_next;
			if (s2 != 0
			    && s2->staff == staff
			    && s2->time == s->time
			    && s2->pits[0] <= 13)	/* G, */
				continue;
		}

		/* go backwards and search where to insert a clef change */
		if (!voice_tb[s->voice].second)
			s3 = s;
		else	s3 = 0;
		for (s2 = s->ts_prev; s2 != last_chg; s2 = s2->ts_prev) {
			if (s2->staff != staff)
				continue;
			if (s2->type == BAR) {
				if (voice_tb[s2->voice].second)
					continue;
				s3 = s2;
				break;
			}
			if (s2->len == 0)	/* neither note nor rest */
				continue;

			/* exit loop if a clef change cannot occur */
			if (s2->type == NOTE) {
				if (clef_type == TREBLE) {
					if (s2->pits[0] >= 19)		/* F */
						break;
				} else {
					if (s2->pits[s2->nhd] <= 13)	/* G, */
						break;
				}
			}

			/* have a 2nd choice if word starts on the main voice */
			if (!voice_tb[s2->voice].second) {
				if (s2->sflags & S_WORD_ST
				    || s3 == 0
				    || (s3->sflags & S_WORD_ST) == 0)
					s3 = s2;
			}
		}
		s2 = last_chg;
		last_chg = s;

		/* if first change, see if any note before */
		if (s2 == 0) {
			struct SYMBOL *s4;

			if ((s4 = s3) != 0) {
				for (s4 = s4->ts_prev; s4 != 0; s4 = s4->ts_prev) {
					if (s4->staff != staff)
						continue;
					if (s4->type == NOTE)
						break;
				}
			}

			/* if no note, change the clef of the staff */
			if (s4 == 0) {
				if (clef_type == TREBLE) {
					clef_type = BASS;
					staff_tb[staff].clef.line = 4;
					staff_tb[staff].clef.transpose = 14;
				} else {
					clef_type = TREBLE;
					staff_tb[staff].clef.line = 2;
					staff_tb[staff].clef.transpose = 0;
				}
				staff_tb[staff].clef.type = clef_type;
				continue;
			}
		}

		/* no change possible if no insert point */
		if (s3 == 0 || s3 == s2)
			continue;

		/* insert a clef change */
		clef_type = clef_type == TREBLE ? BASS : TREBLE;
		insert_clef(s3, clef_type);
	}
}

/* -- sort the symbols by time -- */
/* this function is called only once for the whole tune */
static void def_tssym(void)
{
	struct SYMBOL *s, *t, *prev_sym;
	int time, bars;
	struct VOICE_S *p_voice;

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		p_voice->s_anc = 0;
		p_voice->selected = 0;
	}

	/* sort the symbol by time */
	prev_sym = 0;
	tssym = 0;
	s = 0;		/* compiler warning */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		if ((s = p_voice->sym) == 0) {
			ERROR(("voice %s is empty", p_voice->name));
			continue;
		}
		s->ts_prev = prev_sym;
		if (prev_sym != 0)
			prev_sym->ts_next = s;
		else	tssym = s;
		prev_sym = s;
		p_voice->s_anc = s->next;
	}
	bars = 0;			/* (for errors) */
	time = 0;
	for (;;) {
		int seq;

		/* search the closest next time/sequence */
		time += MAX_TIME;
		seq = 0;
		t = 0;
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			if ((s = p_voice->s_anc) == 0
			    || s->time > time)
				continue;
			if (s->time < time
			    || s->seq < seq) {
				time = s->time;
				seq = s->seq;
				t = s;
			}
		}
		if (t == 0)
			break;		/* echu (finished) */

		/* warn about incorrect number of notes / measures */
		t = 0;
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			if ((s = p_voice->s_anc) != 0
			    && s->time == time
			    && s->seq == seq) {
				p_voice->selected = 1;
				if (s->type == BAR
				    && s->as.u.bar.type != B_INVIS
				    && t == 0)
					t = s;
			} else	p_voice->selected = 0;
		}

		if (t != 0) {
			int ko = 0;

			bars++;
			for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
				if ((s = p_voice->s_anc) == 0)
					continue;
				if (s->len > 0		/* note or rest */
				    || s->time != time) { /* or bar */
					ko = 1;
					break;
				}
			}
			if (ko) {
				int newtime;

				ERROR(("line %d: Too many notes in measure %d"
				       " for voice %s",
				       t->as.linenum, bars, p_voice->name));
				for (p_voice = first_voice;
				     p_voice;
				     p_voice = p_voice->next) {
					if ((t = p_voice->s_anc) == 0
					    || t->type != BAR)
						continue;
					newtime = s->time + s->len;
					for (; t != 0; t = t->next) {
						t->time = newtime;
						newtime += t->len;
					}
				}
				bars--;
				continue;
			}
		}

		/* set the time linkage */
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			if (!p_voice->selected)
				continue;
			s = p_voice->s_anc;
			s->ts_prev = prev_sym;
			prev_sym->ts_next = s;
			prev_sym = s;
			p_voice->s_anc = s->next;
		}
	}
}

/* -- set the staff of the floating voices -- */
/* this function is called only once per tune */
static void set_float(void)
{
	struct VOICE_S *p_voice;
	int staff, staff_chg;
	struct SYMBOL *s, *s1;

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		if (!p_voice->floating)
			continue;
		staff_chg = 0;
		staff = p_voice->staff;
		for (s = p_voice->sym; s != 0; s = s->next) {
			int up, down;

			if (s->type != NOTE) {
				if (staff_chg)
					s->staff++;
				continue;
			}
			if (s->pits[0] >= 19) {		/* F */
				staff_chg = 0;
				continue;
			}
			if (s->pits[s->nhd] <= 13) {	/* G, */
				staff_chg = 1;
				s->staff++;
				continue;
			}
			up = 127;
			for (s1 = s->ts_prev; s1 != 0; s1 = s1->ts_prev) {
				if (s1->staff != staff
				    || s1->voice == s->voice)
					break;
				if (s1->type == NOTE
				    && s1->pits[0] < up)
					up = s1->pits[0];
			}
			if (up == 127) {
				if (staff_chg)
					s->staff++;
				continue;
			}
			if (s->pits[s->nhd] > up - 3) {
				staff_chg = 0;
				continue;
			}
			down = -127;
			for (s1 = s->ts_next; s1 != 0; s1 = s1->ts_next) {
				if (s1->staff != staff + 1
				    || s1->voice == s->voice)
					break;
				if (s1->type == NOTE
				    && s1->pits[s1->nhd] > down)
					down = s1->pits[s1->nhd];
			}
			if (down == -127) {
				if (staff_chg)
					s->staff++;
				continue;
			}
			if (s->pits[0] < down + 3) {
				staff_chg = 1;
				s->staff++;
				continue;
			}
			up -= s->pits[s->nhd];
			down = s->pits[0] - down;
			if (!staff_chg) {
				if (up < down + 3)
					continue;
				staff_chg = 1;
			} else {
				if (up < down - 3) {
					staff_chg = 0;
					continue;
				}
			}
			if (staff_chg)
				s->staff++;
		}
	}
}

/* -- set the pitch of the notes according to the clefs -- */
/* also set the vertical offset of notes and rests */
/* it supposes that the first symbol of each voice is the clef */
/* this function is called only once per tune */
static void set_pitch(void)
{
	struct SYMBOL *s;
	int staff;
	char staff_clef[MAXSTAFF];

	for (s = tssym; s != 0; s = s->ts_next) {
		struct SYMBOL *g;
		int delta;
		int np, m, pav;

		staff = s->staff;
		switch (s->type) {
		case CLEF:
			if (!voice_tb[s->voice].second) {
				delta = 0 - 2 * 2;	/* treble value */
				switch (s->as.u.clef.type) {
				case ALTO: delta = 6 - 3 * 2; break;
				case BASS: delta = 12 - 4 * 2; break;
				}
				staff_clef[staff] = delta
					+ s->as.u.clef.line * 2;
			}
			/* fall thru */
		default:
			s->ymn = -6;
			s->ymx = 24 + 6;
			s->dc_top = 24. + 2.;
			s->dc_bot = -2.;
			if ((g = s->grace) == 0)
				continue;
			delta = staff_clef[staff];
			if (delta != 0) {
				for (; g != 0; g = g->next) {
					for (m = g->nhd; m >= 0; m--)
						g->pits[m] += delta;
				}
				g = s->grace;
			}
			for (; g != 0; g = g->next)
				g->y = 3 * (g->pits[0] - 18);
			continue;
		case MREST:
			s->ymn = -6;
			s->ymx = 24 + 18;
			s->dc_top = 24. + 15.;
			s->dc_bot = -2.;
			continue;
		case REST:
			s->y = 12;
			s->dc_top = 12. + 2.;
			s->dc_bot = 12. - 2.;
			continue;
		case NOTE:
			break;
		}
		np = s->nhd;
		delta = staff_clef[staff];
		if (delta != 0) {
			for (m = np; m >= 0; m--)
				s->pits[m] += delta;
		}
		pav = 0;
		for (m = np; m >= 0; m--)
			pav += s->pits[m];
		s->y = 3 * (s->pits[0] - 18);
		s->ymn = s->y;
		s->ymx = 3 * (s->pits[np] - 18);
		s->yav = 3 * ((pav / (np + 1)) - 18);
		s->dc_top = (float) (s->ymx + 2);
		s->dc_bot = (float) (s->ymn - 2);
	}
}

/* -- set the stem direction when multi-voices -- */
/* and adjust the vertical offset of the rests */
/* this function is called only once per tune */
static void set_multi(void)
{
	struct SYMBOL *s;
	int i, staff;
	struct {
		short nvoice;
		short last;
		struct {
			short voice;
			short nn;
			short ymn;
			short ymx;
		} st[4];		/* (no more than 4 voices per staff) */
	} stb[MAXSTAFF];
	struct VOICE_S *p_voice;

	memset(stb, 0, sizeof stb);
	for (staff = MAXSTAFF; --staff >= 0; ) {
		for (i = 4; --i >= 0; )
			stb[staff].st[i].voice = -1;
	}
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		staff = p_voice->staff;
		for (i = 0; i < 4; i++) {
			if (stb[staff].st[i].voice < 0) {
				stb[staff].st[i].voice = p_voice - voice_tb;
				break;
			}
		}
		if (p_voice->floating) {
			staff++;
			for (i = 0; i < 4; i++) {
				if (stb[staff].st[i].voice < 0) {
					stb[staff].st[i].voice = p_voice - voice_tb;
					break;
				}
			}
		}
	}
	s = tssym;
	while (s != 0) {
		struct SYMBOL *t;

		for (staff = MAXSTAFF; --staff >= 0; ) {
			stb[staff].nvoice = 0;
			stb[staff].last = 0;
			for (i = 4; --i >= 0; ) {
				stb[staff].st[i].nn = 0;
				stb[staff].st[i].ymx = 0;
				stb[staff].st[i].ymn = 24;
			}
		}

		/* go to the next bar and get the max/min offsets */
		for (t = s;
		     t != 0 && t->type != BAR;
		     t = t->ts_next) {
			if (t->len == 0		/* not a note or a rest */
			    || t->as.u.note.invis)
				continue;
			staff = t->staff;
			for (i = 0; i < 4; i++) {
				if (stb[staff].st[i].voice == t->voice)
					break;
			}
			if (i == 4)
				bug("Voice with no staff", 1);
			
			if (++stb[staff].st[i].nn == 1)
				stb[staff].nvoice++;
			if (i > stb[staff].last)
				stb[staff].last = i;
			if (t->type != NOTE)
				continue;
			if (t->ymx > stb[staff].st[i].ymx)
				stb[staff].st[i].ymx = t->ymx;
			if (t->ymn < stb[staff].st[i].ymn)
				stb[staff].st[i].ymn = t->ymn;
		}

		for ( ;
		     s != 0 && s->type != BAR;
		     s = s->ts_next) {
			if (s->len == 0		/* not a note nor a rest */
			    || s->as.u.note.invis)
				continue;
			staff = s->staff;
			if (stb[staff].nvoice <= 1) {

				/* only 1 voice in the staff */
				p_voice = &voice_tb[s->voice];
				if (p_voice->floating) {
					if (s->staff == p_voice->staff)
						s->multi = -1;
					else	s->multi = 1;
				}
				continue;
			}
			for (i = 0; i < 4; i++) {
				if (stb[staff].st[i].voice == s->voice)
					break;
			}
			if (i == stb[staff].last)
				s->multi = -1;	/* last voice */
			else {
				s->multi = 1;	/* first voice(s) */

				/* if 3 voices, and vertical space enough,
				 * have stems down on the 2nd voice */
				if (i != 0
				    && i == stb[staff].last - 1
				    && stb[staff].st[i].ymn - STEM
					> stb[staff].st[i + 1].ymx)
					s->multi = -1;
			}
			if (s->type != REST)
				continue;

			/* set the rest height */
			/* (if visible and invisible rests on the same staff,
			 *  set as if 1 rest only) */
			if (s->multi > 0) {
				if (s->ts_next == 0
				    || s->ts_next->type != REST
				    || !s->ts_next->as.u.note.invis) {
					s->y = stb[staff].st[i + 1].ymx / 6 * 6 + 12;
					if (s->y < 18)
						s->y = 18;
					if (s->y + 6 > s->dc_top)
						s->dc_top = s->y + 6;
				}
			} else {
				if (s->ts_prev->type != REST
				    || !s->ts_prev->as.u.note.invis) {
					s->y = stb[staff].st[i - 1].ymn / 6 * 6 - 12;
					if (s->y > 6)
						s->y = 6;
					if (s->y - 6 < s->dc_bot)
						s->dc_bot = s->y - 6;
				}
			}
		}

		while (s != 0 && s->type == BAR)
			s = s->ts_next;
	}
}

/* -- set the staves and stems when multivoice -- */
/* this function is called only once per tune */
static void set_global(void)
{
	int staff;
	struct SYMBOL *s;
	struct VOICE_S *p_voice;

#ifndef CLEF_TRANSPOSE
	int old_behaviour, done;

	/* adjust the pitches if old abc2ps behaviour of clef definition */
	old_behaviour = done = 0;
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		int max, min;

		if (!p_voice->forced_clef)
			continue;

		/* search if any pitch is too high for the clef */
		max = 100;
		min = -100;
		for (s = p_voice->sym; s != 0; s = s->next) {
			switch (s->type) {
			case CLEF:
				if (s->as.u.clef.transpose != 0)
					break;		/* new behaviour */
				switch (s->as.u.clef.type) {
				case TREBLE:
					max = 100;
					min = -100;
					break;
				case ALTO:
					max = 25;	/* e */
					min = 14;	/* G, */
					break;
				case BASS:
					max = 21;	/* A */
					min = 10;	/* C, */
					break;
				}
			default:
				continue;
			case NOTE:
				if (s->pits[0] < min) {
					done = 1;
					break;		/* new behaviour */
				}
				if (s->pits[s->nhd] <= max)
					continue;
				old_behaviour = 1;
				done = 1;
				break;
			}
			break;
		}
		if (done)
			break;
	}
	if (old_behaviour) {
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			int delta;

			if (!p_voice->forced_clef)
				continue;
			delta = 0;
			for (s = p_voice->sym; s != 0; s = s->next) {
				struct SYMBOL *g;
				int i;

				switch (s->type) {
				case CLEF:
					switch (s->as.u.clef.type) {
					case TREBLE: delta = 0; break;
					case ALTO: delta = -7; break;
					case BASS: delta = -14; break;
					}
				default:
					continue;
				case NOTE:
				case GRACE:
					if (delta == 0)
						continue;
					break;
				}
				if (s->type == NOTE) {
					for (i = s->nhd; i >= 0; i--)
						s->pits[i] += delta;
				}
				if ((g = s->grace) != 0) {
					for (; g != 0; g = g->next) {
						for (i = g->nhd; i >= 0; i--)
							g->pits[i] += delta;
					}
				}
			}
		}
	}
#endif

	/* set the width of the notes/rests, a pitch for all symbols, */
	/* the tie indexes, the start/end of words and the sequence number */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		int pitch, start_flag, seq;
		struct SYMBOL *sym, *last_tie, *lastnote;

		sym = p_voice->sym;

		pitch = 22;				/* 'B' - if no note! */
		for (s = sym; s != 0; s = s->next) {
			if (s->type == NOTE) {
				pitch = s->pits[0];
				break;
			}
		}
		last_tie = 0;
		start_flag = 1;
		lastnote = 0;
		seq = 0;
		for (s = sym; s != 0; s = s->next) {
			switch (s->type) {
			default:
				if ((s->sflags & S_EOLN) == 0)
					break;
				/* fall thru */
			case BAR:
			case MREST:
			case MREP:
				if (lastnote != 0) {
					lastnote->as.u.note.word_end = 1;
					start_flag = 1;
					lastnote = 0;
				}
				if (s->type == BAR
				    && s->next == 0
				    && s->prev->type == NOTE
				    && s->prev->as.u.note.lens[0] >= BREVE)
					s->prev->head = H_SQUARE;
				break;
			case NOTE: {
				int i;

				if (last_tie != 0) {
					for (i = 0; i <= last_tie->nhd; i++) {
						int pit, j;

						if (last_tie->as.u.note.ti1[i] == 0)
							continue;
						pit = last_tie->pits[i];
						last_tie->as.u.note.ti1[i] = 0;
						for (j = 0; j <= s->as.u.note.nhd; j++) {
							if (s->pits[j] == pit) {
								last_tie->as.u.note.ti1[i] = j + 1;
								break;
							}
						}
					}
					last_tie = 0;
				}
				for (i = 0; i <= s->nhd; i++) {
					if (s->as.u.note.ti1[i] != 0) {
						last_tie = s;
						break;
					}
				}
			    }
				/* fall thru */
			case REST:

				/* set the note widths */
				switch (s->head) {
				case H_SQUARE:
					s->wl = s->wr = 8.0;
					break;
				case H_OVAL:
					s->wl = s->wr = 6.0;
					break;
				case H_EMPTY:
					s->wl = s->wr = 5.0;
					break;
				default:
					s->wl = s->wr = 4.5;
					break;
				}

				if (s->nflags == 0) {
					if (lastnote != 0) {
						lastnote->as.u.note.word_end = 1;
						lastnote = 0;
					}
					s->as.u.note.word_end = start_flag = 1;
					s->sflags |= S_WORD_ST;
				} else if (s->type == NOTE) {
					if (start_flag)
						s->sflags |= S_WORD_ST;
					start_flag = s->as.u.note.word_end;
					lastnote = s;
				} else if (s->as.u.note.word_end) {
					if (lastnote != 0) {
						lastnote->as.u.note.word_end = 1;
						lastnote = 0;
					}
					s->as.u.note.word_end = 0;
					start_flag = 1;
				}
				break;
			}
			if (s->type == NOTE) {
				pitch = s->pits[0];
				if (s->prev->type != NOTE) {
					s->prev->pits[0] = (s->prev->pits[0]
							    + pitch) / 2;
				}
			} else	s->pits[0] = pitch;
			switch (s->type) {
			case NOTE:
			case REST:
			case MREST:
			case MREP:
			case FMTCHG:
				seq = 0;
				break;
			default:
				if (s->seq <= seq)
					s->seq = seq + 1;
				seq = s->seq;
				break;
			}
		}
		if (lastnote != 0)
			lastnote->as.u.note.word_end = 1;
	}

	/* sort the symbols by time */
	def_tssym();

	/* set the staff of the floating voices */
	set_float();

	/* set the clefs */
	for (staff = 0; staff <= nstaff; staff++) {
		if (!staff_tb[staff].forced_clef)
			set_clef(staff);
	}

	/* set the starting clefs and adjust the note pitches */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
		memcpy(&p_voice->sym->as.u.clef,
		       &staff_tb[p_voice->staff].clef,
		       sizeof p_voice->sym->as.u.clef);
	set_pitch();
}

/* -- return the left indentation of the staves -- */
static float set_indent(int first_line)
{
	int staff;
	float w;
	float more_shift;
	struct VOICE_S *p_voice;

	/*fixme: split big lines*/
	w = 0;
	if (first_line) {

		/* first line: use main name */
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			if (w < p_voice->nmw)
				w = p_voice->nmw;
		}

		/* if no name, indent the first line when many lines */
		if (w == 0)
			return tsnext == 0
				? 0
				: cfmt.indent;
	} else {

		/* other lines: use subname */
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			if (w < p_voice->snmw)
				w = p_voice->snmw;
		}
		if (w == 0)
			return 0;
	}
	more_shift = 0;
	for (staff = 0; staff <= nstaff; staff++) {
		if (staff_tb[staff].brace
		    || staff_tb[staff].bracket) {
			more_shift = 10;
			break;
		}
	}
	return cfmt.vocalfont.swfac * cfmt.vocalfont.size * (w + 4 * cwid(' '))
		+ more_shift;
}

/* -- set the y offset of the staves and return the whole height -- */
/* !! this routine is tied to draw_vocals() !! */
static float set_staff(void)
{
	struct SYMBOL *s;
	struct VOICE_S *p_voice;
	int	staff;
	float	y, vocal_height;
	float	staffsep, dy;
	struct {
		char avoc, bvoc;	/* number of vocals above and below the staff */
		short vocal;		/* some vocal below the staff */
		float x;
		float ctop, mtop;
		float cbot, mbot;
	} delta_tb[MAXSTAFF], *p_delta;
	int any_part, any_tempo;

	memset(delta_tb, 0, sizeof delta_tb);
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		if (p_voice->nvocal > 0) {
			staff = p_voice->staff;
			if (cfmt.vocalabove
			    || (p_voice->next != 0
				&& p_voice->next->staff == staff
				&& p_voice->next->nvocal > 0)) {
				delta_tb[staff].avoc += p_voice->nvocal;
				if (staff > 0)
					delta_tb[staff - 1].vocal = 1;
			} else {
				delta_tb[staff].bvoc += p_voice->nvocal;
				delta_tb[staff].vocal = 1;
			}
		}
	}
	delta_tb[nstaff].vocal = 1;

	any_part = any_tempo = 0;
	for (s = tssym; s != 0; s = s->ts_next) {
		if (s->voice == 0) {
			switch (s->type) {
			case PART: any_part = 1; break;
			case TEMPO: any_tempo = 1; break;
			}
		}
		staff = s->staff;
		p_delta = &delta_tb[staff];
		if (s->x > p_delta->x) {
			p_delta->x = s->x + 8;
			p_delta->ctop = s->dc_top;
			p_delta->cbot = s->dc_bot;
		} else {
			if (s->dc_top > p_delta->ctop)
				p_delta->ctop = s->dc_top;
			if (s->dc_bot < p_delta->cbot)
				p_delta->cbot = s->dc_bot;
		}

		/* adjust above the staff */
		if (staff == 0
		    || p_delta[-1].vocal) {
			if (p_delta->mtop < p_delta->ctop)
				p_delta->mtop = p_delta->ctop;
		} else {
			dy = p_delta[-1].cbot - p_delta->ctop;
			if (p_delta[-1].mbot > dy)
				p_delta[-1].mbot = dy;
		}

		/* adjust below the staff */
		if (p_delta->vocal) {
			if (p_delta->mbot > p_delta->cbot)
				p_delta->mbot = p_delta->cbot;
		} else {
			if (staff == nstaff)
				dy = p_delta->cbot;
			else	dy = p_delta->cbot - p_delta[1].ctop;
			if (p_delta->mbot > dy)
				p_delta->mbot = dy;
		}
	}

	/* draw the parts and tempo indications if any */
	dy = 0;
	if (any_part || any_tempo)
		dy = draw_partempo(delta_tb[0].mtop,
				   any_part,
				   any_tempo,
				   delta_tb[0].avoc);

	/* set the staff offsets */
	staffsep = cfmt.staffsep * 0.5 + 24.;
	y = 0;
	vocal_height = 1.1 * cfmt.vocalfont.size;
	for (staff = 0, p_delta = delta_tb;
	     staff <= nstaff;
	     staff++, p_delta++) {
		p_delta->ctop = y + dy;
		dy += p_delta->mtop + vocal_height * p_delta->avoc + 2.;
		if (dy < staffsep)
			dy = staffsep;
		y += dy;
		staff_tb[staff].y = -y;
		if (staff == 0)
			staffsep = cfmt.sysstaffsep + 24.;
		dy = -p_delta->mbot;
		p_delta->cbot = y + dy + 2.;
		if (p_delta->bvoc > 0) {
			dy += cfmt.vocalfont.size;
			if (dy < cfmt.vocalspace)
				dy = cfmt.vocalspace;
			dy += vocal_height * (p_delta->bvoc - 1)
				+ cfmt.vocalfont.size * 0.4;
		}
	}
	staffsep = cfmt.staffsep * 0.5;
	if (p_delta->bvoc > 0)
		dy += staffsep * 0.5;
	else if (dy < staffsep)
		dy = staffsep;

	/* set the vocal offsets */
	for (p_voice= first_voice; p_voice; p_voice = p_voice->next) {
		if (p_voice->nvocal > 0) {
			staff = p_voice->staff;
			p_delta = &delta_tb[staff];
			if (cfmt.vocalabove
			    || (p_voice->next != 0
				&& p_voice->next->staff == staff
				&& p_voice->next->nvocal > 0)) {
				p_voice->yvocal = -p_delta->ctop
					 - cfmt.vocalfont.size;
				p_delta->ctop += vocal_height * p_voice->nvocal;
			} else {
				p_voice->yvocal = -p_delta->cbot - cfmt.vocalfont.size
/*??*/					+ 2.;
				p_delta->cbot += vocal_height * p_voice->nvocal;
			}
		}
	}
	return y + dy;
}

/* -- decide on beams and on stem directions -- */
/* this routine is called only once per tune */
static void set_beams(struct SYMBOL *sym)
{
	struct SYMBOL *s;
	int beam, laststem, stem, lasty;
	int do_break;

#if CUT_NPLETS
	/* separate words before and after n-plet */
	{
		struct SYMBOL *lastnote;
		int num, nplet;

		num = nplet = 0;
		lastnote = 0;
		for (s = sym; s != 0; s = s->next) {
			if (s->len == 0)	/* not a note or a rest */
				continue;
			num++;
			if (nplet && num == nplet) {
				if (lastnote != 0)
					lastnote->as.u.note.word_end = 1;
				s->sflags |= S_WORD_ST;
			}
			if (s->as.u.note.p_plet) {
				nplet = s->as.u.note.r_plet;
				num = 0;
				if (lastnote != 0)
					lastnote->as.u.note.word_end = 1;
				s->sflags |= S_WORD_ST;
			}
			lastnote = s;
		}
	}
#endif

	/* set stem directions; near middle, use previous direction */
	beam = 0;
	stem = 0;				/* (compilation warning) */
	laststem = 0;
	lasty = 0;
	do_break = 0;
	for (s = sym; s != 0; s = s->next) {
		if (s->type != NOTE) {
			laststem = 0;
			continue;
		}
		if ((s->stem = s->multi) == 0) {

			/* not set by set_multi() (voice alone on the staff) */
			s->stem = s->yav >= 12 ? -1 : 1;
#ifndef BSTEM_DOWN
			if (laststem != 0
			    && s->yav > 11
			    && s->yav < 13) {
				int dy;

				dy = s->yav - lasty;
				if (dy > -7 && dy < 7)
					s->stem = laststem;
			}
#endif
			/* notes in a beam have the same stem direction */
			if ((s->sflags & S_WORD_ST)
			    && !s->as.u.note.word_end) {	/* start of beam */
				int avg, n;
				struct SYMBOL *t;

				avg = s->yav;
				n = 1;
				for (t = s->next; t != 0; t = t->next) {
					if (t->type == NOTE) {
						avg += t->yav;
						n++;
					}
					if (t->as.u.note.word_end)
						break;
				}
				avg /= n;
				stem = 1;
				if (avg >= 12) {
					stem = -1;
#ifndef BSTEM_DOWN
					if (avg == 12
					    && laststem != 0)
						stem = laststem;
#endif
				}
				beam = 1;
			}
			if (beam)
				s->stem = stem;
		} else {			/* stem set by set_multi */
			if ((s->sflags & S_WORD_ST)
			    && !s->as.u.note.word_end) {	/* start of beam */
				beam = 1;
				stem = s->stem;
			}
		}

		if (s->as.u.note.word_end)
			beam = 0;
		if (voice_tb[s->voice].bagpipe)
			s->stem = -1;
		laststem = (s->len >= MINIM || s->as.u.note.stemless) ? 0 : s->stem;
		lasty = s->yav;
		if (s->len <= SEMIQUAVER / 2
		    && s->prev->len <= SEMIQUAVER / 2)
			do_break = 1;
	}

	/* set the beam breaks on demi-semi-quaver */
	if (do_break) {
		int bartime;

		bartime = 0;
		for (s = sym; s != 0; s = s->next) {
			if (s->type == BAR) {
				bartime = s->time;
				continue;
			}
			if (s->len == 0
			    || s->len > SEMIQUAVER / 2)
				continue;
			if (((s->time - bartime) % (SEMIQUAVER * 2)) == 0
			    && s->prev->len <= SEMIQUAVER / 2) {
				if (s->prev->type == NOTE)
					s->prev->sflags |= S_BEAM_BREAK;
				else	s->prev->prev->sflags |= S_BEAM_BREAK;
			}
		}
	}
}

/* -- set the x offset of the grace notes -- */
static float set_graceoffs(struct SYMBOL *s)
{
	struct SYMBOL *g, *next;
	float xx;

	xx = 0;
	for (g = s->grace; ; g = g->next) {
		if (g->as.u.note.accs[0])
			xx += 3.5;
		g->x = xx;
		if (g->next == 0)
			break;
		if (g->y > g->next->y + 8)
			xx -= 1.6;
		xx += GSPACE;
	}
	xx += GSPACE0 - 4.5;
	if ((next = s->next) != 0
	    && next->type == NOTE) {
		if (g->y >= next->ymx)
			xx -= 1.;	/* above, a bit closer */
		if (s->grace == g
		    && g->y < next->ymn - 7)
			xx += 2.;	/* below with flag, a bit further */
	}

	/* return the whole width */
	return xx;
}

/* -- set a shift when notes overlap -- */
/* this routine is called only once per tune */
static void set_overlap(void)
{
	struct SYMBOL *s, *s1;

/*fixme: the accidentals are not treated.. */
	for (s = tssym; s != 0; s = s->ts_next) {
		struct SYMBOL *s2;
		int d, m, nhd2;
		float d1, d2, x2, dy1, dy2;
		float noteshift;

		if (s->type != NOTE)
			continue;

		s2 = s->ts_next;
		if (s2 == 0
		    || s2->type != NOTE
		    || s->staff != s2->staff
		    || s->time != s2->time
		    || s->seq != s2->seq)
			continue;

		nhd2 = s2->nhd;

		/* align the accidentals when bigger than SEMIBREVE */
		if (s->head >= H_OVAL) {
			if (s2->head < H_OVAL)
				for (m = nhd2; m >= 0; m--)
					s2->shac[m] += 3.;
		} else {
			if (s2->head >= H_OVAL)
				for (m = s->nhd; m >= 0; m--)
					s->shac[m] += 3.;
		}

		d1 = d2 = x2 = dy1 = 0;
		d = s->pits[0] - s2->pits[nhd2];
		s1 = s;

		/* shift the accidentals */
/*fixme: not finished... */
		if (d >= -5 && d <= 5
		    && s1->as.u.note.accs[0]
		    && s2->as.u.note.accs[nhd2]) {
			if (d > 0) {
				if (d >= 4)
					s1->shac[0] += 4.5;
				else	s1->shac[0] += 7.0;
			} else {
				if (d <= -4)
					s2->shac[nhd2] += 4.5;
				else	s2->shac[nhd2] += 7.0;
			}
		}

		/* if voices with a same stem direction, force a shift */
//fixme		if (s1->multi == s2->multi) {
		if (s1->stem == s2->stem) {
			if (d > 0) {
				s1 = s2;
				s2 = s;
				d = -d;
			}
			if ((d < -1
			     && s1->head == H_OVAL)
			    || (d < -3
				&& s1->head == H_SQUARE))
				continue;
			if (s1->stem > 0) {
				if (s2->y < s1->ys)
					continue;
			} else {
				if (s2->ys > s1->y)
					continue;
			}
		}
		switch (d) {
		case 0: {
			int l1, l2;

			if ((l1 = s1->len) >= SEMIBREVE
			    || (l2 = s2->len) >= SEMIBREVE)
				break;
			s2->sflags |= S_NO_HEAD;	/* same head */
			s2->as.u.note.accs[nhd2] = 0;
			d2 = s1->shhd[0];
			dy2 = 0;
			if (l1 == l2)
				goto do_shift;
			if (l1 < l2) {
				l1 = l2;
				l2 = s1->len;
			}
			if (l1 == l2 * 2
			    || l1 == l2 * 4
			    || l1 == l2 * 8
			    || l1 == l2 * 16) {
				if (l1 <= CROTCHET)
					goto do_shift;
				if (l2 < CROTCHET) {	/* (l1 == MINIM) */
					if (s2->len == MINIM) {
						s2->sflags &= ~S_NO_HEAD;
						s1->sflags |= S_NO_HEAD;
						s2->as.u.note.accs[nhd2] =
							s1->as.u.note.accs[0];
						s1->as.u.note.accs[nhd2] = 0;
					}
					goto do_shift;
				}
			}
			s2->sflags &= ~S_NO_HEAD;
			break;
		    }
		case 1:
			break;
		default:
			if ((s1->head == H_SQUARE
			     || s2->head == H_SQUARE)
			    && (d >= -3 && d <= 3))
				break;
			if (d > 0)
				continue;
			if (d < -1
			    && s1->head == H_OVAL
			    && s2->head == H_OVAL)
				continue;
			break;
		}
		if (s1->len >= BREVE
		    || s2->len >= BREVE)
			noteshift = 13;
		else if (s1->len >= SEMIBREVE
			 || s2->len >= SEMIBREVE)
			noteshift = 10;
		else	noteshift = 7.8;
/* fixme: treat the accidentals */
		/* the dot of the 2nd voice should be lower */
		dy2 = -3;

		/* if the 1st voice is below the 2nd one, shift the 1st voice (lowest) */
		if (d < 0) {
			if (d == -1)
				d1 = noteshift;
			else	d1 = noteshift * 0.5;

			/* and shift the dot of the 2nd voice if any */
			if (s2->dots > 0)
				x2 = d1;

			/* the dot of the 1st voice must be lower */
			dy1 = -3;
			dy2 = 0;

		/* if the upper note is dotted but not the lower one,
		 * shift the 1st voice */
		} else if (s1->dots > 0) {
			if (s2->dots == 0)
				d1 = noteshift;

			/* both notes are dotted, have a bigger shift on the 2nd voice */
			else	d2 = 12. + 3.5 * s2->dots;

		/* if the upper note is MINIM or higher, shift the 1st voice */
		} else if (s1->head != H_FULL
			   && s1->len > s2->len)
				d1 = noteshift;

		/* else shift the 2nd voice */
		else	d2 = noteshift;

		/* do the shift, and have a minimal space */
	do_shift:
		if (d1 > 0) {
			for (m = s1->nhd; m >= 0; m--) {
				s1->shhd[m] += d1;
				s1->shac[m] += d1;
			}
			s1->xmx += d1;
			s2->xmx += x2;
			s1->wr += d1 + 2;
			s2->wr += d1 + 2;
		} else /*if (d2 > 0)*/ {
			for (m = nhd2; m >= 0; m--) {
				s2->shhd[m] += d2;
				s2->shac[m] += d2;
			}
			s2->xmx += d2;
			s1->wr += d2 + 2;
			s2->wr += d2 + 2;
		}
		s1->doty = dy1;
		s2->doty = dy2;
	}
}

/* -- set the stem lengths -- */
/* this routine is called only once per tune */
static void set_stems(void)
{
	struct SYMBOL *s;

	for (s = tssym; s != 0; s = s->ts_next) {
		float slen, ymin, ymax;

		if (s->type != NOTE)
			continue;

		/* shift notes in chords (need stem direction to do this) */
		set_head_directions(s);

		/* set height of stem end, without considering beaming for now */
		slen = STEM;
		if (s->nflags >= 2) {
			slen += 2;
			if (s->nflags == 3)
				slen += 2;
			else if (s->nflags >= 4)
				slen += 7;
		}
		if (s->as.u.note.stemless) {
			if (s->stem >= 0) {
				s->y = s->ymn;
				s->ys = s->ymx;
			} else {
				s->ys = s->ymn;
				s->y = s->ymx;
			}
			ymin = (float) (s->ymn - 4);
			ymax = (float) (s->ymx + 4);
		} else if (s->stem >= 0) {
			if (s->nflags > 2)
				slen -= 1;
			if (s->pits[s->nhd] > 26
			    && (s->nflags == 0
				|| !((s->sflags & S_WORD_ST)
				     && s->as.u.note.word_end))) {
				slen -= 2;
				if (s->pits[s->nhd] > 28)
					slen -= 2;
			}
			s->y = s->ymn;
			s->ys = s->ymx + slen;
			ymin = (float) (s->ymn - 4);
			ymax = s->ys + 2;
			if (s->as.u.note.ti1[0] != 0
			    || s->as.u.note.ti2[0] != 0)
				ymin -= 3;
		} else {
			if (s->pits[0] < 18
			    && (s->nflags == 0
				|| !((s->sflags & S_WORD_ST)
				     && s->as.u.note.word_end))) {
				slen -= 2;
				if (s->pits[0] < 16)
					slen -= 2;
			}
			s->ys = s->ymn - slen;
			s->y = s->ymx;
			ymin = s->ys - 2.;
			ymax = (float) (s->y + 4);
			if (s->as.u.note.ti1[s->nhd] != 0
			    || s->as.u.note.ti2[s->nhd] != 0)
				ymax += 3.;
		}
			 
		if (s->dc_bot > ymin)
			s->dc_bot = ymin;
		if (s->dc_top < ymax)
			s->dc_top = ymax;
	}
}

/* -- set width and space of a symbol -- */
static void set_width(struct SYMBOL *s)
{
	int i, m;
	struct SYMBOL *prev;
	float xx, w, wlnote, wlw;
	char t[81];

	switch (s->type) {
	case NOTE:
	case REST:
		/* (the note widths are set in set_global) */
		wlnote = s->wl;

		/* room for shifted heads and accidental signs */
		for (m = 0; m <= s->nhd; m++) {
			xx = s->shhd[m];
			if (xx != 0) {
				AT_LEAST(s->wr, xx + 5.);
				AT_LEAST(wlnote, -xx + 5.);
			}
			if (s->as.u.note.accs[m]) {
				xx -= s->shac[m];
/*3.0.9: was 3.5*/
				AT_LEAST(wlnote, -xx + 4.5);
			}
		}

		/* room for the decorations */
		if (s->as.u.note.dc.n > 0)
			wlnote += deco_width(s);

		/* space for flag if stem goes up on standalone note */
		if ((s->sflags & S_WORD_ST)
		    && s->as.u.note.word_end
		    && s->stem > 0 && s->nflags > 0)
			AT_LEAST(s->wr, 10.);

		/* leave room for dots */
		if (s->dots > 0) {
			AT_LEAST(s->wr, 12. + s->xmx);
			if (s->dots >= 2)
				s->wr += 3.5;
			switch (s->head) {
			case H_SQUARE:
			case H_OVAL:
				s->wr += 2;
				break;
			case H_EMPTY:
				s->wr += 1;
				break;
			}

			/* special case: standalone with up-stem and flags */
			if (s->nflags && s->stem > 0
			    && (s->sflags & S_WORD_ST)
			    && s->as.u.note.word_end
			    && !(s->y % 6))
				s->wr += DOTSHIFT;
		}

		wlw = wlnote;

		/* extra space when down stem follows up stem */
		prev = s->prev;
		if (prev->type == NOTE) {
			if (prev->stem > 0 && s->stem < 0)
				AT_LEAST(wlw, 7);

			/* make sure helper lines don't overlap */
			if (s->y > 27 && prev->y > 27)
				AT_LEAST(wlw, 7.5);
		}

		/* leave room for guitar chord */
		if (s->as.text != 0) {
			struct SYMBOL *k;
			float spc;

			/* rest is same for all guitar chord cases */
			tex_str(t, s->as.text, sizeof t, &w);

			/* may have many lines in guitar chord */
			if (strchr(t, '\n') != 0) {

				/* stacked chord/figured bass */
				float wlower;
				char *p;

				wlower = w = 0;
				p = t;
				for (;;) {
					if (*p == '\0'
					    || *p == '\n') {
						if (wlower > w)
							w = wlower;
						if (*p == '\0')
							break;
						wlower = 0;
						p++;
						continue;
					}
					wlower += cwid(*p);
					p++;
				}
			}
			w += cwid(' ');
			xx = cfmt.gchordfont.size * cfmt.gchordfont.swfac * w;
			spc = xx * GCHPRE;
			if (spc > 8)
				spc = 8;
			k = prev;
			if (k->as.text != 0)
				AT_LEAST(wlw, spc);
			if ((k = s->next) != 0
			    && k->as.text != 0
/*fixme: may have some other symbols with guitar chords?*/
			    && (k->len > 0 || k->type == BAR))
				AT_LEAST(s->wr, xx - spc);
		}

		/* leave room for vocals under note */
		/* related to draw_vocals() */
		/*fixme: pb when lyrics of 2 voices in the same staff */
		if (s->ly) {
			struct lyrics *ly = s->ly;
			float swfac;

			swfac = cfmt.vocalfont.size * cfmt.vocalfont.swfac;
			for (i = 0; i < MAXLY; i++) {
				struct SYMBOL *k;
				float shift;
				char *p;

				if ((p = ly->w[i]) == 0)
					continue;
				p++;
				tex_str(t, p, sizeof t, &w);
				xx = swfac * (w + cwid(' '));
				if (isdigit((unsigned) t[0]))
					shift = LYDIG_SH * swfac * cwid('1');
				else {
					shift = xx * VOCPRE;
					if (shift > 20.)
						shift = 20.;
				}
				AT_LEAST(wlw, shift);
				if ((k = s->next) != 0
				    && k->len > 0	/* note or rest */
				    && (k->ly == 0
					|| k->ly->w[i] == 0))
					xx -= 10.;	/*fixme: which max width?*/
				AT_LEAST(s->wr, xx - shift);
			}
		}

		xx = nwidth(s->len);
		s->pr = bnnp * xx;
		s->pl = (1. - bnnp) * xx;

		/* squeeze notes a bit if big jump in pitch */
		if (s->type == NOTE
		    && s->prev->type == NOTE) {
			int dy;
			float fac;

			dy = s->y - s->prev->y;
			if (dy < 0)
				dy =- dy;
			fac = 1. - 0.01 * dy;
			if (fac < 0.9)
				fac = 0.9;
			s->pl *= fac;
		}

		/* if preceded by a grace sequence, adjust */
		prev = s->prev;
		if (prev->type == GRACE) {
			s->wl = wlnote;
			wlw -= wlnote;
			if (prev->wl + prev->wr < wlw)
				prev->wl = wlw - prev->wr;
			prev->pl = s->pl * 0.5;
			prev->pr = prev->wr;
			s->pl = wlnote + 2;
		} else	s->wl = wlw;

		break;
	case BAR:
		switch (s->as.u.bar.type) {
		case B_SINGLE:
		case B_DASH:
			s->wl = 4; s->wr = 6; break;
		case B_DOUBLE:
			s->wl = 7; s->wr = 8; break;
		case B_LREP:
			s->wl = 5; s->wr = 14; break;
		case B_RREP:
			s->wl = 12; s->wr = 6; break;
		case B_DREP:
			s->wl = 12; s->wr = 14; break;
		case B_THICK_THIN:
			s->wl = 4; s->wr = 9; break;
		case B_THIN_THICK:
			s->wl = 9; s->wr = 5; break;
		case B_INVIS:
			break;
		}
		if (s->as.u.bar.dc.n > 0)
			s->wl += deco_width(s);
		s->pl = s->wl * 1.1;
/*fixme: take the length from the tempo */
		s->pr = s->wr * 1.1;

		/* have room for the repeat numbers / guitar chord */
		if (s->as.text == 0)
			break;
		tex_str(t, s->as.text, sizeof t, &w);
		w += cwid(' ') * 1.5;
		xx = cfmt.gchordfont.size * cfmt.gchordfont.swfac * w;
		prev = s->prev;
		if (!s->as.u.bar.repeat_bar) {
			if (prev->as.text != 0) {
				float spc;

				spc = xx * GCHPRE;
				if (spc > 8.0)
					spc = 8.0;
				AT_LEAST(s->wl, spc);
				s->pl = s->wl;
				xx -= spc;
			}
		}
/*fixme: may have gchord a bit further..*/
		if (s->next != 0
		    && s->next->as.text != 0
		    && s->next->len > 0) {	/*fixme: what if 2 bars?*/
			AT_LEAST(s->wr, xx);
			s->pr = s->wr;
		}
		break;
	case CLEF:
		s->wl = s->pl = 12;
		s->wr = s->pr = s->u ? 10 : 16;
		break;
	case KEYSIG: {
		int n1, n2;
		int esp = 4;

		n1 = s->as.u.key.sf;	/* new key sig */
		n2 = s->u;		/* old key */
		if (n1 * n2 > 0) {
			if (n1 < 0 /*|| n2 < 0*/) {
				n1 = -n1;
				n2 = -n2;
			}
			if (n2 > n1)
				n1 = n2;
		} else {
			n1 -= n2;
			if (n1 < 0)
				n1 = -n1;
			esp += 3;	/* see extra space in draw_keysig() */
		}
		if (n1 > 0) {
			s->pl = s->wl = 2;
			s->pr = s->wr = (float) (5 * n1 + esp + 2);
		}
		break;
	}
	case TIMESIG:
		/* !!tied to draw_timesig()!! */
		w = 3.5;
		for (i = 0; i < s->as.u.meter.nmeter; i++) {
			int l;

			if (s->as.u.meter.meter[i].top[0] == ' ') {
				w += 3;
				continue;
			}
			l = strlen(s->as.u.meter.meter[i].top);
			if (s->as.u.meter.meter[i].bot[0] != '\0') {
				int l2;

				l2 = strlen(s->as.u.meter.meter[i].bot);
				if (l2 > l)
					l = l2;
			}
			w += (float) (3.5 * l);
		}
		s->wl = s->pl = w;
		s->wr = s->pr = w + 5;
		break;
	case MREST:
		s->wl = 40 / 2 + 16;
		s->wr = 40 / 2 + 16;
		s->pl = s->wl + 16;
		s->pr = s->wr + 16;
		break;
	case MREP:
		s->wr = s->wl = 16 / 2 + 8;
		if (s->as.u.bar.len == 1)
			s->pr = s->pl = s->wr + 8;
		else	{
			s->pl = 0;
			s->pr = nwidth(s->len);
		}
		break;
	case GRACE:
		s->wl = 4;
		s->wr = set_graceoffs(s);
		s->pl = s->wl + 4;
		s->pr = s->wr + 4;
		break;
	case FMTCHG:
		if (s->u != STBRK)
			break;
		s->wl = s->xmx;
/*fixme: if the last symbol, remove it ??*/
		if (s->next->type != CLEF)
			s->wr = 8;
		else {
			s->wr = 2;
			s->next->u = 0;	/* big clef */
		}
		s->pl = s->wl + 4;
		s->pr = s->wr + 4;
		break;
	case INVISIBLE:
	case TEMPO:
	case PART:		/* no space */
		break;
	default:
		ERROR(("line %d: Cannot set width for sym type %d",
		       s->as.linenum, s->type));
		break;
	}
}

/* -- set widths and prefered space -- */
/* This routine sets the minimal left and right widths wl,wr
 * so that successive symbols are still separated when
 * no extra glue is put between them. It also sets the prefered
 * spacings pl,pr for good output.
 * All distances in pt relative to the symbol center. */
/* this routine is called only once for the whole tune */
static void set_sym_widths(void)
{
	struct SYMBOL *s;

	for (s = tssym; s != 0; s = s->ts_next)
		set_width(s);
}

/* -- set the end of a piece of tune -- */
/* tsnext is the beginning of the next line */
static void set_piece(struct SYMBOL *s)
{
	struct VOICE_S *p_voice;

	/* if last line, do nothing */
	if ((tsnext = s->ts_next) == 0)
		return;

	/* if the key signature changes on the next line,
	 * put it at the end of the current line */
	if (tsnext->type == KEYSIG) {
		for (s = tsnext; s->ts_next != 0; s = s->ts_next)
			if (s->ts_next->type != KEYSIG)
				break;
		if ((tsnext = s->ts_next) == 0)
			return;
	}
	s->ts_next = 0;

	/* set end of voices */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		int voice;

		voice = p_voice - voice_tb;
		for (s = tsnext->ts_prev; s != 0; s = s->ts_prev) {
			struct SYMBOL *s2;

			if (s->voice != voice)
				continue;

			/* set the word end / start */
			for (s2 = s; s2 != 0; s2 = s2->prev) {
				if (s2->type == NOTE) {
					s2->as.u.note.word_end = 1;
					break;
				}
				if (s2->type == BAR)
					break;
			}
			for (s2 = s->next; s2 != 0; s2 = s2->next) {
				if (s2->type == NOTE) {
					s2->sflags |= S_WORD_ST;
					break;
				}
				if (s2->type == BAR)
					break;
			}
			s->next = 0;
			break;
		}
	}
}

/* -- set the basic spacing before symbol -- */
static void set_spacing(struct SYMBOL *s)
{
	struct SYMBOL *prev;

	prev = s->prev;
	s->shrink = prev->wr + s->wl;
	s->x = prev->pr + s->pl;

	switch (s->type) {
	case NOTE:
	case REST:
		if (prev->type == GRACE)
			break;
		if (prev->type == BAR)
			s->x *= 1.5;
		if ((s->sflags & S_WORD_ST) == 0) /* reduce spacing within a beam */
			s->x *= fnnp;
		if (s->sflags & S_NPLET_END)	/* reduce spacing in n-plet */
			s->x *= gnnp;
		break;
	case CLEF:
		if (prev->len == 0)
			break;
		s->shrink = s->wl + 5;
		s->x = s->pl + 5;
		break;
	}
}

/* -- Set the characteristics of the glue between symbols, then
 *	position the symbols along the staff. If staff is overfull,
 *	only does symbols which fit in, returns this number. -- */
static void set_sym_glue(float width)
{
	float	alfa0, beta0;
	float	alfa, beta;
	int	cut;
	int	voice, time, seq;
	float	space, shrink, stretch;
	float	w;
	struct SYMBOL *s;
	struct SYMBOL *staff_further[MAXSTAFF];

	voice = 0;
	space = shrink = stretch = 0.0;	/* compiler warnings */
	alfa0 = ALFA_X;			/* max shrink and stretch */
	beta0 = BETA_X;
	if (cfmt.continueall) {
		alfa0 = cfmt.maxshrink;
		beta0 = BETA_C;
	}

	/* set spacing for the first symbols (clefs - one for each voice) */
	s = tssym;
	time = s->time;
	seq = s->seq;
	while (s != 0
	       && s->time == time
	       && s->seq == seq) {
		voice = s->voice;
		space = s->x
			= shrink = s->shrink
			= stretch = s->stretch = s->pl;
		staff_further[(int) s->staff] = s;

		/* set spacing for the next symbol */
		if (s->next == 0)
			bug("Only one symbol", 1);
		set_spacing(s->next);
		s = s->ts_next;
	}

	/* then loop over the symbols */
	cut = 0;
	for (;;) {
		struct SYMBOL *s2, *s3, *s4;
		float max_shrink, max_space;
		int len;
		struct VOICE_S *p_voice;

		/* get the notes at this time, set spacing
		 * and get the min shrinking */
		seq = s->seq;
		if (s->type == FMTCHG
		    && (s->u == LMARG
			|| s->u == RMARG)) {
			if (s->u == LMARG)
				cfmt.leftmargin = s->xmx;
			else	cfmt.rightmargin = s->xmx;
			if ((s = s->ts_next) == 0)
				break;
			continue;
		}
		len = s->time - time;
		time = s->time;
		max_shrink = max_space = 0;
		s4 = s;
		for (s2 = s; s2 != 0; s2 = s2->ts_next) {
			if (s2->time != time
			    || s2->seq != seq)
				break;

			/* if the previous symbol length is greater than
			   the length from the previous elements, adjust */
			if (s2->prev->len > len) {
				if (s2->len == 0) {

					/* (no space if cle change) */
#if 0
					if (s2->type == CLEF)
#endif
						s2->shrink = s2->x = 0;
				} else {
					s2->x = (space - s2->prev->x)
						* len / (s2->prev->len - len);
#if 1
					s2->shrink = 6;
#else
					s2->shrink = s2->wl;
#endif
				}
			}
			s3 = staff_further[(int) s2->staff];
			if (s2->shrink < s3->shrink + s3->wr + s2->wl - shrink
			    && !s3->as.u.note.invis
#if 1
			    && (s3->ly != 0
				|| (s2->ymn <= s3->ymx + 12
				    && s2->ymx + 12 >= s3->ymn))
#endif
			    ) {
				s2->shrink = s3->shrink + s3->wr + s2->wl - shrink;
			} else {
/*fixme: to do*/
				/* reduce shrink if s3 is far enough */
			}

			/* set the stretch value */
			switch (s2->type) {
			case NOTE:
			case REST:
				s2->stretch = s2->x * 2.0;
				break;
			case BAR:
			case MREST:
			case MREP:
				s2->stretch = s2->x * 1.4;
				break;
			default:
				s2->stretch = s2->x;
			}

			/* keep the symbol with larger space */
			if (s2->shrink > max_shrink)
				max_shrink = s2->shrink;
			if (s2->x > max_space) {
				max_space = s2->x;
				s4 = s2;
			}
		}

		/* make sure that space >= shrink */
		if (max_space < max_shrink) {
			max_space = max_shrink;
			if (s4->stretch < max_shrink)
				s4->stretch = max_shrink;
		}

		/* set the horizontal position goal */
		shrink += max_shrink;
		space += max_space;
		stretch += s4->stretch;

		/* adjust spacing and advance */
		s4 = s;				/* (for overfull) */
		staff_further[(int) s->staff] = s;
		for ( ; s != s2; s = s->ts_next) {
			voice = s->voice;
			s->x = space;
			s->shrink = shrink;
			s->stretch = stretch;

			s3 = staff_further[(int) s->staff];
			if (s3->time < time
			    || s->wr > s3->wr)
				staff_further[(int) s->staff] = s;

			/* set spacing for the next note */
			if (s->next != 0)
				set_spacing(s->next);
		}

		if (s == 0)
			break;

		/* check the total width */
		if (cfmt.continueall) {
			if (space <= width)
				continue;
			alfa = (space - width) / (space - shrink);
			if (alfa < alfa0)
				continue;
		} else if (shrink < width)
				continue;

		/* may have a key sig change at end of line */
		/*fixme: may also have a meter change*/
		if (s->type == KEYSIG)
			continue;
		s = s4->ts_prev;
		if (!cfmt.continueall) {
			ERROR(("Overfull in line %d",
			       s->as.linenum));
		}
		cut = 1;

		/* go back to the previous bar, if any */
		for (s2 = s; s2 != 0; s2 = s2->ts_prev) {
			if ((s2->type == BAR
			     && s2->as.u.bar.type != B_INVIS)
			    || s2->type == KEYSIG)
				break;
		}

		/* (should have some note) */
		if (s2 != 0
		    && s2->time > tssym->time)
			s = s2;

		/* don't cut on a grace note sequence if followed by note */
		if (s->type == GRACE
		    && s->next != 0
		    && s->next->type == NOTE)
			s = s->prev;

		/* restore the linkages */
		if (tsnext != 0)
			tsnext->ts_prev->ts_next = tsnext;
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			voice = p_voice - voice_tb;
			for (s2 = tsnext; s2 != 0; s2 = s2->ts_next) {
				if (s2->voice == voice) {
					if (s2->prev != 0)
						s2->prev->next = s2;
					break;
				}
			}
		}
		set_piece(s);
		break;
	}

	for (s = tssym; s->ts_next != 0; s = s->ts_next)
		;

	/* get the total space from the last effective symbol */
	while (s->type == TEMPO
	       || s->type == STAVES)
		s = s->ts_prev;
	space = s->x;
	stretch = s->stretch;
	shrink = s->shrink;

	/* if the last symbol is not a bar, add some extra space */
	if (s->type != BAR) {
		shrink += s->wr + 3.;
		space += s->pr + 5.;
		stretch += (s->pr + 5.) * 1.4;
	}

	/* set the glue, calculate final symbol positions */
	alfa = beta = 0;
	if (space > width) {
		alfa = (space - width) / (space - shrink);
		if (alfa > alfa0)
			alfa = alfa0;
	} else {
		beta = (width - space) / (stretch - space);
		if (beta > beta0) {
			if (!cfmt.continueall) {
				ERROR(("Underfull (%.0fpt of %.0fpt) in line %d",
					beta0 * stretch + (1 - beta0) * space,
					width, s->as.linenum));
			}
			if (!cfmt.stretchstaff)
				beta = 0;
			if (!cfmt.stretchlast
			    && tsnext == 0	/* if last line of tune */
			    && beta >= beta_last) {
				alfa = alfa_last; /* shrink underfull last line same as previous */
				beta = beta_last;
			}
		}
	}

	w = alfa * shrink + beta * stretch + (1 - alfa - beta) * space;
	if (w > width + 0.1) {			/* (+ 0.1 for floating point precision error) */
		alfa = width / shrink;
		for (s = tssym; s != 0; s = s->ts_next)
			s->x = alfa * s->shrink;
		w = width;
	} else if (alfa != 0) {
		for (s = tssym; s != 0; s = s->ts_next) {
			s->x = alfa * s->shrink
				+ (1. - alfa) * s->x;
		}
	} else {
		for (s = tssym; s != 0; s = s->ts_next) {
			s->x = beta * s->stretch
				+ (1. - beta) * s->x;
		}
	}

	/* add small random shifts to positions (if only one voice) */
	if (first_voice->next == 0) {
		for (s = voice_tb[0].sym; s->next != 0; s = s->next) {
			if (s->len > 0) {	/* if note or rest */
				float w1, w2;

				w1 = s->x - s->prev->x;
				w2 = s->next->x - s->x;
				if (w2 < w1)
					w1 = w2;
				s->x += RANFAC * ranf(-w1, w1);
			}
		}
	}

	alfa_last = alfa;
	beta_last = beta;

	realwidth = w;
}

/* -- check_bars -- */
static void check_bars(struct VOICE_S *p_voice)
{
	struct SYMBOL *sym;
	struct SYMBOL *s;

	sym = p_voice->sym;
	p_voice->bar_start = -1;
	p_voice->bar_text = 0;
	p_voice->bar_repeat = 0;

	/* split up unsuitable bars at end of staff */
	for (s = sym; s->next != 0; s = s->next)
		;
	if (s->type == KEYSIG && s->prev != 0 && s->prev->type == BAR)
		s = s->prev;
	if (s->type == BAR) {
		switch (s->as.u.bar.type) {
		case B_LREP:
			s->as.u.bar.type = B_SINGLE;
			p_voice->bar_start = B_LREP;
			break;
		case B_DREP:
			s->as.u.bar.type = B_RREP;
			p_voice->bar_start = B_LREP;
			break;
		case B_SINGLE:
		case B_RREP:
			if (s->as.u.bar.repeat_bar) {
				p_voice->bar_start = B_INVIS;
				p_voice->bar_text = s->as.text;
				p_voice->bar_repeat = 1;
				s->as.text = 0;
			}
			break;
		default:
			break;
		}
		s->as.u.bar.repeat_bar = 0;
	}

	/* merge back-to-back repeat bars */
	for (s = sym; s->next != 0; s = s->next) {
		if (s->type == BAR && s->as.u.bar.type == B_RREP) {
			if (s->next->type == BAR && s->next->as.u.bar.type == B_LREP) {
				s->type = INVISIBLE;
				s->next->as.u.bar.type = B_DREP;
				s->next->x = 0.5 * (s->x + s->next->x);
			}
		}
	}

	/* remove single bars next to another bar */
	for (s = sym->next; s->next != 0; s = s->next) {
		if ((s->type == BAR && s->as.u.bar.type == B_SINGLE)
		    && ((s->next->type == BAR && s->next->as.u.bar.type != B_INVIS)
			|| (s->prev->type == BAR && s->prev->as.u.bar.type != B_INVIS)))
			s->type = INVISIBLE;
	}
}

/* -- check if any "real" symbol in the piece -- */
/* and adjust the left and right margin */
static int any_symbol(void)
{
	struct SYMBOL *s;

	for (s = first_voice->sym; s != 0; s = s->ts_next) {
		switch (s->type) {
		case NOTE:
		case REST:
		case MREST:
		case BAR:
		case MREP:
			return 1;
		case FMTCHG:
			if (s->u == LMARG)
				cfmt.leftmargin = s->xmx;
			else if (s->u == RMARG)
				cfmt.rightmargin = s->xmx;
			break;
		}
	}
	return 0;
}

/* -- find one line to output -- */
static void find_piece(void)
{
	struct SYMBOL *s;
	int number, time, seq, i;

	if (!cfmt.continueall) {
		if ((number = cfmt.barsperstaff) == 0) {

			/* find the first end-of-line */
			for (s = tssym; /*s != 0*/; s = s->ts_next) {
				if (s->sflags & S_EOLN)
					break;
				if (s->ts_next == 0) {
					/* when '\' at end of line and 'P:' */
/*					bug("no eoln in piece", 0); */
					break;
				}
			}
		} else {
			int voice;

			/* count the measures */
			voice = first_voice - voice_tb;
			for (s = tssym; s->ts_next != 0; s = s->ts_next)
				if (s->len > 0)		/* if note or rest */
					break;
			for ( ; s->ts_next != 0; s = s->ts_next) {
				if (s->type != BAR
				    || s->as.u.bar.type == B_INVIS)
					continue;
				if (s->prev->type == BAR)
					continue;
				if (s->voice == voice
				    && --number <= 0)
					break;
			}
		}

		/* cut at the last symbol of the sequence */
		time = s->time + s->len;
		seq = s->seq;
		if (s->len > 0) {

			/* note or rest: cut on end time */
			for (; s->ts_next != 0; s = s->ts_next)
				if (s->ts_next->time >= time)
					break;
		} else {

			/* other symbol: cut at end of sequence */
			for (; s->ts_next != 0; s = s->ts_next)
				if (s->ts_next->seq != seq)
					break;
		}
		set_piece(s);
	} else	tsnext = 0;

	for (i = nstaff; i >= 0; i--) {
		staff_tb[i].nvocal = 0;
		staff_tb[i].y = 0;
	}
}

/* -- init symbol list with clef, meter, key -- */
static void init_music_line(struct VOICE_S *p_voice)
{
	struct SYMBOL *s, *sym;
static	int meter_f;

	sym = p_voice->sym;
	p_voice->sym = 0;

	/* add clef */
	s = add_sym(p_voice, CLEF);
	memcpy(&p_voice->clef, &staff_tb[p_voice->staff].clef,
	       sizeof p_voice->clef);
	memcpy(&s->as.u.clef, &p_voice->clef,
	       sizeof s->as.u.clef);

	/* add keysig */
	s = add_sym(p_voice, KEYSIG);
	s->seq++;
	s->as.u.key.sf = p_voice->sf;
	s->as.u.key.bagpipe = p_voice->bagpipe;
	if (p_voice->bagpipe && s->as.u.key.sf == 2)	/* K:Hp */
		s->u = 3;				/* --> Gnat */

	/* add time signature if needed */
	if (insert_meter
	    || (p_voice != first_voice && meter_f)) {

		/* the time signature is taken from the voice 0 */
		if (first_voice->meter.nmeter != 0) {	/* != M:none */
			s = add_sym(p_voice, TIMESIG);
			s->seq += 2;
			memcpy(&s->as.u.meter,
			       &first_voice->meter,
			       sizeof s->as.u.meter);
		}
	}
	if (p_voice == first_voice) {
		meter_f = insert_meter;
		insert_meter = 0;
	}

	/* add tempo if any */
	if (info.tempo) {
		s = info.tempo;
		memset((&s->as) + 1, 0,
		       sizeof (struct SYMBOL) - sizeof (struct abcsym));
		p_voice->last_symbol->next = s;
		s->prev = p_voice->last_symbol;
		p_voice->last_symbol = s;
		s->voice = p_voice - voice_tb;
		s->staff = p_voice->staff;
		s->type = TEMPO;
		s->seq = s->prev->seq + 1;	/*??*/
		info.tempo = 0;
	}

	/* add bar if needed */
	if (p_voice->bar_start >= 0) {
		s = add_sym(p_voice, BAR);
		s->seq += 3;
		s->as.u.bar.type = p_voice->bar_start;
		s->as.text = p_voice->bar_text;
		p_voice->bar_start = -1;
		s->as.u.bar.repeat_bar = p_voice->bar_repeat;
		p_voice->bar_repeat = 0;
	}
	if ((p_voice->last_symbol->next = sym) != 0)
		sym->prev = p_voice->last_symbol;
	p_voice->nvocal = 0;
}

/* -- initialize a new line -- */
static void cut_symbols(void)
{
	struct VOICE_S *p_voice;
	struct SYMBOL *s, *s1;
	int done;
	int j, t;

	clrarena(3);

	s1 = tsnext;

	/* set start of voices */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		int voice;

		p_voice->sym = 0;		/* may have no symbol */
		voice = p_voice - voice_tb;
		for (s = s1; s != 0; s = s->ts_next) {
			if (s->voice == voice) {
				p_voice->sym = s;
				p_voice->staff = s->staff;
				s->prev = 0;
				break;
			}
		}
	}

	/* add the first symbols of the line */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		init_music_line(p_voice);

	/* insert the new symbols into the time sorted list */
		p_voice->s_anc = p_voice->sym;
	}

	done = 0;
	s = 0;
	t = s1->time;
	for (j = 1; ; j++) {
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			s1 = p_voice->s_anc;
			if (s1 == 0)
				continue;
			if (s1->ts_prev != 0) {
				done = 1;	/* old symbol */
				continue;
			}
			if (s == 0)
				tssym = s1;
			else	s->ts_next = s1;
			s1->ts_prev = s;
			s = s1;
			s->seq = j;
			s->time = t;
			p_voice->s_anc = s->next;
			set_width(s);		/* set the width of the symbol */
		}
		if (done)
			break;
	}
	s->ts_next = tsnext;
	tsnext->ts_prev = s;
}

/* -- adjust the values in the postscript buffer -- */
static void buffer_adjust(void)
{
	int i;
	float v_tb[MAXSTAFF];

	for (i = 0; i <= nstaff; i++)
		v_tb[i] = staff_tb[i].y;
	set_buffer(v_tb);
}

/* -- output for parsed symbol list -- */
void output_music(void)
{
	float	lscale, lwidth;
	struct VOICE_S *p_voice;
	int	voice, first_line;

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
		if (p_voice->sym != 0)
			break;
	if (p_voice == 0)
		return;		/* no symbol at all */

	lvlarena(2);

	/* duplicate the voices appearing in many staves */
	voice_dup();

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
		init_music_line(p_voice);

	first_line = 1;
	alfa_last = 0.1;
	beta_last = 0.0;

	lscale = cfmt.scale;

	/* dump buffer if not enough space for a staff line */
	check_buffer();

	set_global();		/* set global characteristics */
	if (first_voice->next != 0)	/* when multi-voices */
		set_multi();	/* set the stems direction in 'multi' */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
		set_beams(p_voice->sym);	/* decide on beams */
	set_stems();		/* set the stem lengths */
	set_overlap();		/* set note shift when voices overlap */
	set_sym_widths();	/* set the symbol widths */

	/* set all symbols (per music line) */
	clrarena(3);
	lvlarena(3);
	for (;;) {		/* loop over pieces of line for output */
		find_piece();

		if (any_symbol()) {
			float indent, line_height;

			lwidth = ((cfmt.landscape ? cfmt.pageheight : cfmt.pagewidth)
				- cfmt.leftmargin - cfmt.rightmargin) / lscale;
			check_margin();
			indent = set_indent(first_line);
			set_sym_glue(lwidth - indent);

			for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
				check_bars(p_voice);
			if (indent != 0)
				PUT1("%.2f 0 T\n", indent);	/* do indentation */
			draw_sym_near();
			line_height = set_staff();
			buffer_adjust();
			draw_staff(first_line, indent);
			for (p_voice = first_voice; p_voice; p_voice = p_voice->next)
				draw_symbols(p_voice);
			draw_all_deco();
			if (indent != 0)
				PUT1("%.2f 0 T\n", -indent);
			bskip(line_height);
			buffer_eob();
			first_line = 0;
		}
		if (tsnext == 0)
			break;
		cut_symbols();
	}
	lvlarena(2);

	/* reset the parser */
	for (voice = MAXVOICE; --voice >= 0; )
		voice_tb[voice].sym = 0;
	check_margin();
}

/* -- reset the generator -- */
void reset_gen(void)
{
	insert_meter = 1;
}

/*  
 * This file is part of abcm2ps.
 * Copyright (C) 1998-2001 Jean-François Moine
 * (adapted from abc2ps, Copyright (C) 1996,1997 Michael Methfessel)
 * See file abc2ps.c for details.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "abcparse.h"
#include "abc2ps.h" 

#define BUFFLN	100		/* max number of lines in output buffer */

static int ln_num;		/* number of lines in buffer */
static float ln_pos[BUFFLN];	/* vertical positions of buffered lines */
static int ln_buf[BUFFLN];	/* buffer location of buffered lines */
static char buf[BUFFSZ];	/* output buffer.. should hold one tune */
static float posx;		/* current left margin */
static float botpage;		/* bottom of page */
static float posy;		/* vertical position on page */
static float bposy;		/* current position in buffered data */
static int nepsf;		/* counter for epsf output files */

char *mbf;			/* where to PUTx() */
int nbuf;			/* number of bytes buffered */
int use_buffer;			/* 1 if lines are being accumulated */

/*  subroutines for postscript output  */

/* -- initialize the postscript file -- */
static void init_ps(FILE *fp,
		    char *str,
		    int  is_epsf)
{
	time_t ltime;
	char tstr[41];

	if (is_epsf) {
/*fixme: no landscape for EPS?*/
		fprintf(fp, "%%!PS-Adobe-3.0 EPSF-3.0\n"
			"%%%%BoundingBox: 0 0 %.0f %.0f\n",
			cfmt.pagewidth - cfmt.leftmargin - cfmt.rightmargin + 20,
			-bposy * cfmt.scale);
	} else	fprintf(fp, "%%!PS-Adobe-3.0\n");

	/* Title */
	fprintf(fp, "%%%%Title: %s\n", str);

	/* CreationDate */
	time(&ltime);
	strcpy(tstr, ctime(&ltime));
	tstr[24]='\0';
	tstr[16]='\0';
	fprintf(fp, "%%%%Creator: abcm2ps " VERSION "\n"
		"%%%%CreationDate: %s %s\n", tstr + 4, tstr + 20);
	if (!is_epsf)
		fprintf(fp, "%%%%Pages: (atend)\n");
	fprintf(fp, "%%%%LanguageLevel: %d\n"
		"%%%%EndComments\n\n",
		PS_LEVEL);

	if (is_epsf)
		fprintf(fp, "gsave /origstate save def mark\n100 dict begin\n\n");

	fprintf(fp, "%%%%BeginSetup\n"
		"/bdef {bind def} bind def\n"
		"/T {translate} bdef\n"
		"/M {moveto} bdef\n"
		"/dlw {0.7 setlinewidth} bdef\n");
#if PS_LEVEL==1
	fprintf(fp, "/selectfont { exch findfont exch dup   %% emulate level 2 op\n"
		"\ttype /arraytype eq {makefont}{scalefont} ifelse setfont\n"
		"} bdef\n");
#endif
	define_encoding(fp, cfmt.encoding);
	define_fonts(fp);
	define_symbols(fp);
	fprintf(fp, "\n0 setlinecap 0 setlinejoin\n");
	write_user_ps(fp);
	fprintf(fp, "%%%%EndSetup\n");
	file_initialized = 1;
}

/* -- close_ps -- */
void close_ps(void)
{
	fprintf(fout, "%%%%Trailer\n"
		"%%%%Pages: %d\n"
		"%%EOF\n",
		pagenum);
}

/* -- initialize postscript page -- */
static void init_page(FILE *fp)
{
	float pheight, pwidth, swidth;
	if (in_page)
		return;

	if (!file_initialized)
		init_ps(fp, in_fname, 0);
	in_page = 1;
	pagenum++;

	fprintf(fp, "\n%%%%Page: %d %d\n"
		"%%%%BeginPageSetup\n",
		pagenum, pagenum);
	if (cfmt.landscape) {
		pheight = cfmt.pagewidth;
		pwidth = cfmt.pageheight;
		fprintf(fp, "%%%%PageOrientation: Landscape\n"
			"gsave 90 rotate %.1f %.1f T\n",
			cfmt.leftmargin, -cfmt.topmargin);
	} else {
		pheight = cfmt.pageheight;
		pwidth = cfmt.pagewidth;
		fprintf(fp, "gsave %.1f %.1f T\n",
			cfmt.leftmargin, pheight - cfmt.topmargin);
	}
/*3.1.0: global scale*/
	fprintf(fp, "%.2f dup scale\n"
		"%%%%EndPageSetup\n",
		cfmt.scale);
	swidth = (pwidth - cfmt.leftmargin - cfmt.rightmargin) / cfmt.scale;

	/* write the page number on the right (not at right:)) side */
	if (pagenumbers) {
		float top;

		top = (cfmt.topmargin - 32.0) / cfmt.scale;
		fprintf(fp, "/Times-Roman 12 selectfont ");
		if (((pagenumbers - 1) & 1) ^ (pagenum & ((pagenumbers - 1) >> 1)))
			fprintf(fp, "%.1f %.1f M (%d) lshow\n",
				swidth, top, pagenum);
		else	fprintf(fp, "%.1f %.1f M (%d) show\n",
 				0.0, top, pagenum);
	}

	/* ouput the footer */
	if (cfmt.footer != 0) {
		char str[128];

		tex_str(str, cfmt.footer, sizeof str, 0);

		fprintf(fp, "%.1f F%d"
			" %.1f %.1f M (%s) cshow\n",
			cfmt.textfont.size,
			cfmt.textfont.fnum,
			swidth * 0.5,
			- (pheight - cfmt.topmargin
				- cfmt.botmargin) / cfmt.scale
				+ (cfmt.textfont.size + 5.),
			str);
	}
}

/* -- close_page -- */
void close_page(FILE *fp)
{
	if (!in_page)
		return;
	in_page = 0;

	fprintf(fp, "\n%%%%PageTrailer\n"
		"grestore\n"
		"showpage\n");
}

/* -- output the EPS file -- */
void write_eps(void)
{
	FILE *feps;
	char fnm[81], finf[81];

	close_output_file();
	if (choose_outname) {
		epsf_title(info.title[0], fnm);
		strcat(fnm, ".eps");
	} else {
		nepsf++;
		sprintf(fnm, "%s%03d.eps", outf, nepsf);
	}
	sprintf(finf, "%s (%s)", in_fname, info.xref);
	if ((feps = fopen(fnm, "w")) == 0) {
		printf("Cannot open output file %s\n", fnm);
		exit(2);
	}
	init_ps(feps, finf, 1);
	fprintf(feps, "%.2f dup scale 10 %.1f T\n",
		cfmt.scale, -bposy);
	write_buffer(feps);
	fprintf(feps, "\nshowpage\nend\n"
		"cleartomark origstate restore grestore\n\n");
	fclose(feps);
}

/* -- write_pagebreak -- */
void write_pagebreak(FILE *fp)
{
	close_page(fp);
	init_page(fp);
	if (page_init[0] != '\0')
		fprintf(fp, "%s\n", page_init);
	posy = ((cfmt.landscape ? cfmt.pagewidth : cfmt.pageheight)
		- cfmt.topmargin) / cfmt.scale;
	if (cfmt.footer != 0)
		posy -= cfmt.textfont.size + 10.;
}

/*  subroutines to handle output buffer  */

/* -- update the output buffer pointer -- */
/* called from the PUTx() macros */
void a2b(void)
{
	int l;

	if (!in_page && !epsf) {
		init_pdims();
		init_page(fout);
	}

	l = strlen(mbf);
	/*  printf ("Append %d <%s>\n", l, t); */

	nbuf += l;
	if (nbuf >= BUFFSZ - 500) {	/* must have place for 1 more line */
		ERROR(("a2b: buffer full, BUFFSZ=%d", BUFFSZ));
		exit(3);
	}

	mbf += l;
}

/* -- translate down by h points in output buffer -- */
void bskip(float h)
{
	if (h * h > 0.01) {
		PUT1("0 %.1f T\n", -h);
		bposy -= h;
	}
}

/* -- do horizontal shift if needed -- */
void check_margin(void)
{
	float dif;

	if (!in_page)
		return;
	dif = cfmt.leftmargin - posx;
	if (dif * dif < 0.01)
		return;

	PUT1("%.1f 0 T\n", dif / cfmt.scale);
	posx = cfmt.leftmargin;
}

/* -- initialize page dimensions -- */
void init_pdims()
{
	posx = cfmt.leftmargin;
	posy = ((cfmt.landscape ? cfmt.pagewidth : cfmt.pageheight)
		- cfmt.topmargin) / cfmt.scale;
	botpage = cfmt.botmargin / cfmt.scale;
	if (cfmt.footer != 0)
		botpage += cfmt.textfont.size + 10.;
}

/* -- clear_buffer -- */
void clear_buffer()
{
	nbuf = 0;
	bposy = 0.0;
	ln_num = 0;
	mbf = buf;
}

/* -- write buffer contents, break at full pages -- */
void write_buffer(FILE *fp)
{
	int i, l, b2;
	float p1, dp;

	if (nbuf == 0)
		return;

	i = 0;
	p1 = 0;
	for (l = 0; l < ln_num; l++) {
		b2 = ln_buf[l];
		dp = ln_pos[l] - p1;
		if (posy + dp < botpage && !epsf)
			write_pagebreak(fp);
		fwrite(&buf[i], 1, b2 - i, fp);
		i = b2;
		posy += dp;
		p1 = ln_pos[l];
	}

	fwrite(&buf[i], 1, nbuf - i, fp);

	clear_buffer();
}

/* -- handle completed block in buffer -- */
/* if the added stuff does not fit on current page, write it out
   after page break and change buffer handling mode to pass though */
void buffer_eob(void)
{
	if (ln_num >= BUFFLN) {
		ERROR(("max number of buffer lines exceeded"
			" -- check BUFFLN"));
		exit(3);
	}

	ln_buf[ln_num] = nbuf;
	ln_pos[ln_num] = bposy;
	ln_num++;

	if (!use_buffer) {
		write_buffer(fout);
		return;
	}

	if ((posy + bposy < botpage
	     || cfmt.oneperpage)
	    && !epsf) {
		if (tunenum != 1)
			write_pagebreak(fout);
		write_buffer(fout);
		use_buffer = 0;
	}
}

/* -- dump buffer if less than nb bytes available -- */
void check_buffer(void)
{
	if (nbuf + BUFFSZ1 > BUFFSZ) {
		ERROR(("possibly bad page breaks, BUFFSZ exceeded at line %d",
		      ln_num));
		write_buffer(fout);
		use_buffer = 0;
	}
}

/* -- return the current vertical offset in the page -- */
float get_bposy(void)
{
	return bposy;
}

/* -- set value in the output buffer -- */
/* The values are flagged as "\x01vnnn.nn" where
 * - 'v' is the variable index ('0'..'z')
 * - 'nnn.nn' is the value to add to the variable
 * The variables are:
 * '0'..'O': staff offsets */
/* this function must be called before buffer_eob() */
void set_buffer(float *p_v)
{
	char *p;
	int i;
	float v;

	if (ln_num == 0)
		p = buf;
	else	p = &buf[ln_buf[ln_num - 1]];
	while ((p = strchr(p, '\x01')) != 0) {
		i = p[1] - '0';
		sscanf(p + 2, "%f", &v);
		p += sprintf(p, "%7.1f", p_v[i] + v);
		*p = ' ';
	}
}

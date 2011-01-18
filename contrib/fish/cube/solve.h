

/**********************************************************************
	Human solvers should try to memorize as many from the following
    five arrays as possible. `CornerMoves' cycles three corners and
    `corners' switches two. Both have the same result, but it's easier to
    use `corners'. The rest of the stuff in this file puts two pieces at
    one time in the middle layer and `all' of them in one shot in the last
    layer. There are 15552 configurations for the last layer.  I don't have
    nearly that number. They all exist, but I haven't found them all. Some
    here are 18 moves. Shorter ones probably exist, but they are hard to
    find.
***********************************************************************/

char *EdgeTwist[]={
   "ar2u2ruarauar2ulfrafal",    /* only 13 moves */
   "lafualfabuarfaurafbau",
   "2r2b2r2uralblar2u2r2b2ru"};

/* THE FOLLOWING 8 CASES MOVE AND TWIST THREE EDGE PIECES.
*
*	     B |\
*	       | \
*	 D     |  \ C
*	       |  /
*	       | /
*	     A |/
*
*	Edge D doesn't move.  Corners never move.
*/

				/*   twist    dir  moves    code  # */
char *EdgeMoves[]=		/*----------------------------------*/
   {				/*  a  b  c			    */
   "2rufab2rafbu2r",            /*  o  o  o  clock   9      021   0 */
   "2raufab2rafbau2r",          /*  o  o  o  anti    9      320   1 */
   "araf2r2d2lab2l2dar",        /*  +  o  +  clock   9      e21   2 */
   "r2d2lb2l2d2rfr",            /*  o  +  +  anti    9      32e   3 */
   "fuafalabaraurbl",           /*  +  +  o  clock  10      e26   4 */
   "alabarurblfauaf",           /*  +  o  +  anti   10      b20   5 */
   "lfrauarafalabub",           /*  o  +  +  clock  10      026   6 */
   "abaublfruarafal",           /*  +  +  o  anti   10      b2e   7 */

/************** switch two pairs of edges - various twists ***************/
/**************     All of these leave corners intact.	   ***************/

   "2f2bad2l2f2b2r2f2bd2f2b",     /* sw red-green no tw  12 210   8 */
   "rbuabau2rafaufur",            /*  "    twist bl-Or   11 21e   9 */
   "fbr2u2b2u2b2u2barafab",       /*  "    twist four    12 76e  10 */
   "rl2uaralafab2ufb",            /* sw red-or no twist  10 301  11 */
   "afaual2ulfur2uar",            /* sw red-or tw red-bl 10 3e6  12 */
   "lar2fralaularafralularafral", /* sw red-gn tw bl-gn  17 71e  13 */
   "fab2rbafufabrbafaufabrbaf",   /* sw red-gn tw rd-or  17 260  14 */
   "ufauaruruafar2uafaufur",      /* sw red-gn tw gn-or  15 710  15 */
   "arauafuf2urfauarauruafau",    /* sw red-gn tw rd-bl  15 26e  16 */
   "lufauafal2ufraubuabaraf2u",   /* sw red-or tw 4      16 be6 New 6/30/92*/
   "blfauafualabarafaufur",       /* cycle 3 tw 4        14 e76 New 7/1/92 */
   "arauafufrblaufuafalab",       /* as above, backwards 14 b7e  19 */
   };

char *corners[]={   /*	 For slow solve. Switch two corners.	*/
		    /*	 These mess up edges.  Use first.	*/
   "alaulf2uafalul",    /* diagonal twist rb + lb */
   "alaulfuafalul",     /* sw front twist rf + rb */
   "araurbauabarur",    /*    front       lf + rb */
   "alaulfauafalul",    /*    back        lf + rb */
   "araurbuabarur",     /*    back        lf + lb */
   "afaufruarafuf",     /*    right       lb + rb */
   "abaublaualabub",    /*    right       lb + rf */
   "abaublualabub",     /*    left        lf + rf */
   "afaufrauarafuf"     /*    left        lb + rf */
   };
char *twistcorners[]={	   /* Mess up edges.  Use corners first then this
			      then edges.  */
   "araurauar2ur",
   "ar2uruarur",
   "afaufauafufauaf2uf",   /*11 *459-231 */
   "r2u2rau2rau2r2ur2u",   /*10 476-102 */
   "2uab2u2bu2bu2b2uab",   /*10 *a79-231 */
   };
/* Following are 18 cases of moving and twisting three corner pieces.
*
*		  d	/|b
*		       / |
*		      /  |
*		     /	 |	Edges never move.
*		  a /____|c	Corner d never moves.
*/
			      /*  twist    dir	*/
char *CornerMoves[]=	      /* a  b  c	*/
   {			      /* -------------- */
   "2rdar2uradar2uar",        /* +  -  0  clock  9 #0  *043-123 */
   "r2urdar2urad2r",          /* +  0  -  anti   9  1  *5b3-123 */
   "arulaurualau",            /* -  -  -  clock  8  2  *8a3-123 */
   "ulauarualaur",            /* +  +  +  anti   8  3  *7b3-123 */
   "af2uafadf2uafd2f",        /* 0  -  +  clock  9  4  *b13-123 */
   "2fadf2uafdf2uf",          /* +  -  0  anti   9  5  *2b3-123 */
   "rbarfrabaraf",            /* -  +  0  clock  8  6  *0a3-123 */
   "frbarafrabar",            /* -  o  +  anti   8  7  *783-123 */
   "f2d2bd2fad2bd2fdaf",      /* +  +  +  clock 11  8  *b43-123 */
   "fad2fad2bd2fad2b2daf",    /* -  -  -  anti  11  9  *583-123 */
   "arafalfraflf",            /* o  +  -  clock  8 10  *813-123 */
   "afalfaraflfr",            /* -  +  o  anti   8 11  *283-123 */
   "2b2lb2rab2lb2rb",         /* -  o  +  clock  9 12  *ba3-123 */
   "ab2rab2lb2rab2l2b",       /* o  -  +  anti   9 13  *703-123 */
   "l2fl2bal2fl2b2l",         /* +  o  -  clock  9 14  *843-123 */
   "2l2bal2fl2bal2fal",       /* o  +  -  anti   9 15  *503-123 */
   "aflaf2rfalaf2r2f",        /* o  o  o  clock  9 16  *013-123 */
   "2f2rflaf2rfalf"           /* o  o  o  anti   9 17  *203-123 */
   };
/***************************************************************************
    End of what might be useful to human solvers.
    Look at bottom of file for explanation of codes.
***************************************************************************/

char *topedges[24]={
   "2rad2f","","2ld2f","2b2d2f","auru","afauru","ualau","fauru","lf",
   "f","alfl","2lf2l","auaru","bulau","ulau","adrafar","rafar","af",
   "araf","2ub2u","ad2f","2d2f","d2f","2f"
   };		/* average 10.16 for all four */
char *topcorns[]={
   "abadbfdaf","","ldalaradr","bf2dabaf","f2dafar2dr","fdaf","daradr",
   "afad2f2daf","al2dlaradr","lardral","ardr","f2daf","fadaf","abfadbaf",
   "bar2drab","ar2dr","dar2dr","aradr","fadafaradr","rd2r2dr","ardrf2daf",
   "f2dafaradr","fdafaradr","ar2drfdaf"};   /* average 18.5 for all four */
char *middle[]={
   "r2uar2uafauf", "b2ldfad2lab", "b2ldafad2lab", "f2ul2fal2uaf",
   "2r2u2r2uarf2rafauaraf2uf","","afarafruf","ab2rauru2rb","l2fu2fau2fal",
   "2f2u2f2u2f", "", "2bauad2rdu2b","arauabrbr", "afufurauar",
   "2badbuabard2b", "", "", "", "f2ulafal2uaf","2r2u2r2u2r" };

char *(quickmiddle[16][16])= /* Puts two pieces at a time in middle layer.*/
   {{
   "",                       /*0-0      */
   "aur2uaraualaf2ufl",      /*0-1   10 */
   "ruaralaf2ufl",           /*0-2    8 */
   "al2daf2uf2dl",           /*0-3    7 */
   "2ualauafuf2l2fal2f",     /*0-4   10 */
   "farafadafdr",            /*0-5    7 */
   "ar2bad2fd2br",           /*0-6    7 */
   "2l2f2ual2u2f2l",         /*0-7    7 */
   "2u2b2dl2d2b2ur",         /*0-8    8 */
   "2ub2dafadladab",         /*0-9    8 */
   "aurfadalauafld",         /*0-10   9 */
   "rfuafaraualaul",         /*0-11   9 */
   "u2lualdaufad2l",         /*0-12   9 */
   "l2ub2lab2ual",           /*0-13   7 one */
   "",                       /*0-14     */
   "al2daf2r2dab2ual",},     /*0-15   8  subtotal 114*/
   {
   "aual2daf2uf2dl",         /*1-0    8 */
   "",                       /*1-1      */
   "2ur2uaraualaf2ufl",      /*1-2   10 */
   "auruaralaf2ufl",         /*1-3    9 */
   "ualauafuf2l2fal2f",      /*1-4   10 */
   "aufarafadafdr",          /*1-5    8 */
   "",                       /*1-6      */
   "l2bd2fad2bal",           /*1-7    7 */
   "u2b2dl2d2b2ur",          /*1-8    8 */
   "ub2dafadladab",          /*1-9    8 */
   "frafadalulafd",          /*1-10   9 */
   "u2fl2faral2fr2f",        /*1-11   9 */
   "2lualdaufad2l",          /*1-12   8 */
   "f2uaf2ualaul",           /*1-13   7 one */
   "urauaradaluld",          /*1-14   9 */
   "aual2daf2r2dab2ual",},   /*1-15   9   sub119 */
   {
   "ufadalfauafld",          /*2-0    9 */
   "drafaufaradf",           /*2-1    8 */
   "",                       /*2-2      */
   "ur2uaraualaf2ufl",       /*2-3   10 */
   "alauafuf2l2fal2f",       /*2-4    9 */
   "2ufarafadafdr",          /*2-5    8 */
   "rauaradaluld",           /*2-6    8 */
   "",                       /*2-7      */
   "2b2dl2d2b2ur",           /*2-8    7 */
   "b2dafadladab",           /*2-9    7 */
   "urfadalauafld",          /*2-10   9 */
   "ruaradalauld",           /*2-11   8 */
   "2lulaudafad2l",          /*2-12   8 */
   "r2bdlad2bar",            /*2-13   7 one */
   "2l2f2ul2u2f2l",          /*2-14   7 */
   "aflafalaufd2rad",},      /*2-15   9  sub 114 */
   {
   "r2uaraualaf2ufl",        /*3-0    9 */
   "fadalfauafld",           /*3-1    8 */
   "ual2daf2uf2dl",          /*3-2    8 */
   "",                       /*3-3      */
   "aualauafuf2l2fal2f",     /*3-4   10 */
   "al2dafauf2dl",           /*3-5    7 */
   "d2ldrau2far2dl",         /*3-6    9 */
   "aurauaradaluld",         /*3-7    9 */
   "au2b2dl2d2b2ur",         /*3-8    8 */
   "aub2dafadladab",         /*3-9    8 */
   "rfadalauafld",           /*3-10   8 */
   "",                       /*3-11     */
   "laualb2df2dab",          /*3-12   8 */
   "r2bdalad2bar",           /*3-13   7 one */
   "adalauldruar",           /*3-14   8 */
   "ual2daf2r2dab2ual",},    /*3-15   9  sub 116*/
   {
   "aufral2uarlf",           /*4-0    8 */
   "2ufral2uarlf",           /*4-1    8 */
   "ufalr2ularf",            /*4-2    8 */
   "fral2uarlf",             /*4-3    7 */
   "",                       /*4-4      */
   "fld2fadafalau2f",        /*4-5    9 */
   "auar2u2b2dal2d2b",       /*4-6    8 */
   "2uar2u2b2dal2d2b",       /*4-7    8 */
   "2f2u2f2u2f",             /*4-8    5 */
   "2ldau2fad",              /*4-9    5 */
   "ful2fdradalaf",          /*4-10   9 */
   "uar2u2b2dal2d2b",        /*4-11   8 */
   "al2ualaualdfaufad",      /*4-12  10 */
   "",                       /*4-13     */
   "ar2u2b2dal2d2b",         /*4-14   7 */
   "2fdau2rad",},            /*4-15   5 sub 105*/
   {""},                     /* no 5s   */
   {
   "af2uabarafrbauf",        /*6-0    9 */
   "",                       /*6-1      */
   "bl2bdafdr2db",           /*6-2    9 */
   "2f2uaf2u2f",             /*6-3    5 */
   "afauarau2fufrf",         /*6-4    9 */
   "fulafalaf",              /*6-5    6 */
   "",                       /*6-6      */
   "bdflafalafadab",         /*6-7    9 */
   "2uafalr2ularaf",         /*6-8    8 */
   "faraf2dal2dr",           /*6-9    7 */
   "flafdradalaf",           /*6-10   8 */
   "adafdrafufar",           /*6-11   8 */
   "af2r2dab2daruar",        /*6-12   8 */
   "l2ubalab2ual",           /*6-13   7 one */
   "2ur2df2uaf2dar",         /*6-14   8 */
   "2f2ufud2rad",},          /*6-15   7 sub 106*/
   {
   "u2f2uf2u2f",             /*7-0    6 */
   "f2ublfalabuaf",          /*7-1    9 */
   "",                       /*7-2      */
   "fauafudaf2ufad",         /*7-3    9 */
   "auafauarau2fufrf",       /*7-4   10 */
   "aufulafalaf",            /*7-5    7 */
   "ur2df2uaf2dar",          /*7-6    8 */
   "",                       /*7-7      */
   "uafalr2ularaf",          /*7-8    8 */
   "aufaraf2dal2dr",         /*7-9    8 */
   "r2fabrbafauafar",        /*7-10   9 */
   "fuarlfalr2uaf",          /*7-11   9 */
   "auaf2r2dab2daruar",      /*7-12   9 */
   "ar2faufu2fr",            /*7-13   7 one*/
   "auadafdrafufar",         /*7-14   9 */
   "au2f2ufud2rad",},        /*7-15   8   sub 116 */
   {
   "lubd2falafadab",         /*8-0    9 */
   "fulu2fauafalaf",         /*8-1    9 */
   "ufrf2uafauarau2f",       /*8-2   10 */
   "2ufulu2fauafalaf",       /*8-3   10 */
   "alfaularfadlafdralaf",   /*8-4   13 twist two*/
   "",                       /*8-5      */
   "aurufauaf2r2fr2f",       /*8-6   10 */
   "2urufauaf2r2fr2f",       /*8-7   10 */
   "",                       /*8-8      */
   "ru2radafadal2dr",        /*8-9    9 */
   "abadafar2dablau2l2dafd", /*8-10  12 messes all four. */
   "urufauaf2r2fr2f",        /*8-11  10 */
   "aradalaflulaudrf",       /*8-12  11 */
   "fauf2ul2fal2u2f",        /*8-13   9twist one*/
   "rufauaf2r2fr2f",         /*8-14   9 */
   "2fauruarafd2rad",},      /*8-15   9    sub 140 */
   {
   "ufadald2uarfr",          /*9-0    9 */
   "fadald2uarfr",           /*9-1    8 */
   "aufadald2uarfr",         /*9-2    9 */
   "2ufadald2uarfr",         /*9-3    9 */
   "bdfl2dbaru2r2dfad",      /*9-4   12 messes all four. no problem? */
   "ldlafaluadal",           /*9-5    8 better than 64 */
   "2ubdabal2badbrb",        /*9-6   10 */
   "lubd2fadalabal",         /*9-7    9 */
   "afauar2fadaldrf",        /*9-8    9 */
   "",                       /*9-9      */
   "rfadafalafdf",           /*9-10   8 */
   "bdabal2badbrb",          /*9-11   9 */
   "",                       /*9-12     */
   "alafalful",              /*9-13   6 one */
   "l2ulaudaf2uafad",        /*9-14   9 */
   "rbraudabalabad",},       /*9-15   9    sub 124 */
   {
   "2uldafdr2dal",           /*10-0   8 */
   "aflf2dr2dal",            /*10-1   7 */
   "ldafdr2dal",             /*10-2   7 */
   "auldafdr2dal",           /*10-3   8 */
   "alau2ldfdr2dal",         /*10-4   9 */
   "afdrau2dbalabd",         /*10-5   9 */
   "arfarafadfu2fd",         /*10-6   9 */
   "brdf2uafadarab",         /*10-7   9 */
   "2ruad2fd",               /*10-8   5 */
   "2f2baud2r2lad",          /*10-9   7 */
   "",                       /*10-10    */
   "urau2rdau2bdal2d",       /*10-11 10 */
   "rdbadrd2uabd2l2d",       /*10-12 11 */
   "2rauad2fdu2r",           /*10-13  7 one */
   "ab2dfdardb",             /*10-14  7 */
   "",},                     /*10-15     sub 113*/
   {
   "dafuf2uadfauaf",         /*11-0   9 */
   "2b2df2d2b",              /*11-1   5 */
   "bau2fuab2ufu2f",         /*11-2   9 */
   "",                       /*11-3     */
   "2uafauarau2fufrf",       /*11-4  10 */
   "2ufulafalaf",            /*11-5   7 */
   "2uadafdrafufar",         /*11-6   9 */
   "r2df2uaf2dar",           /*11-7   7 */
   "afalr2ularaf",           /*11-8   7 */
   "2ufaraf2dal2dr",         /*11-9   8 */
   "fl2fdradal2uaf",         /*11-10  9 */
   "",                       /*11-11    */
   "2uaf2r2dab2daruar",      /*11-12  9 */
   "f2ur2uar2uaf",           /*11-13  7 one */
   "flar2frafal2uaf",        /*11-14  9 */
   "b2d2r2uf2d2l",},         /*11-15  7   sub 112 */
   {
   "u2f2uafauad2ld",         /*12-0   8 */
   "2f2uafauad2ld",          /*12-1   7 */
   "au2f2uafauad2ld",        /*12-2   8 */
   "2f2ufauad2ld",           /*12-3   7 */
   "2fualaulfad2ld",         /*12-4   9 */
   "afar2lafralualf",        /*12-5   9 */
   "ur2df2l2db2ur",          /*12-6   9 */
   "r2df2l2db2ur",           /*12-7   8 */
   "2fuad2ld",               /*12-8   5 */
   "",                       /*12-9     */
   "frfadualafald",          /*12-10  9 */
   "aur2df2l2db2ur",         /*12-11  9 */
   "",                       /*12-12    */
   "2l2u2l2u2l",             /*12-13  5 one */
   "2ur2df2l2db2ur",         /*12-14  9 */
   "2l2d2u2r2d2u",},         /*12-15  6  sub 108 */
   {
   "uflfalauaf",             /*13-0   7 */
   "flfalauaf",              /*13-1   6 usually one*/
   "auflfalauaf",            /*13-2   7 */
   "2uflfalauaf",            /*13-3   7 */
   "",                       /*13-4     */
   "ldarad2falafdrad",       /*13-5  10 */
   "2ur2dfuaf2dar",          /*13-6   9 */
   "flafalafalul",           /*13-7   8 */
   "2fulfd2fadalaf",         /*13-8   9 */
   "bau2dafaduladab",        /*13-9   9 */
   "fdaufrafadaf",           /*13-10  8 */
   "r2dfuaf2dar",            /*13-11  7 */
   "2fadlaudafu2f",          /*13-12  8 */
   "",                       /*13-13    */
   "ruarafarafrf",           /*13-14  8 */
   "fl2rfalrauraf",},        /*13-15  9  sub 112*/
   {
   "",                       /*14-0     */
   "audafuf2uadfauaf",       /*14-1  10 */
   "u2f2uaf2u2f",            /*14-2   6 */
   "fuafd2uafaufad",         /*14-3   9 */
   "uafauarau2fufrf",        /*14-4  10 */
   "ufulafalaf",             /*14-5   7 */
   "2ufuarlfalr2uaf",        /*14-7  10 */
   "uadafdrafufar",          /*14-7   9 */
   "auafalr2ularaf",         /*14-8   8 */
   "f2lafalualaf",           /*14-9   7 */
   "2radfdauaru2r",          /*14-10  8 */
   "aur2df2uaf2dar",         /*14-11  8 */
   "uaf2r2dab2daruar",       /*14-12  9 */
   "alulufauaf",             /*14-13  7 one*/
   "",                       /*14-14    */
   "u2f2ufud2rad",},         /*14-15  8  sub 116 */
   {
   "uf2l2db2dlaul",          /*15-0   9 */
   "f2l2db2dlaul",           /*15-1   8 */
   "auf2l2db2dlaul",         /*15-2   9 */
   "2uf2l2db2dlaul",         /*15-3   9 */
   "ldrfarauaruadalaf",      /*15-4  11 */
   "2fdaruadfau2f",          /*15-5   8 */
   "2rauraduafd2r",          /*15-6   8 */
   "au2rauraduafd2r",        /*15-7   9 */
   "baub2dl2d2br",           /*15-8   8 */
   "aladabdalad2ubad2r2d",   /*15-9  11 */
   "",                       /*15-10    */
   "arurab2daf2db",          /*15-11  8 */
   "2ldrfdauarab2du2l",      /*15-12 11 */
   "2fdarafufad2f",          /*15-13  8 one*/
   "auarurab2daf2db",        /*15-14  9 */
   "",}                      /*15-15 sub126  total 1741/210=8.3 average*/
   };

/****** End of quick middle.  Beginning of third and last layer ***********/

char *fast[]={			/* in pairs 2nd reverse */
   "r2urdal2blad2r",                  /*  9  *5b3-320-0  0 */
   "2rdal2bladar2uar",                /*  9  *043-021-0  1 */
   "abaububalabl",                    /*  8  *0a3-1e7-5  2 */
   "alblabauabub",                    /*  8  *783-1b0-c  3 */
   "fubrabaubarabaf",                 /* 10  *283-e26-3  4 */
   "fbrabubarabauaf",                 /* 10  *813-b20-a  5 */
   "lualaualblab",                    /*  8  *283-2b6-a  6 */
   "balablulaual",                    /*  8  *813-b17-c  7 */
   "arauafuafl2frafalf2u",            /* 12  *2b3-3e1-5  8 */
   "lufauaf2labarblabrb",             /* 13  *0a6-063-6  9 */
   "2radr2uardr2urab2ubuabub",        /* 16  *016-320-0 10 */
   "abaubauab2ubar2uaradr2uard2r",    /* 16  *289-021-0 11 */
   "2ufb2uafauf2uabuaf",              /* 11  *203-203-0 12 */
   "faub2uafuf2uabaf2u",              /* 11  *013-013-0 13 */
   "al2u2lualaual2ublab",             /* 11  *283-17b-c 14 */
   "rabarbubauabal2ulualul",          /* 15  *2b6-12b-9 15 */
   "arur2uaraluraulu",                /* 11  *2b6-320-0 16 */
   "abub2uabafubaufau",               /* 11  *019-021-0 17 */
   "alurauluarafubaufuab",            /* 15  *5b9-301-0 18 */
   "al2ulualu2lauarualauru",          /* 14  *8a9-130-0 19 */
   "bauafuabaufraualuaraul",          /* 15  *b46-301-0 20 */
   "auarulauru2laulaual2ul",          /* 14  *786-102-0 21 */
   "al2bal2fl2bal2f2l2u",             /* 10  *843-301-0 22 */
   "2u2l2fl2bal2fl2bl",               /* 10  *503-301-0 23 fast*/
   "ur2urdar2urad2rbauafuabauf",        /* 17  *7b6-123-0 24 */
   "afubaufuab2rdar2uradar2uarau",      /* 17  *ba9-123-0 25 */
   "alurauluar2fdaf2ufadaf2uafu",       /* 17  *5b6-301-0 26 */
   "auf2ufdaf2ufad2fraualuaraul",       /* 17  *849-301-0 27 */
   "alaulaual2ulbuabauabrbar",        /* 15  *019-12b-9 28 */
   "abaububalablabarfaraf2rb2u",      /* 16  *846-b67-f 29 */
   "2uabarfaraf2rblualaualblab",      /* 16  *2b6-026-a 30 */
   "araflafal2frbuabauabrbar",        /* 15  *019-731-5 31 */
   "auraualuarulualul",               /* 12  *789-312-0 32 */
   "alaulaualaurauluaru",             /* 12  *ba6-230-0 33 */
   "abaubauabaufaubuafau",            /* 12  *589-021-0 34 */
   "ufauabuafubuabub",                /* 12  *846-320-0 35 */
   "abrbar2uar2urb2uab2uabrbar2u",      /* 17  *7b3-76e-f 36 */
   "2urabarb2ub2uabar2ur2urabarb",      /* 17  *8a3-76e-f 37 */
   "al2ul2ulafalfal2ul2ulafalf",      /* 16  *ba6-703-6 38 */
   "aflfal2ual2ulaflfal2ual2ul",      /* 16  *789-e63-3 39 */
   "ab2ub2ubalablab2ub2ubalabl2u",      /* 17  *589-107-c 40 */
   "2ualblab2uab2ubalblab2uab2ub",      /* 17  *846-1be-9 41 */
   "lauarualaurabauarurbu",           /* 14  *043-2e3-3 42 */
   "b2uab2uabrbarbalabl2ul2ual2u",      /* 17  *5b3-703-6 43 */
   "uarfraf2uafaubaufuab",            /* 13  *5b6-e21-9 44 */
   "bauafuabuf2ufarafrau",            /* 13  *849-32e-3 45 */
   "auaflfal2ualaurauluar",           /* 13  *ba9-7b1-c 46 fast*/
   "raualuarul2ulafalfu",             /* 13  *7b6-367-6 47 */
   "afaualulflafalf2uf2uaf",          /* 14  *503-62b-a 48 */
   "f2uaf2uaflfalafalauluf",          /* 14  *843-62b-a 49 */
   "arauafufrfarafr2ur2uar2u",        /* 15  *ba3-3e1-5 50 */
   "2ur2uar2uarfrafarafaufur",        /* 15  *703-3e1-5 51 */
   "alauabub2l2far2falfraf",          /* 13  *583-370-6 52 */
   "rabarbubauabal2brbarbl",          /* 15  *7b9-1b0-c 53 */
   "rubauabarabrbar2uar2ur",          /* 14  *ba3-173-5 54 */
   "ar2ur2urabarbrbuabauar",          /* 14  *703-173-5 55 */
   "furauarafarfraf2uaf2uf2u",        /* 15  *503-b06-a 56 */
   "2uaf2uf2ufarafrfruarauaf",        /* 15  *843-b06-a 57 */
   "bulaual2b2rf2rbarafr",            /* 13  *b43-e1b-9 58 */
   "arfrab2raf2r2blualauab",          /* 13  *583-20b-a 59 */
   "lufauafalar2fl2frafalf2u",        /* 15  *2b3-362-3 60 */
   "2uaflfar2falfaufur",              /* 12  *b13-3e1-5 61 */
   "abauarur2b2uab2uabrbar2u",        /* 14  *043-63e-3 62 */
   "2urabarb2ub2u2baraurub",          /* 14  *5b3-602-a 63 */
   "2urubauabaraf2uf2ufarafr",        /* 15  *ba9-173-5 64 */
   "arfraf2uaf2ufrbuabauar2u",        /* 15  *7b6-173-5 65 */
   "bulaualabar2ur2urabarb",          /* 14  *5b6-b06-a 66 */
   "abrbar2uar2urblualauab",          /* 14  *849-b06-a 67 */
   "2uafaualulfr2uar2uarfraf",        /* 15  *5b6-62b-a 68 */
   "farafr2ur2uarafalauluf2u",        /* 15  *849-62b-a 69 fast*/
   "alauabublf2uaf2uaflfal",          /* 14  *ba9-3e1-5 70 */
   "lafalf2uf2uafalabaubul",          /* 14  *7b6-3e1-5 71 */
   "fuafauaflf2laulaual2ul",          /* 14  *0a6-71e-5 72 */
   "abrbar2uar2urfruarauaf",          /* 14  *0a3-0b2-a 73 */
   "2uab2rfrafrbalblabauabub",        /* 16  *589-7b6-f 74 */
   "lafalf2uf2uafarafaufur",          /* 14  *283-e37-5 75 */
   "furauaraflafalf2uf2uaf",          /* 14  *7b9-607-6 76 */
   "f2uaf2uaflfalfruarauaf",          /* 14  *0a6-6b0-a 77 */
   "2ulufauafalbalabl2ul2ual",        /* 15  *049-2e3-3 78 */
   "l2ual2ualblablfuafaual2u",        /* 15  *586-e13-5 79 */
   "bulaualablafalf2uf2uaf",          /* 14  *503-736-6 80 */
   "f2uaf2uaflfalblualauab",          /* 14  *843-b62-a 81 */
   "lufauafalfarafr2ur2uar2u",        /* 15  *ba3-362-3 82 */
   "2ur2uar2uarfraflfuafaual",        /* 15  *703-731-5 83 */
   "rabarbubaulualaulualauab2u",        /* 17  *7b3-0b7-c 84 */
   "lafalf2uf2uaflfuafaual",          /* 14  *283-1e7-5 85 */
   "furauarafl2ual2ualblab2u",        /* 15  *5b6-b2e-9 86 */
   "2ubalabl2ul2ualfruarauaf",        /* 15  *849-026-a 87 */
   "rubauabarf2uaf2uaflfal",          /* 14  *ba9-071-c 88 */
   "lafalf2uf2uafrbuabauar",          /* 14  *7b6-e73-5 89 */
   "bulau2labl2ul2ual",               /* 11  *b46-b06-a 90 */
   "l2ual2ualb2lualauab",             /* 11  *5b9-b06-a 91 */
   "rubau2barb2ub2uab2u",             /* 12  *786-173-5 92 fast*/
   "2ub2uab2uabr2buabauar",           /* 12  *8a9-173-5 93 */
   "2ubulaualulaualuabauabrbar",        /* 17  *8a3-0b7-c 94 */
   "l2ual2ualblabfruarauaf",          /* 14  *283-0b2-a 95 */
   "rbuabauarafaufruarafufau",        /* 16  *0a3-21e-9 96 */
   "abaububalab2lfarfr2fal2u",        /* 15  *ba9-7eb-f 97 */
   "2b2lbrab2lbarbr2urdar2urad2r2u",    /* 19  *016-301-0 98 */
   "2ul2farafraf2lblabauabub",        /* 15  *7b6-e6b-f 99 */
   };
char *mirr[]={
   "af2uafadb2labd2f",               /*  9  *b13-013-0  0 */
   "2fadb2labdf2uf",                 /*  9  *2b3-203-0  1 */
   "abauarubalabrbl",                /* 10  *283-b2e-9  2 */
   "alabarblabr2blualauab",          /* 13  *ba9-2b6-a  3 */
   "araururabarbaraflafal2fr",       /* 15  *789-7b6-f  4 */
   "aralaflaualflur",                /* 10  *783-e13-5  5 */
   "arual2urauar2ulr2u",             /* 11  *203-320-0  6 */
   "2uaral2uruar2ulaur",             /* 11  *013-021-0  7 */
   "b2u2baubub2ualabl",              /* 11  *0a3-17b-c  8 */
   "bulaual2barbalabrbl",            /* 13  *7b6-b17-c  9 */
   "aufauabuafau2blualauab",         /* 13  *016-06b-a 10 */
   "afaualulaf2laf2rf2laf2raf",      /* 14  *013-736-6 11 */
   "fauaf2ufbauafuabau",             /* 11  *019-013-0 12 */
   "ubaufuabaf2ufuaf",               /* 11  *2b6-203-0 13 */
   "2urad2rad2ld2rad2l2darabaraurub",  /* 18  *ba6-20b-a 14 */
   "2uafd2fd2bad2fd2b2dflfuafaual",    /* 18  *589-e71-5 15 */
   "raualuaraulafalbalab2lfu",       /* 15  *b13-1be-9 16 */
   "aualurauluarfurauaraf",          /* 14  *2b3-e26-3 17 */
   "arauafufrabrbar2uar2ur",         /* 14  *816-2b1-9 18 */
   "ar2ur2urabarbarafaufur",         /* 14  *2b9-317-5 19 mirr*/
   "bulaualabaflfalaualul",          /* 14  *816-673-6 20 */
   "alaululafalfblualauab",          /* 14  *2b9-673-6 21 */
   "ar2flfalfrabrbarauarur",         /* 15  *ba6-b67-f 22 */
   "alaululafal2fruarauaf",          /* 13  *783-760-6 23 */
   "2ulufauafalarfrafauafuf",        /* 14  *286-b01-c 24 */
   "afaufufarafrlfuafaual2u",        /* 14  *b19-3e6-3 25 */
   "rubauabarfarafrurauar",          /* 14  *843-3e6-3 26 */
   "ruarauarfrafrbuabauar",          /* 14  *503-b01-c 27 */
   "2ubulaualabrabarbubauab",        /* 15  *703-673-6 28 */
   "buabauabrbarblualauab2u",        /* 15  *ba3-673-6 29 */
   "f2uaf2uaflfalfurauaraf",         /* 14  *843-eb2-9 30 */
   "2uaraururabarbarafaufur",        /* 15  *503-12b-9 31 */
   "afaualulfarfrafauafuf",          /* 14  *703-3e6-3 32 */
   "afaufufarafrafalauluf",          /* 14  *ba3-b01-c 33 */
   "f2rf2laf2rf2lfalauluf",          /* 14  *203-b62-a 34 */
   "rabr2balbl2b2r",                 /*  9  *203-107-c 35 */
   "2ualabaubul2radr2uardr2ur",      /* 16  *013-37e-5 36 */
   "abauarurabadb2uabdb2ub",         /* 14  *203-b2e-9 37 */
   "rbuabauar2ldal2uladal2ual",      /* 15  *013-13e-5 38 */
   "aflaf2lbalab2l2f",               /*  9  *013-7b1-c 39 */
   "blualaubadb2uabdb2ub",           /* 14  *203-2b6-a 40 */
   "afubaufuabr2balablabarau",       /* 15  *786-7b1-c 41 */
   "urbalbl2barbauafuabauf",         /* 15  *8a9-367-6 42 mirr*/
   "alurauluarbulaualabu",           /* 14  *b46-376-6 43 */
   "aublaflf2lablauarualaur",        /* 15  *5b9-7b1-c 44 */
   "afubaufuabf2rabarbarafau",       /* 15  *789-1be-9 45 */
   "ufrabrb2rafbauafuabauf",         /* 15  *ba6-107-c 46 */
   "uafubaufuabf2rabarbaraf",        /* 15  *846-107-c 47 */
   "frabrb2rafbauafuabaufau",        /* 15  *589-1be-9 48 */
   "arulauru2farafrafalu",           /* 13  *b43-e63-3 49 */
   "2uafalablfalb2lfuafaual",        /* 14  *5b6-e71-5 50 */
   "fauabuafau2rfrafrbau",           /* 13  *583-32e-3 51 */
   "lufauaf2lablafalblf2u",          /* 14  *849-370-6 52 */
   "arulaurualf2rabarbarafu",        /* 15  *ba6-e21-9 53 */
   "aufrabrb2raflauarualaur",        /* 15  *789-32e-3 54 */
   "afubaufuabl2farafrafalau",       /* 15  *589-32e-3 55 */
   "ulfarfr2falbauafuabauf",         /* 15  *846-e21-9 56 */
   "arulaurualaraflafal2fru",        /* 15  *b46-e63-3 57 */
   "auar2flfalfrlauarualaur",        /* 15  *5b9-703-6 58 */
   "afubaufuabafalbalab2lfau",       /* 15  *786-703-6 59 */
   "uaf2lblablfbauafuabauf",         /* 15  *8a9-e63-3 60 */
   "arulaurualafalbalab2lfu",        /* 15  *846-367-6 61 */
   "auaf2lblablflauarualaur",        /* 15  *589-7b1-c 62 */
   "afubaufuabalabrabar2blau",       /* 15  *789-7b1-c 63 */
   "ual2brbarblbauafuabauf",         /* 15  *ba6-367-6 64 */
   "2ub2uab2uabrbarlufauafal",       /* 15  *8a9-b67-f 65 mirr*/
   "lfuafaualrabarb2ub2uab2u",       /* 15  *786-7b6-f 66 */
   "l2ual2ualblabfurauaraf",         /* 14  *5b9-7b6-f 67 */
   "fruarauafbalabl2ul2ual",         /* 14  *b46-b67-f 68 */
   "alabarblabrbfurauaraf",          /* 14  *ba9-e26-3 69 */
   "2ulfarfr2f2l2ul2ulafalf",        /* 15  *7b6-e21-9 70 */
   "alblab2uab2u2b2lafalfalab",      /* 14  *5b6-367-6 71 */
   "blaflf2l2b2ub2ubalabl",          /* 14  *849-7b1-c 72 */
   "abauarur2blfalablafal",          /* 13  *2b9-b2e-9 73 */
   "2uaf2uafadf2uafd2frbuabauar",    /* 16  *816-317-5 74 */
   "alauabub2lfrafalfaraf2u",        /* 14  *b19-071-c 75 */
   "blaualul2ualauabu",              /* 11  *286-2b1-9 76 */
   "afubaufuabaf2lblablfau",         /* 15  *5b3-e63-3 77 */
   "ubauafuabauflufauafal",          /* 14  *043-063-6 78 */
   "raualuaraulr2balablabaru",       /* 15  *b13-32e-3 79 */
   "aurbalbl2baralurauluar",         /* 15  *2b3-e21-9 80 */
   "afubaufu2b2ubuabubau",           /* 14  *5b3-130-0 81 */
   "uabaubauab2u2bauafuabauf",       /* 14  *043-102-0 82 */
   "raualuarau2l2ualaulaualu",       /* 14  *b13-312-0 83 */
   "aulualul2u2lurauluar",           /* 14  *2b3-231-0 84 */
   "2fdaf2ufadaf2u2f2ufuafuf",       /* 15  *5b3-231-0 85 */
   "afaufauaf2u2f2ufdaf2ufad2f",     /* 15  *043-312-0 86 */
   "2radr2uardr2u2r2uaraurauar",     /* 15  *b13-102-0 87 */
   "ruarur2u2r2uaradr2uard2r",       /* 15  *2b3-130-0 88 mirr*/
   "2fdaf2ufadaf2uafar2uruarur",     /* 16  *5b6-203-0 89 */
   "araurauar2urf2ufdaf2ufad2f",     /* 16  *849-013-0 90 */
   "2rdar2uradar2uarab2ubuabub2u",     /* 17  *b19-013-0 91 */
   "2uabaubauab2ubr2urdar2urad2r",     /* 17  *7b6-203-0 92 */
   "2fdaf2ufadaf2uafab2ubuabub",     /* 16  *503-320-0 93 */
   "abaubauab2ubf2ufdaf2ufad2f",     /* 16  *843-021-0 94 */
   "2rdar2uradar2uaral2ulualul2u",     /* 17  *ba3-021-0 95 */
   "2ualaulaual2ulr2urdar2urad2r",     /* 17  *703-320-0 96 */
   "2fdaf2ufadaf2uafal2ulualul",     /* 16  *7b3-130-0 97 */
   "alaulaual2ulf2ufdaf2ufad2f",     /* 16  *8a3-102-0 98 */
   };
char *misc[]={
   "aflfal2ual2ul",                   /*  8  *816-703-6  0 */
   "ar2fl2frafalf",                   /*  8  *2b9-e63-3  1 */
   "2uab2ub2ubalabl",                 /*  9  *b19-1be-9  2 */
   "balabl2ul2ual2u",                 /*  9  *049-7b1-c  3 */
   "b2uab2uabrbar",                   /*  8  *0a6-32e-3  4 */
   "rabarb2ub2uab",                   /*  8  *7b9-e21-9  5 */
   "2ul2ual2ualblab",                 /*  9  *586-367-6  6 */
   "2uarfraf2uaf2uf",                 /*  9  *286-107-c  7 */
   "aluraulauar2uarfrafu",            /* 13  *7b6-e63-3  8 */
   "aufarafr2urualuaraul",            /* 13  *ba9-703-6  9 */
   "afubaufauab2uabrbarau",           /* 13  *849-1be-9 10 */
   "urabarb2ubuafuabauf",             /* 13  *5b6-107-c 11 */
   "alblab2uab2ubar2ur2urabarb2u",      /* 17  *813-367-6 12 */
   "2uabrbar2uar2urab2ub2ubalabl",      /* 17  *283-7b1-c 13 */
   "farafr2ur2uarabrbar2uar2ur",      /* 16  *503-eb7-f 14 */
   "ar2ur2urabarbr2uar2uarfraf",      /* 16  *843-eb7-f 15 */
   "2ulafalf2uf2uafarfraf2uaf2uf",      /* 17  *ba3-eb7-f 16 */
   "af2uf2ufarafrf2uaf2uaflfal2u",      /* 17  *703-eb7-f 17 */
   "ab2ub2ubalablfarafr2ur2uar2u",      /* 17  *583-76e-f 18 */
   "2ur2uar2uarfrafalblab2uab2ub",      /* 17  *b43-76e-f 19 */
   "rabarb2ub2uabrabarb2ub2uab",      /* 16  *846-32e-3 20 */
   "b2uab2uabrbarb2uab2uabrbar",      /* 16  *589-e21-9 21 */
   "farafr2ur2uarfarafr2ur2uar2u",      /* 17  *789-367-6 22 misc*/
   "2ur2uar2uarfrafr2uar2uarfraf",      /* 17  *ba6-7b1-c 23 */
   "afaualulfarfraf2uaf2uf2u",        /* 15  *286-e26-3 24 */
   "blfalablafalrubauabar",           /* 14  *783-1e2-9 25 */
   "r2uar2uarfrafarfraf2uaf2uf",      /* 16  *8a9-301-0 26 */
   "2ul2ualaulaualaflfar2fal2fr",     /* 16  *8a9-b01-c 27 */
   "f2uaf2uaflfalaflfal2ual2ul2u",      /* 17  *5b9-123-0 28 */
   "2ual2ul2ulafalflafalf2uf2uaf",      /* 17  *b46-123-0 29 */
   "rubauab2uar2uarfraf",             /* 12  *583-0b2-a 30 */
   "farafr2ur2ubuabauar",             /* 12  *b43-e37-5 31 */
   "furauarafb2uab2uabrbar2u",        /* 15  *043-b06-a 32 */
   "2urabarb2ub2uabfruarauaf",        /* 15  *5b3-b06-a 33 */
   "rubauabaralblab2uab2ub",          /* 14  *5b9-071-c 34 */
   "ab2ub2ubal2baubul",               /* 11  *b46-3e1-5 35 */
   "bulaualabaflfal2ual2ul2u",        /* 15  *8a9-736-6 36 */
   "ar2ur2urab2raurub2u",             /* 12  *786-62b-a 37 */
   "2ulufauaf2uldar2frad2l",          /* 14  *283-b12-9 38 */
   "2ldar2fradal2ufuafaual2u",        /* 14  *813-236-3 39 */
   "2uaf2uf2ufarafrafalauluf",        /* 15  *b19-b20-a 40 */
   "2u2fadb2labdf2ualauluf",          /* 14  *783-6e2-3 41 */
   "lauarualaurbulaualabu",           /* 14  *8a9-1e2-9 42 */
   "2ual2ul2ulafal2furauaraf",        /* 14  *786-eb2-9 43 */
   "bauafuabaufrubauabarau",          /* 14  *5b9-63e-3 44 */
   "ab2ub2ubalab2lufauafal",          /* 13  *b46-0b7-c 45 misc*/
   "ab2rf2rbarafrfuafuf2uaf",         /* 15  *b46-12b-9 46 */
   "",                   /* duplicate 14  *2b9-317-5 47 */
   "furauarafalaululafalf2u",         /* 15  *7b6-231-0 48 */
   "2uaflfalaualulfruarauaf",         /* 15  *ba9-312-0 49 */
   "rubauabarafaufufarafr",           /* 14  *849-312-0 50 */
   "arfrafauafufrbuabauar",           /* 14  *5b6-231-0 51 */
   "2urubauabarbuflafaufalafab",        /* 17  *5b3-e32-3 52 */
   "raualuarauabrabar2blu",           /* 14  *043-32e-3 53 */
   "lufauafalbflafufalafauab",        /* 16  *0a6-6be-f 54 */
   "buflafaufalafablfuafaual",        /* 16  *7b9-6e7-f 55 */
   "bulaualabrlbalulabalauar2u",        /* 17  *586-e6b-f 56 */
   "2urulbalaulabalarblualauab",        /* 17  *049-7eb-f 57 */
   "2uaf2uf2ufarafrbulaualab",        /* 15  *843-b7e-f 58 */
   "blualauabarfraf2uaf2uf2u",        /* 15  *503-e76-f 59 */
   "ar2ur2urabarblufauafal",          /* 14  *703-b67-f 60 */
   "lfuafaualabrbar2uar2ur",          /* 14  *ba3-7b6-f 61 */
   "bulaualabfuafauaflfal",           /* 14  *813-6e7-f 62 */
   "lafalfufauafblualauab",           /* 14  *283-6be-f 63 */
   "2urubauabuarauarfraf",            /* 13  *5b3-e76-f 64 */
   "farafruraubuabauar2u",            /* 13  *043-b7e-f 65 */
   "2uafaualulaufufarafr",            /* 13  *b13-7eb-f 66 */
   "arfrafauafualauluf2u",            /* 13  *2b3-e6b-f 67 */
   "alauabublaraururabarb",           /* 14  *783-7b6-f 68 misc*/
   "abrbarauaruralabaubul",           /* 14  *0a3-b67-f 69 */
   "lufauafalfarafrurauar",           /* 14  *0a6-26e-3 70 */
   "ruarauarfraflfuafaual",           /* 14  *7b9-710-c 71 */
   "abauarurbarfrafauafuf",           /* 14  *2b9-26e-3 72 */
   "lfauafuf2uafaualau",              /* 11  *816-b17-c 73 */
   "alauabublabrbarauarur2u",         /* 15  *b19-21e-9 74 */
   "2uaraururabarbalabaubul",         /* 15  *286-21e-9 75 */
   "bulaualablafalfufauaf2u",         /* 15  *586-760-6 76 */
   "2ufuafauaflfalblualauab",         /* 15  *049-760-6 77 */
   "ar2fl2frafalflualul2ual2u",       /* 16  *786-3e6-3 78 */
   "",                   /* 16 duplicate *8a9-b01-c 79 */
   "2uruarur2uarabaububalabl",        /* 16  *019-3e6-3 80 */
   "buabub2uabalaululafalf",          /* 15  *2b6-673-6 81 */
   "afaufufarafrbulaualab",           /* 14  *816-be6-f 82 */
   "blualauabarfrafauafuf",           /* 14  *2b9-be6-f 83 */
   "2uaraururabarblufauafal",         /* 15  *286-67b-f 84 */
   "lfuafaualabrbarauarur2u",         /* 15  *b19-67b-f 85 */
   "bulaualulaual2baububalabl",         /* 17  *816-2e3-3 86 */
   "2ulufauafufauaf2laululafalf",       /* 18  *286-607-6 87 */
   "lfuafaualafubaufuabau",           /* 14  *5b3-7e3-5 88 */
   "2urubauabubauabaralaululafalf",     /* 19  *7b3-2b1-9 89 */
   "lualaualbl2baubauab2ub2u",        /* 15  *586-260-a 90 */
   "2uab2ubuabu2balablulaual",        /* 15  *049-71e-5 91 misc*/
   "",                      /* duplicate 14  *0a6-71e-5 92 */
   "al2ulualu2lafalfufauaf",          /* 14  *7b9-260-a 93 */
   "ual2ul2ulafalfbauafuabauf",       /* 16  *586-e21-9 94 */
   "2uafau2faraf2rauarbalablulaual",    /* 17  *049-317-5 95 */
   "fuafauaflfalbu2brb2rur",          /* 16  *0a6-b17-c 96 */
   "arau2rabar2bauablafalfufauaf",    /* 16  *7b9-2b6-a 97 */
   "fruarauafraualuaraulu",           /* 14  *b13-b20-a 98 */
   "b2lafalfalabarfrab2raf2rb",       /* 15  *5b9-1be-9 99 */
   };
char *sept12[]={
   "2fadf2uafdf2ufl2ualaulaual2u",     /* 17  *5b6-320-0  0 */
   "2ulualul2ualaf2uafadf2uafd2f",     /* 17  *849-021-0  1 */
   "2radr2uardr2urf2uafaufauaf",     /* 16  *ba9-021-0  2 */
   "fuafuf2uafar2uaradr2uard2r",     /* 16  *7b6-320-0  3 */
   "2radr2uardr2url2ualaulaual",     /* 16  *ba3-013-0  4 */
   "lualul2ualar2uaradr2uard2r",     /* 16  *703-203-0  5 */
   "2fadf2uafdf2ufb2uabaubauab2u",     /* 17  *503-203-0  6 */
   "2ubuabub2uabaf2uafadf2uafd2f",     /* 17  *843-013-0  7 */
   "2radr2uardr2urb2uabaubauab",     /* 16  *8a3-312-0  8 */
   "buabub2uabar2uaradr2uard2r",     /* 16  *7b3-231-0  9 */
   "uarulaur2ufauafal",              /* 11  *289-2b1-9 10 */
   "b2urauluar2ualab",               /* 10  *016-b17-c 11 */
   "arfraf2uaf2u2furauaraf",         /* 13  *583-12b-9 12 */
   "rbalbl2baralablafalblf",         /* 15  *b43-107-c 13 */
   "rblabarbalabaf2lblablf",         /* 15  *b43-367-6 14 */
   "afalbalab2lfblabrbalabar",       /* 15  *583-7b1-c 15 */
   "afalablfalblf2rabarbaraf",       /* 15  *283-32e-3 16 */
   "alablafalblfrubauabar",          /* 14  *813-1e2-9 17 */
   "araururabar2blaflf2lab",         /* 14  *5b6-6e7-f 18 */
   "b2lafalfal2brbarauarur",         /* 14  *849-6be-f 19 */
   "arafalfrafl2f2rabarbaraf2u",     /* 15  *ba3-e63-3 20 */
   "2ufrabrb2r2falfaraflfr",         /* 15  *703-703-6 21 */
   "afalablfalb2l2farafrafal",       /* 14  *503-703-6 22 sept12*/
   "lfarfr2f2lablafalblf",           /* 14  *843-e63-3 23 */
   "rblabarbal2b2rfrafrb",           /* 14  *ba3-e21-9 24 */
   "abarfaraf2r2blabrbalabar",       /* 14  *703-32e-3 25 */
   "frbarafrab2r2flfalfr2u",         /* 15  *503-32e-3 26 */
   "2uaraflafal2f2rbarfrabaraf",     /* 15  *843-e21-9 27 */
   "arafalfraflfab2ubuabub2u",       /* 16  *049-013-0 28 */
   "2uabaubauab2ubafalfaraflfr",     /* 16  *586-203-0 29 */
   "afalablfalblar2uruarur",         /* 15  *7b9-203-0 30 */
   "araurauar2uralablafalblf",       /* 15  *0a6-013-0 31 */
   "fruarauafarfraf2uaf2uf2u",       /* 15  *286-130-0 32 */
   "2uaf2uf2ufarafrfurauaraf",       /* 15  *b19-1b0-0 33 */
   "rbuabauarabrbar2uar2ur",         /* 14  *816-102-0 34 */
   "ar2ur2urabarbrubauabar",         /* 14  *2b9-130-0 35 */
   "alaulaual2u2lfrafalfaraf",       /* 14  *849-102-0 36 */
   "fraflfaraf2l2ulualul",           /* 14  *5b6-130-0 37 */
   "afalablfalb2ulualul",            /* 13  *2b6-130-0 38 */
   "alaulaual2uablafalblf",          /* 13  *019-102-0 39 */
   "rblabarbal2uabaubauab",          /* 13  *019-312-0 40 */
   "buabub2ulabrbalabar",            /* 13  *2b6-231-0 41 */
   "arafalfraflfabarfaraf2rb2u",     /* 16  *ba6-e63-3 42 */
   "2uab2rfrafrbafalfaraflfr",       /* 16  *789-703-6 43 */
   "afalablfalblaraflafal2fr",       /* 15  *589-703-6 44 */
   "ar2flfalfralablafalblf",         /* 15  *846-e63-3 45 sept12*/
   "afalablfalrabar2bl",             /* 11  *283-1be-9 46 */
   "bulaualabraualuaraulu",          /* 14  *b13-b17-c 47 */
   "lufaualblafalab",                /* 10  *0a3-063-6 48 */
   "alblabauabubafaufauaf2uf2u",     /* 16  *2b6-b70-c 49 */
   "afaualulfruarauarfraf",          /* 14  *7b6-130-0 50 */
   "farafrurauarafalauluf",          /* 14  *ba9-102-0 51 */
   "balablulau2labrabar2bl2u",       /* 15  *ba3-e1b-9 52 */
   "2ual2brbarb2lualaualblab",       /* 15  *703-20b-a 53 */
   "rabarbubau2barfaraf2rb",         /* 14  *503-2e3-3 54 */
   "lfuafau2labarblabrb",            /* 13  *843-7e3-5 55 */
   "aflfalaualu2lfarfr2fal",         /* 14  *ba3-e26-3 56 */
   "l2farafraf2laululafalf",         /* 14  *703-b20-a 57 */
   "abarbalabrb2lufauafal",          /* 13  *503-063-6 58 */
   "f2rabarbar2faufufarafr2u",       /* 15  *843-e71-5 59 */
   "rabarbubauabalabrabar2bl",       /* 15  *283-173-5 60 */
   "lufauafalab2uabadb2uabd2b",      /* 15  *019-063-6 61 */
   "2ualaulaual2ulabaububalabl",     /* 16  *843-367-6 62 */
   "bulaufalablafal",                /* 10  *703-b17-c 63 */
   "rub2uabaubuabarau",              /* 11  *b19-317-5 64 */
   "fur2uarauruarafu",               /* 11  *2b9-2b6-a 65 */
   "ar2fl2frafalfalaulaual2ul2u",    /* 16  *203-62e-3 66 */
   "ab2uabadb2uabdbaraurub",         /* 14  *013-026-a 67 */
   "balablulaualabarfaraf2rb2u",     /* 16  *ba6-e37-5 68 sept12*/
   "2uab2rfrafrblualaualblab",       /* 16  *789-0b2-a 69 */
   "rabarbubauabaraflafal2fr",       /* 15  *589-0b2-a 70 */
   "ar2flfalfrbuabauabrbar",         /* 15  *846-e37-5 71 */
   "balablulaualf2rabarbaraf2u",     /* 16  *ba3-e37-5 72 */
   "2ufruarau2falablfalbl",          /* 14  *703-602-a 73 */
   "rabarbubauabl2farafrafal",       /* 15  *503-0b2-a 74 */
   "lfarfr2falbuabauabrbar",         /* 15  *843-e37-5 75 */
   "2r2balabl2barbar",               /*  9  *013-1be-9 76 */
   "af2ufuafufarfrab2raf2rb",        /* 15  *203-13e-5 77 */
   "rabarbubauabr2balablabar",       /* 15  *583-173-5 78 */
   "2ufuafuf2u2flfar2fal2fr",        /* 15  *013-e32-3 79 */
   "aflfalaualulaf2lblablf",         /* 15  *b43-62b-a 80 */
   "2f2lblab2lfalf",                 /*  9  *203-367-6 81 */
   "rabarbubau2lafalfalab",          /* 13  *589-071-c 82 */
   "blaflf2luabauabrbar",            /* 13  *846-37e-5 83 */
   "2ufarafrurau2balablabar",        /* 14  *ba6-736-6 84 */
   "rbalbl2buarauarfraf2u",          /* 14  *789-b62-a 85 */
   "abaububalablf2rabarbaraf2u",     /* 16  *843-b67-f 86 */
   "2ufrabrb2rafalblabauabub",       /* 16  *503-7b6-f 87 */
   "araururabarbl2farafrafal",       /* 15  *703-7b6-f 88 */
   "lfarfr2falabrbarauarur",         /* 15  *ba3-b67-f 89 */
   "balablulau2l2ulualul2u",         /* 15  *043-01b-c 90 */
   "ab2rfrafrufaubuafau",            /* 13  *5b3-e21-9 91 sept12*/
   "alblabauabu2b2uabaubauab2u",     /* 15  *2b3-b70-c 92 */
   "2ubuabub2u2baububalabl",         /* 15  *b13-076-6 93 */
   "afaualulfbalablulaual2u",        /* 15  *049-b01-c 94 */
   "bulaual2barafrbarfr2u",          /* 14  *586-70b-c 95 */
   "balablulaualaf2ufuafuf2u",       /* 16  *019-b01-c 96 */
   "2uafaufauaf2uflualaualblab",     /* 16  *2b6-3e6-3 97 */
   "",                   /* 15  duplicate   043-01b-c 98 */
   "2ufuafuf2uafabaububalabl",       /* 16  *ba9-1be-9 99 */
   };

char *sept19[]={
   "2uarafrabarfr2blualauab",        /* 14  *049-06b-a  0 */
   "lfalblafal2baraurub",            /* 13  *816-026-a  1 */
   "2ufraflfaraf2labaubul",          /* 14  *286-37e-5  2 */
   "ru2rfr2fufarfrab2raf2rb",        /* 16  *2b9-1e7-5  3 */
   "aufauabuaf2uarurb",              /* 11  *016-607-6  4 */
   "uraualuar2uabubl",               /* 11  *289-1b0-c  5 */
   "ab2rab2lb2rab2lbaublfruarafal",    /* 18  *703-b2e-9  6 */
   "lfalblafualauab",                /* 10  *ba3-2b6-a  7 */
   "2ulualaualblabafalauluf",        /* 15  *586-3e6-3  8 */
   "rabarb2ub2uabr2urdar2urad2r",      /* 17  *049-e21-9  9 */
   "b2uab2uabrbrdar2uradar2uar",     /* 16  *5b6-32e-3 10 */
   "blualubdab2ubad2b",              /* 12  *8a3-2b6-a 11 */
   "2bdab2ubadabaulaualab",          /* 12  *7b3-b17-c 12 */
   "lfuaf2uarualaurau",              /* 11  *016-037-5 13 */
   "bl2uraualuar2uab",               /* 10  *289-2b6-a 14 */
   "alauabu2blab2uab2ub",            /* 11  *5b9-3e1-5 15 */
   "2uabauaru2rbar2uar2ur",          /* 12  *8a9-62b-a 16 */
   "uraualuarau2lufauafal",          /* 13  *7b3-063-6 17 */
   "fruarauafabarbalabrbl",          /* 14  *7b6-b20-a 18 */
   "abarbalabrb2lfuafaual",          /* 13  *7b9-7e3-5 19 */
   "rbuabauarafalablfalbl",          /* 14  *283-13e-5 20 */
   "lfuafau2lblab2uab2ub",           /* 13  *5b9-0b7-c 21 */
   "fruarau2flfal2ual2ul2u",         /* 14  *8a9-037-6 22 sept19*/
   "uraualuarau2lfuafaual",          /* 13  *289-7e3-5 23 */
   "ubauafuabaufrbuabauar",          /* 14  *043-13e-5 24 */
   "rubauabaralurauluarau",          /* 14  *8a3-1e2-9 25 */
   "uraualuaraulrbuabauar",          /* 14  *7b3-13e-5 26 */
   "aualurauluarblualauab",          /* 14  *2b3-2b6-a 27 */
   "ar2uaradr2uard2rblualauab",      /* 15  *286-2b6-a 28 */
   "2u2radr2uardr2urblualauab",      /* 16  *019-06b-a 29 */
   "2badb2uabdb2ublfuafaual",        /* 15  *2b6-7e3-5 30 */
   "abarbalabrblrbuabauar",          /* 14  *503-13e-5 31 */
   "2ul2fl2bal2fl2balfuafaual",      /* 15  *849-e71-5 32 */
   "2ufurauaraf2l2farafr2falfal",    /* 16  *016-e1b-9 33 */
   "rubauabar2f2rabarb2rafraf",      /* 15  *289-2e3-3 34 sept19*/
   };
char *newer[]={
   "aufabauarurbuaf",                 /* 10  *283-2e3-3  0 */
   "fauabaraurubafu",                 /* 10  *813-e13-5  1 */
   "uarlufauafalaur",                 /* 10  *0a3-e26-3  2 */
   "arulfuafaualrau",                 /* 10  *783-b20-a  3 */
   "barauafurabarfr",                 /* 10  *7b3-2e3-3  4 */
   "arafrbaraufurab",                 /* 10  *8a3-e13-5  5 */
   "alfurauaflfaraf",                 /* 10  *8a3-e26-3  6 */
   "frafalfuarauafl",                 /* 10  *7b3-b20-a  7 */
   };
char *diag[]=
   {
   "araurauar2u2rubauabarau",         /* 13  *183-71b-c  0 */
   "ufruarauaf",                      /*  7  *403-713-5  1 */
   "lufauafalu",                      /*  7  *403-706-6  2 */
   "urbuabau2r2uruarur",              /* 13  *183-26b-a  3 */
   "rbuab2rfr2fufau",                 /* 11  *183-213-0 same backwards */
   "ul2farafraf2lauabubl",            /* 13  *403-67e-f  5 */
   "faub2dabuaf2ufaub2dabuafau",      /* 16  *103-012-0  6 */
   "ufaub2dabuaf2ufaub2dabuaf",       /* 16  *103-230-0  7 */
   "rubauab2raflafal2frau",           /* 13  *a83-b26-a  8 */
   "raul2daluaraur2ul2dal2uar",       /* 15  *a03-230-0  9 */
   "auabaraurublu2lbl2bub",           /* 15  *a83-7e6-f 10 */
   "uar2flfalf2rbuabauar",            /* 13  *4b3-b26-a 11 */
   "abau2balab2laualabauarurbu",      /* 14  *4b3-b60-f 12 */
   "uruaruraubauabar",                /* 11  *a89-062-a 13 */
   "aulfuafaufuafau2lauabubl",        /* 16  *489-1eb-9 14 */
   "l2u2lblabl2ualu",                 /* 10  *183-2e6-3 15 */
   "aul2ualbalab2l2ual",              /* 10  *183-b10-c 16 */
   "uaf2ufuafufalauabubl",            /* 14  *a89-1e3-5 17 */
   "abauarurbrbalbl2baru",            /* 14  *183-0b1-c 18 */
   "arauafufru",                      /*  7  *403-62e-3 19 */
   "aur2balablabarabaraurub",         /* 14  *183-3e7-5 20 */
   "blualaulualaulualauabau",         /* 15  *489-230-0 21 */
   "afaualul2frabrb2rafu",            /* 13  *4b3-1e3-5 22 diag*/
   "fraubuabarafu",                   /*  9  *403-b02-a 23 */
   "auf2rabarbar2falauluf",           /* 13  *a83-1e3-5 24 */
   "aurbuabaubuabau2rauafufr",        /* 16  *489-603-6 25 */
   "auarafaufuafaufu2rubauabar",      /* 16  *489-326-3 26 */
   "aualabaubuabaubu2lufauafal",      /* 16  *489-b71-c 27 */
   "ual2ul2ulafalfabrbal2bar2bl",       /* 17  *a03-b76-f 28 */
   "l2ual2ualblablauarualaur",        /* 15  *103-be2-9 29 */
   "abufaubuaflafalf2uf2uaf2u",       /* 16  *103-632-3 30 */
   "arulaurualbalabl2ul2ual",         /* 15  *103-036-6 31 */
   "afubaufuabrabarb2ub2uab2u",       /* 16  *103-07b-c 32 */
   "balablulau2laululafalfau",        /* 16  *103-310-0 33 */
   "aurauaf2dfuar2urauaf2dfuar",      /* 16  *489-012-0 34 */
   "uabarbaf2labrb2larfr",            /* 13  *403-012-0 35 */
   "furauar2f2lblablfu",              /* 13  *183-360-6 36 */
   "uaflfalaualu2lualaualblab",       /* 15  *103-201-0 37 */
   "ulfarfr2falrubauabar",            /* 14  *a89-b6e-f 38 */
   "aur2urdar2urad2rab2ub2ubalabl",     /* 18  *a03-07b-c 39 */
   "2fdab2rbadaf2uafarulaurual",      /* 16  *a89-201-0 40 */
   "aualblabauabu2buabauabrbar",      /* 16  *103-213-0 41 */
   "duplicate",      /* 16  *103-102-0 42 */
   "auarfrafauafu2fuafauaflfal",      /* 16  *103-120-0 43 */
   "auar2uaradr2uard2rf2uaf2uaflfal",   /* 18  *a03-be2-9 44 */
   "fruaraurabrb2rafu",               /* 12  *4b3-e12-9 45 diag*/
   "uaf2ufuafufarafaufur",            /* 14  *a89-b10-c 46 */
   "al2ulbaualauluab",                /* 10  *403-201-0 47 */
   "f2u2fau2fau2f2u2furauarafu",      /* 15  *a83-073-6 48 */
   "arafrabarfrbfauabuafaub",         /* 15  *4b3-230-0 49 */
   "abufaubuafabarafrbarfr",          /* 15  *a83-012-0 50 */
   "auafalbalab2l2fruarauaf",         /* 13  *183-7e1-5 51 */
   "ur2uaraurau2rafaufur",            /* 13  *403-701-c 52 */
   "furauarafab2rfrafrbu",            /* 14  *a89-1b2-9 53 */
   "uar2uruaru2rbuabauar",            /* 13  *403-12e-9 54 */
   "rubauabaralaulaual2ulau",         /* 14  *a83-be2-9 55 */
   "ual2ulualuabaubul",               /* 12  *4b3-067-6 56 */
   "ulualul2ufuafaual",               /* 12  *183-be2-9 57 */
   "lufauafalabaubauab2ubau",         /* 14  *183-603-6 58 diag*/
   "uab2ubuabublfuafaual",            /* 14  *183-6e3-3 59 */
   "lufauaf2ualaulaualau",            /* 12  *183-036-6 60 */
   "uaraurauar2u2rbuabauar",          /* 13  *a89-326-3 61 */
   "uruarur2u2rafaufur",              /* 13  *a89-603-6 62 */
   "furauarafab2ubuabubu",            /* 14  *183-e62-3 63 */
   "auabaubauab2ubfruarauaf",         /* 14  *183-730-6 64 */
   "bulaualabar2uruaruru",            /* 14  *4b3-b10-c 65 */
   "auf2rabarbarurauaraf",            /* 12  *a83-23e-3 66 */
   "alauabubaulaual2ulau",            /* 12  *a83-7b0-c 67 */
   "ruarur2uarafalaulufu",            /* 14  *4b3-2e6-3 68 */
   "fu2flf2labublau",                 /* 11  *183-120-0 69 */
   "araurauarurauar2uafaufuru",       /* 16  *a03-e62-3 70 */
   "uabauarur2ubuabaubuabub",         /* 16  *a03-017-c 71 */
   "ul2ualaulualaulaualafalauluf",      /* 18  *a03-763-6 72 diag*/
   "uafl2fal2ual2ulafarafaufur",      /* 16  *489-062-a 73 */
   "urubauabaraf2u2fu2fu2f2uaf",      /* 16  *489-07b-c 74 */
   "fal2dlaf2ufal2dlurauarafu",       /* 16  *a03-170-c 75 */
   "blualauar2drab2ubar2drabu",       /* 16  *a03-b60-a 76 */
   "afr2darf2uafr2daraualulfu",       /* 16  *a03-2e1-9 77 */
   "abaraurul2dalb2uabl2dalbu",       /* 16  *a03-26b-a 78 */
   };
char *corntwist[]={	    /* twist corners & cycle edges */
   "araurauar2ur2u",                  /* 8   *a53-320-0  0 */
   "2uar2uruarur",                    /* 8   *473-021-0  1 */
   "2uaraflafal2fr",                  /* 8   *a53-e21-9  2 */
   "ar2flfalfr2u",                    /* 8   *473-32e-3  3 */
   "2uf2uafaufauaf",                  /* 8   *a53-203-0  4 */
   "fuafuf2uaf2u",                    /* 8   *473-013-0  5 */
   "2ub2lafalfalab",                  /* 8   *a53-e63-3  6 */
   "blaflf2lab2u",                    /* 8   *473-703-6  7 */
   "2ubu2brb2rur",                    /* 9   *a53-623-3  8 */
   "arau2rabar2bauab2u",              /* 9   *473-623-3  9 */
   "rubauabubauabar",                 /* 10  *a79-13e-5 10 */
   "lfuafaufuafaual",                 /* 10  *a79-063-6 11 */
   "blualauabarauafufr2u",            /* 13  *473-67b-f 12 */
   "2uarafaufurbulaualab",            /* 13  *a53-67b-f 13 */
   "lualau2f2lbalflafab2l2f",         /* 14  *a73-673-6 14 */
   "2f2lbfalaflab2l2fulaual",         /* 14  *453-673-6 15 */
   "2ubal2blbal2bl2urbarab",          /* 14  *473-123-0 16 */
   "brabar2ual2blabal2blab2u",        /* 14  *a53-123-0 17 */
   "rubauabarlfuafaual",              /* 12  *a79-032-0 18 */
   "l2ualaulualaulaual",              /* 11  *459-203-0 19 */
   "buabubauabub2uab",                /* 11  *459-312-0 20 */
   "r2uarauruaraurauar",              /* 11  *459-130-0 21 */
   "fuafufauafuf2uaf",                /* 11  *459-021-0 22 corntwist*/
   "abaubauabubauab2ub",              /* 11  *459-320-0 23 */
   "afaufauafufauaf2uf",              /* 11  *459-231-0 24 */
   "ar2uruarauruarur",                /* 11  *459-013-0 25 */
   "al2ulualaulualul",                /* 11  *459-102-0 26 */
   "bulaufauafualulaufauafualab",       /* 18  *459-123-0 27 */
   "b2u2bau2bauabuabub",              /* 11  *a53-301-0 28 */
   "afaufauaf2ufabarfaraf2rb",        /* 14  *459-21e-9 29 */
   "2uruarauarfr2ufdaf2ufad2f",       /* 16  *459-602-a 30 */
   "abaubauab2ubafalbalab2lf",        /* 14  *459-760-6 31 */
   "aualf2daflaualf2dafl",            /* 12  *a79-301-0 32 */
   "abaubaubu2bu2b2uab",              /* 11  *473-301-0 33 */
   "2rdar2uradar2uabarbubauab2u",     /* 16  *459-2b1-9 34 */
   "2uaf2ufuafaufuafaufuafuf",        /* 16  *153-301-0 35 */
   "uraf2uf2uf2rafr",                 /* 10  *a79-7b1-c 36 */
   "2uraf2dfar2uraf2dfar",            /* 12  *153-123-0 37 */
   "abarbalabrbaladl2ualdl2ul2u",       /* 17  *453-301-0 38 */
   "2ual2ualadl2ualdlabarblabrb",       /* 17  *a73-301-0 39 */
   "abr2bar2uar2urab2u",              /* 10  *a79-e21-9 40 */
   "alblaf2lab2l2f2ufdaf2ufad2f",     /* 16  *a53-7b1-c 41 */
   "r2uraf2dfar2uraf2df2r",           /* 13  *453-123-0 42 */
   "2raf2dfar2uraf2dfar2uar",         /* 13  *a73-123-0 43 */
   "afr2df2radr2uard2raf2darf2u",     /* 16  *a79-123-0 44 */
   "aflfal2ual2ularaururabarb2u",       /* 17  *a53-2b6-a 45 corntwist*/
   "2ulfauruaruafaual",               /* 11  *473-b01-c 46 */
   "alauabublarafaufur",              /* 12  *a79-210-0 47 */
   "l2ual2ualbl2ubdab2ubad2b",        /* 15  *153-1be-9 48 */
   "2fdaf2ufadaf2uarafr2ur2uar",      /* 15  *153-e63-3 49 */
   "afaluabaubauluf2u",               /* 11  *a53-b01-c 50 */
   "lualaualblab2rdar2uradar2uar",      /* 17  *473-2b6-a 51 */
   "abaubl2ualabublfuafaual",         /* 15  *473-b12-9 52 */
   "lualab2ublaualabaraurub",         /* 15  *a53-703-6 53 */
   "2ur2uar2uarfraf2ldal2uladal2ual",   /* 18  *a73-e21-9 54 */
   "b2ubdab2ubad2bl2ual2ualblab",       /* 17  *453-1be-9 55 */
   "arulaurualbuabauabrbarau",        /* 16  *a73-2b6-a 56 */
   "urabarbubauablauarualaur",        /* 16  *453-b17-c 57 */
   "frbauabuarafrubauabar",           /* 14  *a79-b62-a 58 */
   "ul2ual2ualblabraualuaraul",       /* 16  *a73-7b1-c 59 */
   "frbauabuaraflfuafaual",           /* 14  *a79-76e-f 60 */
   "2f2rabafrfarb2r2fauarur",         /* 14  *453-623-3 61 */
   "arauru2f2rabrafarfb2r2f",         /* 14  *a73-623-3 62 */
   "alauabubauabubl",                 /* 10  *a79-1e7-5 63 */
   "arafaufuafaufur",                 /* 10  *a79-2e3-3 64 */
   "2uafalabublfrau2r2uruarur",       /* 17  *473-1e7-5 65 */
   "2ublabfuafaubalab",               /* 11  *a53-e71-5 66 */
   "buabub2uabfrabrb2raf",            /* 14  *459-e32-3 67 corntwist*/
   "al2brbarblar2uruarur",            /* 14  *459-0b7-c 68 */
   "blfalablafl2farafr2falfal",       /* 16  *153-703-6 69 */
   "rabr2balbl2barbarfrabaraf",       /* 16  *153-107-c 70 */
   "2uab2u2bu2bu2b2uab",              /* 10  *a79-231-0 71 */
   };


char *(first[4][4])={{"","au","2u","u"},
		     {"u","","au","2u"},
		     {"2u","u","","au"},
		     {"au","2u","u",""}};

/*     Labeling used for solving top layer:
*      Each side is labeled clockwise.	It doesn't matter where you start
*      as long as you start in a corner.   (Hexadecimal numbers)
*
*			     side 3
*			     back
*			 -------------
*			 | f | f | c |
*			 -------------
*			 | e |or | c |
*			 -------------	 side 4     side 5
*			 | e | d | d |	 right	    bottom
*	     -------------------------------------------------
*	     | b | b | 8 | 3 | 3 | 0 | 13| 13| 10| 15| 15| 16|
*	     ------------------------------------------------
*     left   | a |gn | 8 | 2 |top| 0 | 12|blu| 10| 14|yel| 16|
*    side 2  -------------------------------------------------
*	     | a | 9 | 9 | 2 | 1 | 1 | 12| 11| 11| 14| 17| 17|
*	     -------------------------------------------------
*			 | 7 | 7 | 4 |
*			 -------------
*			 | 6 |red| 4 |	  top is side 0
*			 -------------	  for turning and orienting.
*			 | 6 | 5 | 5 |
*			 -------------
*			     front
*			     side 1
*
*   Cube labeling for middle and last layer:
*
*   For codes such as "813-320" the first three indicate the SOURCE of
*   the corner belonging in positions 1,2 and 3 respectively.  The second
*   three indicate the SOURCE of the edge belonging in positions 1,2 & 3.
*   When everything is in place the code will be 123-123.  "813" means move
*   corner piece who's upper color is in box 8 into position 1, 1 into 2 and
*   3 is already where it belongs.  In other words use string #10 from the
*   array CornerMoves. "320" means cycle three edge pieces counterclockwise
*   without twisting them.  Use string #1 from EdgeMoves.  This way position
*   and twist can be denoted by a single digit.  This system is for the
*   convenience of the computer, not anyone else.  It takes up a little less
*   memory.
*
*				   back
*			       -------------
*			       |   |   |   |
*			       -------------
*			       | c |   | a |
*			       -------------
*			       | 9 | b | 8 |
*		   -------------------------------------
*		   |   | 9 | 6 | 3 | 3 | 0 | b | f |   |
*		   -------------------------------------
*	      left |   |   | 7 | 2 |top| 0 | e |   |   | right
*		   -------------------------------------
*		   |   | 8 | 7 | 2 | 1 | 1 | a | d |   |
*		   -------------------------------------
*			       | 5 | 6 | 4 |
*			       -------------
*			       | 5 |   | 4 |
*			       -------------
*			       |   |   |   |
*			       -------------
*				   front
*/

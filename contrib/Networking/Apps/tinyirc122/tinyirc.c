/* TinyIRC 1.22n
   Copyright (C) 1991-1996 Nathan I. Laredo
   Copyright (c) 2000-2001 Thorsten Glaser <mirabilos@ePost.de>
    * fixes; compiles with libncurses (and libresolv for glibc) only
    * openprojects.net is default IRC net; server in UK
    * protects privacy: /IGNORE up to 4 users
    * accepts /QUOTE, finally
    * /M = /MSG and not /MODE...

   This program is modifiable/redistributable under the terms
   of the GNU General Public Licence.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   Send your comments and all your spare pocket change to
   laredo@gnu.ai.mit.edu (Nathan Laredo) or to PSC1, BOX 709,
   Lackland AFB, TX, 78236-5128
 */
#ifdef __AROS__
#include <bsdsocket.h>
#include <termcap.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <errno.h>
struct Library *SocketBase;
#endif
#include <stdio.h>
#include <string.h>
#ifndef __AROS__
#include <termios.h> 
#endif /* AROS */
#include <pwd.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#ifndef __AROS__
#include <curses.h> 
#endif /* AROS */
#include <stdlib.h>
#ifndef __AROS__
#include <term.h> 
#endif /* __AROS__ */
struct dlist {
    char name[64];
    char mode[64];
    struct dlist *next;
};

#define ischan(x) (*x == '#' || *x == '&' || *x == '+')
struct dlist *obj = NULL, *olist = NULL, *newobj;
unsigned short IRCPORT = 6667;
int my_tcp, sok = 1, my_tty, hist_line, dumb = 0, CO, LI, column;
char *tmp, *linein, *CM, *CS, *CE, *SO, *SE, *DC, *ptr, *term, *fromhost,
 *TOK[20], IRCNAME[32], IRCLOGIN[64], inputbuf[512], ib[1024],
 serverdata[512], ch, lineout[512], *hist[5], termcap[1024];
int cursd=0,curli=0,curx=0,noinput=0,reconnect=1,debug=0,ignt=0;
FILE *debugf;
char ignoreusers[4][64]={""}; short ign[4]={0,0,0,0};
fd_set readfs;
struct timeval time_out;
struct tm *timenow;
static time_t idletimer, datenow, wasdate, tmptime;
struct passwd *userinfo;
#ifndef __AROS__
struct termios iop, iopo; 
#endif /* AROS */
int putchar_x(c)
int c;
{
    return putchar(c);
}
/* #define	tputs_x(s) (tputs(s,0,putchar_x)) */
#define	tputs_x(s) puts(s);
int my_stricmp(str1, str2)
char *str1, *str2;
{
    int cmp;
    while (*str1 != 0 && str2 != 0) {
	if (isalpha(*str1) && isalpha(*str2)) {
	    cmp = *str1 ^ *str2;
	    if ((cmp != 32) && (cmp != 0))
		return (*str1 - *str2);
	} else {
	    if (*str1 != *str2)
		return (*str1 - *str2);
	}
	str1++;
	str2++;
    }
    return (*str1 - *str2);
}
struct dlist *additem(item, p)
char *item;
struct dlist *p;
{
    newobj = (struct dlist *) malloc(sizeof(struct dlist));
    strcpy(newobj->name, item);
    newobj->mode[0] = '\0';
    newobj->next = p;
    return newobj;
}
struct dlist *finditem(item, p)
char *item;
struct dlist *p;
{
    while (p != NULL)
	if (my_stricmp(item, p->name) == 0)
	    break;
	else
	    p = p->next;
    return p;
}
struct dlist *delitem(item, p)
char *item;
struct dlist *p;
{
    struct dlist *prev = NULL, *start = p;
    while (p != NULL)
	if (my_stricmp(item, p->name) == 0) {
	    newobj = p->next;
	    if (obj == p)
		obj = NULL;
	    free(p);
	    if (prev == NULL)
		return newobj;
	    else {
		prev->next = newobj;
		return start;
	    }
	} else {
	    prev = p;
	    p = p->next;
	}
    return start;
}

char encoded[512];
void hexascii(s)
char *s;
{
    int ch, i, j, k, l;

    j = k = l = 0;
    for (i = 0; i < strlen(s) && j < 400; i++) {
	ch = toupper(s[i]);
	if ((ch >= '0') && (ch <= '9')) {
	    (l = (l << 4) | (ch - '0'), k++);
	} else if ((ch >= 'A') && (ch <= 'F')) {
	    (l = (l << 4) | (ch - 'A' + 10), k++);
	}
	if (k == 2)
	    (encoded[j++] = l, k = 0);
    }
    encoded[j] = '\0';
}
void asciihex(s)
char *s;
{
    int i;

    *encoded = '\0';
    for (i = 0; i < strlen(s); i++)
	sprintf(&encoded[strlen(encoded)], "%02x", s[i]);
}
int sendline()
{
    if (debug)
	fputs(lineout,debugf);
    return ((write(my_tcp, lineout, strlen(lineout)) < 1)?0:1);
}
void updatestatus()
{
    int n;
    if (!dumb) {
	if (60 < (datenow = time(NULL)) - wasdate) {
	    wasdate = datenow;
	    timenow = localtime(&datenow);
	    tputs_x(tgoto(CM, 0, LI - 2));
	    tputs_x(SO);
	    if (obj != NULL)
		n = printf("%02d:%02d %s %s (%s) %s", timenow->tm_hour,
			   timenow->tm_min, IRCNAME, obj->name,
			   obj->mode, "AROSIRC 1.22n");
	    else
		n = printf("%02d:%02d %s * * %s", timenow->tm_hour,
			   timenow->tm_min, IRCNAME, "AROSIRC 0.1");
	    while((n++)<CO)
		putchar(' ');
	    tputs_x(SE);
	}
    }
}
static int nop()
{
    return 1;
}
static int doerror()
{
    column = printf("*** ERROR:");
    return 2;
}
static int doinvite()
{
    printf("*** %s (%s) invites you to join %s.",
	   TOK[0], fromhost,(char *) &TOK[3]);
    return 0;
}
static int dojoin()
{
    if (strcmp(TOK[0], IRCNAME) == 0) {
	obj = olist = additem(TOK[2], olist);
	sprintf(lineout, "MODE :%s\n", obj->name);
	sendline();
	printf("*** Now talking in %s", obj->name);
	wasdate = 0;
    } else
	printf("*** %s (%s) joined %s", TOK[0], fromhost, TOK[2]);
    return 0;
}
static int dokick()
{
    printf("*** %s was kicked from %s by %s (%s)",
	   TOK[3], TOK[2], TOK[0], TOK[4]);
    if (strcmp(TOK[3], IRCNAME) == 0) {
	olist = delitem(TOK[2], olist);
	if (obj == NULL)
	    obj = olist;
	if (obj != NULL)
	    printf("\n\r*** Now talking in %s", obj->name);
	wasdate = 0;
    }
    return 0;
}
static int dokill()
{
    printf("*** %s killed by %s: ", TOK[3], TOK[0]);
    if (strcmp(TOK[3], IRCNAME) == 0)
	reconnect = 0;
    return 4;
}
static int domode()
{
    char *t = TOK[3], op = *TOK[3];
    printf("*** %s changed %s to:", TOK[0], TOK[2]);
    if ((newobj = finditem(TOK[2], olist)) != NULL) {
	while ((t = strpbrk(t, "-+psitnml")) != NULL) {
	    if (*t == '-' || *t == '+')
		op = *t;
	    else if (op == '-')
		for (tmp = strchr(newobj->mode, *t); *tmp != '\0'; tmp++)
		    *tmp = *(tmp + 1);
	    else
		strncat(newobj->mode, t, 1);
	    t++;
	}
	if (newobj == obj)
	    wasdate = 0;
    }
    return 3;
}
static int donick()
{
    if (strcmp(TOK[0], IRCNAME) == 0) {
	wasdate = 0;
	strcpy(IRCNAME, TOK[2]);
    }
    printf("*** %s is now known as %s", TOK[0], TOK[2]);
    return 0;
}
static int donotice()
{
    if (!ischan(TOK[2]))
	column = printf("-%s-", TOK[0]);
    else
	column = printf("-%s:%s-", TOK[0], TOK[2]);
    return 3;
}
static int dopart()
{
    printf("*** %s (%s) left %s", TOK[0], fromhost,
	   TOK[2]);
    if (strcmp(TOK[0], IRCNAME) == 0) {
	olist = delitem(TOK[2], olist);
	if (obj == NULL)
	    obj = olist;
	if (obj != NULL)
	    printf("\n\r*** Now talking in %s", obj->name);
	wasdate = 0;
    }
    return 0;
}
static int dopong()
{
    column = printf("*** Got PONG from %s:", TOK[0]);
    return 3;
}
static int doprivmsg()
{
    for(ignt=0;ignt<4;++ignt)
	if(!strncmp(TOK[0],ignoreusers[ignt],63)) {
	    ignt=1;
	    return 0;
	}
    ignt=0;
    if (!ischan(TOK[2]))
	column = printf("\E[1m*%s*", TOK[0]);
    else if (obj != NULL && my_stricmp(obj->name, TOK[2]))
	column = printf("<%s:%s>", TOK[0], TOK[2]);
    else
	column = printf("<%s>", TOK[0]);
    return 3;
}
static int doquit()
{
    printf("*** %s (%s) Quit (%s)", TOK[0], fromhost, TOK[2]);
    return 0;
}
static int dotopic()
{
    printf("*** %s set %s topic to \"%s\"", TOK[0], TOK[2],
	   TOK[3]);
    return 0;
}
int donumeric(num)
int num;
{
    switch (num) {
    case 352:
	column = printf("%-14s %-10s %-3s %s@%s :", TOK[3], TOK[7],
			TOK[8], TOK[4], TOK[5]);
	return 9;
    case 311:
	column = printf("*** %s is %s@%s", TOK[3], TOK[4], TOK[5]);
	return 6;
    case 324:
	if ((newobj = finditem(TOK[3], olist)) != NULL) {
	    strcpy(newobj->mode, TOK[4]);
	    wasdate = 0;
	}
	break;
    case 329:
	tmptime = atoi(TOK[4]);
	strcpy(lineout, ctime(&tmptime));
	tmp = strchr(lineout, '\n');
	if (tmp != NULL)
	    *tmp = '\0';
	column = printf("*** %s formed %s", TOK[3], lineout);
	return 0;
    case 333:
	tmptime = atoi(TOK[5]);
	strcpy(lineout, ctime(&tmptime));
	tmp = strchr(lineout, '\n');
	if (tmp != NULL)
	    *tmp = '\0';
	column = printf("*** %s topic set by %s on %s", TOK[3], TOK[4],
			lineout);
	return 0;
    case 317:
	tmptime = atoi(TOK[5]);
	strcpy(lineout, ctime(&tmptime));
	tmp = strchr(lineout, '\n');
	if (tmp != NULL)
	    *tmp = '\0';
	column = printf("*** %s idle %s second(s), on since %s",
			TOK[3], TOK[4], lineout);
	return 0;
    case 432:
    case 433:
	printf("*** You've chosen an invalid nick.  Choose again.");
/*	tmp = IRCNAME;
	if (!dumb) {
	    tputs_x(tgoto(CM, 0, LI - 1));
	    tputs_x(CE);
	    resetty();
	}
	printf("New Nick? ");
	while ((ch = getchar()) != '\n')
	    if (strlen(IRCNAME) < 9)
		*(tmp++) = ch;
	*tmp = '\0';
	if (!dumb) {
	    tcsetattr(STDIN_FILENO, TCSANOW, &iop);
	    tputs_x(tgoto(CM, 0, LI - 1));
	    tputs_x(CE);
	}
	sprintf(lineout, "NICK :%s\n", IRCNAME);
	sendline();
	return wasdate = 0; */
    default:
	break;
    }
    column = printf("%s", TOK[1]);
    return 3;
}
#define LISTSIZE 52
static char *clist[LISTSIZE] =
{"ADMIN", "AWAY", "CLOSE", "CONNECT", "DESCRIBE", "DIE", "DNS", "ERROR", "HASH",
 "HELP", "INFO", "INVITE", "ISON", "JOIN", "KICK", "KILL", "LINKS", "LIST",
 "LUSERS", "MSG", "MOTD", "MODE", "NAMES", "NICK", "NOTE", "NOTICE", "OPER",
 "PART", "PASS", "PING", "PONG", "PRIVMSG", "QUIT", "REHASH", "RESTART",
 "SERVER", "SQUIT", "STATS", "SUMMON", "TIME", "TOPIC", "TRACE", "USER",
 "USERHOST", "USERS", "VERSION", "WALLOPS", "WHO", "WHOIS", "WHOWAS",
 "IGNORE", "QUOTE"};
#define DO_JOIN 13
#define DO_MSG 19
#define DO_PART 27
#define DO_PRIVMSG 31
#define DO_QUIT 32
#define DO_DESCRIBE 4
#define DO_IGNORE 50
#define DO_QUOTE 51
static int numargs[LISTSIZE] =
{
    15, 1, 15, 3, 2, 15, 15, 15, 1, 15, 15, 15, 15, 15, 3, 2, 15, 15, 15,
    2, 15, 15, 1, 1, 2, 2, 15, 15, 1, 1, 1, 2, 1, 15, 15, 15, 2, 15,
    15, 15, 2, 15, 4, 15, 15, 15, 1, 15, 15, 15, 1, 15
};
static int (*docommand[LISTSIZE]) () =
{
    nop, nop, nop, nop, nop, nop, nop, doerror, nop, nop, nop, doinvite,
	nop, dojoin, dokick, dokill, nop, nop, nop, nop, nop, domode, nop,
	donick, nop, donotice, nop, dopart, nop, nop, dopong, doprivmsg,
	doquit, nop, nop, nop, nop, nop, nop, nop, dotopic, nop, nop,
	nop, nop, nop, nop, nop, nop, nop, nop, nop
};
int wordwrapout(p, count)
char *p;
int count;
{
    while (p != NULL) {
	if ((tmp = strchr(p, ' ')))
	    *(tmp++) = '\0';
	if (strlen(p) < CO - count)
	    count += printf(" %s", p);
	else
	    count = printf("\n\r   %s", p);
	p = tmp;
    }
    return count;
}
int parsedata()
{
    int i, found = 0;

    if(debug) {
	fprintf(debugf, "%s\n", serverdata);
	fflush(debugf);
    }

    if((tmp=strstr(serverdata, "PING :"))) {
	strncpy(lineout,tmp,512);
	lineout[510]='\0'; /* for security */
	lineout[1]='O';
	strcat(lineout,"\n"); /* fsck buxfixxes */
	return sendline();
    }
    
    if (!dumb)
	tputs_x(tgoto(CM, 0, LI - 3));
    TOK[i = 0] = serverdata;
    TOK[i]++;
    while (TOK[i] != NULL && i < 15)
	if (*TOK[i] == ':')
	    break;
	else {
	    if ((tmp = strchr(TOK[i], ' '))) {
		TOK[++i] = &tmp[1];
		*tmp = '\0';
	    } else
		TOK[++i] = NULL;
	}
    if (TOK[i] != NULL && *TOK[i] == ':')
	TOK[i]++;
    TOK[++i] = NULL;
    if ((tmp = strchr(TOK[0], '!'))) {
	fromhost = &tmp[1];
	*tmp = '\0';
    } else
	fromhost = NULL;
    if ((!dumb)&&(!ignt))
	putchar('\n');
    ignt=column = 0;
    if ((i = atoi(TOK[1])))
	i = donumeric(i);
    else {
	for (i = 0; i < LISTSIZE && !found; i++)
	    found = (strcmp(clist[i], TOK[1]) == 0);
	if (found)
	    i = (*docommand[i - 1]) ();
	else
	    i = nop();
    }
    if (i) {
	if (*TOK[i] == '@' && TOK[i + 1] == NULL) {
	    hexascii(&TOK[i][1]);
	    printf("\E[1m");
	    wordwrapout(encoded);
	    printf("\E[m");
	} else {
	    while (TOK[i])
		column = wordwrapout(TOK[i++], column);
	    printf("\E[m");
	}
    }
    if (dumb)
	putchar('\n');
    if (strncmp(TOK[1], "Closing", 7) == 0)
	return (reconnect = 0);
    return 1;
}
int serverinput()
{
    int count, i;
    if ((count = read(my_tcp, ib, 1024)) < 1)
	return 0;
    for (i = 0; i < count; i++)
	if (ib[i] == '\n') {
	    serverdata[cursd] = '\0';
	    cursd = 0;
	    if (!parsedata())
		return 0;
	} else if (ib[i] != '\r')
	    serverdata[cursd++] = ib[i];
    return 1;
}
void parseinput()
{
    int i, j, outcol, c, found = 0;
    if (*linein == '\0')
	return;
    strcpy(inputbuf, linein);
    TOK[i = 0] = inputbuf;
    while (TOK[i] != NULL && i < 5)
	if ((tmp = strchr(TOK[i], ' '))) {
	    TOK[++i] = &tmp[1];
	    *tmp = '\0';
	} else
	    TOK[++i] = NULL;
    TOK[++i] = NULL;
    if (!dumb) {
	tputs_x(tgoto(CM, 0, LI - 3));
	putchar('\n');
    }
    if (*TOK[0] == '/') {
	TOK[0]++;
	for (i = 0; i < strlen(TOK[0]) && isalpha(TOK[0][i]); i++)
	    TOK[0][i] = toupper(TOK[0][i]);
	for (i = 0; i < LISTSIZE && !found; i++)
	    found = (strncmp(clist[i], TOK[0], strlen(TOK[0])) == 0);
	i--;
	if (!found) {
	    printf("*** Invalid command");
	    return;
	}
	if (i == DO_JOIN) {
	    if ((newobj = finditem(TOK[1], olist)) != NULL) {
		wasdate = 0;
		obj = newobj;
		printf("*** Now talking in %s", obj->name);
		return;
	    } else if (!ischan(TOK[1])) {
		obj = olist = additem(TOK[1], olist);
		printf("*** Now talking to %s", obj->name);
		wasdate = 0;
		return;
	    }
	}
	if((i == DO_PART) && (obj != NULL)) {
		if(TOK[1]==NULL)
		    TOK[1]=obj->name;
		if(!ischan(TOK[1]))
		    if ((newobj = finditem(TOK[1], olist)) != NULL) {
			wasdate = 0;
			olist = delitem(TOK[1], olist);
			if (obj == NULL)
			    obj = olist;
			printf("*** No longer talking to %s", TOK[1]);
			if (obj != NULL)
			    printf(", now %s", obj->name);
			wasdate = 0;
			return;
		}
	}
	if (i == DO_MSG)
	    i = DO_PRIVMSG;
	if (i == DO_PRIVMSG && (TOK[1] == NULL || TOK[2] == NULL)) {
	    printf("*** Unable to parse message");
	    return;
	}
	if(i == DO_QUOTE) {
	    sprintf(lineout,"%s",TOK[j=1]);
	    while(TOK[++j])
		sprintf(lineout,"%s %s",lineout,TOK[j]);
	    printf("= %s",lineout);
	    sprintf(lineout,"%s\n",lineout);
	    sendline();
	    return;
	}
	if(i == DO_IGNORE) {
	    if((TOK[1][0]>='0')&&(TOK[1][0]<'4')) {
		j=(int)(TOK[1][0]-'0');
		ign[j]=0;
		printf("\E[1m*** UNIGNORE %d: %s\E[m",j,ignoreusers[j]);
		return;
	    }
	    for(j=0;j<4;j++)
		if (!ign[j]) {
		    ign[j]=1;
		    strncpy(ignoreusers[j],TOK[1],63);
		    ignoreusers[j][63]='\0';
		    printf("\E[1m*** IGNORING %d: %s\E[m",j,ignoreusers[j]);
		    sprintf(lineout,"PRIVMSG %s :PLONK AROSIRC\n",ignoreusers[j]);
		    sendline();
		    goto ign_got1;
		}
	    printf("\E[1m*** IGNORE %s FAILED!\E[m",TOK[1]);
	ign_got1:
	    return;
	}
	if(i == DO_DESCRIBE)
	if(TOK[1] == NULL) {
	    printf("*** Unable to parse message");
	    return;
	} else {
	    sprintf(lineout, "PRIVMSG %s :\1ACTION %s", obj->name, TOK[1]);
	    outcol = printf("> ACTION %s", TOK[j = 1]);
	    while (TOK[++j]) {
		outcol = wordwrapout(TOK[j], outcol);
		strcat(lineout, " ");
		strcat(lineout, TOK[j]);
	    }
	    strcat(lineout, "\1\n");
	} else {
	strcpy(lineout, clist[i]);
	if (i == DO_QUIT)
	    reconnect = 0;
	if (i == DO_QUIT && TOK[1] == NULL)
	    strcat(lineout, " :AROSIRC");
	j = 0;
	if (i != DO_PRIVMSG || TOK[1] == NULL)
	    outcol = printf("= %s", lineout);
	else if (ischan(TOK[1]))
	    outcol = printf(">%s>", TOK[1]);
	else outcol = printf("\E[1m-> *%s*", TOK[1]);
	while (TOK[++j]) {
	    c = strlen(lineout);
	    sprintf(&lineout[c], "%s%s", ((j == numargs[i] &&
			      TOK[j + 1] != NULL) ? " :" : " "), TOK[j]);
	    if (j > 1 || i != DO_PRIVMSG)
		outcol = wordwrapout(TOK[j], outcol);
	    printf("\E[m");
	}
	strcat(lineout, "\n");
    }} else {
	if (obj == NULL) {
	    printf("*** Nowhere to send");
	    return;
	}
	if (*TOK[0] == '@') {
	    asciihex(&linein[1]);
	    strcpy(&linein[1], encoded);
	} else if (*TOK[0] == '#') {
	    hexascii(&linein[1]);
	    outcol = wordwrapout(encoded);
	    return;
	}
	sprintf(lineout, "PRIVMSG %s :%s\n", obj->name, linein);
	outcol = printf("> %s", TOK[j = 0]);
	while (TOK[++j])
	    outcol = wordwrapout(TOK[j], outcol);
    }
    sendline();
    idletimer = time(NULL);
}
void mypchar(c)
char c;
{
    if (c >= ' ')
	putchar(c);
    else {
	tputs_x(SO);
	putchar(c + 64);
	tputs_x(SE);
    }
}
void ppart()
{
    int i, x = (curx / CO) * CO;
    tputs_x(tgoto(CM, 0, LI - 1));
    for (i = x; i < x + CO && i < curli; i++)
	mypchar(linein[i]);
    tputs_x(CE);
    tputs_x(tgoto(CM, curx % CO, LI - 1));
}
void histupdate()
{
    linein = hist[hist_line];
    curx = curli = strlen(linein);
    ppart();
}

void userinput()
{
    int i, z;
    if (dumb) {
	fgets(linein, 500, stdin);
	tmp = strchr(linein, '\n');
	if (tmp != NULL)
	    *tmp = '\0';
	parseinput();
	putchar('\n');
    } else {
	read(my_tty, &ch, 1);
	if (ch == '\177')
	    ch = '\10';
	switch (ch) {
	case '\3':
	    kill(getpid(), SIGINT);
	    break;
	case '\0':
	    if (curx >= CO) {
		curx = 0;
		ppart();
	    } else
		tputs_x(tgoto(CM, curx = 0, LI - 1));
	    break;
	case '\4':
	case '\10':
	    if (curx) {
		if (ch == '\4' && curx < curli)
		    curx++;
		if (curli == curx)
		    linein[(--curx)] = '\0';
		else
		    for (i = (--curx); i < curli; i++)
			linein[i] = linein[i + 1];
		curli--;
		if (DC != NULL && curx % CO != CO - 1) {
		    tputs_x(tgoto(CM, curx % CO, LI - 1));
		    tputs_x(DC);
		} else
		    ppart();
	    }
	    break;
	case '\2':
	    if (curx > 0)
		curx--;
	    if (curx % CO == CO - 1)
		ppart();
	    else
		tputs_x(tgoto(CM, curx % CO, LI - 1));
	    break;
	case '\5':
	    curx = curli;
	case '\14':
	    ppart();
	    break;
	case '\6':
	    if (curx < curli)
		curx++;
	    tputs_x(tgoto(CM, curx % CO, LI - 1));
	    break;
	case '\1':
	    if ((++hist_line) >= 5)
		hist_line = 0;
	    histupdate();
	    break;
	case '\23':
	    if ((--hist_line) < 0)
		hist_line = 4;
	    histupdate();
	    break;
	case '\r':
	case '\n':
	    if (!curli)
		return;
	    tputs_x(tgoto(CM, 0, LI - 1));
	    tputs_x(CE);
	    parseinput();
	    if ((++hist_line) >= 5)
		hist_line = 0;
	    curx = curli = 0;
	    linein = hist[hist_line];
	    break;
	case '\27':
	    if (obj == NULL)
		break;
	    obj = obj->next;
	    if (obj == NULL)
		obj = olist;
	    wasdate = 0;
	    break;
	case '\32':
	    kill(getpid(), SIGTSTP);
	    break;
	default:
	    if (curli < 499) {
		if (curli == curx) {
		    linein[++curli] = '\0';
		    linein[curx++] = ch;
		    mypchar(ch);
		    tputs_x(CE);
		} else {
		    for (i = (++curli); i >= curx; i--)
			linein[i + 1] = linein[i];
		    linein[curx] = ch;
		    for (i = (curx % CO); i < CO &&
			 (z = (curx / CO) * CO + i) < curli; i++)
			mypchar(linein[z]);
		    tputs_x(CE);
		    curx++;
		}
	    }
	    break;
	}
    }
}
void cleanup(sig)
int sig;
{
    if (!dumb) {
#ifndef __AROS__
	resetty();
#endif /* __AROS__ */
	tputs_x(tgoto(CS, -1, -1));
	tputs_x(tgoto(CM, 0, LI - 1));
	fflush(stdout);
    }
#ifndef __AROS__
    psignal(sig, "AROSIRC");
#endif /* __AROS__ */
    if (sig != SIGTSTP)
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
	exit(0);
    kill(getpid(), SIGSTOP);
}
void stopin()
{
    signal(SIGTTIN, stopin);
    noinput = 1;
}

void redraw()
{
    signal(SIGCONT, redraw);
    signal(SIGTSTP, cleanup);
    if (!dumb) {
	if (noinput) {
#ifndef __AROS__
	    tcsetattr(STDIN_FILENO, TCSANOW, &iop);
#endif
	}
	wasdate = 0;
	tputs_x(tgoto(CS, LI - 3, 0));
	updatestatus();
	tputs_x(tgoto(CM, LI - 3, 0));
    }
    noinput = 0;
}

int makeconnect(hostname)
char *hostname;
{
printf("AROS Entered Makeconnect\n");
    struct sockaddr_in sa;
    struct hostent *hp;
    int s, t;
    if ((hp = gethostbyname(hostname)) == NULL)
	return -1;
printf("AROS Makeconnect: Return from gethostbyname()\n");
    for (t = 0, s = -1; s < 0 && hp->h_addr_list[t] != NULL; t++) {
	bzero(&sa, sizeof(sa));
	bcopy(hp->h_addr_list[t], (char *) &sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;
	sa.sin_port = htons((unsigned short) IRCPORT);
	s = socket(hp->h_addrtype, SOCK_STREAM, 0);
printf("AROS Makeconnect: Return from socket()\n");
	if (s > 0) {
	    if (connect(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		close(s);
		s = -1;
printf("AROS Makeconnect: Bad connect()\n");
	    } else {
printf("AROS connect is good\n");
		fcntl(s, F_SETFL, O_NDELAY);
		my_tcp = s;
		sprintf(lineout, "USER %s * * :AROSIRC - Yea\n", IRCLOGIN);
		printf("D: USER %s * * :AROSIRC - Yea\n", IRCLOGIN);
		sendline();
		sprintf(lineout, "NICK :%s\n", IRCNAME);
		printf("D: NICK :%s\n", IRCNAME);
		sendline();
		for (obj = olist; obj != NULL; obj = olist->next) {
		    sprintf(lineout, "JOIN %s\n", obj->name);
		    sendline();
		} 
	    }
	}
    }
    return s;
}
int main(argc, argv)
int argc;
char **argv;
{
#ifdef __AROS__
  SocketBase = OpenLibrary("bsdsocket.library", 3);
  if(SocketBase == NULL)
    return RETURN_FAIL;
  SetErrnoPtr(&errno, sizeof(errno));
#endif /* __AROS__ */

    char hostname[64];
    int i = 0;
    printf("TinyIRC 1.22n (c) 1991-1996 Nathan Laredo; 2000-2001 mirabilos[TM]\n");
    if (!(tmp = (char *) getenv("IRCSERVER")))
	strcpy(hostname, "irc.freenode.net");
    else {
	while (*tmp && *tmp != ':')
	    hostname[i++] = *(tmp++);
	hostname[i] = '\0';
	if (*tmp == ':')
	    IRCPORT = (unsigned short) atoi(++tmp);
    }
    if (!(argc==1)) {
	if ((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == 'd'))
	    dumb = 1;
	else if ((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == 'g')) {
	    if ( (debugf=fopen("TinyIRC.LOG","ab")) != NULL)
		debug=1;
	} else if ((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == 'G')) {
	    debug=1; dumb=1;
	    debugf=stdout;
	} else {
	    fprintf(stderr,"usage: %s [-dumb|-gG]\nenvironment:\n", argv[0]);
	    fprintf(stderr," TERM IRCSERVER IRCNICK\n");
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
	    exit(1);
	}
    }
/*    if ((my_tty = open("/dev/tty", O_RDWR, 0)) == -1) */
	my_tty = fileno(stdin);
#ifndef __AROS__
    userinfo = getpwuid(getuid());
    strcpy(IRCLOGIN, userinfo->pw_name);
#else
    strcpy(IRCLOGIN, "aros");
#endif
    tmp = (char *) getenv("IRCNICK");
    if (tmp == NULL)
		strncpy(IRCNAME, IRCLOGIN, sizeof(IRCNAME));
    else	strncpy(IRCNAME, tmp, sizeof(IRCNAME));
    fprintf(stderr, "*** User is %s\n", IRCLOGIN);
    printf("*** trying port %d of %s\n\n", IRCPORT, hostname);
       /*  if (makeconnect(hostname) < 0) { */
	if (makeconnect(hostname) < 0) {
	fprintf(stderr, "*** %s refused connection, aborting\n", hostname);
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
	exit(0);
    }
    idletimer = time(NULL);
    ptr = termcap;
    if ((term = (char *) getenv("TERM")) == NULL) {
	fprintf(stderr, "tinyirc: TERM not set\n");
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
	exit(1);
    }
#ifndef __AROS__ /* fix this later */
    if (tgetent(NULL, term) < 1) {
	fprintf(stderr, "tinyirc: no termcap entry for %s\n", term);
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
	exit(1);
    }
#endif /* !__AROS__ */
    if ((CO = tgetnum("co") - 2) < 20)
	CO = 78;
    if ((LI = tgetnum("li")) == -1)
	LI = 24;

    if (!dumb) {
#define tgs(x) ((char *) tgetstr(x, &ptr))
	if ((CM = tgs("cm")) == NULL)
	    CM = tgs("CM");
	if ((SO = tgs("so")) == NULL)
	    SO = "";
	if ((SE = tgs("se")) == NULL)
	    SE = "";
	if (!CM || !(CS = tgs("cs")) ||
	    !(CE = tgs("ce"))) {
	    printf("tinyirc: sorry, no termcap cm,cs,ce: dumb mode set\n");
	    dumb = 1;
	}
	if (!dumb) {
#ifndef __AROS__
/* 	    DC = tgs("dc");
	    tcgetattr(STDIN_FILENO, &iop);
	    iopo=iop;
	    cfmakeraw(&iop);
	    tcsetattr(STDIN_FILENO, TCSANOW, &iop); */
#endif /* __AROS__ */
	}

    }
    redraw();
    signal(SIGINT, cleanup);
    signal(SIGHUP, cleanup);
    signal(SIGTERM, cleanup);
    signal(SIGSEGV, cleanup);
    signal(SIGTTIN, stopin);
    for (i = 0; i < 5; i++)
	hist[i] = (char *) calloc(512, sizeof(char));
    linein = hist[hist_line = 0];
    while (sok) {
	FD_ZERO(&readfs);
	FD_SET(my_tcp, &readfs);
	if (!noinput)
	    FD_SET(my_tty, &readfs);
	if (!dumb) {
	    time_out.tv_sec = 61;
	    time_out.tv_usec = 0;
	}
	if (select(FD_SETSIZE, &readfs, NULL, NULL, (dumb ? NULL : &time_out))) {
	    if (FD_ISSET(my_tty, &readfs))
		userinput();
	    if (FD_ISSET(my_tcp, &readfs))
		sok = serverinput();
	    if (!wasdate)
		updatestatus();
	} else
	    updatestatus();
	if (!sok && reconnect) {
	    close(my_tcp);
	    printf("*** trying port %d of %s\n\n", IRCPORT, hostname);
	/*    if (makeconnect(hostname) < 0) { */
	    if (makeconnect(hostname) < 0) {
		fprintf(stderr, "*** %s refused connection\n", hostname);
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
		exit(0);
	    }
	    sok++;
	}
	if (!dumb)
	    tputs_x(tgoto(CM, curx % CO, LI - 1));
	fflush(stdout);
    }
    if (!dumb) {
	tputs_x(tgoto(CS, -1, -1));
	tputs_x(tgoto(CM, 0, LI - 1));
#ifndef __AROS__
/*	tcsetattr(STDIN_FILENO, TCSANOW, &iopo); */
#endif /* __AROS__ */
    }
#ifdef __AROS__
  CloseLibrary(SocketBase);
#endif /* __AROS__ */
    exit(0);
}

/* compile-command: (linux-libc)
 * gcc -Wall -O2 -fomit-frame-pointer -o tinyirc tinyirc.c -lncurses
 * compile-command: (fsck'ing GNU libc)
 * gcc -Wall -O2 -fomit-frame-pointer -o tinyirc tinyirc.c -lncurses -lresolv
 */

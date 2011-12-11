/*
 *                     Unshar V1.3 (C) Copyright Eddy Carroll 1990
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Usage: Unshar {-overwrite} {-nosort} <filename> ...
 *
 * Extracts files from a SHAR'd archive.
 *
 * This utility has a few advantages over the version of SH on Fish Disk 92.
 * For a start, it doesn't crash if it gets a slightly unusual format! It
 * also has a (limited) capability for extracting files from shar archives
 * which use 'SED' rather than 'CAT' (typically, this is done so that
 * each line in the file may be prefixed with an 'X' or similar, so that
 * indentation is preserved). Unshar will spot 'SED' lines, and treat them
 * the same as 'CAT' (allowing for different parameters of course) with
 * the exception that any leading characters matching the string specified
 * in the SED command are discarded.
 *
 * Unshar checks files being extracted to see if they are to be stored
 * within a sub-directory. If they are, and the sub-directory does not
 * already exist, it is created.
 *
 * One other small addition is that any filenames which are prefixed with
 * the characters "./" have these characters removed. Some shar files
 * use this prefix to ensure that the files are stored in the current
 * directory.
 *
 * Files are extracted into the current directory. As each file is extracted,
 * an appropriate message is printed on the screen. If the file already
 * exists, the user is warned and given the chance to avoid overwriting it
 * "Overwrite file (Yes/No/All)? ". The default is Yes. If All is selected,
 * then this prompt is supressed for the rest of the current file. It may
 * be disabled for all the files by specifying the -o switch on the
 * command line.
 *
 * By default, unshar will do a `prescan' over all the files listed, looking
 * at the first few lines of each for a Subject: line. If one is found, then
 * it examines it for Issue numbers and Part numbers, and unshars those files
 * having the lowest numbers first. This results in the shar files being
 * extracted in the correct order, regardless of what order they were listed
 * in on the command line. You can override this behaviour and unshar files
 * in the command line order by specifying the -n switch.
 * 
 * DISTRIBUTION
 * I retain copyright rights to this source code, though it may be _freely_
 * distributed. The executable file created from this source code is in
 * the Public Domain and may be distributed without any restrictions.
 *
 *
 * N.b. The code is starting to look a bit messy; could be it will get
 *      a complete overhaul for the next revision.
 *
 */

/* Compiles under Lattice V5.04 */

#include <string.h>
#include <exec/types.h>
#include <ctype.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>


#define YES			1
#define NO			0
#define CR			'\015'
#define EOL			'\012'
#define SINGLEQUOTE '\''
#define DOUBLEQUOTE '\042'
#define MAXSTRING	512		/* Maximum length of input line */

/*
 *		New handler for Ctrl-C. Checks if CTRL-C received, and if it has,
 *		sets the global CtrlC variable to true.
 */
#define chkabort() (CtrlC |= ((SetSignal(0,0) & SIGBREAKF_CTRL_C)))


char HelpMsg[] = "\
Unshar V1.3 by Eddy Carroll 1990 Public Domain, extracts Unix shar archives.\
\n\
Usage: unshar {-overwrite} {-nosort} <filename> ...\n";

char DiskMsg[]  = "Unshar aborted - Disk write error (disk full?)\n";
char ErrorMsg[] = "Unshar: Invalid CAT or SED command at line ";

int linenum;
int CtrlC = NO;

/*
 * --------------------------------------------------------------------------
 * The following block may be removed `as-is' and used in other programs.
 * It provides basic buffered i/o on two files, an input file and an output
 * file. It also provides output to the current standard output via
 * print. Buffering is done using buffers of size MAXBUF.
 *
 * The following routines are provided:
 *
 * getc() returns an integer corresponding to the next character read from
 * infile, or EOF if the end of file has been reached.
 *
 * putc(c) outputs a character to outfile. If a diskerror occurs, the global
 * diskerror is set to YES, and all further diskwrites are ignored.
 *
 * getline() returns a pointer to a string containing the next line
 * read in from infile. getline() also checks for CTRL-C via chkabort()
 * 
 * putline(s) outputs a string to outfile, returning non-zero if an
 * error occurred during the write.
 *
 * flushin() resets getc() and getline() for input from a new file
 *
 * flushout() flushes output buffer; call prior to closing output file.
 *
 * input() returns a pointer to a string containing a line from stdin.
 *
 * print(s) prints a message on standard output.
 *
 * print3(s1,s2,s3) outputs three strings on standard output.
 *
 * numtostr(n) returns a pointer to the ascii representation of n.
 *
 * Special Notes
 * ~~~~~~~~~~~~~
 * You should ensure that you use the filenames 'infile' and 'outfile'
 * when you are opening the input and output files in main(). Also,
 * do not #define EOF or MAXBUF elsewhere in your program.
 *
 */

#define EOF		-1
#define MAXBUF	10000

BPTR infile, outfile;
LONG maxin = MAXBUF, maxout = MAXBUF, inbuf = MAXBUF, outbuf = 0;
unsigned char inbuffer[MAXBUF], outbuffer[MAXBUF];
int diskerror = NO;

/*
 *		int getc()
 *		----------
 *		Returns next character from infile, or EOF if end of file.
 *
 *		Replaced by a macro to improve performance. Original function was:
 *
 *		int getc()
 *		{
 *			if (!maxin)
 *				return (EOF);
 *
 *			if (inbuf >= maxin) {
 *				maxin = Read(infile, inbuffer, MAXBUF);
 *				inbuf = 0;
 *				if (!maxin)
 *					return (EOF);
 *			}
 *			return (inbuffer[inbuf++]);
 *		}
 *
 */
#define IF(x,y,z) ((x) ? (y) : (z))

#define getc()	IF(!maxin, EOF, \
				IF(inbuf >= maxin, ( \
					inbuf = 0, maxin = Read(infile, inbuffer, MAXBUF), \
					IF(!maxin, EOF, inbuffer[inbuf++]) \
				), inbuffer[inbuf++])) \

/* 
 *		Prepares getc() for input from a new file
 */
#define flushin() (maxin = MAXBUF, inbuf = MAXBUF)

/*
 *		putc(ch)
 *		--------
 *		Outputs character ch to disk. If a diskerror is detected, then all
 *		further output is ignored and the global diskerror is set to YES.
 *
 *		Replaced by a macro for performance reasons. Original function was:
 *
 *		void putc(ch)
 *		int ch;
 *		{
 *			if (ch == EOF)
 *				maxout = outbuf;
 *			else
 *				outbuffer[outbuf++] = ch;
 *		
 *			if (outbuf >= maxout) {
 *				if (!diskerror && Write(outfile, outbuffer, maxout) == -1)
 *					diskerror = YES;
 *				outbuf = 0;
 *				maxout = MAXBUF;
 *			}
 *		}
 */
#define flushout() (maxout = outbuf, \
					IF(!diskerror && Write(outfile, outbuffer, maxout) == -1, \
						diskerror = YES, \
						0), \
					outbuf = 0, maxout = MAXBUF)

#define putc(ch) (outbuffer[outbuf++] = ch, \
				  IF(outbuf >= maxout, \
				  	(IF (!diskerror && \
							Write(outfile, outbuffer, maxout) == -1, \
						diskerror = YES, \
						0), \
					 outbuf = 0, maxout = MAXBUF), \
					0))

/*
 *		print(s)
 *		--------
 *		Outputs a message to std output
 */
void print(s)
char *s;
{
	Write(Output(),s,strlen(s));
}

/*
 *		print3()
 *		--------
 *		Outputs three strings to std output.
 *		Useful for sequences like print3("string", variable, "string");
 */
void print3(s1,s2,s3)
char *s1,*s2,*s3;
{
	print(s1);
	print(s2);
	print(s3);
}

/*
 *		getline()
 *		---------
 *		Reads in a line from current infile into string, and returns a
 *		pointer to that string. Returns NULL if EOF encountered.
 */
char *getline()
{
	register int ch, i = 0;
	static char line[MAXSTRING];

	ch = getc();
	if (ch == EOF)
		return (NULL);

	while (i < (MAXSTRING-1) && ch != EOF && ch != EOL) {
		line[i++] = ch;
		ch = getc();
	}

	line[i] = '\0';
	linenum++;
	chkabort();
	return (line);
}

/*
 *		putline()
 *		---------
 *		Outputs a string to the current output file (terminating it with LF).
 *		Returns 0 for success, non-zero for disk error
 */
int putline(s)
char *s;
{
	while (*s)
		putc(*s++);
	putc(EOL);
	return (diskerror);
}

/*
 *		input()
 *		-------
 *		Reads a line from keyboard and returns pointer to it
 */
char *input()
{
	static char s[80];
	int len;

	s[0] = '\0';
	len = Read(Input(),s,75);
	if (len < 0)
		len = 0;
	s[len] = '\0';
	chkabort();
	return(s);
}

/*
 *		numtostr()
 *		----------
 *		Converts integer to string and returns pointer to it.
 */
char *numtostr(n)
int n;
{
	static char s[20];
	int i = 19;

	s[19] = '\0';
	if (n)
		while (n)
			s[--i] = '0' + (n % 10), n /= 10;
	else
		s[--i] = '0';
	return(&s[i]);
}

/*
 *		--------------------* End of Buffered IO routines *-----------------
 */

/*
 *		myindex()
 *		-------
 *		Like standard Unix index(), but skips over quotes if skip == true.
 *		Also skips over chars prefixed by a \. Returns pointer to first
 *		occurance of char c inside string s, or NULL.
 */

char *myindex(s,c,skip)
char *s,c;
int skip;
{
	register char *p = s;
	register int noquotes = YES, literal = NO;

	while (*p) {
		if (literal) {
			literal = NO;
			p++;
		} else {
			if (skip && ((*p == SINGLEQUOTE) || (*p == DOUBLEQUOTE)))
				noquotes = !noquotes;
			if (noquotes && (*p == c))
				return(p);
			literal = (*p == '\\');
			p++;
		}
	}
	return (NULL);
}

/*
 *		getname()
 *		---------
 *		Extracts a string from start of string s1 and stores it in s2.
 *		Leading spaces are discarded, and quotes, if present, are used to
 *		indicate the start and end of the filename. If mode is MODE_FILE,
 *		then if the name starts with either './' or '/', this prefix is
 *		stripped. This doesn't happen if the mode is MODE_TEXT. A pointer
 *		to the first character after the string in s1 is returned. In
 *		addition, any characters prefixed with are passed through without
 *		checking.
 */

#define MODE_FILE 1
#define MODE_TEXT 2

char *getname(s1,s2,mode)
char *s1,*s2;
{
	char endchar = ' ';

	while (*s1 == ' ')
		s1++;

	if (*s1 == SINGLEQUOTE || *s1 == DOUBLEQUOTE)
		endchar = *s1++;

	if (mode == MODE_FILE) {
		if (s1[0] == '.' && s1[1] == '/')
			s1 += 2;
		while (*s1 == '/')
			s1++;
	}

	while (*s1 && *s1 != endchar) {
		if (*s1 == '\\' && *(s1+1))
			s1++;
		*s2++ = *s1++;
	}
	*s2 = '\0';

	if (*s1 == endchar)
		return(++s1);
	else
		return(s1);
}


/*
 *		checkfordir()
 *		-------------
 *		Checks filename to see if it is inside a subdirectory. If it is,
 *		then checks if subdirectory exists, and creates it if it doesn't.
 */
void checkfordir(filename)
char *filename;
{
	char dir[80], *p;
	int i, x;
	BPTR dirlock;

	p = filename;

	while ((p = myindex(p, '/', 1))) {

		/* Dir exists, so copy dir part of filename into dir name area */

		x = p - filename;
		for (i = 0; i < x; i++)
			dir[i] = filename[i];
		dir[i] = '\0';

		/* Now, see if directory exists, if not then create */
		if (((dirlock = Lock(dir,ACCESS_READ))) == BNULL) {
			dirlock = CreateDir(dir);
			if (dirlock) {
				print3("Creating directory ", dir, "\n");
			}
		}
		if (dirlock)
			UnLock(dirlock);

		p++;
	}
}

/*
 *		unshar()
 *		--------
 *		Extracts all stored files from a shar file. Returns zero for success,
 *		non-zero if a disk error occurred. If echofilename is non-zero, then
 *		the name of each shar file is output before unsharing it. If
 *		overwrite is non-zero, then existing files are overwritten without
 *		any warning. If title is non-NULL, then it points to a string
 *		which is printed out before any files are extracted.
 */
int unshar(sharfile, title, echofilename, overwrite)
char *sharfile, *title;
int echofilename, overwrite;
{
	register char *s, *p;
	char endmarker[100], filename[100],sedstring[100];
	int endlen, stripfirst = 0, startfile, found = NO, err = NO, skip, sedlen = 0;
	int append;
	BPTR filelock = BNULL;

	if (((infile = Open(sharfile, MODE_OLDFILE))) == BNULL) {
		print3("Can't open file ", sharfile, " for input\n");
		return(1);
	}

	linenum = 0;
	if (echofilename)
		print3("\033[7m Shar file: ", sharfile, " \033[0m\n");
	if (title)
		print(title);

	while (!err && !CtrlC && (s = getline()) != NULL) {
		startfile = NO;
		if (strncmp(s,"cat ",4) == 0) {
			startfile  = YES;
			stripfirst = NO;
		}
		if (strncmp(s,"sed ",4) == 0) {
			startfile  = YES;
			stripfirst = YES;
			sedlen = 0;
			/*
			 *		Note - tons of sanity checks done here to ensure that a
			 *		sed line of the form:
			 *
			 *			sed >s/somefile <<'endmarker' -e 's/X//'
			 *
			 *		Will be interpreted correctly.
			 */

#define ISPREFIX(ch)	(ch == DOUBLEQUOTE || ch == SINGLEQUOTE || ch == ' ')
#define ISMETA(ch)		(ch == '<' || ch == '>')
#define ISOK(s)			(s[1] == '/' && ISPREFIX(s[-1]) && !ISMETA(s[-2]))

			p = s;
			while ((p = myindex(p,'s',0)) != NULL && !ISOK(p))
				p++;
			if (p != NULL) {
				p += 2;				/* Skip over the 's/' bit	*/
				if (*p == '^')		/* Skip past starting char	*/
					p++;
				while (*p && *p != '/')
					sedstring[sedlen++] = *p++;
			} 
		}

		if (startfile) {	
			if (found == NO) {
				found = YES;
			}
			if ((p = myindex(s,'>',1)) == NULL) {
				print3(ErrorMsg, numtostr(linenum), "(a)\n");
			} else {
				/*
				 *	   This next bit checks to see if we are creating or
				 *	   appending to the output file (i.e. >file or >>file)
				 */
				if (*++p == '>') {
					p++;
					append = YES;
				} else
					append = NO;
				getname(p,filename,MODE_FILE);
				p = s;
				while ((p = myindex(p,'<',1)) && (p[1] != '<'))
					;
				if (p)
					getname(p+2,endmarker,MODE_TEXT);

				endlen = strlen(endmarker);

				if (strlen(filename) && endlen) {

					checkfordir(filename);

					/* Found a valid line so perform extract */

					/* Check if file exists */

					skip = NO;
					outfile = BNULL;
					if (!overwrite) {
						filelock = Lock(filename, ACCESS_READ);
						if (filelock) {
							UnLock(filelock);
							if (!append) {
								print3("Overwrite file ", filename,
									" (Yes, [No], All)? ");

								switch (tolower(*input())) {
									case 'a': overwrite = YES;	break;
									case 'y': skip = NO;		break;
									default : skip = YES;		break;
								}
							}
						}
					}

					/*
					 *		Open as old file and seek to the end if
					 *		appending AND the file already exists. If
					 *		it doesn't exist, then just open as new file.
					 */
					if (filelock && append) {
						outfile = Open(filename, MODE_READWRITE);
						if (outfile)
							Seek(outfile, 0, OFFSET_END);
					} else if (!skip)
						outfile = Open(filename, MODE_NEWFILE);

					if (!outfile && !skip) {
						print3("Couldn't open file ",filename," for output\n");
						skip = YES;
					}
					if (!skip) {
						if (filelock && append)
							print3("Extending file ", filename, "\n");
						else
							print3("Unsharing file ", filename, "\n");
					}
					s = getline();
					err = NO;
					while (s && strncmp(s,endmarker,endlen) && !CtrlC) {
						if (stripfirst && !strncmp(sedstring,s,sedlen))
							s += sedlen;
						if (!skip && (err = putline(s)))
							break;
						s = getline();
					}
					if (!skip) {
						flushout();
						if (err || diskerror)
							print(DiskMsg), err = YES;
						Close(outfile);
					}
				} else
					print(ErrorMsg, numtostr(linenum), "\n");
			}
		}
	}

	if (!err && !CtrlC) {
		if (found)
			print("Unshar done\n");
		else
			print("No files to unshar\n");
	}
	Close(infile);
	flushin();
	return(err);
}

/*
 *		readheader()
 *		------------
 *		Reads in the first few lines (actually 480 bytes) of filename, and
 *		scans for a subject line. If the subject line is found, then
 *		it is stored in subject (up to 100 chars in length), else a null
 *		string is stored. The subject line is also examined, and a sequence
 *		number determined. If the subject line starts with i or I, followed
 *		by a number, then this is taken as the sequence number. Otherwise,
 *		the first number after `Part' or `part' is uses. This sequence
 *		number is returned in seqnum. I-type sequence numbers have 1000
 *		added on to them first of all, to keep them seperated from 'part'
 *		types.
 *
 *		The idea is that successive parts of a set of several shar files
 *		will have increasing sequence numbers.
 *
 *		Zero is returned if an error occurred.
 */
int readheader(filename, subject, seqnum)
char *filename, *subject;
int *seqnum;
{
	static char buf[480];
	BPTR file;
	int len, i;
	char *p;

	*subject = '\0';

	file = Open(filename, MODE_OLDFILE);
	if (!file) {
		print3("Can't open file ", filename, " for input\n");
		return (0);
	}

	len = Read(file, buf, 480);
	Close(file);
	if (len == -1) {
		print3("Error reading header from file ", filename, "\n");
		return (0);
	}

	/*
	 *		Now analyse file for a Subject: line
	 */
	for (i = 0; i < len; i++) {
		if (buf[i] == '\n' && strnicmp(&buf[i+1], "Subject:", 8) == 0) {
			/*
			 *		Copy subject line into subject string, ensuring
			 *		it is properly terminated with a \n and \0
			 */
			i++;
			strncpy(subject, buf + i + 9, 98);
			subject[98] = '\0';
			for (p = subject; *p; p++) {
				if (*p == '\n') {
					break;
				}
			}
			*p++ = '\n';
			*p = '\0';
			/*
			 *		Now scan new subject string looking for sequence number
			 */
			p = subject;
			while (*p) {
				if (*p == 'i') {
					*seqnum = atoi(p+1);
					if (*seqnum != 0)
						return 1;
				}
				if (strnicmp(p, "Part", 4) == 0) {
					p += 4;
					while (*p == ' ')
						p++;
					*seqnum = atoi(p);
					if (*seqnum != 0) {
						*seqnum += 1000;
						return 1;
					}
				} else
					p++;
			}
			*seqnum = 10000;
			return (1);
		}
	}
	*seqnum = 10000;
	return (1);
}

/*
 *		Start of mainline
 */
int main(argc,argv)
int argc;
char *argv[];
{

	int i, numfiles;
	int overwrite = NO, sortfiles = YES;
	char **filenames;

	if ((argc == 1) || (*argv[1] == '?')) {
		print(HelpMsg);
		return (10);
	}
	while (argc > 1 && *argv[1] == '-') {
		switch (tolower(argv[1][1])) {

			case 'o':
				overwrite = YES;
				break;

			case 'n':
				sortfiles = NO;
				break;

			default:
				print(HelpMsg);
				return (10);
		}
		argc--; argv++;
	}

	numfiles  = argc - 1;
	filenames = &argv[1];

	if (!sortfiles) {
		/*
		 *		Just process files in the order they occur
		 */
		for (i = 0; i < numfiles && !CtrlC; i++) {
			if (unshar(filenames[i], NULL, numfiles > 1, overwrite) != 0)
				break;
		}
	} else {
		/*
		 *		Do a prescan through all the files, and then unshar them
		 *		in the right order.
		 */	
		typedef struct SortRecord {
			struct  SortRecord *next;	/* Next record in list				*/
			int		index;				/* Index into filenames [] array	*/
			int		seqnum;				/* Sequence number of file			*/
			char	name[100];			/* Original subject line			*/
		} SORT;

		SORT *filehdrs, *head = NULL, *cur;

		filehdrs = AllocMem(sizeof(SORT) * numfiles, 0);
		if (!filehdrs) {
			print("Couldn't allocate memory to store file headers\n");
			goto endsort;
		}

		for (i = 0; i < numfiles && !CtrlC; i++) {
			int seqnum;
			SORT **ptr;

			if (!readheader(filenames[i], filehdrs[i].name, &seqnum))
				continue;	/* If couldn't read file, move to next file */

			/*
			 *		Now insert name at correct position in linked list
			 */
			for (ptr = &head; *ptr && (*ptr)->seqnum <= seqnum;
												ptr = &(*ptr)->next)
				;
			filehdrs[i].next = *ptr;
			*ptr = &filehdrs[i];

			filehdrs[i].seqnum = seqnum;
			filehdrs[i].index  = i;
		}
		/*
		 *		Now we have a sorted list of files, so just walk down
		 *		the list unsharing files as we go.
		 */
		for (cur = head; cur && !CtrlC; cur = cur->next) {
			if (unshar(filenames[cur->index], cur->name,
											numfiles > 1, overwrite) != 0)
				break;
		}
endsort:
		if (filehdrs)
			FreeMem(filehdrs, sizeof(SORT) * numfiles);
	}

	/*
	 *		All files handled, now just tidy up and exit. If CtrlC was
	 *		pressed, let the user know.
	 */
	if (CtrlC)
		print("^C\n");

	return 0;
}

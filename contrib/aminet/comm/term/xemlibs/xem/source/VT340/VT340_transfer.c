#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


       /* DCL-ASCII File-Transfer Funktionen zu VT340 Emulator.. */

/* Original source-code by Tuomo Mikkelson */
/* Adapted to work with xemvt340.library by Ueli Kaufmann */


#define SEOF        "\x1A"

#define TERMCHAR    '%'
#define TERMLEN     4
#define TERMSTRING  "%%%%"
#define TERMDEL     "\x7f\x7f\x7f\x7f"

#define DEVLEN      32
#define COMLEN      128
#define PATHLEN     128
#define PACKET      512
#define MAXLINE     512

#define BUFSIZE     1024
#define CONVSIZE    (1024+512)

#define PAUSE       (50 * 2)
#define ATTR        0


STATIC VOID flushserial(struct VT340Console *con)
{
	con->io->xem_sflush();
}


STATIC VOID startserial(struct VT340Console *con)
{
	con->io->xem_sstart();
}


STATIC BOOL stopserial(struct VT340Console *con)
{
	return((BOOL)(con->io->xem_sstop() == 0L));
}


STATIC ULONG readserial(struct VT340Console *con, BOOL linemode)
{
	return((ULONG)con->io->xem_sread(con->buf, BUFSIZE, 1 * 1000000));
}


STATIC VOID sendstring(struct VT340Console *con, UBYTE *string)
{
	con->io->xem_swrite(string, -1);
}


STATIC BOOL congets(struct VT340Console *con, UBYTE *prompt, UBYTE *cp, ULONG len)
{
	BOOL success;

	if(cp)
	{
		cp[0] = '\0';
		success = con->io->xem_tgets(prompt, cp, len);

		if(success == FALSE)
			cp[0] = '\0';
	}

	return(success);
}


STATIC VOID conprint(struct VT340Console *con, UBYTE *cp)
{
	VT340_writecon((APTR)con, cp, -1);
}


STATIC BOOL writeasync(struct VT340Console *con, ULONG len)
{
	con->io->xem_swrite(con->conv, len);
	return(FALSE);
}

STATIC VOID VT340_hostsetup(struct VT340Console *con);
STATIC VOID VT340_send(struct VT340Console *con);
STATIC VOID VT340_receive(struct VT340Console *con);

VOID VT340_transfer(struct VT340Console *con)
{
	BOOL quit = FALSE;
	UBYTE buf[4];

	if(stopserial(con))
		flushserial(con);
	else
	{
		conprint(con, "\r\nCannot Initialize Serial-Line.\r\n");
		return;
	}

	con->buf		= AllocMem(BUFSIZE, MEMF_PUBLIC | MEMF_CLEAR);
	con->conv	= AllocMem(CONVSIZE, MEMF_PUBLIC | MEMF_CLEAR);

	if(con->buf  &&  con->conv)
	{
		while(quit == FALSE)
		{
			if(congets(con, "S = send, R = receive, Q = quit, H = host setup", buf, 2))
			{
				switch(buf[0])
				{
					case 'r':
					case 'R':
						VT340_receive(con);
					break;

					case 's':
					case 'S':
						VT340_send(con);
					break;

					case 'q':
					case 'Q':
						quit = TRUE;
					break;

					case 'h':
					case 'H':
						VT340_hostsetup(con);
					break;

					default:
						quit = TRUE;
					break;

				}
			}
			else
				quit = TRUE;
		}
	}

	if(con->buf)
	{
		FreeMem(con->buf, BUFSIZE);
		con->buf = NULL;
	}

	if(con->conv)
	{
		FreeMem(con->conv, CONVSIZE);
		con->conv = NULL;
	}

	con->io->xem_tbeep(2, 12);
	startserial(con);
}


STATIC VOID VT340_hostsetup(struct VT340Console *con)
{
	BPTR fh;
	LONG len;
	auto UBYTE line[80];

	if(fh = Open("ENV:host.setup", MODE_OLDFILE))
	{
		while((len = Read(fh, line, 80)) > 0)
		{
			UBYTE i;

			for(i=0; i<len; i++)
			{
				if(line[i] == LF)
					line[i] = CR;
			}

			sendstring(con, line);
		}
		Close(fh);
	}

	if(len = readserial(con, 0))
		VT340_writecon((APTR)con, con->buf, len);
}


STATIC VOID VT340_send(struct VT340Console *con)
{
/*	struct FileInfoBlock fib;*/
	BPTR fh;
	UWORD cnt, lenght, termcount, error, stat;
	UBYTE *ccp, *cp;
	auto UBYTE dest[PATHLEN], filename[PATHLEN];
	auto UBYTE command[COMLEN], files[COMLEN];
	UBYTE device[] = "SYS$INPUT";
	UBYTE readsync[] = "SET TERMINAL/READSYNC/NOPASTHRU\x0D";
	UBYTE vms[] = "\x0D";
	UBYTE notopen[] = "Can't open file %s\r\n";
	UBYTE *fname;

	sendstring(con, readsync);
	stat = TRUE;
	ccp = readsync;

	while(stat)
	{
		cnt = readserial(con, 0);
		cp = con->buf;
		while(cnt)
		{
			if(*cp++ != *ccp++)
				ccp = readsync;

			if(!*ccp)
			{
				stat = FALSE;
				break;
			}

			cnt--;
		}
	}

	stat = TRUE;
	while(cnt)
	{
		if(*cp++ == LF)
		{
			stat = FALSE;
			break;
		}
		cnt--;
	}

	while(stat)
	{
		cnt = readserial(con, 0);
		cp = con->buf;
		while(cnt)
		{
			if(*cp++ == LF)
			{
				stat = FALSE;
				break;
			}
			cnt--;
		}
	}

	congets(con, "VMS destination directory:", dest, PATHLEN);
	for(;;)
	{
		congets(con, "AmigaDOS file expression: ", files, COMLEN);
		if(files[0] == '\0')
		{
			sendstring(con,vms);
			return;
		}

/*
*		error = dfind(&fib, files, ATTR);
*		cnt = strlen(files);
*		cp = files + cnt - 1;
*		while(cnt  &&  *cp != ':'  &&  *cp != '/')
*		{
*			cp--;
*			cnt--;
*		}
*
*		cp++;
*		*cp = '\0';
*/

		cnt = strlen(files);
		cp = files + cnt - 1;
		while(cnt  &&  *cp != ':'  &&  *cp != '/')
		{
			cp--;
			cnt--;
		}

		cp++;
		fname = cp;


		error = FALSE;
		while(!error)
		{
			strcpy(filename, files);
/*			strcat(filename, fib.fib_FileName);*/
			fh = Open(filename, MODE_OLDFILE);
			if(fh == NULL)
			{
/*				sprintf(command, notopen, fib.fib_FileName);*/
				sprintf(command, notopen, fname);
            conprint(con, command);
            sendstring(con, vms);
            return;
			}

			sprintf(command, "\r\nsending file: %s\r\n", fname);
			conprint(con, command);
			sprintf(command, "COPY %s %s%s\x0D", device, dest, fname);
			sendstring(con,command);

			ccp = command;
			while(stat)
			{
				cnt = readserial(con, 0);
				cp = con->buf;
				while(cnt)
				{
					if(*cp++ != *ccp++)
						ccp = command;

					if(!*ccp)
					{
						stat = FALSE;
						break;
					}
					cnt--;
				}
			}

			stat = TRUE;
			while(cnt)
			{
				if(*cp++ == LF)
				{
					stat = FALSE;
					break;
				}
				cnt--;
			}

			while(stat)
			{
				cnt = readserial(con, 0);
				cp = con->buf;
				while(cnt)
				{
					if(*cp++ == LF)
					{
						stat = FALSE;
						break;
					}
					cnt--;
				}
			}

			Delay(PAUSE);

			lenght = PACKET;
			while(lenght == PACKET)
			{
				lenght = Read(fh, con->conv, PACKET);
				cp = con->conv;
				cnt = lenght;
				while(cnt--)
				{
					if(*cp == LF)
						*cp = CR;
					cp++;
				}

				if(lenght)
				{
					if(writeasync(con,lenght))
						break;
				}

            while(readserial(con, 0))
					;
			}

			Close(fh);
			sendstring(con, SEOF);
			readserial(con, 0);
			sendstring(con, TERMSTRING);
			termcount = 0;
			while(termcount < TERMLEN)
			{  
				lenght = readserial(con, 0);
				cp = con->buf;
				while(lenght)
				{
					if(*cp++ == TERMCHAR)
						termcount++;
					else
						termcount = 0;
					lenght--;
				}
			}

			sendstring(con,TERMDEL);
			lenght = 0;
			while(lenght < TERMLEN)
			lenght += readserial(con, 0);

/*			error = dnext(&fib);*/
			error = TRUE;
		}
	}
}


STATIC VOID VT340_receive(struct VT340Console *con)
{
	BPTR fh;
	BOOL wild, stat;
	UWORD lenght, bufcount, termcount, len, tlen;
	UWORD saved, first;
	UBYTE *cp, *dcp, *icp, *name, *outbuf;
	auto UBYTE filename[COMLEN], line[MAXLINE];
	auto UBYTE files[COMLEN], command[COMLEN], dest[PATHLEN];
	UBYTE vms[] = "\x0D";
	UBYTE notopen[] = "Can't open file %s\r\n";

	outbuf = con->conv;
	congets(con, "AmigaDOS destination directory:", dest, PATHLEN);

	if(dest[0])
	{
		BPTR lock;

		if((lock = Lock(dest, ACCESS_READ)) == NULL)
			return;
		else
		{
			UnLock(lock);
			lock = NULL;
		}

		cp = dest + strlen(dest) - 1;
		if(*cp != ':'  &&  *cp != '/')
		{
			cp++;
			*cp++ = '/';
			*cp = '\0';              
		}
	}

	for(;;)
	{
		congets(con, "VMS file expression:", files, COMLEN);
		if(files[0] == '\0')
		{
			sendstring(con, vms);
			return;
		}

		if(strchr(files, '*') != NULL)
		{
			wild = TRUE;
			fh = NULL;
		}
		else
		{
			wild = FALSE;
			strcpy(filename, dest);
			lenght = strlen(files);
			cp = files + lenght - 1;
			while(lenght--)
			{
				if(*cp == ']'  ||  *cp == '>'  ||  *cp == ':')
					break;
				cp--;
			}
			cp++;
			strcat(filename,cp);
			fh = Open(filename, MODE_NEWFILE);
			if(fh == NULL)
			{
				sprintf(command, notopen, filename);
				conprint(con, command);
				sendstring(con, vms);
				return;
			}
		}

		sprintf(command, "type %s\x0D", files);
		sendstring(con,command);
		stat = TRUE;

		while(stat)
		{
			tlen = readserial(con, 0);
			cp = con->buf;
			while(tlen)
			{
				tlen--;
				if(*cp++ == LF)
				{
					stat = FALSE;
					break;
				}
			}
		}

		sendstring(con,TERMSTRING);
		if(!wild)
		{
			sprintf(command,"\r\nreceiving file: %s\r\n",filename);
			conprint(con, command);
		}

		termcount = 0;
		saved = 0;
		bufcount = 0;
		dcp = line;
		lenght = 0;
		first = 0;
		while(termcount < TERMLEN)
		{
			if(tlen)
			{
				icp = cp;
				len = tlen;
				tlen = 0;
			}
			else
			{
				len = readserial(con, 0);
				icp = con->buf;
			}

			while(len)
			{
				if(*icp == CR  ||  !*icp)
				{
					icp++;
					len--;
				}

				if(len)
				{
					if(*icp == TERMCHAR)
						termcount++;
					else
						termcount = 0;

					if(*icp == LF  ||  lenght == MAXLINE-2)
					{
						if(first)
						{
							first = 0;
							lenght = 0;
						}
						else
						{
							*dcp++ = *icp;
							lenght++;
						}
						*dcp = '\0';
						cp = line;

						if(wild  &&  *cp != '%')
						{
							while((cp = strchr(cp,':')) != NULL)
							{
								cp++;
								if(*cp == '['  ||  *cp == '<')
								{
									if((name = strchr(cp,']')) == NULL)
										name = strchr(cp,'>');

									if(name != NULL)
									{
										name++;
										if((cp = strchr(name,';')) != NULL)
										{
											*cp = '\0';
											if(fh != NULL)
											{
												Write(fh, outbuf, bufcount);
												bufcount = 0;
											}

											dcp = line;
											lenght = 0;
											saved = 0;
											first = 1;
											if(fh != NULL)
												Close(fh);

											strcpy(filename,dest);
											strcat(filename,name);
											fh = Open(filename, MODE_NEWFILE);
											if(fh == NULL)
											{
												sprintf(command, notopen, filename);
												conprint(con, command);
												sendstring(con, vms);
												return;
											}

											sprintf(command, "\r\nreceiving file: %s\r\n", name);
											conprint(con, command);
										}
									}
								}
							}
						}

						if(fh != NULL)
						{
							cp = line + lenght - 2;
							while(lenght > 1  &&  *cp <= ' '  &&  *cp >= '\0')
							{
								*cp = LF;
								cp--;
								lenght--;
							}

							if(lenght == 1)
							{
								lenght = 0;
								saved++;
							}

							dcp = outbuf + bufcount;
							if(saved == 2)
							{
								*dcp++ = LF;
								bufcount++;
								if(bufcount > CONVSIZE-2)
								{
									Write(fh, outbuf, bufcount);
									bufcount = 0;
									dcp = outbuf;
								}
								saved = 1;
							}

							if(lenght)
							{
								if(saved)
								{
									*dcp++ = LF;
									bufcount++;
									saved = 0;
								}

								cp = line;
								while(lenght)
								{
									*dcp++ = *cp++;
									bufcount++;
									if(bufcount > CONVSIZE-2)
									{
										Write(fh, outbuf, bufcount);
										bufcount = 0;
										dcp = outbuf;
									}
									lenght--;
								}
							}
						}

						dcp = line;
						lenght = 0;
					}
					else
					{
						*dcp++ = *icp;
						lenght++;
					}

					len--;
					icp++;
				}
			}
		}

		if(fh != NULL)
		{
			if(bufcount)
				Write(fh, outbuf, bufcount);
				Close(fh);
		}

		sendstring(con,TERMDEL);
		lenght = 0;
		while(lenght < TERMLEN)
			lenght += readserial(con, 0);
	}
}

/* end of source-code */

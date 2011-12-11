/*
**	ControlSequences.c
**
**	Routines that handle control sequences
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* CallMenu(STRPTR Name,ULONG Code):
	 *
	 *	Call a menu function either through the name of the corresponding
	 *	menu item or a menu number.
	 */

STATIC VOID
CallMenu(STRPTR Name,ULONG Code)
{
	LONG MenuNum = -1,Item = 0,Sub = 0,i;

		/* Are we to look for a name? */

	if(Name)
	{
		LONG Len = strlen(Name);

			/* Scan the menu list... */

		for(i = 0 ; TermMenu[i].nm_Type != NM_END ; i++)
		{
			switch(TermMenu[i].nm_Type)
			{
				case NM_TITLE:

					MenuNum++;
					Item = Sub = 0;
					break;

				case NM_ITEM:

					Sub = 0;
					break;
			}

				/* Did we get a valid name string? */

			if(TermMenu[i].nm_Label != NM_BARLABEL)
			{
					/* Does the name match our template? */

				if(!Strnicmp(TermMenu[i].nm_Label,Name,Len))
				{
					struct MenuItem *MenuItem = ItemAddress(Menu,FULLMENUNUM(MenuNum,Item,Sub));

					if(MenuItem && (MenuItem->Flags & ITEMENABLED))
						HandleMenuCode((IPTR)TermMenu[i].nm_UserData,0);

					break;
				}
			}

			switch(TermMenu[i].nm_Type)
			{
				case NM_ITEM:

					Item++;
					break;

				case NM_SUB:

					Sub++;
					break;
			}
		}
	}
	else
	{
		LONG	TheMenu	=  Code % 100,
				TheItem	= (Code / 100) % 100,
				TheSub	=  Code / 10000;

			/* Scan the menu list... */

		for(i = 0 ; TermMenu[i].nm_Type != NM_END ; i++)
		{
			switch(TermMenu[i].nm_Type)
			{
				case NM_TITLE:

					MenuNum++;
					Item = Sub = 0;
					break;

				case NM_ITEM:

					Sub = 0;
					break;
			}

				/* Is it the menu number we want? */

			if(TheMenu == MenuNum && TheItem == Item && TheSub == Sub)
			{
				if(TermMenu[i].nm_Label != NM_BARLABEL)
				{
					struct MenuItem *MenuItem = ItemAddress(Menu,FULLMENUNUM(MenuNum,Item,Sub));

					if(MenuItem && (MenuItem->Flags & ITEMENABLED))
						HandleMenuCode((IPTR)TermMenu[i].nm_UserData,0);
				}

				break;
			}

			switch(TermMenu[i].nm_Type)
			{
				case NM_ITEM:

					Item++;
					break;

				case NM_SUB:

					Sub++;
					break;
			}
		}
	}
}

	/* CommonControlProcessing(STRPTR String,BOOL LocalOnly):
	 *
	 *	Processes a control sequenced; effects are either limited
	 *	to the console window or will affect the serial interface
	 *	and the main program body.
	 */

STATIC VOID
CommonControlProcessing(CONST_STRPTR String,BOOL LocalOnly)
{
	UBYTE LocalBuffer[256];
	BOOL GotControl,GotEscape;
	SENDLINE SendLineLocal;
	LONG Count,i,Len;
	UBYTE c;

	if(LocalOnly)
		SendLineLocal = (SENDLINE)ConProcess;
	else
		SendLineLocal = (SENDLINE)SendLine;

	GotControl = GotEscape = FALSE;
	Count = 0;

		/* Scan the string. */

	Len = strlen(String);

	for(i = 0 ; i < Len ; i++)
	{
			/* We are looking for plain characters
			 * and the control ('\') and escape
			 * ('^') characters.
			 */

		if(!GotControl && !GotEscape)
		{
			switch(String[i])
			{
					/* Got a control character,
					 * the next byte will probably be
					 * a command sequence.
					 */

				case '\\':

					GotControl = TRUE;
					break;

					/* Got an escape character,
					 * the next byte will be some
					 * kind of control character
					 * (such as XON, XOF, bell, etc.).
					 */

				case '^':

					GotEscape = TRUE;
					break;

					/* This tells us to wait another
					 * second before continuing with
					 * the scanning.
					 */

				case '~':

					if(Count)
					{
						(*SendLineLocal)(LocalBuffer,Count);
						Count = 0;
					}

					DelayTime(0,MILLION / 2);

					if(!LocalOnly)
						RunJob(SerialJob);

					break;

					/* Stuff the character into the
					 * buffer.
					 */

				default:

					LocalBuffer[Count++] = String[i];
					break;
			}
		}
		else
		{
				/* Convert the character to a control
				 * style character (^C, etc.).
				 */

			if(GotEscape)
			{
				if(ToUpper(String[i]) >= 'A' && ToUpper(String[i]) <= '_')
					LocalBuffer[Count++] = ToUpper(String[i]) - '@';
				else
					LocalBuffer[Count++] = String[i];

				GotEscape = FALSE;
			}

				/* The next character represents a command. */

			if(GotControl)
			{
				switch(ToUpper(String[i]))
				{
						/* Fall back to default send mode. */

					case '0':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
							SendLineLocal = (SENDLINE)SendLine;

						break;

						/* Select `direct' send mode. */

					case '1':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
							SendLineLocal = (SENDLINE)SendLineSimple;

						break;

						/* Select `echo' send mode. */

					case '2':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
						{
							if(Config->ClipConfig->SendTimeout)
								SendLineLocal = (SENDLINE)SendLineEcho;
							else
								SendLineLocal = (SENDLINE)SendLineSimple;
						}

						break;

						/* Select `any echo' send mode. */

					case '3':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
						{
							if(Config->ClipConfig->SendTimeout)
								SendLineLocal = (SENDLINE)SendLineAnyEcho;
							else
								SendLineLocal = (SENDLINE)SendLineSimple;
						}

						break;

						/* Select `prompt' send mode. */

					case '4':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
						{
							if(Config->ClipConfig->SendTimeout)
								SendLineLocal = (SENDLINE)SendLinePrompt;
							else
								SendLineLocal = (SENDLINE)SendLineSimple;
						}

						break;

						/* Select `delay' send mode. */

					case '5':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
						{
							if(Config->ClipConfig->LineDelay || Config->ClipConfig->CharDelay)
								SendLineLocal = (SENDLINE)SendLineDelay;
							else
								SendLineLocal = (SENDLINE)SendLineSimple;
						}

						break;

						/* Select `keyboard' send mode. */

					case '6':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
							SendLineLocal = (SENDLINE)SendLineKeyDelay;

						break;

						/* Translate code. */

					case '*':

						i++;

						while(i < Len && String[i] == ' ')
							i++;

						if(i < Len)
						{
							UBYTE DummyBuffer[5];
							LONG j = 0,Char;

							if(String[i] >= '0' && String[i] <= '9')
							{
								while(j < 3 && i < Len)
								{
									Char = String[i++];

									if(Char >= '0' && Char <= '9')
										DummyBuffer[j++] = Char;
									else
									{
										i--;

										break;
									}
								}
							}
							else
							{
								while(j < 4 && i < Len)
								{
									Char = ToLower(String[i++]);

									if((Char >= '0' && Char <= '9') || (Char >= 'a' && Char <= 'z'))
										DummyBuffer[j++] = Char;
									else
									{
										i--;

										break;
									}
								}
							}

							DummyBuffer[j] = 0;

							LocalBuffer[Count++] = NameToCode(DummyBuffer);
						}

						i--;

						break;

						/* Execute an AmigaDOS command. */

					case 'D':

						if(!InRexx && !LocalOnly)
							LaunchCommandAsync(&String[i + 1]);

						return;

						/* Execute an ARexx command. */

					case 'A':

						if(!InRexx && !LocalOnly)
							LaunchRexxAsync(&String[i + 1]);

						return;

						/* Add the control character ('\'). */

					case '\\':

						LocalBuffer[Count++] = '\\';
						break;

						/* This is a backspace. */

					case 'B':

						LocalBuffer[Count++] = '\b';
						break;

						/* This is a form feed. */

					case 'F':

						LocalBuffer[Count++] = '\f';
						break;

						/* This is a line feed. */

					case 'N':

						if(String[i] == 'n' || Config->TerminalConfig->SendLF == EOL_LF)
							LocalBuffer[Count++] = '\n';
						else
						{
							STRPTR What = NULL;
							LONG j;

							switch(Config->TerminalConfig->SendLF)
							{
								case EOL_LFCR:

									What = "\n\r";
									break;

								case EOL_CRLF:

									What = "\r\n";
									break;

								case EOL_CR:

									What = "\r";
									break;
							}

							for(j = 0 ; What[j] ; j++)
							{
								LocalBuffer[Count++] = What[j];

									/* If the buffer is full, release it. */

								if(Count == sizeof(LocalBuffer))
								{
									(*SendLineLocal)(LocalBuffer,Count);

									Count = 0;
								}
							}
						}

						break;

						/* Send the current password. */

					case 'P':

						if(Password[0])
						{
							if(Count)
							{
								(*SendLineLocal)(LocalBuffer,Count);

								Count = 0;
							}

							(*SendLineLocal)(Password,strlen(Password));
						}

						break;

						/* This is a carriage return. */

					case 'R':

						if(String[i] == 'r' || Config->TerminalConfig->SendCR == EOL_CR)
							LocalBuffer[Count++] = '\r';
						else
						{
							STRPTR What = NULL;
							LONG j;

							switch(Config->TerminalConfig->SendCR)
							{
								case EOL_LFCR:

									What = "\n\r";
									break;

								case EOL_CRLF:

									What = "\r\n";
									break;

								case EOL_LF:

									What = "\n";
									break;
							}

							for(j = 0 ; What[j] ; j++)
							{
								LocalBuffer[Count++] = What[j];

									/* If the buffer is full, release it. */

								if(Count == sizeof(LocalBuffer))
								{
									(*SendLineLocal)(LocalBuffer,Count);

									Count = 0;
								}
							}
						}

						break;

						/* This is a tab. */

					case 'T':

						LocalBuffer[Count++] = '\t';
						break;

						/* Send the current user name. */

					case 'U':

						if(UserName[0])
						{
							if(Count)
							{
								(*SendLineLocal)(LocalBuffer,Count);

								Count = 0;
							}

							(*SendLineLocal)(UserName,strlen(UserName));
						}

						break;

						/* Send a break across the serial line. */

					case 'X':

						if(Count)
						{
							(*SendLineLocal)(LocalBuffer,Count);

							Count = 0;
						}

						if(!LocalOnly)
							SendBreak();

						break;

						/* Feed the contents of the
						 * clipboard into the input
						 * stream.
						 */

					case 'I':

						if(Count)
							(*SendLineLocal)(LocalBuffer,Count);

						Count = LoadClip(LocalBuffer,sizeof(LocalBuffer));

						break;

						/* Send a string to the clipboard. */

					case 'G':

						if(String[i + 1])
							SaveClip(&String[i + 1],strlen(&String[i + 1]));

						return;

						/* Send a string to the clipboard. */

					case 'H':

						if(String[i + 1])
							AddClip(&String[i + 1],strlen(&String[i + 1]));

						return;

						/* Produce the escape character. */

					case 'E':

						LocalBuffer[Count++] = ESC;
						break;

						/* Call a menu item. */

					case 'C':

						i++;

							/* Scan for a menu number or
							 * a single quote...
							 */

						while(i < Len)
						{
							if(String[i] >= '0' && String[i] <= '9')
								break;

							if(String[i] == '\'')
								break;

							if(String[i] != ' ')
								break;

							i++;
						}

						if(i < Len)
						{
							UBYTE DummyBuffer[sizeof(LocalBuffer)];

								/* Did we get a quote? */

							if(String[i] == '\'')
							{
								LONG Start = ++i;

								if(String[Start])
								{
									LONG Length;

									while(i < Len)
									{
										if(String[i] != '\'')
											i++;
										else
											break;
									}

									if(String[i] == '\'')
										Length = i - Start;
									else
										Length = i - Start + 1;

									CopyMem(&String[Start],DummyBuffer,Length);

									DummyBuffer[Length] = 0;

									CallMenu(DummyBuffer,0);
								}
							}
							else
							{
								if(String[i] >= '0' && String[i] <= '9')
								{
									LONG Start = i,Length;

									while(i < Len)
									{
										if(String[i] >= '0' && String[i] <= '9')
											i++;
										else
											break;
									}

									if(i == Start)
										Length = 1;
									else
										Length = i - Start;

									CopyMem(&String[Start],DummyBuffer,Length);

									DummyBuffer[Length] = 0;

									CallMenu(NULL,Atol(DummyBuffer));
								}
							}
						}

						break;

						/* Insert the current dialing mode suffix. */

					case 'W':

						switch(Config->ModemConfig->DialMode)
						{
							case DIALMODE_TONE:

								c = 'T';
								break;

							case DIALMODE_MODEM:

								c = 'M';
								break;

							case DIALMODE_ISDN:

								c = 'I';
								break;

							default:

								c = 'P';	/* Pulse */
								break;
						}

						LocalBuffer[Count++] = c;

						break;

						/* Stuff the character into the buffer. */

					default:

						LocalBuffer[Count++] = String[i];
						break;
				}

				GotControl = FALSE;
			}
		}

			/* If the buffer is full, release it. */

		if(Count == sizeof(LocalBuffer))
		{
			(*SendLineLocal)(LocalBuffer,Count);

			Count = 0;
		}
	}

	if(Count)
		(*SendLineLocal)(LocalBuffer,Count);
}

	/* SerialCommand(STRPTR String):
	 *
	 *	Send a command string to the serial line and
	 *	interprete the control sequences.
	 */

VOID
SerialCommand(CONST_STRPTR String)
{
	CommonControlProcessing(String,FALSE);
}

	/* ConsoleCommand(STRPTR String):
	 *
	 *	Just like SerialCommand(), but addresses the local
	 *	terminal side.
	 */

VOID
ConsoleCommand(CONST_STRPTR String)
{
	CommonControlProcessing(String,TRUE);
}

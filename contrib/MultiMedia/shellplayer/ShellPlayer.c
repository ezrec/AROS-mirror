#include <devices/ahi.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/ptplay.h>

struct UserArgs
{
	STRPTR file;
	LONG   *freq;
};

CONST TEXT Version[] = "$VER: ShellPlayer 1.0 (4.4.06)";

STATIC struct Library *PtPlayBase;
STATIC struct Task *maintask;
STATIC APTR modptr;
STATIC LONG frequency;
STATIC VOLATILE int player_done = 0;

STATIC VOID AbortAHI(struct MsgPort *port, struct IORequest *r1, struct IORequest *r2)
{
	if (!CheckIO(r1))
	{
		AbortIO(r1);
		WaitIO(r1);
	}

	if (!CheckIO(r2))
	{
		AbortIO(r2);
		WaitIO(r2);
	}

	GetMsg(port);
	GetMsg(port);
}

STATIC VOID StartAHI(struct AHIRequest *r1, struct AHIRequest *r2, WORD *buf1, WORD *buf2)
{
	PtRender(modptr, (BYTE *)(buf1), (BYTE *)(buf1+1), 4, frequency, 1, 16, 2);
	PtRender(modptr, (BYTE *)(buf2), (BYTE *)(buf2+1), 4, frequency, 1, 16, 2);

	r1->ahir_Std.io_Command = CMD_WRITE;
	r1->ahir_Std.io_Offset  = 0;
	r1->ahir_Std.io_Data    = buf1;
	r1->ahir_Std.io_Length  = frequency*2*2;
	r2->ahir_Std.io_Command = CMD_WRITE;
	r2->ahir_Std.io_Offset  = 0;
	r2->ahir_Std.io_Data    = buf2;
	r2->ahir_Std.io_Length  = frequency*2*2;

	r1->ahir_Link = NULL;
	r2->ahir_Link = r1;

	SendIO((struct IORequest *)r1);
	SendIO((struct IORequest *)r2);
}

STATIC VOID PlayerRoutine(void)
{
	struct AHIRequest req1, req2;
	struct MsgPort *port;
	WORD *buf1, *buf2;

	buf1 = AllocVec(frequency*2*2, MEMF_ANY);
	buf2 = AllocVec(frequency*2*2, MEMF_ANY);

	if (buf1 && buf2)
	{
		port = CreateMsgPort();

		if (port)
		{
			req1.ahir_Std.io_Message.mn_Node.ln_Pri = 0;
			req1.ahir_Std.io_Message.mn_ReplyPort = port;
			req1.ahir_Std.io_Message.mn_Length = sizeof(req1);
			req1.ahir_Version = 2;

			if (OpenDevice("ahi.device", 0, (struct IORequest *)&req1, 0) == 0)
			{
				req1.ahir_Type           = AHIST_S16S;
				req1.ahir_Frequency      = frequency;
				req1.ahir_Volume         = 0x10000;
				req1.ahir_Position       = 0x8000;

				CopyMem(&req1, &req2, sizeof(struct AHIRequest));

				StartAHI(&req1, &req2, buf1, buf2);

				for (;;)
				{
					struct AHIRequest *io;
					ULONG sigs;

					sigs = Wait(SIGBREAKF_CTRL_C | 1 << port->mp_SigBit);

					if (sigs & SIGBREAKF_CTRL_C)
						break;

					if ((io = (struct AHIRequest *)GetMsg(port)))
					{
						if (GetMsg(port))
						{
							// Both IO request finished, restart

							StartAHI(&req1, &req2, buf1, buf2);
						}
						else
						{
							APTR link;
							WORD *buf;

							if (io == &req1)
							{
								link = &req2;
								buf = buf1;
							}
							else
							{
								link = &req1;
								buf = buf2;
							}

							PtRender(modptr, (BYTE *)buf, (BYTE *)(buf+1), 4, frequency, 1, 16, 2);

							io->ahir_Std.io_Command = CMD_WRITE;
							io->ahir_Std.io_Offset  = 0;
							io->ahir_Std.io_Length  = frequency*2*2;
							io->ahir_Std.io_Data    = buf;
							io->ahir_Link = link;

							SendIO((struct IORequest *)io);
						}
					}
				}

				AbortAHI(port, (struct IORequest *)&req1, (struct IORequest *)&req2);
				CloseDevice((struct IORequest *)&req1);
			}

			DeleteMsgPort(port);
		}
	}

	FreeVec(buf1);
	FreeVec(buf2);

	Forbid();
	player_done = 1;
	Signal(maintask, SIGBREAKF_CTRL_C);
}

int main(void)
{
	struct RDArgs *args;
	struct UserArgs params;

	int rc = RETURN_FAIL;

	maintask = FindTask(NULL);

	args = ReadArgs("FILE/A,FREQ/K/N", (IPTR *)&params, NULL);

	if (args)
	{
		PtPlayBase = OpenLibrary("ptplay.library", 0);

		if (PtPlayBase)
		{
			BPTR fh;

			if (params.freq)
			{
				frequency = *params.freq;
			}

			if (frequency < 4000 || frequency > 96000)
				frequency = 48000;

			fh = Open(params.file, MODE_OLDFILE);

			if (fh)
			{
				struct FileInfoBlock fib;
				APTR buf;

				ExamineFH(fh, &fib);

				buf = AllocVec(fib.fib_Size, MEMF_ANY);

				if (buf)
				{
					Read(fh, buf, fib.fib_Size);
				}

				Close(fh);

				if (buf)
				{
					ULONG type;

					type = PtTest(params.file, buf, 1200);

					modptr = PtInit(buf, fib.fib_Size, frequency, type);

					if (modptr)
					{
						struct Process *player;

						player = CreateNewProcTags(
							NP_Entry, &PlayerRoutine,
							NP_Priority, 1,
							NP_Name, "Player Process",
							#ifdef __MORPHOS__
							NP_CodeType, CODETYPE_PPC,
							#endif
							TAG_DONE);

						if (player)
						{
							rc = RETURN_OK;
							Printf("Now playing \033[1m%s\033[22m at %ld Hz... Press CTRL-C to abort.\n", params.file, frequency);

							do
							{
								Wait(SIGBREAKF_CTRL_C);

								Forbid();
								if (!player_done)
								{
									Signal(&player->pr_Task, SIGBREAKF_CTRL_C);
								}
								Permit();
							}
							while (!player_done);
						}

						PtCleanup(modptr);
					}
					else
					{
						PutStr("Unknown file!\n");
					}
				}
				else
				{
					PutStr("Not enough memory!\n");
				}
			}
			else
			{
				PutStr("Could not open file!\n");
			}

			CloseLibrary(PtPlayBase);
		}

		FreeArgs(args);
	}

	if (rc == RETURN_FAIL)
		PrintFault(IoErr(), NULL);
	return rc;
}

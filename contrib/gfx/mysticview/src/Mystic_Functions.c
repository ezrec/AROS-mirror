
#include <stdio.h>

#ifdef __MORPHOS__
#include <public/proto/libamiga/amiga_protos.h>
#else
#include <clib/alib_protos.h>
#endif

#include <proto/intuition.h>
#include <proto/dos.h>
#ifndef NO_NEWICONS
#include <proto/newicon.h>
#endif
#include <proto/wb.h>
#include <proto/utility.h>

#include "Mystic_Global.h"
#include "Mystic_Keyfile.h"
#include "Mystic_Icons.h"
#include "Mystic_Window.h"
#include "Mystic_Tools.h"
#include "Mystic_InitExit.h"
#include "Mystic_FileHandling.h"
#include "Mystic_Subtask.h"
#include "MysticView.h"
#include "Mystic_Texts.h"
#include "Mystic_Settings.h"

#include "Mystic_HandlerWrapper.h"



//--------------------------------------------------------------------------------
//
//		MVFunction_CreateThumbnail(mvwindow, settings, viewdata)
//
//--------------------------------------------------------------------------------

BOOL MVFunction_CreateThumbnail(struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata)
{
	BOOL done = FALSE;

	WindowBusy(win);

	if (viewdata && settings)
	{
		if (viewdata->showpic)
		{
			if (NewIconBase)
			{
				char *fullname;
				if ((fullname = FullName(viewdata->pathname, viewdata->filename)))
				{
					struct NewDiskObject *dob = NULL;

					done = PutNewIcon(viewdata->showpic,
						settings->thumbsize, settings->thumbsize, settings->thumbnumcolors,
						fullname, dob, settings->defaulttool);

					if (dob)
					{
					#ifndef NO_NEWICONS
						FreeNewDiskObject(dob);
					#endif
					}

					Free(fullname);
				}
			}
			else
			{
				EasyRequest(win->window, &newiconreq, NULL, NULL);
			}
		}
		else
		{
			EasyRequest(win->window, &nopicreq, NULL, NULL);
		}
	}

	WindowFree(win);

	return done;
}



//--------------------------------------------------------------------------------
//
//		MVFunction_About(mview)
//
//--------------------------------------------------------------------------------

LONG SAVEDS aboutfunc(struct subtask *subtask, BYTE abortsignal)
{
	struct mview *mv;
	char *name;
	struct Window *win = NULL;

	mv = ObtainData(subtask);
	LOCKREAD(mv);

	if (mv->window)
	{
		win = mv->window->window;
	}

	if ((name = GetKeyLicensee()))
	{
		EasyRequest(win, &aboutregisteredreq, NULL, (IPTR)name);
		Free(name);
	}
	else
	{
		EasyRequest(win, &aboutreq, NULL, NULL);
	}

	ReleaseData(subtask);
	UNLOCK(mv);

	return TRUE;
}

void MVFunction_About(struct mview *mv)
{
	if (mv)
	{
		if (mv->abouttask)
		{
			if (SubTaskReady(mv->abouttask))
			{
				CloseSubTask(mv->abouttask);
				mv->abouttask = NULL;
			}
		}

		if (!mv->abouttask)
		{
			LOCK(mv);
			mv->abouttask = SubTask(aboutfunc, (APTR) mv, ABOUTSTACK, globalpriority, "MysticView AboutTask (%x)", NULL, FALSE);
			UNLOCK(mv);
		}
	}
}


//--------------------------------------------------------------------------------
//
//		success = MVFunction_UsePreset(mv, *settings, viewdata, slot, immediately)
//
//--------------------------------------------------------------------------------

BOOL MVFunction_UsePreset(struct mview *mv, struct mainsettings **mainsettings,
	struct pathsettings *settings,
	struct viewdata *viewdata, int slot, BOOL immediately)
{
	char *fullname = NULL;
	BOOL newsettings = FALSE;

	if (mv && settings && mainsettings && viewdata)
	{
		if (immediately)
		{
			fullname = _StrDup(settings->preset[slot]);
		}

		if (!fullname)
		{
			char *pathname, *filename = NULL;
			char *newpathname, *newfilename;

			if (settings->preset[slot])
			{
				pathname = DupPath(settings->preset[slot]);
				filename = _StrDup(FilePart(settings->preset[slot]));
			}
			else
			{
				pathname = _StrDup(settings->presetpath);
			}

			if (FileRequest(mv->presetfreq, mv->window->window, MVTEXT_SELECTPRESET,
				pathname, filename, &newpathname, &newfilename))
			{
				if (newpathname)
				{
					if (newfilename)
					{
						fullname = FullName(newpathname, newfilename);

						Free(settings->preset[slot]);
						settings->preset[slot] = _StrDup(fullname);
					}
				}
				Free(newpathname);
				Free(newfilename);
			}

			Free(pathname);
			Free(filename);
		}

		if (fullname)
		{
			struct mainsettings *new;
			if ((new = LoadPreset(fullname, *mainsettings)))
			{
				DeleteMainSettings(*mainsettings);
				*mainsettings = new;
				newsettings = TRUE;
			}
			Free(fullname);
		}
	}

	return newsettings;
}


//--------------------------------------------------------------------------------
//
//		MVFunction_CopyMoveToPath(mv, settings, pathsettings, viewdata, slot, action, immediately, buttons)
//
//--------------------------------------------------------------------------------

void MVFunction_CopyMoveToPath(struct mview *mv, struct mainsettings *settings,
	struct pathsettings *pathsettings,
	struct viewdata *viewdata, int slot, int action, BOOL immediately, struct MVButtons *buttons)
{
	char *fullsourcename, *fulldestname = NULL;

	if (mv && settings && viewdata)
	{
		if (viewdata->showpic)
		{
			if ((fullsourcename = FullName(viewdata->pathname, viewdata->filename)))
			{
				if (immediately)
				{
					fulldestname = FullName(pathsettings->destpath[slot], viewdata->filename);
				}

				if (!fulldestname)
				{
					char *newpathname, *newfilename;

					if (SaveRequest(mv->destfreq, mv->window->window, MVTEXT_SELECTDESTINATION, pathsettings->destpath[slot], viewdata->filename, &newpathname, &newfilename))
					{
						if (newpathname)
						{
							Free(pathsettings->destpath[slot]);
							pathsettings->destpath[slot] = _StrDup(newpathname);
							fulldestname = FullName(newpathname, newfilename);
						}
						Free(newpathname);
						Free(newfilename);
					}
				}

				if (fulldestname)
				{
					LOCK(mv);
					if (PerformFileAction(mv, fullsourcename, fulldestname, action, settings->confirmlevel))
					{
						if (action == FILEACTION_MOVE)
						{
							if (RenamePicture(viewdata->pichandler, fulldestname))
							{
								Free(viewdata->filename);
								Free(viewdata->pathname);
								viewdata->filename = _StrDup(FilePart(fulldestname));
								viewdata->pathname = DupPath(fulldestname);
								SetViewText(viewdata, SetPicInfoText(viewdata, viewdata->showpic, settings));
							}

						//	if (RemovePicture(viewdata->pichandler))
						//	{
						//		DeleteViewPic(viewdata, NULL, settings);
						//		GetCurrentPicture(mv, buttons, viewdata, settings, mv->logopic);
						//	}
						}
					}
					UNLOCK(mv);
					Free(fulldestname);
				}

				Free(fullsourcename);
			}
		}
		else
		{
			EasyRequest(mv->window->window, &nopicreq, NULL, NULL);
		}
	}
}



//--------------------------------------------------------------------------------
//
//		MVFunction_CopyMoveTo(mv, settings, viewdata, slot, action, immediately, struct MVButtons *buttons)
//
//--------------------------------------------------------------------------------

void MVFunction_CopyMoveTo(struct mview *mv, struct mainsettings *settings,
	struct pathsettings *pathsettings,
	struct viewdata *viewdata, int action, BOOL immediately, struct MVButtons *buttons)
{
	char *fullsourcename, *fulldestname = NULL;

	if (mv && settings && viewdata)
	{
		if (viewdata->showpic)
		{
			if ((fullsourcename = FullName(viewdata->pathname, viewdata->filename)))
			{
				if (immediately)
				{
					fulldestname = FullName(pathsettings->copypath, viewdata->filename);
				}

				if (!fulldestname)
				{
					char *newpathname, *newfilename;

					if (SaveRequest(mv->copyfreq, mv->window->window, MVTEXT_SELECTDESTINATION, pathsettings->copypath, viewdata->filename, &newpathname, &newfilename))
					{
						if (newpathname)
						{
							Free(pathsettings->copypath);
							pathsettings->copypath = _StrDup(newpathname);
							fulldestname = FullName(newpathname, newfilename);
						}
						Free(newpathname);
						Free(newfilename);
					}
				}

				if (fulldestname)
				{
					if (PerformFileAction(mv, fullsourcename, fulldestname, action, settings->confirmlevel))
					{
						if (action == FILEACTION_MOVE)
						{
							if (RenamePicture(viewdata->pichandler, fulldestname))
							{
								Free(viewdata->filename);
								Free(viewdata->pathname);
								viewdata->filename = _StrDup(FilePart(fulldestname));
								viewdata->pathname = DupPath(fulldestname);
								SetViewText(viewdata, SetPicInfoText(viewdata, viewdata->showpic, settings));
							}

						//	if (RemovePicture(viewdata->pichandler))
						//	{
						//	 	DeleteViewPic(viewdata, NULL, settings);
						//		GetCurrentPicture(mv, buttons, viewdata, settings, mv->logopic);
						//	}
						}
					}
					Free(fulldestname);
				}

				Free(fullsourcename);
			}
		}
		else
		{
			EasyRequest(mv->window->window, &nopicreq, NULL, NULL);
		}
	}
}


//--------------------------------------------------------------------------------
//
//		MVFunction_SetHideMode(mv, settings)
//
//--------------------------------------------------------------------------------

void MVFunction_SetHideMode(struct mview *mv, struct mainsettings *settings, PICTURE *picture)
{
	if (mv && settings)
	{
		switch (settings->iconifymode)
		{
			case ICONIFY_APPICON:
			{
				if (mv->appitem) RemoveAppMenuItem(mv->appitem); mv->appitem = NULL;
				if (mv->appicon) RemoveAppIcon(mv->appicon); mv->appicon = NULL;

				if (!mv->appicon)
				{
					struct DiskObject *dob = mv->iconifyicon ? mv->iconifyicon : mv->diskobject;

					if (dob)
					{
						mv->appicon = AddAppIcon(1, 0, PROGNAME, mv->appmsgport, NULL, dob, NULL);
					}
				}
				break;
			}

			case ICONIFY_APPITEM:
				if (mv->appitem) RemoveAppMenuItem(mv->appitem); mv->appitem = NULL;
				if (mv->appicon) RemoveAppIcon(mv->appicon); mv->appicon = NULL;
				mv->appitem = AddAppMenuItem(1, 0, PROGNAME, mv->appmsgport, NULL);
				break;

			default:
			case ICONIFY_INVISIBLE:
				if (mv->appitem) RemoveAppMenuItem(mv->appitem); mv->appitem = NULL;
				if (mv->appicon) RemoveAppIcon(mv->appicon); mv->appicon = NULL;
				break;
		}
	}
}



//--------------------------------------------------------------------------------
//
//		MVFunction_CreateAutoThumbnail(settings, filename, pic)
//
//--------------------------------------------------------------------------------

BOOL MVFunction_CreateAutoThumbnail(struct mvwindow *window, struct mainsettings *settings, struct viewdata *viewdata)
{
	BOOL done = FALSE;

	if (settings && viewdata && window)
	{
	//	if (settings->thumbnailmode != THUMBNAIL_OFF)
	//	{

		if (settings->createthumbnails)
		{
			if (viewdata->showpic)
			{
				if (NewIconBase)
				{
					char *fullname;

					if ((fullname = FullName(viewdata->pathname, viewdata->filename)))
					{
						char checkstring[200];
						char **tt;
						BOOL createthumbnail = TRUE;
						
					#ifdef __AROS__
						sprintf(checkstring, "%d,%d", settings->thumbsize, settings->thumbnumcolors);
					#else
						sprintf(checkstring, "%ld,%ld", settings->thumbsize, settings->thumbnumcolors);
					#endif

						if ((tt = GetToolTypes(fullname)))
						{
							//	icon found

							char *s;

							if ((s = ArgString((UBYTE **)tt, "MYSTICVIEW_THUMBNAIL", NULL)))
							{
								// mysticview thumbnail recognized

								createthumbnail = FALSE;

							// JA	if (settings->thumbnailmode == THUMBNAIL_UPDATE)
								{
									//	check if thumbnail size changed

									if (Stricmp(s, checkstring))
									{
										createthumbnail = TRUE;			// it changed
									}
									else
									{
										if ((s = ArgString((UBYTE **)tt, "MYSTICVIEW_THUMBDATE", NULL)))
										{
											char strDate[30], strTime[30];

											if (sscanf(s, "%29s %29s", strDate, strTime) == 2)
											{
												struct DateStamp *filedate;

												if ((filedate = GetFileDate(fullname)))
												{
													struct DateTime thumbdate;
													thumbdate.dat_Format = FORMAT_CDN;
													thumbdate.dat_Flags = 0;
													thumbdate.dat_StrDate = strDate;
													thumbdate.dat_StrTime = strTime;
													thumbdate.dat_StrDay = NULL;
													if (StrToDate(&thumbdate))
													{
														// file neuer als thumbdate?

														if (CompareDates(filedate, &thumbdate.dat_Stamp) < 0)
														{
															createthumbnail = TRUE;
														}
													}

													Free(filedate);
												}
											}
										}
										else
										{
											createthumbnail = TRUE;
										}
									}
								}
							}
							DeleteStringList(tt);
						}

						if (createthumbnail)
						{
						//	printf("creating thumbnail for %s...\n", fullname);

							if (MVFunction_CreateThumbnail(window, settings, viewdata))
							{
								if ((tt = GetToolTypes(fullname)))
								{
									struct List *tlist;

									if ((tlist = CreateListFromArray(tt)))
									{
										char **newtt;
										char strDate[30], strTime[30];
										struct DateTime thumbdate;

										thumbdate.dat_Format = FORMAT_CDN;
										thumbdate.dat_Flags = 0;
										thumbdate.dat_StrDate = strDate;
										thumbdate.dat_StrTime = strTime;
										thumbdate.dat_StrDay = NULL;
										DateStamp(&thumbdate.dat_Stamp);

										if (DateToStr(&thumbdate))
										{
											char timestr[60];
											sprintf(timestr, "%s %s", strDate, strTime);
											SetPrefsNode(tlist, "MYSTICVIEW_THUMBDATE", timestr, TRUE);
										}

										SetPrefsNode(tlist, "MYSTICVIEW_THUMBNAIL", checkstring, TRUE);

										if ((newtt = CreateArrayFromList(tlist)))
										{
											PutToolTypes(fullname, newtt);
											DeleteStringList(newtt);
											done = TRUE;
										}
										DeleteList(tlist);
									}

									DeleteStringList(tt);
								}
							}
						}

						Free(fullname);
					}
				}
				else
				{
					EasyRequest(window->window, &newiconreq, NULL, NULL);
				}
			}
		}
	}

	return done;
}

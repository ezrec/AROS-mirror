/*********************************************************************
----------------------------------------------------------------------

	guigfx_drawhandle

----------------------------------------------------------------------
*********************************************************************/

#include <math.h>
#ifdef MATH_68882
	#include <m68881.h>
#endif

#include <render/render.h>
#include <libraries/cybergraphics.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
//#include <proto/mathtrans.h>

#include <guigfx/guigfx.h>

#include "guigfx_global.h"


#include "guigfx_yuvtab.c"

/*--------------------------------------------------------------------

	ReleasePenTab(pentab,colormap,numcolors)

	Gibt eine Pentab frei.

--------------------------------------------------------------------*/

void ReleasePenTab(UBYTE *pentab, struct ColorMap *cm, UWORD numcolors)
{
	int i;
	
	for(i=0; i<numcolors; ++i)
	{
		ReleasePen(cm,(ULONG)pentab[i]);
	}
}


/*--------------------------------------------------------------------

	success = ObtainPenTab(palette,colormap,pentab,numcolors,precision)

	Fordert eine Palette an und hinterlegt die Pens in einer Pentab.

--------------------------------------------------------------------*/

BOOL ObtainPenTab (	PALETTE palette, struct ColorMap *cm, UBYTE *pentab,
					UWORD numcolors, ULONG precision )
{
	int i;
	LONG pen;
	ULONG rgb[3];
	UWORD count = 0;

	for(i=0; i < numcolors; ++i)
	{
		ExportPalette(palette,rgb,	RND_PaletteFormat,PALFMT_RGB32,
									RND_FirstColor,i,
									RND_NumColors,1,
									TAG_DONE);

		pen = ObtainBestPen(cm,rgb[0],rgb[1],rgb[2],OBP_Precision,precision,TAG_DONE);

		if (pen == -1)
		{
			ReleasePenTab(pentab,cm,count);
			return FALSE;
		}
		else
		{
			pentab[i] = (UBYTE) pen;
			count++;
		}
	}

	return TRUE;
}


/*********************************************************************
----------------------------------------------------------------------

	ReleaseDrawHandle(drawhandle)

	gibt die allozierten Farben eines DrawHandles frei
	und löscht das DrawHandle.

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM ReleaseDrawHandle(REG(a0) DRAWHANDLE *dh)
{
	if (dh)
	{
		if(dh->mapengine)
		{
			DeleteMapEngine(dh->mapengine);
		}
	
		if(dh->realpalette)
		{
			DeletePalette(dh->realpalette);
		}
	
		if(dh->mainnumcolors)
		{
			ReleasePenTab(dh->mainpentab, dh->colormap, dh->mainnumcolors);
		}	
	
		if(dh->mainpalette)
		{
			DeletePalette(dh->mainpalette);
		}
	
		if(dh->rasthandle)
		{
			DeleteRastHandle(dh->rasthandle);
		}
	
		FreeRenderVec(dh);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	drawhandle = ObtainDrawHandleA(rastport, colormap, psm, tags);

	Fordert die Farben einer PenShareMap von einer colormap an
	und erzeugt dafür ein drawhandle für einen rastport.

	OBP_Precision
	GGFX_DitherMode				bestimmt Dither-Modus
	GGFX_DitherAmount
	GGFX_DitherThreshold		Schwellwert für Autodither
	GGFX_AutoDither				default: TRUE
	GGFX_MaxAllocPens			default: keine Beschränkung
	GGFX_ModeID
	GGFX_BGColor				definierte hintergrundfarbe

----------------------------------------------------------------------
*********************************************************************/

DRAWHANDLE SAVE_DS ASM *ObtainDrawHandleA(
	REG(a0) PSM *psm,
	REG(a1) struct RastPort *rp,
	REG(a2) struct ColorMap *cm,
	REG(a3) TAGLIST tags)
{
	DRAWHANDLE *dh;


	if((dh = AllocRenderVecClear(MemHandler, sizeof(DRAWHANDLE))))
	{
		BOOL success = FALSE;

		dh->dithermode =
				GetTagData(GGFX_DitherMode, DEFAULT_DITHERMODE, tags);

		dh->bgcolor =
				GetTagData(GGFX_BGColor, 0xffffffff, tags);

		dh->autodither =
				GetTagData(GGFX_AutoDither, DEFAULT_AUTODITHER, tags);

		dh->ditherthreshold =
				GetTagData(GGFX_DitherThreshold, env_autoditherthreshold, tags);

		dh->ditheramount =
				GetTagData(GGFX_DitherAmount, DEFAULT_DITHERAMOUNT, tags);

		dh->modeID =
				GetTagData(GGFX_ModeID, INVALID_ID, tags);


		DB(kprintf("Creating drawhandle with threshold %ld\n", dh->ditherthreshold));

		if ((dh->rasthandle = CreateRastHandle(rp, dh->modeID)))
		{
			success = TRUE;

			if (!dh->rasthandle->truecolor)
			{
				//	wenn keine Pensharemap übergeben wurde, einen
				//	9bit-Farbraum erzeugen
			
				if (!(dh->psm = psm))
				{
					success = FALSE;

					if ((psm = CreatePenShareMapA(NULL)))
					{
							struct TagItem tags[2] = {{GGFX_PixelFormat, PIXFMT_0RGB_32}, {TAG_DONE}};
							success = !!AddPixelArrayA(psm, yuvtab, 6*6*6, 1, tags);
					
					#if 0
						ULONG *colortable;
						if (colortable = AllocRenderVec(MemHandler, sizeof(ULONG) * 6*6*6))
						{
							int y,u,v;
							DOUBLE Y, U, V;
							ULONG *p = colortable;
							struct TagItem tags[2] = {{GGFX_PixelFormat, PIXFMT_0RGB_32}, {TAG_DONE}};
							
							for (y = 0; y < 6; ++y)
							{
								for (u = 0; u < 6; ++u)
								{
									for (v = 0; v < 6; ++v)
									{
											Y = (255 * y / 5);
											U = (222 * u / 5) - 111;
											V = (312 * v / 5) - 156;
											
											*p++ =
													((int)(Y + 1.140 * V) << 16) +
													((int)(Y - 0.396 * U - 0.581 * V) << 8) +
													(int)(Y + 2.029 * U);
									}
								}
							}
							success = !!AddPixelArrayA(psm, colortable, 6*6*6, 1, tags);
							FreeRenderVec(colortable);
						}
					#endif
					
					}
				}


				if (success)
				{
					ObtainSemaphore(&psm->semaphore);

					if (psm->modified)
					{
						if (psm->histogram)
						{
							DeleteHistogram(psm->histogram);
							psm->histogram = NULL;
						}
					}
			
					if (!psm->histogram)
					{
						success = FALSE;
			
						if((psm->histogram = CreateHistogram(RND_RMHandler, (IPTR)MemHandler, 
							RND_HSType, psm->hstype, TAG_DONE)))
						{
							if (!IsListEmpty(&psm->colorlist))
							{
								struct Node *node = (struct Node *) psm->colorlist.lh_Head;
								struct Node *nextnode;
								
								COLORHANDLE *ch;
								DOUBLE minf = 10000, f;
								success = TRUE;
				
								/* Gewichtungsfaktor für Addhistogram berechnen */
							
								while ((nextnode = node->ln_Succ))
								{
									ch = (COLORHANDLE *)node;
									f = (DOUBLE) ch->weight / (DOUBLE) ch->numpixels;
									if (f < minf)
									{
										minf = f;
									}
									node = nextnode;
								}
								node = (struct Node *) psm->colorlist.lh_Head;
							
								while ((nextnode = node->ln_Succ) && success)
								{
									ch = (COLORHANDLE *)node;
									if (ch->histogram)
		
									success = (AddHistogram(psm->histogram, ch->histogram->histogram, 
										RND_Weight, (ULONG) ((DOUBLE) ch->weight / ((DOUBLE) ch->numpixels * (DOUBLE) minf)),
										TAG_DONE) == ADDH_SUCCESS);
		
									node = nextnode;
								}
							}
						}
			
						if (success)
						{
							psm->modified = FALSE;
						}
					}
			
			
					if (success)
					{
						ULONG maxcolors;
						ULONG maxallocpens;
						ULONG histoColors;
						UWORD firstcolor;
	
						success = FALSE;
	
						switch (dh->rasthandle->colormode)
						{
							case COLORMODE_HAM6:
								maxcolors = 15;
								firstcolor = 1;
								break;

							case COLORMODE_HAM8:
								maxcolors = 63;
								firstcolor = 1;
								break;

							default:
								if ((dh->dithermode != DITHERMODE_NONE) /*&& (dh->autodither)*/)
								{
									maxcolors = 254;
									firstcolor = 2;
								}
								else
								{
									maxcolors = 256;
									firstcolor = 0;
								}
			/*
								if (dh->bgcolor != 0xffffffff)
								{
									maxcolors--;
									firstcolor++;
								}
			*/
								break;
						}
						
				
						dh->colormap = cm;
		
						dh->precision = GetTagData(OBP_Precision, PRECISION_IMAGE, tags);
		
						maxallocpens = GetTagData(GGFX_MaxAllocPens, maxcolors, tags);
						
						if((histoColors = QueryHistogram(psm->histogram, RND_NumColors)) > 0)
						{
							LONG mainColors;
							struct PaletteExtra *pe;

							mainColors = MIN(histoColors, maxallocpens);
					
							if (!dh->rasthandle->truecolor)
							{
								DB(kprintf("ObtainDrawHandle: bitmap is not truecolor.\n"));
		
								if ((pe = cm->PalExtra))
								{
									UWORD numsharable;
									ObtainSemaphoreShared(&pe->pe_Semaphore);
									numsharable = pe->pe_NFree;
									ReleaseSemaphore(&pe->pe_Semaphore);
									if (numsharable > 1)
									{
										mainColors = MIN(numsharable*3, mainColors);
									}
								}
							}
			
							if((dh->mainpalette = CreatePalette(RND_RMHandler, (IPTR)MemHandler,
									RND_HSType, psm->hstype, TAG_DONE)))
							{		
								DB(kprintf("ObtainDrawHandle: extracting palette.\n"));
			
								if (ExtractPalette(psm->histogram, dh->mainpalette,
									mainColors, RND_ColorMode, dh->rasthandle->colormode,
									RND_FirstColor, firstcolor,	
									RND_RGBWeight, DEFAULT_RGBWEIGHT, 
									TAG_DONE) == EXTP_SUCCESS)
								{
									ULONG rgb[3];
	
									if (!dh->rasthandle->truecolor)
									{
										int insertpos = 0;
										switch (dh->rasthandle->colormode)
										{
											case COLORMODE_CLUT:
											{
												if (dh->dithermode != DITHERMODE_NONE)
												{
													//	schwarz und weiß einfügen für dithering
												
													ULONG rgb[2] = {0x00000000, 0x00ffffff};
													DB(kprintf("ObtainDrawHandle: inserting black & white for dithering.\n"));
													ImportPalette(dh->mainpalette, rgb, 2,
														RND_NewPalette, FALSE, TAG_DONE);
													mainColors += 2;
													insertpos += 2;
												}

							/*
												if (dh->bgcolor != 0xffffffff)
												{
													//	definierte hintergrundfarbe einfügen
												
													DB(kprintf("ObtainDrawHandle: inserting background color.\n"));
													ImportPalette(dh->mainpalette, &dh->bgcolor, 1,
														RND_FirstColor, insertpos, RND_NewPalette, FALSE, TAG_DONE);
													mainColors ++;
												}
							*/

												SortPalette(dh->mainpalette, 
												//	PALMODE_SATURATION,
												//	PALMODE_POPULARITY,	
														PALMODE_SIGNIFICANCE,
														RND_Histogram, (IPTR)psm->histogram, TAG_DONE);

												break;
											}

											case COLORMODE_HAM8:
												GetRGB32(cm, 0, 1, rgb);
												ImportPalette(dh->mainpalette, rgb, 1,
														RND_PaletteFormat, PALFMT_RGB32,
														RND_NewPalette, FALSE, TAG_DONE);
												break;
	
											case COLORMODE_HAM6:
											{
												UWORD rgb;
												rgb = (UWORD) GetRGB4(cm, 0);
												ImportPalette(dh->mainpalette, &rgb, 1,
														RND_PaletteFormat, PALFMT_RGB4,
														RND_NewPalette, FALSE, TAG_DONE);
												break;
											}
										}
									}
			
									if(ObtainPenTab(dh->mainpalette, cm,
										dh->mainpentab, mainColors, dh->precision))
									{
		
										DB(kprintf("ObtainDrawHandle: Pass 2 - %ld pens allocated.\n", mainColors));
		
										if(dh->realpalette)
										{
											DeletePalette(dh->realpalette);
										}
							
										if((dh->realpalette = CreatePalette(RND_HSType, psm->hstype,
												RND_RMHandler, (IPTR)MemHandler, TAG_DONE)))
										{
											int i;
	
											switch (dh->rasthandle->colormode)
											{
												case COLORMODE_HAM6:
												{
													UWORD rgb;
													for(i=0; i<mainColors; ++i)
													{
														rgb = (UWORD) GetRGB4(cm, dh->mainpentab[i]);
														ImportPalette(dh->realpalette, &rgb, 1,
															RND_PaletteFormat, PALFMT_RGB4,
																RND_NewPalette, FALSE,
																RND_FirstColor, i,
																TAG_DONE);
													}
													break;
												}												
												default:
													for(i=0; i<mainColors; ++i)
													{
														GetRGB32(cm, dh->mainpentab[i], 1, rgb);
														ImportPalette(dh->realpalette, rgb, 1,
															RND_PaletteFormat, PALFMT_RGB32,
																RND_NewPalette, FALSE,
																RND_FirstColor, i,
																TAG_DONE);
													}
													break;
											}
											
											dh->mainnumcolors = mainColors;

											InitSemaphore(&dh->mapsemaphore);

											if (!dh->psm && psm)
											{
												if ((!dh->rasthandle->truecolor) && (psm->hstype == HSTYPE_12BIT_TURBO))
												{
													/*dh->mapengine = CreateMapEngine(dh->realpalette,
															RND_Histogram, psm->histogram,
															RND_RMHandler, (IPTR)MemHandler, TAG_DONE)*/;
													dh->mapengine = CreateMapEngine(dh->realpalette,
															RND_RMHandler, (IPTR)MemHandler, TAG_DONE);
												}
											}
		
											if (dh->bgcolor != 0xffffffff)
											{
												dh->bgpen = dh->mainpentab[BestPen(dh->mainpalette, dh->bgcolor)];
												
											//		 ObtainBestPen(cm, RED_RGB32(dh->bgcolor),
											//			GREEN_RGB32(dh->bgcolor), BLUE_RGB32(dh->bgcolor),
											//			OBP_Precision, dh->precision, TAG_DONE);
											}
					/*					else
											{
												dh->bgpen = ObtainBestPen(cm, 0,0,0,
														OBP_Precision, PRECISION_GUI, TAG_DONE);
												dh->bgcolor = 0x000000;
											}
					*/
											success = TRUE;
										}
									}
								}				
							}
						}
					}

					ReleaseSemaphore(&psm->semaphore);
				}

				if (!dh->psm && psm)
				{
					//	es wurde keine PenShareMap übergeben, interne Pensharemap löschen
				
					DeletePenShareMap(psm);
				}

			}
			#ifndef NDEBUG
			else
			{
				DB(kprintf("Bitmap is truecolor. No pens allocated.\n"));
			}
			#endif
		}
		
		if (success == FALSE)
		{
			ReleaseDrawHandle(dh);
			dh = NULL;
		}


	}
		
	return dh;
}


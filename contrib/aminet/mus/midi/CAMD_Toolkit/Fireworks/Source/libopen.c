
#include "fireworks.h"
#include "fireworks_protos.h"

/*
extern BOOL __stdargs __fpinit(void);
extern void __stdargs __fpterm(void);
*/


/* Library bases */

#if !defined(__AROS__)
struct ExecBase			*SysBase;
#endif
struct DosLibrary		*DOSBase;
struct GfxBase			*GfxBase;
struct Library			*LayersBase;
struct IntuitionBase	*IntuitionBase;
struct Library 			*GadToolsBase;
struct UtilityBase		*UtilityBase;
struct Library			*DatatypesBase;
struct Library			*AslBase;
struct Library			*WorkbenchBase;
struct Library			*CamdBase;
struct Library			*TimerBase;
struct Library *DataTypesBase;
BOOL					 FPBase;	/* pseudo floating point base */




/*----------------*/
/* Open Libraries */
/*----------------*/

BOOL OpenLibs(void)
{
	BOOL Success=FALSE;
	
	if ((DOSBase=(struct DosLibrary*)OpenLibrary("dos.library",39L)))
	{
		if ((GfxBase=(struct GfxBase*)OpenLibrary("graphics.library",39L)))
		{
			if ((LayersBase=OpenLibrary("layers.library",39L)))
			{
				if ((IntuitionBase=(struct IntuitionBase*)OpenLibrary("intuition.library",39L)))
				{
					if ((GadToolsBase=OpenLibrary("gadtools.library", 39L)))
					{
						if ((UtilityBase=(struct UtilityBase *)OpenLibrary("utility.library",39L)))
						{
							if (!(DatatypesBase=OpenLibrary("datatypes.library",39L)))
							{
								Message("This program requires datatypes.library!",NULL);
							}
							else
							{
								if (!(AslBase=OpenLibrary("asl.library",39L)))
								{
									Message("This program requires asl.library!",NULL);
								}
								else
								{
									if (!(WorkbenchBase=OpenLibrary("workbench.library",39L)))
									{
										Message("This program requires workbench.library!",NULL);
									}
									else
									{
										if (!(CamdBase=OpenLibrary("camd.library",0L)))
										{
											Message("This program requires camd.library!",NULL);
										}
										else
										{
										  if(!(DataTypesBase=OpenLibrary("datatypes.library",0L)))
										    {
										      Message("This program requires datatypes.library!",NULL);
										    }else{
										      //if ( __fpinit() )
											//{
											//Message("Unable to initialize floating point!",NULL);
											//}
										      //else
											//{
											FPBase=TRUE;
											
											Success=TRUE;
											
											//}
										    }
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (!Success) CloseLibs();
	
	return(Success);
}


/*-----------------*/
/* Close Libraries */
/*-----------------*/

void CloseLibs(void)
{
  if(DataTypesBase){
    CloseLibrary(DataTypesBase);
    DataTypesBase=NULL;
  }

	if (FPBase)
	{
//		__fpterm();
		FPBase=FALSE;
	}
	
	if (CamdBase)
	{
		CloseLibrary(CamdBase);
		CamdBase=NULL;
	}
	
	if (WorkbenchBase)
	{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase=NULL;
	}
	
	if (AslBase)
	{
		CloseLibrary(AslBase);
		AslBase=NULL;
	}
	
	if (DatatypesBase)
	{
		CloseLibrary(DatatypesBase);
		DatatypesBase=NULL;
	}
	
	if (UtilityBase)
	{
		CloseLibrary((struct Library *)UtilityBase);
		UtilityBase=NULL;
	}
	
	if (GadToolsBase)
	{
		CloseLibrary(GadToolsBase);
		GadToolsBase=NULL;
	}
	
	if (IntuitionBase)
	{
		CloseLibrary((struct Library*)IntuitionBase);
		IntuitionBase=NULL;
	}
	
	if (LayersBase)
	{
		CloseLibrary((struct Library*)LayersBase);
		LayersBase=NULL;
	}
	
	if (GfxBase)
	{
		CloseLibrary((struct Library*)GfxBase);
		GfxBase=NULL;
	}
	
	if (DOSBase)
	{
		CloseLibrary((struct Library*)DOSBase);
		DOSBase=NULL;
	}
}

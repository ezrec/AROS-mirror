/* Graffiti.h
   $VER 1.0
   11.11.1996

 */

/* Graffiti-Screen structure */

struct GRF_Screen
 {
  long *GRF_ScreenID;        //ScreenID

  char *GRF_Plane1,*GRF_Plane2,*GRF_Plane3,*GRF_Plane4;

                             /*Pointer to the 4 bitplanes.
                               Please note, that at the first 10 lines
                               of each plane is a part of the graffiti-
                               command list, which should NOT(!) be
                               manipulated in any way. Better don`t try
                               direct access to the planes anyway.
                             */

  long *GRF_Viewport;        //Viewport

  long *GRF_MUCopList;       //Custom-Copperlist

  long GRF_ChipSet;          /* 0=OCS
                                1=ECS
                                2=AGA
                              */

  long GRF_OneLine;          /* length of a bitplane-line. Either
                                80 bytes (mode 0 and 1) or 160 bytes
                                (mode 2, aga only)
                              */

  long GRF_ScreenSize;       /* Size of the visible chunky-screen in bytes
                              */
  long GRF_NrOfPlanes;
                             /* Nr of used bitplanes to diplay the chunky-
                              data.
                              */

  long GRF_Resolution;       /* Resolution of the used amiga-screen.
                                Either 0 (hires) or 1 (shires)
                              */
  };



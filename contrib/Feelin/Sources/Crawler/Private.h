/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define       GfxBase               FeelinBase -> Graphics

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {

        FM_Crawler_Crawl

        };

enum    {

        FA_Crawler_Delay,
        FA_Crawler_Micros

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FRender                        *OwnRender;
    struct FeelinSignalHandler      sh;
    struct RastPort                 OwnRPort;
    uint8                           delay;
    uint8                           saved;
    int16                           pos;
    uint16                          bmpw;
    uint16                          bmph;
//  preferences
    STRPTR                          p_Delay;
    STRPTR                          p_Micros;
};

#define _node_x                                 node -> AreaPublic -> Box.x
#define _node_y                                 node -> AreaPublic -> Box.y
#define _node_w                                 node -> AreaPublic -> Box.w
#define _node_h                                 node -> AreaPublic -> Box.h
#define _node_minw                              node -> AreaPublic -> MinMax.MinW
#define _node_minh                              node -> AreaPublic -> MinMax.MinH
#define _node_maxw                              node -> AreaPublic -> MinMax.MaxW
#define _node_maxh                              node -> AreaPublic -> MinMax.MaxH

#define FV_DEF_DELAY                            20
#define FV_DEF_MICROS                           40000


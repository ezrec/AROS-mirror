/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <aros/debug.h>
#include <exec/types.h>
#include <utility/tagitem.h>
#include <libraries/mui.h>
#include <zune/customclasses.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#include "locale.h"
#include "emulator.h"
#include "drivelisteditor.h"

STRPTR pageLabels[7];

static CONST_STRPTR MEM_Z3[]     = {NULL, "1 MiB", "2 MiB", "4 MiB", "8 MiB", "16 MiB", "32 MiB", "64 MiB", "128 MiB", "256 MiB", "512 MiB", NULL};
static CONST_STRPTR CPU_Type[]   = {"68000", "68010", "68020", "68020+68881", "68040", NULL};
static CONST_STRPTR MEM_Chip[]   = {"256 kiB", "512 kiB", "1 MiB", "2 MiB", "4 MiB", "8 MiB", NULL};
static CONST_STRPTR MEM_Fast[]   = {NULL, "1 MiB", "2 MiB", "4 MiB", "8 MiB", NULL};
static CONST_STRPTR MEM_Slow[]   = {NULL, "512 kiB", "1 MiB", "1.5 MiB", NULL};
static CONST_STRPTR GFX_Memory[] = {NULL, "1 MiB", "2 MiB", "4 MiB", "8 MiB", "16 MiB", "32 MiB", NULL};
static CONST_STRPTR SND_Mode[5];
static CONST_STRPTR CPU_Speed[4];
static CONST_STRPTR GFX_Chipset[5];
static CONST_STRPTR SND_Channels[4];
static CONST_STRPTR PRT_Gameport[7];
static CONST_STRPTR SND_Resolution[3];
static CONST_STRPTR GFX_SpriteCollisions[5];

/*** Private methods ********************************************************/
#define MUIM_ConfigurationEditor_AddFloppyDrive (TAG_USER | 0x13000000)
#define MUIM_ConfigurationEditor_RemFloppyDrive (TAG_USER | 0x13000001)
#define MUIM_ConfigurationEditor_UpdateCPU      (TAG_USER | 0x13000002)
#define MUIM_ConfigurationEditor_UpdateMemory   (TAG_USER | 0x13000003)
#define MUIM_ConfigurationEditor_UpdateSound    (TAG_USER | 0x13000004)

/*** Instance data **********************************************************/
struct ConfigurationEditor_DATA
{
    Object *ced_CPU_Type,
           *ced_CPU_Speed,
           *ced_CPU_Ratio,
           *ced_CPU_24bit,
           *ced_CPU_Compatible;
    
    Object *ced_MEM_Kickstart,
           *ced_MEM_KickstartKey,
           *ced_MEM_Chip,
           *ced_MEM_Fast,
           *ced_MEM_Slow,
           *ced_MEM_Z3;
           
    Object *ced_GFX_Chipset,
           *ced_GFX_SpriteCollisions,
           *ced_GFX_Memory,
           *ced_GFX_CopperSpeedup,
           *ced_GFX_ImmediateBlits,
           *ced_GFX_Framerate;
           
    Object *ced_SND_Mode,
           *ced_SND_Channels,
           *ced_SND_Resolution;
    
    ULONG   ced_DRV_FloppyCount;
    Object *ced_DRV_FloppyRootGroup,
           *ced_DRV_Floppies[4],
           *ced_DRV_FloppyLabels[4],
           *ced_DRV_FloppyPopasls[4],
           *ced_DRV_FloppyMore,
           *ced_DRV_FloppyLess,
           *ced_DRV_ListEditor;
    
    Object *ced_PRT_Gameport0,
           *ced_PRT_Gameport1;
};

/*** Methods ****************************************************************/
IPTR ConfigurationEditor__OM_NEW
(
    Class *CLASS, Object *self, struct opSet *message
)
{
    struct ConfigurationEditor_DATA *data = NULL;
    int i;
    
    Object *cpu_Type, 
           *cpu_Speed, 
           *cpu_Ratio, 
           *cpu_24bit, 
           *cpu_Compatible;
    Object *mem_Kickstart, 
           *mem_KickstartKey, 
           *mem_Chip, 
           *mem_Fast, 
           *mem_Slow, 
           *mem_Z3;
    Object *gfx_Chipset,
           *gfx_SpriteCollisions,
           *gfx_Memory,
           *gfx_CopperSpeedup,
           *gfx_ImmediateBlits,
           *gfx_Framerate;
    Object *snd_Mode,
           *snd_Channels,
           *snd_Resolution;
    Object *drv_FloppyRootGroup,
           *drv_Floppies[4],
           *drv_FloppyLabels[4],
           *drv_FloppyPopasls[4],
           *drv_FloppyMore,
           *drv_FloppyLess,
           *drv_ListEditor;
    Object *prt_Gameport0,
           *prt_Gameport1;
    
    /*== Localize strings ==================================================*/
    pageLabels[0]  = _(MSG_CFG_CPU);
    pageLabels[1]  = _(MSG_CFG_MEMORY);
    pageLabels[2]  = _(MSG_CFG_GRAPHICS);
    pageLabels[3]  = _(MSG_CFG_SOUND);
    pageLabels[4]  = _(MSG_CFG_DRIVES);
    pageLabels[5]  = _(MSG_CFG_PORTS);
    pageLabels[6]  = NULL;
    
    CPU_Speed[0]   = _(MSG_CFG_CPU_SPEED_FAST);
    CPU_Speed[1]   = _(MSG_CFG_CPU_SPEED_COMPATIBLE);
    CPU_Speed[2]   = _(MSG_CFG_CPU_SPEED_ADJUSTABLE);
    CPU_Speed[3]   = NULL;
    
    MEM_Fast[0]    = _(MSG_CFG_MEM_NONE);
    MEM_Slow[0]    = _(MSG_CFG_MEM_NONE);
    MEM_Z3[0]      = _(MSG_CFG_MEM_NONE);
    
    GFX_Memory[0]  = _(MSG_CFG_MEM_NONE);
    GFX_Chipset[0] = _(MSG_CFG_GFX_CHIPSET_OCS);
    GFX_Chipset[1] = _(MSG_CFG_GFX_CHIPSET_ECS_AGNUS);
    GFX_Chipset[2] = _(MSG_CFG_GFX_CHIPSET_ECS_FULL);
    GFX_Chipset[3] = _(MSG_CFG_GFX_CHIPSET_AGA);
    GFX_Chipset[4] = NULL;

    GFX_SpriteCollisions[0] = _(MSG_CFG_GFX_SC_NONE);
    GFX_SpriteCollisions[1] = _(MSG_CFG_GFX_SC_SPRITES);
    GFX_SpriteCollisions[2] = _(MSG_CFG_GFX_SC_SPRITES_AND_PLAYFIELDS);
    GFX_SpriteCollisions[3] = _(MSG_CFG_GFX_SC_FULL);
    GFX_SpriteCollisions[4] = NULL;
        
    SND_Mode[0] = _(MSG_CFG_SND_MODE_NONE);
    SND_Mode[1] = _(MSG_CFG_SND_MODE_MUTED);
    SND_Mode[2] = _(MSG_CFG_SND_MODE_NORMAL);
    SND_Mode[3] = _(MSG_CFG_SND_MODE_ACCURATE);
    SND_Mode[4] = NULL;
    
    SND_Channels[0] = _(MSG_CFG_SND_CHANNELS_MONO);
    SND_Channels[1] = _(MSG_CFG_SND_CHANNELS_STEREO);
    SND_Channels[2] = _(MSG_CFG_SND_CHANNELS_MIXED);
    SND_Channels[3] = NULL;
    
    SND_Resolution[0] = _(MSG_CFG_SND_RESOLUTION_8BIT);
    SND_Resolution[1] = _(MSG_CFG_SND_RESOLUTION_16BIT);
    SND_Resolution[2] = NULL;
    
    PRT_Gameport[0] = _(MSG_CFG_PRT_GAMEPORT_JOYSTICK0);
    PRT_Gameport[1] = _(MSG_CFG_PRT_GAMEPORT_JOYSTICK1);
    PRT_Gameport[2] = _(MSG_CFG_PRT_GAMEPORT_MOUSE);
    PRT_Gameport[3] = _(MSG_CFG_PRT_GAMEPORT_NUMERIC);
    PRT_Gameport[4] = _(MSG_CFG_PRT_GAMEPORT_KEYS0);
    PRT_Gameport[5] = _(MSG_CFG_PRT_GAMEPORT_KEYS1);
    PRT_Gameport[6] = NULL;
    
    /*== Create objects and layot ==========================================*/
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        InnerSpacing(4, 4),
        MUIA_Group_Horiz, TRUE,
        
        Child, (IPTR) RegisterGroup(pageLabels),
            Child, (IPTR) VGroup,
                /*-- CPU ---------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_CPU_TYPE)),
                    Child, (IPTR) cpu_Type  = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) CPU_Type),
                    Child, (IPTR) Label(_(MSG_CFG_CPU_SPEED)),
                    Child, (IPTR) cpu_Speed = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) CPU_Speed),
                    Child, (IPTR) Label(_(MSG_CFG_CPU_RATIO)),
                    Child, (IPTR) cpu_Ratio = SliderObject,
                        MUIA_Weight,         0,
                        MUIA_Numeric_Min,    1,
                        MUIA_Numeric_Max,    5119,
                        MUIA_Numeric_Format, __(MSG_CFG_CPU_RATIO_FORMAT),
                    End,
                    Child, (IPTR) RectangleObject,
                        MUIA_Weight, 0,
                    End,
                    Child, (IPTR) ColGroup(3),
                        Child, (IPTR) cpu_24bit = MUI_MakeObject(MUIO_Checkmark, NULL),
                        Child, (IPTR) LLabel2(_(MSG_CFG_CPU_24BIT)),
                        Child, (IPTR) HVSpace,
                        Child, (IPTR) cpu_Compatible = MUI_MakeObject(MUIO_Checkmark, NULL),
                        Child, (IPTR) LLabel2(_(MSG_CFG_CPU_SLOW_COMPATIBLE)),
                        Child, (IPTR) HVSpace,
                    End,
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Memory ------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    GroupFrameT(_(MSG_CFG_MEM_ROM)),
                    
                    Child, (IPTR) Label2(_(MSG_CFG_MEM_ROM_KICKSTART)),
                    Child, (IPTR) PopaslObject,
                        MUIA_Popstring_String, (IPTR) mem_Kickstart = StringObject,
                            StringFrame,
                        End,
                        MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                    End,
                    Child, (IPTR) Label2(_(MSG_CFG_MEM_ROM_KEY)),
                    Child, (IPTR) PopaslObject,
                        MUIA_Popstring_String, (IPTR) mem_KickstartKey = StringObject,
                            StringFrame,
                        End,
                        MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                    End,
                End,
                Child, (IPTR) ColGroup(2),
                    GroupFrameT(_(MSG_CFG_MEM_RAM)),
                    
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_CHIP)),
                    Child, (IPTR) mem_Chip = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Chip),
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_SLOW)),
                    Child, (IPTR) mem_Slow = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Slow),
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_FAST)),
                    Child, (IPTR) mem_Fast = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Fast),
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_Z3)),
                    Child, (IPTR) mem_Z3 = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Z3),
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Graphics ----------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_GFX_CHIPSET)),
                    Child, (IPTR) gfx_Chipset = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) GFX_Chipset),
                    Child, (IPTR) Label(_(MSG_CFG_GFX_SPRITECOLLISIONS)),
                    Child, (IPTR) gfx_SpriteCollisions = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) GFX_SpriteCollisions),
                    Child, (IPTR) RectangleObject,
                        MUIA_Weight, 0,
                    End,
                    Child, (IPTR) ColGroup(3),
                        Child, (IPTR) gfx_CopperSpeedup = MUI_MakeObject(MUIO_Checkmark, NULL),
                        Child, (IPTR) LLabel(_(MSG_CFG_GFX_COPPER_SPEEDUP)),
                        Child, (IPTR) HVSpace,
                        Child, (IPTR) gfx_ImmediateBlits = MUI_MakeObject(MUIO_Checkmark, NULL),
                        Child, (IPTR) LLabel(_(MSG_CFG_GFX_IMMEDIATE_BLITS)),
                        Child, (IPTR) HVSpace,
                    End,
                    Child, (IPTR) Label(_(MSG_CFG_GFX_FRAMERATE)),
                    Child, (IPTR) gfx_Framerate = SliderObject,
                        MUIA_Weight,                0,
                        MUIA_Numeric_Min,           1,
                        MUIA_Numeric_Max,           20,
                        MUIA_Numeric_Format, (IPTR) "1/%ld",
                    End,
                    Child, (IPTR) Label(_(MSG_CFG_GFX_P96RAM)),
                    Child, (IPTR) gfx_Memory = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) GFX_Memory),
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Sound -------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_SND_MODE)),
                    Child, (IPTR) snd_Mode = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) SND_Mode),
                    Child, (IPTR) Label(_(MSG_CFG_SND_CHANNELS)),
                    Child, (IPTR) snd_Channels = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) SND_Channels),
                    Child, (IPTR) Label(_(MSG_CFG_SND_RESOLUTION)),
                    Child, (IPTR) snd_Resolution = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) SND_Resolution),
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Drives ------------------------------------------------*/
                Child, (IPTR) HGroup,
                    GroupFrameT(_(MSG_CFG_DRV_FLOPPY)),
                    
                    Child, (IPTR) drv_FloppyRootGroup = ColGroup(2),
                        Child, (IPTR) drv_FloppyLabels[0] = Label("DF0:"),
                        Child, (IPTR) drv_FloppyPopasls[0] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) drv_Floppies[0] = StringObject,
                                StringFrame,
                            End,
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End,
                        Child, (IPTR) drv_FloppyLabels[1] = Label("DF1:"),
                        Child, (IPTR) drv_FloppyPopasls[1] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) drv_Floppies[1] = StringObject,
                                StringFrame,
                            End,
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End,
                        Child, (IPTR) drv_FloppyLabels[2] = Label("DF2:"),
                        Child, (IPTR) drv_FloppyPopasls[2] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) drv_Floppies[2] = StringObject,
                                StringFrame,
                            End,
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End,
                        Child, (IPTR) drv_FloppyLabels[3] = Label("DF3:"),
                        Child, (IPTR) drv_FloppyPopasls[3] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) drv_Floppies[3] = StringObject,
                                StringFrame,
                            End,
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End,
                    End,
                    Child, (IPTR) VGroup,
		        MUIA_Weight, 0,
                        Child, (IPTR) drv_FloppyMore = SimpleButton(_(MSG_CFG_DRV_FLOPPY_MORE)),
                        Child, (IPTR) drv_FloppyLess = SimpleButton(_(MSG_CFG_DRV_FLOPPY_LESS)),
			Child, (IPTR) VSpace(),
                    End,
                End,
                
                Child, (IPTR) VGroup,
                    GroupFrameT(_(MSG_CFG_DRV_HARD)),
                    
                    Child, (IPTR) drv_ListEditor = DriveListEditorObject,
                    End,
                End,
            End,
            Child, (IPTR) VGroup,
                /*-- Ports -------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_PRT_GAMEPORT1)),
                    Child, (IPTR) prt_Gameport0 = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) PRT_Gameport),
                    Child, (IPTR) Label(_(MSG_CFG_PRT_GAMEPORT2)),
                    Child, (IPTR) prt_Gameport1 = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) PRT_Gameport),
                End,
                Child, (IPTR) HVSpace,
            End,
        End,
        
        TAG_DONE
    );
    
    if (self != NULL)
    {
        data = INST_DATA(CLASS, self);
        
        /*-- Store important variables -------------------------------------*/
        data->ced_CPU_Type             = cpu_Type;
        data->ced_CPU_Speed            = cpu_Speed;
        data->ced_CPU_Ratio            = cpu_Ratio;
        data->ced_CPU_24bit            = cpu_24bit;
        data->ced_CPU_Compatible       = cpu_Compatible;
    
        data->ced_MEM_Kickstart        = mem_Kickstart;
        data->ced_MEM_KickstartKey     = mem_KickstartKey;
        data->ced_MEM_Chip             = mem_Chip;
        data->ced_MEM_Fast             = mem_Fast;
        data->ced_MEM_Slow             = mem_Slow;
        data->ced_MEM_Z3               = mem_Z3;
        
        data->ced_GFX_Chipset          = gfx_Chipset;
        data->ced_GFX_SpriteCollisions = gfx_SpriteCollisions;
        data->ced_GFX_Memory           = gfx_Memory;
        data->ced_GFX_CopperSpeedup    = gfx_CopperSpeedup;
        data->ced_GFX_ImmediateBlits   = gfx_ImmediateBlits;
        data->ced_GFX_Framerate        = gfx_Framerate;
           
        data->ced_SND_Mode             = snd_Mode;
        data->ced_SND_Channels         = snd_Channels;
        data->ced_SND_Resolution       = snd_Resolution;
    
        data->ced_DRV_FloppyCount      = 4;
        data->ced_DRV_FloppyRootGroup  = drv_FloppyRootGroup;
        data->ced_DRV_FloppyMore       = drv_FloppyMore;
        data->ced_DRV_FloppyLess       = drv_FloppyLess;
        data->ced_DRV_ListEditor       = drv_ListEditor;
        
        for (i = 0; i < 4; i++)
        {
            data->ced_DRV_Floppies[i]      = drv_Floppies[i];
            data->ced_DRV_FloppyLabels[i]  = drv_FloppyLabels[i];
            data->ced_DRV_FloppyPopasls[i] = drv_FloppyPopasls[i];
        }
        
        data->ced_PRT_Gameport0        = prt_Gameport0;
        data->ced_PRT_Gameport1        = prt_Gameport1;
        
        /*-- Setup notifications -------------------------------------------*/
        DoMethod
        (
            drv_FloppyMore, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_ConfigurationEditor_AddFloppyDrive
        );
        DoMethod
        (
            drv_FloppyLess, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_ConfigurationEditor_RemFloppyDrive
        );
        
        DoMethod
        (
            cpu_Type, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_ConfigurationEditor_UpdateCPU
        );
        DoMethod
        (
            cpu_Speed, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_ConfigurationEditor_UpdateCPU
        );
        
        DoMethod
        (
            mem_Chip, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_ConfigurationEditor_UpdateMemory
        );
        
        DoMethod
        (
            snd_Mode, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (IPTR) self, 1, MUIM_ConfigurationEditor_UpdateSound
        );
        
        /*-- Sensible defaults ---------------------------------------------*/
        SET(mem_Chip, MUIA_Cycle_Active, 3);
        SET(cpu_Type, MUIA_Cycle_Active, 3);
        SET(cpu_Speed, MUIA_Cycle_Active, 0);
        SET(gfx_Chipset, MUIA_Cycle_Active, 2);
        SET(gfx_SpriteCollisions, MUIA_Cycle_Active, 1);
        SET(gfx_CopperSpeedup, MUIA_Selected, TRUE);
        SET(gfx_ImmediateBlits, MUIA_Selected, TRUE);
        SET(snd_Mode, MUIA_Cycle_Active, 0);
        SET(prt_Gameport0, MUIA_Cycle_Active, 2);
        
        /*-- Miscellanous initialization -----------------------------------*/
        SET(drv_FloppyMore, MUIA_Disabled, TRUE);        
    }
    
    return (IPTR) self;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_AddFloppyDrive
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);
    
    if (data->ced_DRV_FloppyCount >= 0 && data->ced_DRV_FloppyCount <= 3)
    {
        DoMethod(data->ced_DRV_FloppyRootGroup, MUIM_Group_InitChange);
        
        DoMethod
        (
            data->ced_DRV_FloppyRootGroup, OM_ADDMEMBER,
            (IPTR) data->ced_DRV_FloppyLabels[data->ced_DRV_FloppyCount]
        );
        DoMethod
        (
            data->ced_DRV_FloppyRootGroup, OM_ADDMEMBER,
            (IPTR) data->ced_DRV_FloppyPopasls[data->ced_DRV_FloppyCount]
        );
              
        DoMethod(data->ced_DRV_FloppyRootGroup, MUIM_Group_ExitChange);
        
        data->ced_DRV_FloppyCount++;
        if (data->ced_DRV_FloppyCount >= 4)
        {
            SET(data->ced_DRV_FloppyMore, MUIA_Disabled, TRUE);
        }
        SET(data->ced_DRV_FloppyLess, MUIA_Disabled, FALSE);
    }
    
    return NULL;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_RemFloppyDrive
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);
    
    if (data->ced_DRV_FloppyCount >= 2 && data->ced_DRV_FloppyCount <= 4)
    {
        DoMethod(data->ced_DRV_FloppyRootGroup, MUIM_Group_InitChange);

        DoMethod
        (
            data->ced_DRV_FloppyRootGroup, OM_REMMEMBER,
            (IPTR) data->ced_DRV_FloppyPopasls[data->ced_DRV_FloppyCount - 1]
        );
        DoMethod
        (
            data->ced_DRV_FloppyRootGroup, OM_REMMEMBER,
            (IPTR) data->ced_DRV_FloppyLabels[data->ced_DRV_FloppyCount - 1]
        );
        
        DoMethod(data->ced_DRV_FloppyRootGroup, MUIM_Group_ExitChange);
        
        data->ced_DRV_FloppyCount--;
        if (data->ced_DRV_FloppyCount <= 1)
        {
            SET(data->ced_DRV_FloppyLess, MUIA_Disabled, TRUE);
        }
        SET(data->ced_DRV_FloppyMore, MUIA_Disabled, FALSE);
    }
    
    return NULL;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_UpdateCPU
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);
    IPTR type = XGET(data->ced_CPU_Type, MUIA_Cycle_Active);
    
    SET
    (
        data->ced_CPU_Compatible, MUIA_Disabled,
        type >= 1 ? TRUE : FALSE
    );
    
    SET
    (
        data->ced_CPU_24bit, MUIA_Disabled,
        type == 2 || type == 3 ? FALSE : TRUE
    );
    
    SET
    (
        data->ced_CPU_Ratio, MUIA_Disabled,
        XGET(data->ced_CPU_Speed, MUIA_Cycle_Active) != 2 ? TRUE : FALSE
    );
    
    return NULL;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_UpdateMemory
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);
    
    SET
    (
        data->ced_MEM_Fast, MUIA_Disabled, 
        XGET(data->ced_MEM_Chip, MUIA_Cycle_Active) >= 4 ? TRUE : FALSE
    );
    
    return NULL;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_UpdateSound
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);
    
    if (XGET(data->ced_SND_Mode, MUIA_Cycle_Active) == 0)
    {
        SET(data->ced_SND_Channels, MUIA_Disabled, TRUE);
        SET(data->ced_SND_Resolution, MUIA_Disabled, TRUE);
    }
    else
    {
        SET(data->ced_SND_Channels, MUIA_Disabled, FALSE);
        SET(data->ced_SND_Resolution, MUIA_Disabled, FALSE);
    }
    
    return NULL;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_6
(
    ConfigurationEditor, NULL, MUIC_Group, NULL,
    OM_NEW,                                  struct opSet *,
    MUIM_ConfigurationEditor_AddFloppyDrive, Msg,
    MUIM_ConfigurationEditor_RemFloppyDrive, Msg,
    MUIM_ConfigurationEditor_UpdateCPU,      Msg,
    MUIM_ConfigurationEditor_UpdateMemory,   Msg,
    MUIM_ConfigurationEditor_UpdateSound,    Msg
);

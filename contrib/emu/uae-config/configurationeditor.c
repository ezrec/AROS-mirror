/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#define DEBUG 1
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

#include "configurationeditor.h"
#include "locale.h"
#include "drivelisteditor.h"
#include "drive.h"
#include "support.h"
#include "asl.h"
#include "config.h"

CONST_STRPTR pageLabels[7];

/* You need an additional entry for the NULL */
static CONST_STRPTR MEM_Z3[12];
static CONST_STRPTR CPU_Type[6];
static CONST_STRPTR MEM_Chip[7];
static CONST_STRPTR MEM_Fast[6];
static CONST_STRPTR MEM_Slow[5];
static CONST_STRPTR GFX_Memory[8];
static CONST_STRPTR SND_Mode[5];
static CONST_STRPTR CPU_Speed[4];
static CONST_STRPTR GFX_Chipset[5];
static CONST_STRPTR SND_Channels[4];
static CONST_STRPTR PRT_Gameport[7];
static CONST_STRPTR SND_Resolution[3];
static CONST_STRPTR GFX_SpriteCollisions[5];

#define StartUAEButton()   MakePushButton("\eI[1:17]", _(MSG_TB_START_SH))
#define StartEUAEButton()  MakePushButton("\eI[1:17]", _(MSG_TB_START_SH))
#define ResetButton()      MakePushButton("\eI[1:21]", _(MSG_TB_RESTART_SH))
#define LoadStateButton()  MakePushButton("\eI[1:10]", _(MSG_TB_LOAD_STATE_SH))
#define SaveStateButton()  MakePushButton("\eI[1:12]", _(MSG_TB_SAVE_STATE_SH))
#define ToolbarSpace()     HSpace(5)

/*** Private methods ********************************************************/
#define MUIM_ConfigurationEditor_UpdateCPU      (TAG_USER | 0x13000002)
#define MUIM_ConfigurationEditor_UpdateMemory   (TAG_USER | 0x13000003)
#define MUIM_ConfigurationEditor_UpdateSound    (TAG_USER | 0x13000004)
#define MUIM_ConfigurationEditor_LoadState      (TAG_USER | 0x13000005)
#define MUIM_ConfigurationEditor_SaveState      (TAG_USER | 0x13000006)
#define MUIM_ConfigurationEditor_ResetState     (TAG_USER | 0x13000007)
#define MUIM_ConfigurationEditor_StartUAE       (TAG_USER | 0x13000008)
#define MUIM_ConfigurationEditor_StartEUAE      (TAG_USER | 0x13000009)

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
           *ced_DRV_ListEditor;
    
    Object *ced_PRT_Gameport0,
           *ced_PRT_Gameport1;

    struct UAEConfigdata configdata;
};


static void config2gui(struct ConfigurationEditor_DATA *data)
{
    ULONG i;

    SET(data->ced_CPU_Type, MUIA_Cycle_Active, data->configdata.cpu_type);
    SET(data->ced_CPU_Speed, MUIA_Cycle_Active, data->configdata.cpu_speed);
    SET(data->ced_CPU_Ratio, MUIA_Numeric_Value, data->configdata.cpu_ratio);
    SET(data->ced_CPU_Compatible, MUIA_Selected, data->configdata.cpu_compatible);

    SET(data->ced_MEM_Kickstart, MUIA_String_Contents, data->configdata.mem_kickstart);
    SET(data->ced_MEM_KickstartKey, MUIA_String_Contents, data->configdata.mem_kickstart_key);
    SET(data->ced_MEM_Chip, MUIA_Cycle_Active, data->configdata.mem_chip);
    SET(data->ced_MEM_Fast, MUIA_Cycle_Active, data->configdata.mem_fast);
    SET(data->ced_MEM_Slow, MUIA_Cycle_Active, data->configdata.mem_slow);
    SET(data->ced_MEM_Z3, MUIA_Cycle_Active, data->configdata.mem_z3);

    SET(data->ced_GFX_Chipset, MUIA_Cycle_Active, data->configdata.gfx_chipset);
    SET(data->ced_GFX_SpriteCollisions, MUIA_Cycle_Active, data->configdata.gfx_sprite_collisions);
    SET(data->ced_GFX_Memory, MUIA_Cycle_Active, data->configdata.gfx_memory);
    SET(data->ced_GFX_ImmediateBlits, MUIA_Cycle_Active, data->configdata.gfx_immediate_blits);
    SET(data->ced_GFX_Framerate, MUIA_Numeric_Value, data->configdata.gfx_framerate);

    SET(data->ced_SND_Mode, MUIA_Cycle_Active, data->configdata.snd_mode);
    SET(data->ced_SND_Channels, MUIA_Cycle_Active, data->configdata.snd_channels);
    SET(data->ced_SND_Resolution, MUIA_Cycle_Active, data->configdata.snd_resolution);

    for (i=0 ; i<4 ; i++)
    {
        SET(data->ced_DRV_Floppies[i], MUIA_String_Contents, data->configdata.drv_floppy_path[i]);
    }

    DoMethod(data->ced_DRV_ListEditor, MUIM_DriveListEditor_RemoveAll);
    for (i=0 ; i<data->configdata.drv_drive_count ; i++)
    {
        DoMethod
        (
            data->ced_DRV_ListEditor,
            MUIM_DriveListEditor_AddDrive,
            &data->configdata.drv_drive[i]
        );
    }

    SET(data->ced_PRT_Gameport0, MUIA_Cycle_Active, data->configdata.prt_gameport0);
    SET(data->ced_PRT_Gameport1, MUIA_Cycle_Active, data->configdata.prt_gameport1);
}

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
           *drv_ListEditor;
    Object *prt_Gameport0,
           *prt_Gameport1;
    Object *startUAEButton,
	   *startEUAEButton,
	   *saveStateButton,
	   *loadStateButton;

    
    /*== Init/Localize strings =============================================*/
    pageLabels[0]  = _(MSG_CFG_CPU);
    pageLabels[1]  = _(MSG_CFG_MEMORY);
    pageLabels[2]  = _(MSG_CFG_GRAPHICS);
    pageLabels[3]  = _(MSG_CFG_SOUND);
    pageLabels[4]  = _(MSG_CFG_DRIVES);
    pageLabels[5]  = _(MSG_CFG_PORTS);
    pageLabels[6]  = NULL;

    CPU_Type[CPU_68000]    = "68000";
    CPU_Type[CPU_68010]    = "68010";
    CPU_Type[CPU_68020]    = "68020";
    CPU_Type[CPU_68020_81] = "68020+68881";
    CPU_Type[CPU_68040]    = "68040";
    CPU_Type[CPU_CNT]      = NULL;

    CPU_Speed[SPD_MAX]     = _(MSG_CFG_CPU_SPEED_FAST);
    CPU_Speed[SPD_REAL]    = _(MSG_CFG_CPU_SPEED_COMPATIBLE);
    CPU_Speed[SPD_ADJ]     = _(MSG_CFG_CPU_SPEED_ADJUSTABLE);
    CPU_Speed[SPD_CNT]     = NULL;

    MEM_Chip[MCHP_256]     = "256 kiB";
    MEM_Chip[MCHP_512]     = "512 kiB";
    MEM_Chip[MCHP_1M]      = "1 MiB";
    MEM_Chip[MCHP_2M]      = "2 MiB";
    MEM_Chip[MCHP_4M]      = "4 MiB";
    MEM_Chip[MCHP_8M]      = "8 MiB";
    MEM_Chip[MCHP_CNT]     = NULL;

    MEM_Fast[MFST_NONE]    = _(MSG_CFG_MEM_NONE);
    MEM_Fast[MFST_1]       = "1 MiB";
    MEM_Fast[MFST_2]       = "2 MiB";
    MEM_Fast[MFST_4]       = "4 MiB";
    MEM_Fast[MFST_8]       = "8 MiB";
    MEM_Fast[MFST_CNT]     = NULL;

    MEM_Slow[MSLW_NONE]    = _(MSG_CFG_MEM_NONE);
    MEM_Slow[MSLW_512]     = "512 kiB";
    MEM_Slow[MSLW_1M]      = "1 MiB";
    MEM_Slow[MSLW_1_5M]    = "1.5 MiB";
    MEM_Slow[MSLW_CNT]     = NULL;

    MEM_Z3[MZ3_NONE]       = _(MSG_CFG_MEM_NONE);
    MEM_Z3[MZ3_1]          = "1 MiB";
    MEM_Z3[MZ3_2]          = "2 MiB";
    MEM_Z3[MZ3_4]          = "4 MiB";
    MEM_Z3[MZ3_8]          = "8 MiB";
    MEM_Z3[MZ3_16]         = "16 MiB";
    MEM_Z3[MZ3_32]         = "32 MiB";
    MEM_Z3[MZ3_64]         = "64 MiB";
    MEM_Z3[MZ3_128]        = "128 MiB";
    MEM_Z3[MZ3_256]        = "256 MiB";
    MEM_Z3[MZ3_512]        = "512 MiB";
    MEM_Z3[MZ3_CNT]        = NULL;

    GFX_Memory[MGFX_NONE]  = _(MSG_CFG_MEM_NONE);
    GFX_Memory[MGFX_1]     = "1 MiB";
    GFX_Memory[MGFX_2]     = "2 MiB";
    GFX_Memory[MGFX_4]     = "4 MiB";
    GFX_Memory[MGFX_8]     = "8 MiB";
    GFX_Memory[MGFX_16]    = "16 MiB";
    GFX_Memory[MGFX_32]    = "32 MiB";
    GFX_Memory[MGFX_CNT]   = NULL;

    GFX_Chipset[CHP_OCS]      = _(MSG_CFG_GFX_CHIPSET_OCS);
    GFX_Chipset[CHP_ECSAGNUS] = _(MSG_CFG_GFX_CHIPSET_ECS_AGNUS);
    GFX_Chipset[CHP_ECSFULL]  = _(MSG_CFG_GFX_CHIPSET_ECS_FULL);
    GFX_Chipset[CHP_AGA]      = _(MSG_CFG_GFX_CHIPSET_AGA);
    GFX_Chipset[CHP_CNT]      = NULL;

    GFX_SpriteCollisions[COLL_NONE]       = _(MSG_CFG_GFX_SC_NONE);
    GFX_SpriteCollisions[COLL_SPRITES]    = _(MSG_CFG_GFX_SC_SPRITES);
    GFX_SpriteCollisions[COLL_PLAYFIELDS] = _(MSG_CFG_GFX_SC_SPRITES_AND_PLAYFIELDS);
    GFX_SpriteCollisions[COLL_FULL]       = _(MSG_CFG_GFX_SC_FULL);
    GFX_SpriteCollisions[COLL_CNT]        = NULL;
        
    SND_Mode[SND_NONE]         = _(MSG_CFG_SND_MODE_NONE);
    SND_Mode[SND_MUTED]        = _(MSG_CFG_SND_MODE_MUTED);
    SND_Mode[SND_NORMAL]       = _(MSG_CFG_SND_MODE_NORMAL);
    SND_Mode[SND_EXACT]        = _(MSG_CFG_SND_MODE_ACCURATE);
    SND_Mode[SND_CNT]          = NULL;
    
    SND_Channels[SNDCH_MONO]   = _(MSG_CFG_SND_CHANNELS_MONO);
    SND_Channels[SNDCH_STEREO] = _(MSG_CFG_SND_CHANNELS_STEREO);
    SND_Channels[SNDCH_MIXED]  = _(MSG_CFG_SND_CHANNELS_MIXED);
    SND_Channels[SNDCH_CNT]    = NULL;
    
    SND_Resolution[SNDBIT_8]   = _(MSG_CFG_SND_RESOLUTION_8BIT);
    SND_Resolution[SNDBIT_16]  = _(MSG_CFG_SND_RESOLUTION_16BIT);
    SND_Resolution[SNDBIT_CNT] = NULL;
    
    PRT_Gameport[JP_JOY0]    = _(MSG_CFG_PRT_GAMEPORT_JOYSTICK0);
    PRT_Gameport[JP_JOY1]    = _(MSG_CFG_PRT_GAMEPORT_JOYSTICK1);
    PRT_Gameport[JP_MOUSE]   = _(MSG_CFG_PRT_GAMEPORT_MOUSE);
    PRT_Gameport[JP_NUMERIC] = _(MSG_CFG_PRT_GAMEPORT_NUMERIC);
    PRT_Gameport[JP_KEYS0]   = _(MSG_CFG_PRT_GAMEPORT_KEYS0);
    PRT_Gameport[JP_KEYS1]   = _(MSG_CFG_PRT_GAMEPORT_KEYS1);
    PRT_Gameport[JP_CNT]     = NULL;
    
    /*== Create objects and layot ==========================================*/
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        
        InnerSpacing(4, 4),
        Child, HGroup,
            ButtonFrame,
            MUIA_Group_Horiz, TRUE,
            MUIA_Background,  MUII_GroupBack,

                Child, (IPTR) HGroup,
                GroupSpacing(2),
                MUIA_Weight,         0,
                MUIA_Group_SameSize, TRUE,

                Child, (IPTR) (loadStateButton = LoadStateButton()),
                Child, (IPTR) (saveStateButton = SaveStateButton()),
                Child, (IPTR) ToolbarSpace(),
                Child, (IPTR) (startUAEButton = StartUAEButton()),
                Child, (IPTR) (startEUAEButton = StartEUAEButton()),
            End,
            Child, (IPTR) HVSpace,
        End,
        Child, (IPTR) RegisterGroup(pageLabels),
            Child, (IPTR) VGroup,
                /*-- CPU ---------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_CPU_TYPE)),
                    Child, (IPTR) (cpu_Type  = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) CPU_Type)),
                    Child, (IPTR) Label(_(MSG_CFG_CPU_SPEED)),
                    Child, (IPTR) (cpu_Speed = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) CPU_Speed)),
                    Child, (IPTR) Label(_(MSG_CFG_CPU_RATIO)),
                    Child, (IPTR) (cpu_Ratio = SliderObject,
                        MUIA_Weight,         0,
                        MUIA_Numeric_Min,    1,
                        MUIA_Numeric_Max,    5119,
                        MUIA_Numeric_Format, __(MSG_CFG_CPU_RATIO_FORMAT),
                    End),
                    Child, (IPTR) RectangleObject,
                        MUIA_Weight, 0,
                    End,
                    Child, (IPTR) ColGroup(3),
                        Child, (IPTR) (cpu_24bit = MUI_MakeObject(MUIO_Checkmark, NULL)),
                        Child, (IPTR) LLabel2(_(MSG_CFG_CPU_24BIT)),
                        Child, (IPTR) HVSpace,
                        Child, (IPTR) (cpu_Compatible = MUI_MakeObject(MUIO_Checkmark, NULL)),
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
                        MUIA_Popstring_String, (IPTR) (mem_Kickstart = StringObject,
                            StringFrame,
                        End),
                        MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                    End,
                    Child, (IPTR) Label2(_(MSG_CFG_MEM_ROM_KEY)),
                    Child, (IPTR) PopaslObject,
                        MUIA_Popstring_String, (IPTR) (mem_KickstartKey = StringObject,
                            StringFrame,
                        End),
                        MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                    End,
                End,
                Child, (IPTR) ColGroup(2),
                    GroupFrameT(_(MSG_CFG_MEM_RAM)),
                    
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_CHIP)),
                    Child, (IPTR) (mem_Chip = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Chip)),
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_SLOW)),
                    Child, (IPTR) (mem_Slow = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Slow)),
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_FAST)),
                    Child, (IPTR) (mem_Fast = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Fast)),
                    Child, (IPTR) Label(_(MSG_CFG_MEM_RAM_Z3)),
                    Child, (IPTR) (mem_Z3 = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) MEM_Z3)),
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Graphics ----------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_GFX_CHIPSET)),
                    Child, (IPTR) (gfx_Chipset = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) GFX_Chipset)),
                    Child, (IPTR) Label(_(MSG_CFG_GFX_SPRITECOLLISIONS)),
                    Child, (IPTR) (gfx_SpriteCollisions = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) GFX_SpriteCollisions)),
                    Child, (IPTR) RectangleObject,
                        MUIA_Weight, 0,
                    End,
                    Child, (IPTR) ColGroup(3),
                        Child, (IPTR) (gfx_CopperSpeedup = MUI_MakeObject(MUIO_Checkmark, NULL)),
                        Child, (IPTR) LLabel(_(MSG_CFG_GFX_COPPER_SPEEDUP)),
                        Child, (IPTR) HVSpace,
                        Child, (IPTR) (gfx_ImmediateBlits = MUI_MakeObject(MUIO_Checkmark, NULL)),
                        Child, (IPTR) LLabel(_(MSG_CFG_GFX_IMMEDIATE_BLITS)),
                        Child, (IPTR) HVSpace,
                    End,
                    Child, (IPTR) Label(_(MSG_CFG_GFX_FRAMERATE)),
                    Child, (IPTR) (gfx_Framerate = SliderObject,
                        MUIA_Weight,                0,
                        MUIA_Numeric_Min,           1,
                        MUIA_Numeric_Max,           20,
                        MUIA_Numeric_Format, (IPTR) "1/%ld",
                    End),
                    Child, (IPTR) Label(_(MSG_CFG_GFX_P96RAM)),
                    Child, (IPTR) (gfx_Memory = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) GFX_Memory)),
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Sound -------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_SND_MODE)),
                    Child, (IPTR) (snd_Mode = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) SND_Mode)),
                    Child, (IPTR) Label(_(MSG_CFG_SND_CHANNELS)),
                    Child, (IPTR) (snd_Channels = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) SND_Channels)),
                    Child, (IPTR) Label(_(MSG_CFG_SND_RESOLUTION)),
                    Child, (IPTR) (snd_Resolution = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) SND_Resolution)),
                End,
                Child, (IPTR) HVSpace,
            End,
            Child, (IPTR) VGroup,
                /*-- Drives ------------------------------------------------*/
                Child, (IPTR) HGroup,
                    GroupFrameT(_(MSG_CFG_DRV_FLOPPY)),
                    
                    Child, (IPTR) (drv_FloppyRootGroup = ColGroup(2),
                        Child, (IPTR) (drv_FloppyLabels[0] = Label("DF0:")),
                        Child, (IPTR) (drv_FloppyPopasls[0] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) (drv_Floppies[0] = StringObject,
                                StringFrame,
                            End),
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End),
                        Child, (IPTR) (drv_FloppyLabels[1] = Label("DF1:")),
                        Child, (IPTR) (drv_FloppyPopasls[1] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) (drv_Floppies[1] = StringObject,
                                StringFrame,
                            End),
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End),
                        Child, (IPTR) (drv_FloppyLabels[2] = Label("DF2:")),
                        Child, (IPTR) (drv_FloppyPopasls[2] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) (drv_Floppies[2] = StringObject,
                                StringFrame,
                            End),
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End),
                        Child, (IPTR) (drv_FloppyLabels[3] = Label("DF3:")),
                        Child, (IPTR) (drv_FloppyPopasls[3] = PopaslObject,
                            MUIA_Popstring_String, (IPTR) (drv_Floppies[3] = StringObject,
                                StringFrame,
                            End),
                            MUIA_Popstring_Button, (IPTR) PopButton(MUII_PopFile),
                        End),
                    End),
                End,
                
                Child, (IPTR) VGroup,
                    GroupFrameT(_(MSG_CFG_DRV_HARD)),
                    
                    Child, (IPTR) (drv_ListEditor = DriveListEditorObject,
                    End),
                End,
            End,
            Child, (IPTR) VGroup,
                /*-- Ports -------------------------------------------------*/
                Child, (IPTR) HVSpace,
                Child, (IPTR) ColGroup(2),
                    Child, (IPTR) Label(_(MSG_CFG_PRT_GAMEPORT1)),
                    Child, (IPTR) (prt_Gameport0 = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) PRT_Gameport)),
                    Child, (IPTR) Label(_(MSG_CFG_PRT_GAMEPORT2)),
                    Child, (IPTR) (prt_Gameport1 = MUI_MakeObject(MUIO_Cycle, NULL, (IPTR) PRT_Gameport)),
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
        DoMethod
        (
            loadStateButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_ConfigurationEditor_LoadState
        );
        DoMethod
        (
            saveStateButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_ConfigurationEditor_SaveState
        );
        DoMethod
        (
            startUAEButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_ConfigurationEditor_StartUAE
        );
        DoMethod
        (
            startEUAEButton, MUIM_Notify, MUIA_Pressed, FALSE,
            (IPTR) self, 1, MUIM_ConfigurationEditor_StartEUAE
        );
        
        /*-- Sensible defaults ---------------------------------------------*/
        config_init(&data->configdata);
        DoMethod(self, MUIM_ConfigurationEditor_ResetState);
        
    }
    
    return (IPTR) self;
}

IPTR ConfigurationEditor__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(cl, obj);

    config_dispose(&data->configdata);

    return DoSuperMethodA(cl, obj, msg);
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
    
    return TRUE;
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
    
    return TRUE;
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
    
    return TRUE;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_LoadState
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);

    STRPTR filename = ASL_SelectFile(AM_LOAD);
    if (filename != NULL)
    {
        config_read(&data->configdata, filename);
        config2gui(data);

        FreeVec(filename);
    }
    
    return 0;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_SaveState
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);

    ULONG i;
    STRPTR filename = ASL_SelectFile(AM_SAVE);
    if (filename != NULL)
    {
        data->configdata.cpu_type = XGET(data->ced_CPU_Type, MUIA_Cycle_Active);
        data->configdata.cpu_speed = XGET(data->ced_CPU_Speed, MUIA_Cycle_Active);
        data->configdata.cpu_ratio = XGET(data->ced_CPU_Ratio, MUIA_Numeric_Value);
        data->configdata.cpu_compatible = XGET(data->ced_CPU_Compatible, MUIA_Selected);

        data->configdata.mem_kickstart = StrDup((STRPTR)XGET(data->ced_MEM_Kickstart, MUIA_String_Contents));
        data->configdata.mem_kickstart_key =
            StrDup((STRPTR)XGET(data->ced_MEM_KickstartKey, MUIA_String_Contents));
        data->configdata.mem_chip = XGET(data->ced_MEM_Chip, MUIA_Cycle_Active);
        data->configdata.mem_fast = XGET(data->ced_MEM_Fast, MUIA_Cycle_Active);
        data->configdata.mem_slow = XGET(data->ced_MEM_Slow, MUIA_Cycle_Active);
        data->configdata.mem_z3   = XGET(data->ced_MEM_Z3, MUIA_Cycle_Active);

        data->configdata.gfx_chipset = XGET(data->ced_GFX_Chipset, MUIA_Cycle_Active);
        data->configdata.gfx_sprite_collisions =
            XGET(data->ced_GFX_SpriteCollisions, MUIA_Cycle_Active);
        data->configdata.gfx_memory = XGET(data->ced_GFX_Memory, MUIA_Cycle_Active);
        data->configdata.gfx_immediate_blits = XGET(data->ced_GFX_ImmediateBlits, MUIA_Cycle_Active);
        data->configdata.gfx_framerate = XGET(data->ced_GFX_Framerate, MUIA_Numeric_Value);

        data->configdata.snd_mode = XGET(data->ced_SND_Mode, MUIA_Cycle_Active);
        data->configdata.snd_channels = XGET(data->ced_SND_Channels, MUIA_Cycle_Active);
        data->configdata.snd_resolution = XGET(data->ced_SND_Resolution, MUIA_Cycle_Active);

        for (i=0 ; i<4 ; i++)
        {
            data->configdata.drv_floppy_path[i] = StrDup((STRPTR)XGET(data->ced_DRV_Floppies[i], MUIA_String_Contents));
        }

        data->configdata.drv_drive_count = XGET(data->ced_DRV_ListEditor, MUIA_List_Entries);
        if (data->configdata.drv_drive_count > MAX_DRIVE_CNT)
        {
            data->configdata.drv_drive_count = MAX_DRIVE_CNT;
        }
        for (i=0 ; i<data->configdata.drv_drive_count ; i++)
        {
            struct Drive *drv;
            DoMethod(data->ced_DRV_ListEditor, MUIM_List_GetEntry, i, &drv);
            data->configdata.drv_drive[i] = *drv;
            data->configdata.drv_drive[i].d_Path = StrDup(drv->d_Path);
            if (drv->d_Type == DT_FILESYSTEM)
            {
                data->configdata.drv_drive[i].d_Parameters.FS.VolumeName = StrDup(drv->d_Parameters.FS.VolumeName);
            }
        }

        data->configdata.prt_gameport0 = XGET(data->ced_PRT_Gameport0, MUIA_Cycle_Active);
        data->configdata.prt_gameport1 = XGET(data->ced_PRT_Gameport1, MUIA_Cycle_Active);

        config_write(&data->configdata, filename);
        
        FreeVec(filename);
    }

    return 0;
}


IPTR ConfigurationEditor__MUIM_ConfigurationEditor_ResetState
(
    Class *CLASS, Object *self, Msg message
)
{
    struct ConfigurationEditor_DATA *data = INST_DATA(CLASS, self);

    config_reset(&data->configdata);
    config2gui(data);

    return 0;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_StartUAE
(
    Class *CLASS, Object *self, Msg message
)
{
    SystemTags
    (
        "SYS:Extras/Emu/UAE/uae -f ram:test.uaerc",
        SYS_Asynch, TRUE,
        SYS_Input, "CON:",
        SYS_Output, NULL,
        TAG_DONE
    );
    return 0;
}

IPTR ConfigurationEditor__MUIM_ConfigurationEditor_StartEUAE
(
    Class *CLASS, Object *self, Msg message
)
{
    SystemTags
    (
        "SYS:Extras/Emu/E-UAE/i386-aros-uae -f ram:test.uaerc",
        SYS_Asynch, TRUE,
        SYS_Input, "CON:",
        SYS_Output, NULL,
        TAG_DONE
    );
    return 0;
}

/*** Setup ******************************************************************/
__ZUNE_CUSTOMCLASS_START(ConfigurationEditor)
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__OM_NEW, OM_NEW, struct opSet *);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__OM_DISPOSE, OM_DISPOSE, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_UpdateCPU, MUIM_ConfigurationEditor_UpdateCPU, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_UpdateMemory, MUIM_ConfigurationEditor_UpdateMemory, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_UpdateSound, MUIM_ConfigurationEditor_UpdateSound, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_LoadState, MUIM_ConfigurationEditor_LoadState, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_SaveState, MUIM_ConfigurationEditor_SaveState, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_ResetState, MUIM_ConfigurationEditor_ResetState, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_StartUAE, MUIM_ConfigurationEditor_StartUAE, Msg);
__ZUNE_CUSTOMCLASS_METHOD(ConfigurationEditor__MUIM_ConfigurationEditor_StartEUAE, MUIM_ConfigurationEditor_StartEUAE, Msg);
__ZUNE_CUSTOMCLASS_END(ConfigurationEditor, NULL, MUIC_Group, NULL)


/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <exec/types.h>

enum {CPU_68000, CPU_68010, CPU_68020, CPU_68020_81, CPU_68040, CPU_CNT};
enum {SPD_MAX=21, SPD_REAL, SPD_CNT};
enum {MZ3_NONE, MZ3_1, MZ3_2, MZ3_4, MZ3_8, MZ3_16, MZ3_32, MZ3_64, MZ3_128, MZ3_256, MZ3_512, MZ3_CNT};
enum {MCHP_256, MCHP_512, MCHP_1M, MCHP_2M, MCHP_4M, MCHP_8M, MCHP_CNT};
enum {MFST_NONE, MFST_1, MFST_2, MFST_4, MFST_8, MFST_CNT};
enum {MSLW_NONE, MSLW_512, MSLW_1M, MSLW_1_5M, MSLW_CNT};
enum {MGFX_NONE, MGFX_1, MGFX_2, MGFX_4, MGFX_8, MGFX_16, MGFX_32, MGFX_CNT};
enum {CHP_OCS, CHP_ECSAGNUS, CHP_ECSFULL, CHP_AGA, CHP_CNT};
enum {COLL_NONE, COLL_SPRITES, COLL_PLAYFIELDS, COLL_FULL, COLL_CNT};
enum {SND_NONE, SND_MUTED, SND_NORMAL, SND_EXACT, SND_CNT};
enum {SNDCH_MONO, SNDCH_STEREO, SNDCH_MIXED, SNDCH_CNT};
enum {SNDBIT_8, SNDBIT_16, SNDBIT_CNT};
enum {JP_JOY0, JP_JOY1, JP_MOUSE, JP_NUMERIC, JP_KEYS0, JP_KEYS1, JP_CNT};

struct UAEConfigdata
{
    ULONG cpu_type;
    ULONG cpu_speed;
    ULONG cpu_ratio;
    BOOL cpu_24bit;
    BOOL cpu_compatible;
    
    STRPTR mem_kickstart;
    STRPTR mem_kickstart_key;
    ULONG mem_chip;
    ULONG mem_fast;
    ULONG mem_slow;
    ULONG mem_z3;
           
    ULONG gfx_chipset;
    ULONG gfx_sprite_collisions;
    ULONG gfx_memory;
    BOOL gfx_immediate_blits;
    ULONG gfx_framerate;
           
    ULONG snd_mode;
    ULONG snd_channels;
    ULONG snd_resolution;
    
    ULONG drv_floppy_count;
    
    ULONG prt_gameport0;
    ULONG prt_gameport1;
};

void config_init(struct UAEConfigdata *cfg);
void config_reset(struct UAEConfigdata *cfg);
BOOL config_write(struct UAEConfigdata *cfg, STRPTR filename);
BOOL config_read(struct UAEConfigdata *cfg, STRPTR filename);
void config_dispose(struct UAEConfigdata *cfg);

#endif

/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <exec/types.h>

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

BOOL config_write(struct UAEConfigdata *cfg, STRPTR filename);
BOOL config_read(struct UAEConfigdata *cfg, STRPTR filename);

#endif

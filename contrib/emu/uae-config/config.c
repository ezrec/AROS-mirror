/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <exec/types.h>

#include <proto/alib.h>
#include <proto/dos.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define TEST

static TEXT buffer[255];
static TEXT intbuf[20];

static CONST_STRPTR cpu_type[CPU_CNT];
static CONST_STRPTR cpu_speed[SPD_CNT];
static CONST_STRPTR mem_z3[MZ3_CNT];
static CONST_STRPTR mem_chip[MCHP_CNT];
static CONST_STRPTR mem_fast[MFST_CNT];
static CONST_STRPTR mem_slow[MSLW_CNT];
static CONST_STRPTR mem_gfx[MGFX_CNT];
static CONST_STRPTR chip_type[CHP_CNT];
static CONST_STRPTR coll_type[COLL_CNT];
static CONST_STRPTR sound[SND_CNT];
static CONST_STRPTR sound_ch[SNDCH_CNT];
static CONST_STRPTR sound_bit[SNDBIT_CNT];
static CONST_STRPTR joyport[JP_CNT];

static BOOL config_parse_line(char *buffer, char **variable, char **value);
static BOOL config_search_array(ULONG *var, char *value, CONST_STRPTR *array, LONG count);
static BOOL config_check_bool(char *value);
static void config_write_bool(BPTR fh, CONST_STRPTR var, LONG val);
static void config_write_string(BPTR fh, CONST_STRPTR var, CONST_STRPTR val);
static void config_write_int(BPTR fh, CONST_STRPTR var, ULONG val, STRPTR buffer);

void config_init(struct UAEConfigdata *cfg)
{
    cpu_type[CPU_68000]    = "68000";
    cpu_type[CPU_68010]    = "68010";
    cpu_type[CPU_68020]    = "68020";
    cpu_type[CPU_68020_81] = "68020/68881";
    cpu_type[CPU_68040]    = "68040";

    cpu_speed[SPD_MAX]  = "max";
    cpu_speed[SPD_REAL] = "real";

    mem_z3[MZ3_NONE] = "0";
    mem_z3[MZ3_1]    = "1";
    mem_z3[MZ3_2]    = "2";
    mem_z3[MZ3_4]    = "4";
    mem_z3[MZ3_8]    = "8";
    mem_z3[MZ3_16]   = "16";
    mem_z3[MZ3_32]   = "32";
    mem_z3[MZ3_64]   = "64";
    mem_z3[MZ3_128]  = "128";
    mem_z3[MZ3_256]  = "256";
    mem_z3[MZ3_512]  = "512";

    mem_chip[MCHP_256] = "0";
    mem_chip[MCHP_512] = "1";
    mem_chip[MCHP_1M]  = "2";
    mem_chip[MCHP_2M]  = "4";
    mem_chip[MCHP_4M]  = "8";
    mem_chip[MCHP_8M]  = "16";

    mem_fast[MFST_NONE] = "0";
    mem_fast[MFST_1]    = "1";
    mem_fast[MFST_2]    = "2";
    mem_fast[MFST_4]    = "4";
    mem_fast[MFST_8]    = "8";

    mem_slow[MSLW_NONE] = "0";
    mem_slow[MSLW_512]  = "2";
    mem_slow[MSLW_1M]   = "4";
    mem_slow[MSLW_1_5M] = "6";

    mem_gfx[MGFX_NONE] = "0";
    mem_gfx[MGFX_1]    = "1";
    mem_gfx[MGFX_2]    = "2";
    mem_gfx[MGFX_4]    = "4";
    mem_gfx[MGFX_8]    = "8";
    mem_gfx[MGFX_16]   = "16";
    mem_gfx[MGFX_32]   = "32";

    chip_type[CHP_OCS]      = "ocs";
    chip_type[CHP_ECSAGNUS] = "ecs_agnus";
    chip_type[CHP_ECSFULL]  = "ecs";
    chip_type[CHP_AGA]      = "aga";

    coll_type[COLL_NONE]       = "none";
    coll_type[COLL_SPRITES]    = "sprites";
    coll_type[COLL_PLAYFIELDS] = "playfields";
    coll_type[COLL_FULL]       = "full";

    sound[SND_NONE]   = "none";
    sound[SND_MUTED]  = "interrupts";
    sound[SND_NORMAL] = "normal";
    sound[SND_EXACT]  = "exact";

    sound_ch[SNDCH_MONO]   = "mono";
    sound_ch[SNDCH_STEREO] = "stereo";
    sound_ch[SNDCH_MIXED]  = "mixed";

    sound_bit[SNDBIT_8]  = "8";
    sound_bit[SNDBIT_16] = "16";
    
    joyport[JP_JOY0]    = "joy0";
    joyport[JP_JOY1]    = "joy1";
    joyport[JP_MOUSE]   = "mouse";
    joyport[JP_NUMERIC] = "kbd1";
    joyport[JP_KEYS0]   = "kbd2";
    joyport[JP_KEYS1]   = "kbd3";
    
    memset(cfg, 0, sizeof(*cfg));
    config_reset(cfg);
}

void config_reset(struct UAEConfigdata *cfg)
{
    cfg->cpu_type       = CPU_68000;
    cfg->cpu_speed      = SPD_REAL;
    cfg->cpu_ratio      = 10;
    cfg->cpu_24bit      = FALSE;
    cfg->cpu_compatible = FALSE;
    
    FreeVec(cfg->mem_kickstart);
    cfg->mem_kickstart     = NULL;
    FreeVec(cfg->mem_kickstart_key);
    cfg->mem_kickstart_key = NULL;
    cfg->mem_chip          = MCHP_512;
    cfg->mem_fast          = MFST_NONE;
    cfg->mem_slow          = MSLW_512;
    cfg->mem_z3            = MZ3_NONE;
           
    cfg->gfx_chipset           = CHP_ECSAGNUS;
    cfg->gfx_sprite_collisions = COLL_PLAYFIELDS;
    cfg->gfx_memory            = MGFX_NONE;
    cfg->gfx_immediate_blits   = FALSE;
    cfg->gfx_framerate         = 1;
           
    cfg->snd_mode       = SND_NONE;
    cfg->snd_channels   = SNDCH_MONO;
    cfg->snd_resolution = SNDBIT_8;
    
    cfg->drv_floppy_count = 2;
    
    cfg->prt_gameport0 = JP_MOUSE;
    cfg->prt_gameport1 = JP_KEYS0;
}

void config_dispose(struct UAEConfigdata *cfg)
{
    FreeVec(cfg->mem_kickstart);
    cfg->mem_kickstart = NULL;
    FreeVec(cfg->mem_kickstart_key);
    cfg->mem_kickstart_key = NULL;
}

BOOL config_write(struct UAEConfigdata *cfg, STRPTR filename)
{
#ifdef TEST
    filename = "RAM:test.uaerc";
#endif
    BPTR fh = Open(filename, MODE_NEWFILE);
    if (fh)
    {
        /* CPU */
        config_write_string(fh, "cpu_type", cpu_type[cfg->cpu_type]);

        // TODO cpu_speed/cpu_ratio
        
        config_write_bool(fh, "cpu_compatible", cfg->cpu_compatible);
        
        /* Memory */
        if (cfg->mem_kickstart)
        {
            config_write_string(fh, "kickstart_rom_file", cfg->mem_kickstart);
        }
        if (cfg->mem_kickstart_key)
        {
            config_write_string(fh, "kickstart_key_file", cfg->mem_kickstart_key);
        }
        
        config_write_string(fh, "chipmem_size", mem_chip[cfg->mem_chip]);
        config_write_string(fh, "fastmem_size", mem_fast[cfg->mem_fast]);
        config_write_string(fh, "bogomen_size", mem_slow[cfg->mem_slow]);
        config_write_string(fh, "z3mem_size", mem_z3[cfg->mem_z3]);
        
        /* Graphics */
        config_write_string(fh, "chipset", chip_type[cfg->gfx_chipset]);
        config_write_string(fh, "collision_level", coll_type[cfg->gfx_sprite_collisions]);
        config_write_string(fh, "gfxcard_size", mem_gfx[cfg->gfx_memory]);
        config_write_bool(fh, "immediate_blits", cfg->gfx_immediate_blits);
        config_write_int(fh, "gfx_framerate", cfg->gfx_framerate, intbuf);
        
        /* Sound */
        config_write_string(fh, "sound_output", sound[cfg->snd_mode]);
        config_write_string(fh, "sound_channels", sound_ch[cfg->snd_channels]);
        config_write_string(fh, "sound_bits", sound_bit[cfg->snd_resolution]);
        
        /* Drives */
        config_write_int(fh, "nr_floppies", cfg->drv_floppy_count, intbuf);
        
        /* Gameports */
        config_write_string(fh, "joyport0", joyport[cfg->prt_gameport0]);
        config_write_string(fh, "joyport1", joyport[cfg->prt_gameport1]);
        
        Close(fh);
    }
    return TRUE;
}

BOOL config_read(struct UAEConfigdata *cfg, STRPTR filename)
{
    char *variable;
    char *value;
#ifdef TEST
    filename = "RAM:test.uaerc";
#endif
    config_reset(cfg);
    
    BPTR fh = Open(filename, MODE_OLDFILE);
    if (fh)
    {
        while (FGets(fh, buffer, sizeof(buffer)))
        {
            if (config_parse_line(buffer, &variable, &value))
            {
                /* CPU */
                if (!strcmp(variable, "cpu_type"))
                {
                    config_search_array(&cfg->cpu_type, value, cpu_type, sizeof(cpu_type)/sizeof(STRPTR));
                }
            
                // TODO cpu_speed/cpu_ratio
            
                else if (!strcmp(variable, "cpu_compatible"))
                {
                    cfg->cpu_compatible = config_check_bool(value);
                }

                /* Memory */
                else if (!strcmp(variable, "kickstart_rom_file"))
                {
                    cfg->mem_kickstart = StrDup(value);
                }
                else if (!strcmp(variable, "kickstart_key_file"))
                {
                    cfg->mem_kickstart_key = StrDup(value);
                }
                else if (!strcmp(variable, "chipmem_size"))
                {
                    config_search_array(&cfg->mem_chip, value, mem_chip, sizeof(mem_chip)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "fastmem_size"))
                {
                    config_search_array(&cfg->mem_fast, value, mem_fast, sizeof(mem_fast)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "bogomem_size"))
                {
                    config_search_array(&cfg->mem_slow, value, mem_slow, sizeof(mem_slow)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "z3mem_size"))
                {
                    config_search_array(&cfg->mem_z3, value, mem_z3, sizeof(mem_z3)/sizeof(STRPTR));
                }
                
                /* Graphics */
                else if (!strcmp(variable, "chipset"))
                {
                    config_search_array(&cfg->gfx_chipset, value, chip_type, sizeof(chip_type)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "collision_level"))
                {
                    config_search_array(&cfg->gfx_sprite_collisions, value, coll_type, sizeof(coll_type)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "gfxcard_size"))
                {
                    config_search_array(&cfg->gfx_memory, value, mem_gfx, sizeof(mem_gfx)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "immediate_blits"))
                {
                    cfg->gfx_immediate_blits = config_check_bool(value);
                }
                else if (!strcmp(variable, "gfx_framerate"))
                {
                    cfg->gfx_framerate = atoi(value);
                }
            
                /* Sound */
                else if (!strcmp(variable, "sound_output"))
                {
                    config_search_array(&cfg->snd_mode, value, sound, sizeof(sound)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "sound_channels"))
                {
                    config_search_array(&cfg->snd_channels, value, sound_ch, sizeof(sound_ch)/sizeof(STRPTR));
                }
                else if (!strcmp(variable, "sound_bits"))
                {
                    config_search_array(&cfg->snd_resolution, value, sound_bit, sizeof(sound_bit)/sizeof(STRPTR));
                }
                
                /* Drives */
                else if (!strcmp(variable, "nr_floppies"))
                {
                    cfg->drv_floppy_count = atoi(value);
                }
                
                /* Gameport */
                else if (!strcmp(variable, "joyport0"))
                {
                    config_search_array(&cfg->prt_gameport0, value, joyport, sizeof(joyport)/sizeof(joyport));
                }
                else if (!strcmp(variable, "joyport1"))
                {
                    config_search_array(&cfg->prt_gameport1, value, joyport, sizeof(joyport)/sizeof(joyport));
                }
            }
        }
        Close(fh);
    }
    return TRUE;
}


static BOOL config_parse_line(char *buffer, char **variable, char **value)
{
    *variable = buffer;
    *value = NULL;

    char *equal = strchr(buffer, '=');
    if (equal)
    {
        *equal = '\0';
        *value = equal+1;
        while (isspace(**variable))
        {
            (*variable)++;
        }
        while (isspace(**value))
        {
            (*value)++;
        }
        while ((equal >= buffer) && isspace(*equal))
        {
            *equal = '\0';
            equal--;
        }
        equal = *variable + strlen(*variable) - 1;
        while ((equal >= *variable) && isspace(*equal))
        {
            *equal = '\0';
            equal--;
        }
        return TRUE;
    }
    return FALSE;
}

static BOOL config_search_array(ULONG *var, char *value, CONST_STRPTR *array, LONG count)
{
    LONG index = 0;
    while (index < count)
    {
        if (!strcmp(value, array[index]))
        {
            *var = index;
            return TRUE;
        }
        index++;
    }
    return FALSE;
}

static BOOL config_check_bool(char *value)
{
    if (!strcmp(value, "yes"))
    {
        return TRUE;
    }
    return FALSE;
}

static void config_write_bool(BPTR fh, CONST_STRPTR var, LONG val)
{
    FPuts(fh, var);
    FPuts(fh, " = ");
    if (val)
    {
        FPuts(fh, "yes");
    }
    else
    {
        FPuts(fh, "no");
    }
    FPutC(fh, '\n');
}

static void config_write_string(BPTR fh, CONST_STRPTR var, CONST_STRPTR val)
{
    FPuts(fh, var);
    FPuts(fh, " = ");
    FPuts(fh, val);
    FPutC(fh, '\n');
}

static void config_write_int(BPTR fh, CONST_STRPTR var, ULONG val, STRPTR buffer)
{
    sprintf(buffer, "%lu", val);
    FPuts(fh, var);
    FPuts(fh, " = ");
    FPuts(fh, buffer);
    FPutC(fh, '\n');
}

#include "dosboot_intern.h"

void InitBootConfig(struct BootConfig *bootcfg, APTR BootLoaderBase)
{
    bootcfg->gfxlib  = NULL;
    bootcfg->gfxhidd = NULL;
}

#include "quicktime.h"

void quicktime_smhd_init(quicktime_smhd_t *smhd)
{
     smhd->version = 0;
     smhd->flags = 0;
     smhd->balance = 0;
     smhd->reserved = 0;
}

void quicktime_smhd_delete(quicktime_smhd_t *smhd)
{
}

void quicktime_read_smhd(quicktime_t *file, quicktime_smhd_t *smhd)
{
     smhd->version = quicktime_read_char(file);
     smhd->flags = quicktime_read_int24(file);
     smhd->balance = quicktime_read_int16(file);
     smhd->reserved = quicktime_read_int16(file);
}


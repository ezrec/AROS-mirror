#include <stdio.h>
#include "quicktime.h"

void quicktime_mdat_delete(quicktime_mdat_t *mdat)
{
}

void quicktime_read_mdat(quicktime_t *file, quicktime_mdat_t *mdat, quicktime_atom_t *parent_atom)
{
     mdat->atom.size = parent_atom->size;
     mdat->atom.start = parent_atom->start;
     quicktime_atom_skip(file, parent_atom);
}

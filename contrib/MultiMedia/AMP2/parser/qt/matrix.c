#include "quicktime.h"

void quicktime_matrix_init(quicktime_matrix_t *matrix)
{
     int i;
     for(i = 0; i < 9; i++) matrix->values[i] = 0;
     matrix->values[0] = matrix->values[4] = 1;
     matrix->values[8] = 16384;
}

void quicktime_matrix_delete(quicktime_matrix_t *matrix)
{
}

void quicktime_read_matrix(quicktime_t *file, quicktime_matrix_t *matrix)
{
     int i = 0;
     for(i = 0; i < 9; i++)
     {
          matrix->values[i] = quicktime_read_fixed32(file);
     }
}


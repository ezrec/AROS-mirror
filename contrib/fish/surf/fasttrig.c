#include <aros/oldprograms.h>
#include <stdlib.h>
#include "fasttrig.h"

float *sintab;
float *costab;

extern struct Library * MathTransBase;

static int oldn = -1,
       oldrange = -1,
       oldstart = -1;


/*
 * return true if could not get memory for trig lookup tables
 */
bool InitFastTrig(angstart, angrange, n)
    int angstart;
    int angrange;
    int n;
{


    int i;
    float AngleInc;
    float curangle;

    if( n == oldn && oldrange == angrange && oldstart == angstart ) {
        return(false);
    }

    if( n != oldn ) {
        if( sintab ) free(sintab);
        if( costab ) free(costab);
        sintab = (float *)malloc( n * sizeof( float ));
        costab = (float *)malloc( n * sizeof( float ));
        if( !sintab || !costab ) {
           return(true);
        }
    }

    oldn = n;
    oldrange = angrange;
    oldstart = angstart;

    AngleInc = (PI*angrange)/(180 *(n-1));
    for( curangle = angstart*PI/180.0, i = 0;
         i< n; i++, curangle += AngleInc ) {

        sintab[i] = sin( curangle );
        costab[i] = cos( curangle );
    }
    return(false);
}

#include "drmP.h"

#define DEBUG 1
#include <aros/debug.h>

unsigned long drm_get_resource_len(struct drm_device *dev, unsigned int resource)
{
    D(bug("drm_get_resource_len\n"));
    return 0;
}

/**
 * Compute size order.  Returns the exponent of the smaller power of two which
 * is greater or equal to given number.
 *
 * \param size size.
 * \return order.
 *
 * \todo Can be made faster.
 */
int drm_order(unsigned long size)
{
    int order;
    unsigned long tmp;

    for (order = 0, tmp = size >> 1; tmp; tmp >>= 1, order++) ;

    if (size & (size - 1))
        ++order;

    return order;
}
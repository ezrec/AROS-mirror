#ifndef __GDKCONFIG_H__
#define __GDKCONFIG_H__

/* Some lowlevel config options
 *
 * Usage:
 *
 * USE_PENS       - pen support (ObtainBestPen() etc.)
 *
 * Fill in your own settings and modify source code if needed...
 */

#if defined (__MORPHOS__) || (__AROS__)

  #define USE_PENS        0

#else

  #define USE_PENS        1

#endif

#endif /* __GDKCONFIGH_H__ */



/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*drivers registry */
#include <config.h>
#include <ui.h>
extern struct ui_driver svga_driver, x11_driver, dog_driver, plan9_driver,
  curses_driver, plan9_driver, mac_driver, os2vio_driver, os2txt_driver,
  be_driver, aalib_driver, aros_driver;
struct ui_driver *drivers[] =
{
#ifdef SVGA_DRIVER
  &svga_driver,
#endif
#ifdef X11_DRIVER
  &x11_driver,
#endif
#ifdef OS2VIO_DRIVER
  &os2vio_driver,
#endif
#ifdef OS2TXT_DRIVER
  &os2txt_driver,
#endif
#ifdef DOG_DRIVER
  &dog_driver,
#endif
#ifdef PLAN9_DRIVER
  &plan9_driver,
#endif
#ifdef AA_DRIVER
  &aalib_driver,
#endif
#ifdef CURSES_DRIVER
  &curses_driver,
#endif
#ifdef _MAC
  &mac_driver,
#endif
#ifdef __BEOS__
  &be_driver,
#endif
#ifdef AROS
  &aros_driver,
#endif
  NULL
};
int ndrivers = (sizeof (drivers) / sizeof (*drivers) - 1);

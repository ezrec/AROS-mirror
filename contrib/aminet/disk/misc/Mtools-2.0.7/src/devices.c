/*
 * Device tables.  See the Configure file for a complete description.
 */

#include <stdio.h>
#include "msdos.h"

#ifdef __AROS__
struct device devices[] = {
/*	{'A', "/flat/PC0", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18}, */
	{'A', "mfm.device",  0L, 12, 0, (int (*) ()) 0, 0, -1, 13},
	{'B', "scsi.device", 0L, 16, 1, (int (*) ()) 0, 0, -1, 0}
/*                              ^ Unit Nr!! */
};
#endif /* DELL */


#ifdef AMIGA
struct device devices[] = {
/*	{'A', "/flat/PC0", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18}, */
	{'A', "mfm.device",  0L, 12, 0, (int (*) ()) 0, 0, -1, 13},
	{'B', "scsi.device", 0L, 16, 1, (int (*) ()) 0, 0, -1, 0}
/*                              ^ Unit Nr!! */
};
#endif /* DELL */

#ifdef DELL
struct device devices[] = {
	{'A', "/dev/rdsk/f0d9dt", 0L, 12, 0, (int (*) ()) 0, 40, 2, 9},
	{'A', "/dev/rdsk/f0q15dt", 0L, 12, 0, (int (*) ()) 0, 80, 2, 15},
	{'A', "/dev/rdsk/f0d8dt", 0L, 12, 0, (int (*) ()) 0, 40, 2, 8},
	{'B', "/dev/rdsk/f13ht", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18},
	{'B', "/dev/rdsk/f13dt", 0L, 12, 0, (int (*) ()) 0, 80, 2, 9},
	{'C', "/dev/rdsk/dos", 0L, 16, 0, (int (*) ()) 0, 0, 0, 0},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* DELL */

#ifdef ISC
struct device devices[] = {
	{'A', "/dev/rdsk/f0d9dt", 0L, 12, 0, (int (*) ()) 0, 40, 2, 9},
	{'A', "/dev/rdsk/f0q15dt", 0L, 12, 0, (int (*) ()) 0, 80, 2, 15},
	{'A', "/dev/rdsk/f0d8dt", 0L, 12, 0, (int (*) ()) 0, 40, 2, 8},
	{'B', "/dev/rdsk/f13ht", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18},
	{'B', "/dev/rdsk/f13dt", 0L, 12, 0, (int (*) ()) 0, 80, 2, 9},
	{'C', "/dev/rdsk/0p1", 0L, 16, 0, (int (*) ()) 0, 0, 0, 0},
	{'D', "/usr/vpix/defaults/C:", 8704L, 12, 0, (int (*) ()) 0, 0, 0, 0},
	{'E', "$HOME/vpix/C:", 8704L, 12, 0, (int (*) ()) 0, 0, 0, 0},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* ISC */

#ifdef SPARC
struct device devices[] = {
	{'A', "/dev/rfd0c", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18},
	{'A', "/dev/rfd0c", 0L, 12, 0, (int (*) ()) 0, 80, 2, 9},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* SPARC */
#ifdef RT_ACIS
struct device devices[] = {
      {'A', "/dev/rfd0", 0L, 12, 0, (int (*) ()) 0, 80, 2, 15},
      {'A', "/dev/rfd0", 0L, 12, 0, (int (*) ()) 0, 40, 2, 9},
      {'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* RT_ACIS */


#ifdef UNIXPC
#include <sys/gdioctl.h>
#include <fcntl.h>

int init_unixpc();

struct device devices[] = {
	{'A', "/dev/rfp020", 0L, 12, O_NDELAY, init_unixpc, 40, 2, 9},
	{'C', "/usr/bin/DOS/dvd000", 0L, 12, 0, (int (*) ()) 0, 0, 0, 0},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};

int
init_unixpc(fd, ntracks, nheads, nsect)
int fd, ntracks, nheads, nsect;
{
	struct gdctl gdbuf;

	if (ioctl(fd, GDGETA, &gdbuf) == -1) {
		ioctl(fd, GDDISMNT, &gdbuf);
		return(1);
	}

	gdbuf.params.cyls = ntracks;
	gdbuf.params.heads = nheads;
	gdbuf.params.psectrk = nsect;

	gdbuf.params.pseccyl = gdbuf.params.psectrk * gdbuf.params.heads;
	gdbuf.params.flags = 1;		/* disk type flag */
	gdbuf.params.step = 0;		/* step rate for controller */
	gdbuf.params.sectorsz = 512;	/* sector size */

	if (ioctl(fd, GDSETA, &gdbuf) < 0) {
		ioctl(fd, GDDISMNT, &gdbuf);
		return(1);
	}
	return(0);
}
#endif /* UNIXPC */

#ifdef RT_ACIS
struct device devices[] = {
	{'A', "/dev/rfd0", 0L, 12, 0, (int (*) ()) 0, 80, 2, 15},
	{'A', "/dev/rfd0", 0L, 12, 0, (int (*) ()) 0, 40, 2, 9},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* RT_ACIS */

#ifdef SUN386
struct device devices[] = {
	{'A', "/dev/rfdl0c", 0L, 12, 0, (int (*) ()) 0, 80, 2, 9},
	{'A', "/dev/rfd0c", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* SUN386 */

  
#ifdef SPARC_ODD
#include <sys/types.h>
#include <sun/dkio.h>
#include <fcntl.h>

int init_sparc();

struct device devices[] = {
	{'A', "/dev/rfd0c", 0L, 12, 0, init_sparc, 80, 2, 0},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};

/*
 * Stuffing back the floppy parameters into the driver allows for gems
 * like 10 sector or single sided floppies from Atari ST systems.
 * 
 * Martin Schulz, Universite de Moncton, N.B., Canada, March 11, 1991.
 */

int
init_sparc(fd, ntracks, nheads, nsect)
int fd, ntracks, nheads, nsect;
{
	struct fdk_char dkbuf;
	struct dk_map   dkmap;

	if (ioctl(fd, FDKIOGCHAR, &dkbuf) != 0) {
		ioctl(fd, FDKEJECT, NULL);
		return(1);
	}

	if (ioctl(fd, DKIOCGPART, &dkmap) != 0) {
		ioctl(fd, FDKEJECT, NULL);
		return(1);
	}

	if (ntracks)
		dkbuf.ncyl = ntracks;
	if (nheads)
		dkbuf.nhead = nheads;
	if (nsect)
		dkbuf.secptrack = nsect;

	if (ntracks && nheads && nsect )
		dkmap.dkl_nblk = ntracks * nheads * nsect;

	if (ioctl(fd, FDKIOSCHAR, &dkbuf) != 0) {
		ioctl(fd, FDKEJECT, NULL);
		return(1);
	}

	if (ioctl(fd, DKIOCSPART, &dkmap) != 0) {
		ioctl(fd, FDKEJECT, NULL);
		return(1);
	}
	return(0);
}
#endif /* SPARC_ODD */
  
#ifdef XENIX
struct device devices[] = {
	{'A', "/dev/fd096ds15", 0L, 12, 0, (int (*) ()) 0, 80, 2, 15},
	{'A', "/dev/fd048ds9", 0L, 12, 0, (int (*) ()) 0, 40, 2, 9},
	{'B', "/dev/fd1135ds18", 0L, 12, 0, (int (*) ()) 0, 80, 2, 18},
	{'B', "/dev/fd1135ds9", 0L, 12, 0, (int (*) ()) 0, 80, 2, 9},
	{'C', "/dev/hd0d", 0L, 16, 0, (int (*) ()) 0, 0, 0, 0},
	{'\0', (char *) NULL, 0L, 0, 0, (int (*) ()) 0, 0, 0, 0}
};
#endif /* XENIX */

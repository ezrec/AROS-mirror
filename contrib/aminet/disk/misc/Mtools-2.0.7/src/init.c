/*
 * Initialize an MSDOS diskette.  Read the boot sector, and switch to the
 * proper floppy disk device to match the format on the disk.  Sets a bunch
 * of global variables.  Returns 0 on success, or 1 on failure.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "msdos.h"
#include "amiga_devices.h"

#include "mtools.h"

struct device_data * ddata = 0;

#ifdef AMIGA
#define DEFAULT_CACHE_SIZE 128
#else
#define FULL_CYL
#define DEFAULT_CACHE_SIZE 1
#endif

#define WORD(x) ((boot->x)[0] + ((boot->x)[1] << 8))
#define DWORD(x) ((boot->x)[0] + ((boot->x)[1] << 8) + ((boot->x)[2] << 16) + ((boot->x)[3] << 24))

unsigned int num_clus;			/* total number of cluster */
int num_fat;				/* the number of FAT tables */
long disk_offset;			/* skip this many bytes */
int fat_bits;				/* the FAT encoding scheme */

extern int fd, fat_len, dir_len, dir_start, clus_size, dir_dirty, disk_dirty;
extern int fat_error, disk_size;
extern long disk_current;
extern char *mcwd;
extern unsigned char *fat_buf, *disk_buf, *dir_buf;
extern struct device devices[];
static struct bootsector *read_boot();

void free_amiga_device(void) {
	
	if (ddata != 0) {
		free_device (ddata);
		ddata = 0;
	}
}

int
init(drive, mode)
char drive;
int mode;
{
	int fat_start, tracks, heads, sectors, old_dos;
	char buf[256], *malloc(), *name, *expand();
	void perror(), exit(), reset_chain(), free(), fat_read();
	struct bootsector *boot;
	struct device *dev;

	if (fd != -1) {
		Close(fd);
		free((char *) fat_buf);
		free((char *) disk_buf);
		free((char *) dir_buf);
	}
	
	free_amiga_device();
	
					/* check out the drive letter */
	dev = devices;
	while (dev->drive) {
		if (dev->drive == drive)
			break;
		dev++;
	}
	if (!dev->drive) {
		fprintf(stderr, "Drive '%c:' not supported\n", drive);
		return(1);
	}
					/* open the device */
	while (dev->name) {
		if (dev->drive != drive)
			break;

		if (dev->heads >= 0) {
			name = expand(dev->name);
			if ((fd = Open(name, mode | dev->mode)) < 0) {
				sprintf(buf, "init: open \"%s\"", name);
				perror(buf);
				exit(1);
			}
					/* lock the device on writes */
			if (mode == 2 && lock_dev(fd)) {
				fprintf(stderr, "Device \"%s\" is busy\n", dev->name);
				exit(1);
			}
						/* set default parameters, if needed */
			if (dev->gioctl) {
				if ((*(dev->gioctl)) (fd, dev->tracks, dev->heads, dev->sectors))
					goto try_again;
			}
		} else {
			/* open amiga device!! */
			
			ddata = alloc_device  (dev->name, dev->mode, 0, sizeof(struct IOStdReq));
			
			if (ddata == NULL) {
				fprintf(stderr, "unable to open \"%s\" \n", dev->name);
				exit(1);
			}
			
			fd = 1;
			
			atexit( free_amiga_device );
		}
		
					/* read the boot sector */
		disk_offset = dev->offset;
		if ((boot = read_boot()) == NULL)
			goto try_again;

		heads = WORD(nheads);
		sectors = WORD(nsect);
		if (heads && sectors)
			tracks = WORD(psect) / (unsigned) (heads * sectors);

					/* sanity checking */
		old_dos = 0;
		if (!heads || heads > 100 || !sectors || sectors > 500 || tracks > 5000 || !boot->clsiz) {
			/*
			 * The above technique will fail on diskettes that
			 * have been formatted with very old MSDOS, so we
			 * resort to the old table-driven method using the
			 * media signature (first byte in FAT).
			 */
			unsigned char temp[MSECTOR_SIZE];
			
			if (ddata != 0) {
				if (MSECTOR_SIZE != (int) device_read( ddata, (ulong) disk_offset+MSECTOR_SIZE, (ulong) MSECTOR_SIZE, temp) ) {
					temp[0] = '0';
				}
			}
			else {
				if (Read(fd, (char *) temp, MSECTOR_SIZE) != MSECTOR_SIZE) {
					temp[0] = '0';
				}
			}

			switch (temp[0]) {
				case 0xfe:	/* 160k */
					tracks = 40;
					sectors = 8;
					heads = 1;
					dir_start = 3;
					dir_len = 4;
					clus_size = 1;
					fat_len = 1;
					num_clus = 313;
					break;
				case 0xfc:	/* 180k */
					tracks = 40;
					sectors = 9;
					heads = 1;
					dir_start = 5;
					dir_len = 4;
					clus_size = 1;
					fat_len = 2;
					num_clus = 351;
					break;
				case 0xff:	/* 320k */
					tracks = 40;
					sectors = 8;
					heads = 2;
					dir_start = 3;
					dir_len = 7;
					clus_size = 2;
					fat_len = 1;
					num_clus = 315;
					break;
				case 0xfd:	/* 360k */
					tracks = 40;
					sectors = 9;
					heads = 2;
					dir_start = 5;
					dir_len = 7;
					clus_size = 2;
					fat_len = 2;
					num_clus = 354;
					break;
				default:
					fprintf(stderr, "Probable non-MSDOS disk\n");
					Close(fd);
					fd = -1;
					return(1);
			}
			fat_start = 1;
			num_fat = 2;
			old_dos = 1;
		}
					/* check the parameters */
		if (dev->tracks && !dev->gioctl) {
			if (dev->tracks == tracks && dev->heads == heads && dev->sectors == sectors)
				break;
		}
		else
			break;

try_again:	
		free_amiga_device();
		Close(fd);
		fd = -1;
		dev++;
	}
	if (fd == -1) {
		if (boot != NULL && dev->tracks)
			fprintf(stderr, "No support for %d tracks, %d heads, %d sector diskettes\n", tracks, heads, sectors);
		return(1);
	}
					/* set new parameters, if needed */
	if (dev->gioctl) {
		if ((*(dev->gioctl)) (fd, tracks, heads, sectors)) {
			fprintf(stderr, "Can't set disk parameters\n");
			Close(fd);
			fd = -1;
			return(1);
		}
	}

	/*
	 * all numbers are in sectors, except num_clus (which is in clusters)
	 */
	if (!old_dos) {
		clus_size = boot->clsiz;
		fat_start = WORD(nrsvsect);
		fat_len = WORD(fatlen);
		dir_start = fat_start + (boot->nfat * fat_len);
		dir_len = WORD(dirents) * MDIR_SIZE / (unsigned) MSECTOR_SIZE;
		/*
		 * For DOS partitions > 32M
		 */
		if (WORD(psect) == 0)
			num_clus = (unsigned int) (DWORD(bigsect) - dir_start - dir_len) / clus_size;
		else
			num_clus = (unsigned int) (WORD(psect) - dir_start - dir_len) / clus_size;
		num_fat = boot->nfat;
	}
					/* more sanity checking */
	if (clus_size * MSECTOR_SIZE > MAX_CLUSTER) {
		fprintf(stderr, "Cluster size of %d is larger than max of %d\n", clus_size * MSECTOR_SIZE, MAX_CLUSTER);
		Close(fd);
		fd = -1;
		return(1);
	}
	if (!old_dos && WORD(secsiz) != MSECTOR_SIZE) {
		fprintf(stderr, "Sector size of %d is not supported\n", WORD(secsiz));
		Close(fd);
		fd = -1;
		return(1);
	}
					/* full cylinder buffering */

#ifdef FULL_CYL
	disk_size = (dev->sectors) ? (sectors * heads) : DEFAULT_CACHE_SIZE;
#else /* FULL_CYL */
	disk_size = (dev->sectors) ? sectors : DEFAULT_CACHE_SIZE;
#endif /* FULL_CYL */

/* printf ("disk size: %d\n",disk_size); */

	disk_buf = (unsigned char *) malloc((unsigned int) disk_size * MSECTOR_SIZE);
	if (disk_buf == NULL) {
		perror("init: malloc");
		exit(1);
	}
					/* read the FAT sectors */
	disk_current = -1000L;
	disk_dirty = 0;
	fat_error = 0;
	fat_bits = dev->fat_bits;
	fat_read(fat_start);
					/* set dir_chain[] to root directory */
	dir_dirty = 0;
	reset_chain(NEW);
	return(0);
}

/*
 * Fix the info in the MCWD file to be a proper directory name.  Always
 * has a leading separator.  Never has a trailing separator (unless it is
 * the path itself).
 */

char *
fix_mcwd()
{
	FILE *fp;
	struct stat sbuf;
	char *s, *strcpy(), *strcat(), *mcwd_path, *getenv(), *strncpy();
	char buf[BUFSIZ], *file, *expand();
	static char ans[MAX_PATH];
	long now;// time();

	mcwd_path = getenv("MCWD");
	if (mcwd_path == NULL || *mcwd_path == '\0')
		mcwd_path = "$HOME/.mcwd";

	file = expand(mcwd_path);
	if (stat(file, &sbuf) < 0)
		return("A:/");
	/*
	 * Ignore the info, if the file is more than 6 hours old
	 */
	time(&now);
	if (now - sbuf.st_mtime > 6 * 60 * 60) {
		fprintf(stderr, "Warning: \"%s\" is out of date, contents ignored\n", file);
		return("A:/");
	}
	
	if (!(fp = fopen(file, "r")))
		return("A:/");

	if (!fgets(buf, BUFSIZ, fp))
		return("A:/");

	buf[strlen(buf) -1] = '\0';
	fclose(fp);
					/* drive letter present? */
	s = buf;
	if (buf[0] && buf[1] == ':') {
		strncpy(ans, buf, 2);
		ans[2] = '\0';
		s = &buf[2];
	}
	else 
		strcpy(ans, "A:");
					/* add a leading separator */
	if (*s != '/' && *s != '\\') {
		strcat(ans, "/");
		strcat(ans, s);
	}
	else
		strcat(ans, s);
					/* translate to upper case */
	for (s = ans; *s; ++s) {
		if (islower(*s))
			*s = toupper(*s);
		if (*s == '\\')
			*s = '/';
	}
					/* if only drive, colon, & separator */
	if (strlen(ans) == 3)
		return(ans);
					/* zap the trailing separator */
	if (*--s == '/')
		*s = '\0';
	return(ans);
}

/*
 * Read the boot sector.  We glean the disk parameters from this sector.
 */

static struct bootsector *
read_boot()
{
	long lseek();
	static struct bootsector boot;
	
	if (ddata != 0) {
		if (MSECTOR_SIZE != (int) device_read( ddata, (ulong) disk_offset, (ulong) MSECTOR_SIZE, &boot) ) {
			return(NULL);
		}
	}
	else {
		if (lseek(fd, disk_offset, 0) < 0) {
			return(NULL);
		}
		/* read the first sector */
		if (Read(fd, (char *) &boot, MSECTOR_SIZE) != MSECTOR_SIZE) {
			return(NULL);
		}
	}

	return(&boot);
}

/*
 * Create an advisory lock on the device to prevent concurrent writes.
 * Uses either lockf, flock, or fcntl locking methods.  See the Makefile
 * and the Configure files for how to specify the proper method.
 */

static int
lock_dev(fd)
int fd;
{
#ifdef LOCKF
#include <unistd.h>

	if (lockf(fd, F_TLOCK, 0) < 0)
		return(1);
#endif /* LOCKF */

#ifdef FLOCK
#include <sys/file.h>

	if (flock(fd, LOCK_EX|LOCK_NB) < 0)
		return(1);
#endif /* FLOCK */

#ifdef FCNTL
#include <fcntl.h>
	struct flock flk;

	flk.l_type = F_WRLCK;
	flk.l_whence = 0;
	flk.l_start = 0L;
	flk.l_len = 0L;

	if (fcntl(fd, F_SETLK, &flk) < 0)
		return(1);
#endif /* FCNTL */
	return(0);
}

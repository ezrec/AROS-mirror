/*
 * Do full cylinder buffered reads from slow devices.  Uses a simple
 * buffered read/delayed write algorithm.
 */

#include <stdio.h>
#include "msdos.h"
#include "amiga_devices.h"

#include "mtools.h"

extern struct device_data * ddata;

unsigned char *disk_buf;		/* disk read/write buffer */
int disk_size;				/* size of read/write buffer */
long disk_current;			/* first sector in buffer */
int disk_dirty;				/* is the buffer dirty? */

extern int fd;
extern long disk_offset;

void
disk_read(start, buf, len)
long start;
unsigned char *buf;
int len;
{
	register long i;
	int length;
	unsigned char *buf_ptr, *disk_ptr;
	char *memcpy();
	long where, tail, lseek();
	void perror(), exit(), disk_flush();

	
					/* don't use cache? */
	if (disk_size == 1) {
		where = (start * MSECTOR_SIZE) + disk_offset; 
		
		
		if (ddata != 0) {
			if (len != (int) device_read( ddata, (ulong) where, (ulong) len, buf) ) {
				perror("disk_read: read");
				exit(1);
			}
		}
		else {
			if (lseek(fd, where, 0) < 0) {
				perror("disk_read: lseek");
				exit(1);
			}
						/* read it! */
			if (Read(fd, (char *) buf, (unsigned int) len) != len) {
				perror("disk_read: read");
				exit(1);
			}
		}
		return;
	}

	tail = start + (len / MSECTOR_SIZE) - 1;
	for (i = start; i <= tail; i++) {
					/* a "cache" miss */
		if (i < disk_current || i >= disk_current + disk_size) {

			if (disk_dirty)
				disk_flush();

			disk_current = (i / disk_size) * disk_size;
			where = (disk_current * MSECTOR_SIZE) + disk_offset;
			length = disk_size * MSECTOR_SIZE;

/* printf ("read it where: %d len: %d\n", where, length); */
			if (ddata != 0) {
				if (length != (int) device_read( ddata, (ulong) where, (ulong) length, disk_buf) ) {
					perror("disk_read: read");
					exit(1);
				}
			}
			else {
						/* move to next location */
				if (lseek(fd, where, 0) < 0) {
					perror("disk_read: lseek");
					exit(1);
				}
					/* read it! */
				if (Read(fd, (char *) disk_buf, (unsigned int) length) != length) {
					perror("disk_read: read");
					exit(1);
				}
			}
		}
					/* a cache hit... */
		buf_ptr = buf + ((i - start) * MSECTOR_SIZE);
		disk_ptr = disk_buf + ((i - disk_current) * MSECTOR_SIZE);
		memcpy((char *) buf_ptr, (char *) disk_ptr, MSECTOR_SIZE);
	}
	return;
}

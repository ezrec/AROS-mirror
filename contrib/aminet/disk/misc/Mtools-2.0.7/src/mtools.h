
// definitions needed to get aros to build mtools



#include <proto/exec.h>
#include <proto/dos.h>

#ifdef __AROS__
#define AMIGA
#define BSD
#undef MKDIR
#endif

#include <stdio.h>

#include <strings.h>
#include <stdlib.h> 

/* init.c */

void            free_amiga_device(void);
int             init(char drive, int mode);
char            *fix_mcwd(void);
struct          bootsector *read_boot(void);
int             lock_dev(int fd);

/* buf_read.c */

void            disk_read(long start, unsigned char *buf, int len);

/* buf_write.c */

void            disk_write(long start, unsigned char *buf, int len);
void            disk_flush(void);
int             read_cyl(long sector);

/* subdir.c */

int             subdir(char drive, char *pathname);
int             descend(char *path);

/* match.c */

int             match(char *s,char *p);

/* dir_read.c */

struct          directory *dir_read(int num);
int             fill_chain(unsigned int num);   
void            reset_chain(int code);   
char            *unix_name(unsigned char *name, unsigned char *ext);

/* is_dir.c */

int             is_dir(char *path);

/* mcopy.c */

char *          unixname(char *filename);
int             chain(int argc, char *argv[]);

/* fat_free.c */

int             fat_free(unsigned int fat);

/* fat_write.c */

int             fat_encode(unsigned int num, unsigned int code);
void            fat_write(void);

/* fat_read.c */

unsigned int    fat_decode(unsigned int num);

/* dir_make.c */

int             dir_grow(unsigned int fat);

/* file_read.c */

int             file_read(FILE *fp, unsigned int fat, int textmode, int stripmode, long size);



/*----------------------------------------------------------------------*/
/*		LHarc Archiver Driver for UNIX				*/
/*									*/
/*		Copyright(C) MCMLXXXIX  Yooichi.Tagawa			*/
/*									*/
/*  V1.00  Fixed				1989.09.22  Y.Tagawa	*/
/*  V0.03  LHa for UNIX				1991.12.18  M.Oki	*/
/*----------------------------------------------------------------------*/

#ifndef _LHARC_H_
#define _LHARC_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "stat.h"

/*----------------------------------------------------------------------*/
/*			DIRECTORY ACCESS STUFF				*/
/*----------------------------------------------------------------------*/
#include <dirent.h>
#define DIRENTRY	struct dirent
#define NAMLEN(p)	strlen (p->d_name)

#include <unistd.h>
extern struct interfacing interface;
/*----------------------------------------------------------------------*/
/*				FILE ATTRIBUTES				*/
/*----------------------------------------------------------------------*/

/* If file mode is not compatible between your Machine/OS and
   LHarc standard UNIX file mode.
   (See UNIX Manual stat(1), <sys/stat.h>,
   and/or below UNIX_* definitions. ) */
/* #define NOT_COMPATIBLE_MODE */
#define is_directory(statp)	(((statp)->st_mode & S_IFMT) == S_IFDIR)
#define is_regularfile(statp)	(((statp)->st_mode & S_IFMT) == S_IFREG)

/* #define WRITE_BINARY	"wb" */
/* #define READ_BINARy	"rb" */
#define WRITE_BINARY	"w"
#define READ_BINARY	"r"

/*----------------------------------------------------------------------*/
/*			MEMORY AND STRING FUNCTIONS			*/
/*----------------------------------------------------------------------*/

#ifndef USG
#include <strings.h>
#else
#include <string.h>
#endif	/* USG */

#ifdef AROS
#define index strchr
#define rindex strrchr
#endif

#ifdef AROS
#define bcmp(a,b,n) memcmp ((a),(b),(n))
#define bzero(d,n) memset((d),0,(n))
#define bcopy(s,d,n) memmove((d),(s),(n))
#endif	/* NOBSTRING */

#define strucmp(p,q) stricmp((p),(q))

/*----------------------------------------------------------------------*/
/*				YOUR CUSTOMIZIES			*/
/*----------------------------------------------------------------------*/
/* These definitions are changable to you like. 			*/
/* #define ARCHIVENAME_EXTENTION		".LZH"			*/
/* #define BACKUPNAME_EXTENTION			".BAK"			*/
/* #define TMP_FILENAME_TEMPLATE		"/usr/tmp/lhXXXXXX"	*/
/* #define MULTIBYTE_CHAR						*/
/* #define USE_PROF							*/

#ifndef ARCHIVENAME_EXTENTION
#define ARCHIVENAME_EXTENTION	".lzh"
#endif
#ifndef BACKUPNAME_EXTENTION
#define BACKUPNAME_EXTENTION	".bak"
#endif
#ifndef TMP_FILENAME_TEMPLATE
#define TMP_FILENAME_TEMPLATE	"/tmp/lhXXXXXX"
#endif

#define SJC_FIRST_P(c)			\
  (((unsigned char)(c) >= 0x80) &&	\
   (((unsigned char)(c) < 0xa0) ||	\
    ((unsigned char)(c) >= 0xe0) &&	\
    ((unsigned char)(c) < 0xfd)))
#define SJC_SECOND_P(c)			\
  (((unsigned char)(c) >= 0x40) &&	\
   ((unsigned char)(c) < 0xfd) &&	\
   ((ungigned char)(c) != 0x7f))

#ifdef MULTIBYTE_CHAR
#define MULTIBYTE_FIRST_P	SJC_FIRST_P
#define MULTIBYTE_SECOND_P	SJC_SECOND_P
#endif /* MULTIBYTE_CHAR */

/*----------------------------------------------------------------------*/
/*			OTHER DIFINITIONS				*/
/*----------------------------------------------------------------------*/

/* Your C-Compiler has no 'void' */
#ifdef NO_VOID
#define void
#endif

#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

#define FILENAME_LENGTH	1024


/* non-integral functions */
extern struct tm *localtime ();
extern char *getenv ();

#ifndef FALSE
#define	FALSE	0
#define TRUE	1
#endif
typedef int boolean;


/*----------------------------------------------------------------------*/
/*			LHarc FILE DEFINITIONS				*/
/*----------------------------------------------------------------------*/
#define METHOD_TYPE_STRAGE	5
#define LZHUFF0_METHOD		"-lh0-"
#define LZHUFF1_METHOD		"-lh1-"
#define LZHUFF2_METHOD		"-lh2-"
#define LZHUFF3_METHOD		"-lh3-"
#define LZHUFF4_METHOD		"-lh4-"
#define LZHUFF5_METHOD		"-lh5-"
#define LARC4_METHOD		"-lz4-"
#define LARC5_METHOD		"-lz5-"
#define LZHDIRS_METHOD		"-lhd-"

#define I_HEADER_SIZE			0
#define I_HEADER_CHECKSUM		1
#define I_METHOD			2
#define I_PACKED_SIZE			7
#define I_ORIGINAL_SIZE			11
#define I_LAST_MODIFIED_STAMP		15
#define I_ATTRIBUTE			19
#define I_HEADER_LEVEL			20
#define I_NAME_LENGTH			21
#define I_NAME				22

#define I_CRC				22 /* + name_length */
#define I_EXTEND_TYPE			24 /* + name_length */
#define I_MINOR_VERSION			25 /* + name_length */
#define I_UNIX_LAST_MODIFIED_STAMP	26 /* + name_length */
#define I_UNIX_MODE			30 /* + name_length */
#define I_UNIX_UID			32 /* + name_length */
#define I_UNIX_GID			34 /* + name_length */
#define I_UNIX_EXTEND_BOTTOM		36 /* + name_length */

#define I_GENERIC_HEADER_BOTTOM		I_EXTEND_TYPE



#define EXTEND_GENERIC		0
#define EXTEND_UNIX		'U'
#define EXTEND_MSDOS		'M'
#define EXTEND_MACOS		'm'
#define EXTEND_OS9		'9'
#define EXTEND_OS2		'2'
#define EXTEND_OS68K		'K'
#define EXTEND_OS386		'3'		/* OS-9000??? */
#define EXTEND_HUMAN		'H'
#define EXTEND_CPM		'C'
#define EXTEND_FLEX		'F'
#define EXTEND_RUNSER		'R'
/*	this OS type is not official */
#define EXTEND_TOWNSOS		'T'
#define EXTEND_XOSK		'X'
/*------------------------------*/

#define GENERIC_ATTRIBUTE			0x20
#define GENERIC_DIRECTORY_ATTRIBUTE		0x10
#define HEADER_LEVEL0				0x00
#define HEADER_LEVEL1				0x01
#define HEADER_LEVEL2				0x02

#define CURRENT_UNIX_MINOR_VERSION		0x00

#define DELIM ('/')
#define DELIM2 (0xff)
#define DELIMSTR "/"


typedef struct LzHeader {
  unsigned char		header_size;
  char			method[METHOD_TYPE_STRAGE];
  long			packed_size;
  long			original_size;
  long			last_modified_stamp;
  unsigned char	attribute;
  unsigned char header_level;
  char			name[256];
  unsigned short	crc;
  boolean		has_crc;
  unsigned char		extend_type;
  unsigned char		minor_version;
  /*  extend_type == EXTEND_UNIX  and convert from other type. */
  time_t		unix_last_modified_stamp;
  unsigned short	unix_mode;
  unsigned short	unix_uid;
  unsigned short	unix_gid;
} LzHeader;

#define OSK_RW_RW_RW			0000033
#define OSK_FILE_REGULAR		0000000
#define OSK_DIRECTORY_PERM		0000200
#define OSK_SHARED_PERM			0000100
#define OSK_OTHER_EXEC_PERM		0000040
#define OSK_OTHER_WRITE_PERM		0000020
#define OSK_OTHER_READ_PERM		0000010
#define OSK_OWNER_EXEC_PERM		0000004
#define OSK_OWNER_WRITE_PERM		0000002
#define OSK_OWNER_READ_PERM		0000001

#define UNIX_FILE_TYPEMASK		0170000
#define UNIX_FILE_REGULAR		0100000
#define UNIX_FILE_DIRECTORY		0040000
#define UNIX_SETUID			0004000
#define UNIX_SETGID			0002000
#define UNIX_STYCKYBIT			0001000
#define UNIX_OWNER_READ_PERM		0000400
#define UNIX_OWNER_WRITE_PERM		0000200
#define UNIX_OWNER_EXEC_PERM		0000100
#define UNIX_GROUP_READ_PERM		0000040
#define UNIX_GROUP_WRITE_PERM		0000020
#define UNIX_GROUP_EXEC_PERM		0000010
#define UNIX_OTHER_READ_PERM		0000004
#define UNIX_OTHER_WRITE_PERM		0000002
#define UNIX_OTHER_EXEC_PERM		0000001
#define UNIX_RW_RW_RW			0000666

#define LZHEADER_STRAGE			4096

#ifndef AROS
#define GETSTAT lstat
#else
#define GETSTAT stat
#endif

/* used by qsort() for alphabetic-sort */
#define STRING_COMPARE(a,b) strcmp((a),(b))

struct string_pool {
  int used;
  int size;
  int n;
  char *buffer;
};


/*----------------------------------------------------------------------*/
/*				OPTIONS					*/
/*----------------------------------------------------------------------*/

/* command line options (common options) */
extern boolean	quiet;
extern boolean	text_mode;
extern boolean	verbose;
extern boolean	noexec;		/* debugging option */
extern boolean	force;
extern boolean	prof;
extern boolean	delete_after_append;

/* list command flags */
extern boolean	verbose_listing;

/* extract/print command flags */
extern boolean	output_to_stdout;

/* add/update/delete command flags */
extern boolean	new_archive;
extern boolean	update_if_newer;
extern boolean	generic_format;


/*----------------------------------------------------------------------*/
/*				VARIABLES				*/
/*----------------------------------------------------------------------*/

extern char	**cmd_filev;
extern int	cmd_filec;

extern char	*archive_name;
extern char	expanded_archive_name[FILENAME_LENGTH];
extern char	temporary_name[FILENAME_LENGTH];
extern char	backup_archive_name[FILENAME_LENGTH];

extern char *reading_filename, *writting_filename;
extern boolean	remove_temporary_at_error;
extern boolean	recover_archive_when_interrupt;
extern boolean	remove_extracting_file_when_interrupt;

extern int	archive_file_mode;
extern int	archive_file_gid;

/*----------------------------------------------------------------------*/
/*				Functions				*/
/*----------------------------------------------------------------------*/

#ifdef __STDC__
/* Prototypes for functions defined in append.c */

int encode_lzhuf(FILE *infp,
                 FILE *outfp,
                 long size,
                 long *original_size_var,
                 long *packed_size_var,
                 char *name,
                 char *hdr_method);
void start_indicator(char *name,
                     long size,
                     char *msg,
                     long def_indicator_threshold);
void finish_indicator2(char *name,
                       char *msg,
                       int pcnt);
void finish_indicator(char *name,
                      char *msg);


/* Prototypes for functions defined in crcio.c */

void make_crctable(void);
unsigned short calccrc(unsigned char *p,
                       unsigned int n);
void fillbuf(unsigned char n);
unsigned short getbits(unsigned char n);
void putcode(unsigned char n,
             unsigned short x);
void putbits(unsigned char n,
             unsigned short x);
int fread_crc(unsigned char *p,
              int n,
              FILE *fp);
void fwrite_crc(unsigned char *p,
                int n,
                FILE *fp);
void init_code_cache(void);
void init_getbits(void);
void init_putbits(void);
int fwrite_txt(unsigned char *p,
               int n,
               FILE *fp);
int fread_txt(unsigned char *p,
              int n,
              FILE *fp);


/* Prototypes for functions defined in dhuf.c */

void start_c_dyn(void);
void decode_start_dyn(void);
unsigned short decode_c_dyn(void);
unsigned short decode_p_dyn(void);
void output_dyn(int code,
                unsigned int pos);
void encode_end_dyn(void);


/* Prototypes for functions defined in extract.c */

int decode_lzhuf(FILE *infp,
                 FILE *outfp,
                 long original_size,
                 long packed_size,
                 char *name,
                 int method);


/* Prototypes for functions defined in header.c */

int calc_sum(char *p,
             int len);
boolean get_header(FILE *fp,
                   LzHeader *hdr);
void init_header(char *name,
                 struct stat *v_stat,
                 LzHeader *hdr);
void write_header(FILE *nafp,
                  LzHeader *hdr);


/* Prototypes for functions defined in huf.c */

void output_st1(int c,
                int p);
unsigned char *alloc_buf(void);
void encode_start_st1(void);
void encode_end_st1(void);
unsigned short decode_c_st1(void);
unsigned short decode_p_st1(void);
void decode_start_st1(void);


/* Prototypes for functions defined in larc.c */

unsigned short decode_c_lzs(void);
unsigned short decode_p_lzs(void);
void decode_start_lzs(void);
unsigned short decode_c_lz5(void);
unsigned short decode_p_lz5(void);
void decode_start_lz5(void);


/* Prototypes for functions defined in lhadd.c */

FILE *append_it(char *name,
                 FILE *oafp,
                 FILE *nafp);
void temporary_to_new_archive_file(long new_archive_size);
void cmd_add(void);
void cmd_delete(void);


/* Prototypes for functions defined in lharc.c */

void main(int argc,
          char **argv);
void message(char *subject,
             char *name);
void warning(char *subject,
             char *name);
void error(char *subject,
           char *msg);
void fatal_error(char *msg);
void write_error(void);
void read_error(void);
void interrupt(int signo);
char *xmalloc(int size);
char *xrealloc(char *old,
                int size);
void init_sp(struct string_pool *sp);
void add_sp(struct string_pool *sp,
            char *name,
            int len);
void finish_sp(struct string_pool *sp,
               int *v_count,
               char ***v_vector);
void free_sp(char **vector);
void cleaning_files(int *v_filec,
                    char ***v_filev);
boolean find_files(char *name,
                   int *v_filec,
                   char ***v_filev);
void free_files(int filec,
                char **filev);
void build_temporary_name(void);
void build_backup_name(char *buffer,
                       char *original);
void build_standard_archive_name(char *buffer,
                                 char *orginal);
boolean need_file(char *name);
FILE *xfopen(char *name,
              char *mode);
FILE *open_old_archive(void);
int inquire(char *msg,
            char *name,
            char *selective);
void write_archive_tail(FILE *nafp);
void copy_old_one(FILE *oafp,
                  FILE *nafp,
                  LzHeader *hdr);


/* Prototypes for functions defined in lhdir.c */

#ifndef SYSV_SYSTEM_DIR
int closedir(DIR *dirp);
#endif


/* Prototypes for functions defined in lhext.c */

void cmd_extract(void);


/* Prototypes for functions defined in lhlist.c */

void cmd_list(void);


/* Prototypes for functions defined in maketbl.c */

void make_table(int nchar,
                unsigned char *bitlen,
                int tablebits,
                unsigned short *table);


/* Prototypes for functions defined in maketree.c */

void make_code(int n,
               unsigned char *len,
               unsigned short *code);
short make_tree(int nparm,
                unsigned short *freqparm,
                unsigned char *lenparm,
                unsigned short *codeparm);


/* Prototypes for functions defined in patmatch.c */

int patmatch(char *p,
             char *s,
             int f);


/* Prototypes for functions defined in shuf.c */

void decode_start_st0(void);
void encode_p_st0(unsigned short j);
void encode_start_fix(void);
void decode_start_fix(void);
unsigned short decode_c_st0(void);
unsigned short decode_p_st0(void);


/* Prototypes for functions defined in slide.c */

int encode_alloc(int method);
void get_next_match(void);
void encode(struct interfacing *interface);
void decode(struct interfacing *interface);


/* Prototypes for functions defined in util.c */

long copyfile(FILE *, FILE *, long, int);
int encode_stored_crc(FILE *, FILE *, long, long *, long *);
unsigned char *convdelim(unsigned char *, unsigned char);
boolean archive_is_msdos_sfx1(char *);
boolean skip_msdos_sfx1_code(FILE *);

#else
extern void interrupt ();

extern void message ();
extern void warning ();
extern void error ();
extern void fatal_error ();

extern boolean need_file ();
extern int inquire ();
extern FILE *xfopen ();

extern boolean find_files ();
extern void free_files ();

extern void init_sp ();
extern void add_sp ();
extern void finish_sp ();
extern void free_sp ();
extern void cleaning_files ();

extern void build_temporary_name ();
extern void build_backup_file_name ();
extern void build_standard_archive_name ();

extern FILE *open_old_archive ();
extern void init_header ();
extern boolean get_header ();
extern boolean archive_is_msdos_sfx1 ();
extern boolean skip_msdos_sfx1_code ();
extern void write_header ();
extern void write_archive_tail ();
extern void copy_old_one ();
extern unsigned char *convdelim ();
extern long copyfile ();

extern void cmd_list (), cmd_extract (), cmd_add (), cmd_delete ();
#endif

extern boolean ignore_directory;
extern boolean compress_method;
extern boolean verify_mode;

extern char *extract_directory;
extern FILE *temporary_fp;

#endif /* _LHARC_H_ */

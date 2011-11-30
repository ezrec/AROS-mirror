/** xproto.h
 *
 *   Include file for External Protocol Handling
 *   Updated to XPR3 level including 2.001 extensions 
 *   by Robert Williamson 
 *
 **/
struct XPR_UPDATE;
struct xpr_option;
/*
   *   The structure
 */
struct XPR_IO
  {
    /* File name(s) */
    char *xpr_filename;
    /* Open file */
    BPTR (*__asm xpr_fopen) (
                             register __a0 char *filename,
                             register __a1 char *accessmode);
    /* Close file */
    long (*__asm xpr_fclose) (
                             register __a0 BPTR filepointer);
    /* Get char from file */
    long (*__asm xpr_fread) (
                            register __a0 char *buffer,
                            register __d0 long size,
                            register __d1 long count,
                            register __a1 BPTR fileptr);
    /* Put string to file */
    long (*__asm xpr_fwrite) (
                              register __a0 char *buffer,
                              register __d0 long size,
                              register __d1 long count,
                              register __a1 BPTR fileptr);
    /* Get char from serial */
    long (*__asm xpr_sread) (
                             register __a0 char *buffer,
                             register __d0 long size,
                             register __d1 long timeout);
    /* Put string to serial */
    long (*__asm xpr_swrite) (
                              register __a0 char *buffer,
                              register __d0 long size);
    /* Flush serial input buffer */
    long (*xpr_sflush) (void);  

    /* Print stuff */
    long (*__asm xpr_update) (
                    register __a0 struct XPR_UPDATE * updatestruct);
    /* Check for abort */
    long (*xpr_chkabort) (void);  

    /* Check misc. stuff */
    void (*xpr_chkmisc) (void);  

    /* Get string interactively */
    long (*__asm xpr_gets) (
                            register __a0 char *prompt,
                            register __a1 char *buffer);
    /* Set and Get serial info */
    long (*__asm xpr_setserial) (
                                register __d0 long newstatus);

    /* Find first file name */
    long (*__asm xpr_ffirst) (
                              register __a0 char *buffer,
                              register __a1 char *pattern);
    /* Find next file name */
    long (*__asm xpr_fnext) (
                             register __d0 long oldstate,
                             register __a0 char *buffer,
                             register __a1 char *pattern);
    /* Return file info */
    long (*__asm xpr_finfo) (
                             register __a0 char *filename,
                             register __d0 long typeofinfo);
    /* Seek in a file */
    long (*__asm xpr_fseek) (
                             register __a0 BPTR fileptr,
                             register __d0 long offset,
                             register __d1 long origin);
    /* Number of extensions */
    long xpr_extension;

    /* Initialized by Setup. */
    long *xpr_data;

    /* Multiple XPR options. */
    long (*__asm xpr_options) (
                               register __d0 long n,
                               register __a0
                               struct xpr_option ** opt);
    /* Delete a file. */
    long (*__asm xpr_unlink) (
                              register __a0 char *filename);

    /* Query serial device */
    long (*xpr_squery) (void);  

    /* Get various host ptrs */
    long (*__asm xpr_getptr) (
                              register __d0 long type);
  };
/*
   *   Number of defined extensions
 */
#define XPR_EXTENSION 4L

/*
   *   The functions
 */
#ifndef LATTICE
extern long XProtocolSend (), XProtocolReceive (), XProtocolSetup (), XProtocolCleanup ();
#endif
/*
   *   Flags returned by XProtocolSetup()
 */
#define XPRS_FAILURE    0x00000000L 
#define XPRS_SUCCESS    0x00000001L 

#define XPRS_NORECREQ   0x00000002L 
/* Protocol requires no communications program supplied file */
/* requester for receive: either the XPR supplies the file  */
/* requester or the protocol is capable of receiving file name  */
/* information through other means (e.g., from the host). */

#define XPRS_NOSNDREQ   0x00000004L
/* Protocol requires no communications program supplied file */
/* requester for send: either the XPR supplies the file requester */
/* or the protocol is capable of receiving file name information */
/* through other means. */

#define XPRS_HOSTMON    0x00000008L
/* Communications program is requested to call XProtocolHostMon() */
/* for all serial port input */

#define XPRS_USERMON    0x00000010L
/* Communications program is requested to call XProtocolUserMon() */
/* for all user input */

#define XPRS_HOSTNOWAIT 0x00000020L
/* Communications program is requested to call XProtocolHostMon() */
/* without waiting for serial input */

#define XPRS_NOUPDATE   0x00008000L
/* Communication program should not open transfer status screen */

#define XPRS_XPR2001    0x00010000L
/* Protocol follows XPR2.001 definitions. */
/* (Upload files are always open with mode "r") */

#define XPRS_DOUBLE     0x00020000L
/* Protocol uses internal double buffer, xpr_swrite can */
/* return immediately if previous xpr_swrite is done. */
/* If previous xpr_swrite isn't done you must wait till */
/* it's sent and return after that. Ie. comprog don't */
/* wait current xpr_swrite. */

/*
   *   The update structure
 */
struct XPR_UPDATE
  {
    long xpru_updatemask;
    char *xpru_protocol;
    char *xpru_filename;
    long xpru_filesize;
    char *xpru_msg;
    char *xpru_errormsg;
    long xpru_blocks;
    long xpru_blocksize;
    long xpru_bytes;
    long xpru_errors;
    long xpru_timeouts;
    long xpru_packettype;
    long xpru_packetdelay;
    long xpru_chardelay;
    char *xpru_blockcheck;
    char *xpru_expecttime;
    char *xpru_elapsedtime;
    long xpru_datarate;
    long xpru_status;
    long xpru_reserved2;
    long xpru_reserved3;
    long xpru_reserved4;
    long xpru_reserved5;
  };
/*
   *   The possible bit values for the xpru_updatemask are:
 */
#define XPRU_PROTOCOL           0x00000001L
#define XPRU_FILENAME           0x00000002L
#define XPRU_FILESIZE           0x00000004L
#define XPRU_MSG                0x00000008L
#define XPRU_ERRORMSG           0x00000010L
#define XPRU_BLOCKS             0x00000020L
#define XPRU_BLOCKSIZE          0x00000040L
#define XPRU_BYTES              0x00000080L
#define XPRU_ERRORS             0x00000100L
#define XPRU_TIMEOUTS           0x00000200L
#define XPRU_PACKETTYPE         0x00000400L
#define XPRU_PACKETDELAY        0x00000800L
#define XPRU_CHARDELAY          0x00001000L
#define XPRU_BLOCKCHECK         0x00002000L
#define XPRU_EXPECTTIME         0x00004000L
#define XPRU_ELAPSEDTIME        0x00008000L
#define XPRU_DATARATE           0x00010000L
#define XPRU_STATUS             0x00020000L
#define XPRU_RESERVED2          0x00040000L
#define XPRU_RESERVED3          0x00080000L
#define XPRU_RESERVED4          0x00100000L
#define XPRU_RESERVED5          0x00200000L
#define XPRU_DNLOAD             0x40000000L
#define XPRU_UPLOAD             0x80000000L
/*
   *   The xpro_option structure
 */
struct xpr_option
  {
    char *xpro_description; /* description of the option                  */
    long xpro_type;         /* type of option                             */
    char *xpro_value;       /* pointer to a buffer with the current value */
    long xpro_length;       /* buffer size                                */
  };
/*
   *   Valid values for xpro_type are:
 */
#define XPRO_BOOLEAN 1L     /* xpro_value is "yes", "no", "on" or "off"   */
#define XPRO_LONG    2L     /* xpro_value is string representing a number */
#define XPRO_STRING  3L     /* xpro_value is a string               */
#define XPRO_HEADER  4L     /* xpro_value is ignored                */
#define XPRO_COMMAND 5L     /* xpro_value is ignored                */
#define XPRO_COMMPAR 6L     /* xpro_value contains command parameters     */

/** xproto.h
*
*   Include file for External Protocol Handling
*
**/

#ifndef _COMPILER_H
#define REG(x) register __ ## x
#define ASM __asm
#endif	/* _COMPILER_H */

/*
*   Number of defined extensions
*/
#define XPR_EXTENSION 4L

/*
*   Flags returned by XProtocolSetup()
*/
#define XPRS_FAILURE    0x00000000L
#define XPRS_SUCCESS    0x00000001L
#define XPRS_NORECREQ   0x00000002L
#define XPRS_NOSNDREQ   0x00000004L
#define XPRS_HOSTMON    0x00000008L
#define XPRS_USERMON    0x00000010L
#define XPRS_HOSTNOWAIT 0x00000020L
/*
*   The update structure
*/
struct XPR_UPDATE {     ULONG  xpru_updatemask;
                        STRPTR xpru_protocol;
                        STRPTR xpru_filename;
                        LONG   xpru_filesize;
                        STRPTR xpru_msg;
                        STRPTR xpru_errormsg;
                        LONG   xpru_blocks;
                        LONG   xpru_blocksize;
                        LONG   xpru_bytes;
                        LONG   xpru_errors;
                        LONG   xpru_timeouts;
                        LONG   xpru_packettype;
                        LONG   xpru_packetdelay;
                        LONG   xpru_chardelay;
                        STRPTR xpru_blockcheck;
                        STRPTR xpru_expecttime;
                        STRPTR xpru_elapsedtime;
                        LONG   xpru_datarate;
                        LONG   xpru_reserved1;
                        LONG   xpru_reserved2;
                        LONG   xpru_reserved3;
                        LONG   xpru_reserved4;
                        LONG   xpru_reserved5;
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
/*
*   The xpro_option structure
*/
struct xpr_option {
   STRPTR xpro_description;      /* description of the option                  */
   LONG   xpro_type;             /* type of option                             */
   STRPTR xpro_value;            /* pointer to a buffer with the current value */
   LONG   xpro_length;           /* buffer size                                */
};
/*
*   Valid values for xpro_type are:
*/
#define XPRO_BOOLEAN 1L         /* xpro_value is "yes", "no", "on" or "off"   */
#define XPRO_LONG    2L         /* xpro_value is string representing a number */
#define XPRO_STRING  3L         /* xpro_value is a string                     */
#define XPRO_HEADER  4L         /* xpro_value is ignored                      */
#define XPRO_COMMAND 5L         /* xpro_value is ignored                      */
#define XPRO_COMMPAR 6L         /* xpro_value contains command parameters     */

	/* Structure forward declaration for the GNU `C' compiler. */

#ifdef __GNUC__
struct Buffer;
#endif	/* __GNUC__ */

/*
*   The structure
*/
struct XPR_IO {
                STRPTR        xpr_filename;
                  LONG (* ASM xpr_fopen)(REG(a0) STRPTR,REG(a1) STRPTR);
                  LONG (* ASM xpr_fclose)(REG(a0) struct Buffer *);
                  LONG (* ASM xpr_fread)(REG(a0) APTR,REG(d0) LONG,REG(d1) LONG,REG(a1) struct Buffer *);
                  LONG (* ASM xpr_fwrite)(REG(a0) APTR,REG(d0) LONG,REG(d1) LONG,REG(a1) struct Buffer *);
                  LONG (* ASM xpr_sread)(REG(a0) APTR,REG(d0) ULONG,REG(d1) ULONG);
                  LONG (* ASM xpr_swrite)(REG(a0) APTR,REG(d0) LONG);
                  LONG (*     xpr_sflush)(VOID);
                  LONG (* ASM xpr_update)(REG(a0) struct XPR_UPDATE *);
                  LONG (*     xpr_chkabort)(VOID);
                  LONG (*     xpr_chkmisc)(VOID);
                  LONG (* ASM xpr_gets)(REG(a0) STRPTR,REG(a1) STRPTR);
                  LONG (* ASM xpr_setserial)(REG(d0) LONG);
                  LONG (* ASM xpr_ffirst)(REG(a0) STRPTR,REG(a1) STRPTR);
                  LONG (* ASM xpr_fnext)(REG(d0) LONG,REG(a0) STRPTR,REG(a1) STRPTR);
                  LONG (* ASM xpr_finfo)(REG(a0) STRPTR,REG(d0) LONG);
                  LONG (* ASM xpr_fseek)(REG(a0) struct Buffer *,REG(d0) LONG,REG(d1) LONG);
                  LONG        xpr_extension;
                  LONG       *xpr_data;
                 ULONG (* ASM xpr_options)(REG(d0) LONG,REG(a0) struct xpr_option **);
                  LONG (* ASM xpr_unlink)(REG(a0) STRPTR);
                  LONG (*     xpr_squery)(VOID);
                  LONG (* ASM xpr_getptr)(REG(d0) LONG);
              };

/*
*   The functions
*/
#ifdef __AROS__
#include <proto/xpr.h>
#else
LONG XProtocolCleanup(struct XPR_IO *);
LONG XProtocolSetup(struct XPR_IO *);
LONG XProtocolSend(struct XPR_IO *);
LONG XProtocolReceive(struct XPR_IO *);
LONG XProtocolHostMon(struct XPR_IO *,APTR,LONG,LONG);
LONG XProtocolUserMon(struct XPR_IO *,APTR,LONG,LONG);

/* Pragmas for SAS/Lattice-C V5.0 */
#ifndef NO_PRAGMAS
#pragma libcall XProtocolBase XProtocolCleanup 1e 801
#pragma libcall XProtocolBase XProtocolSetup 24 801
#pragma libcall XProtocolBase XProtocolSend 2a 801
#pragma libcall XProtocolBase XProtocolReceive 30 801
#pragma libcall XProtocolBase XProtocolHostMon 36 109804
#pragma libcall XProtocolBase XProtocolUserMon 3c 109804
#endif	/* NO_PRAGMAS */
#endif

/* For use in comm programs written in SAS/C; allows calling XProtocol
 * library functions directly without glue routines.
 */

#pragma libcall XProtocolBase XProtocolCleanup 1E 801
#pragma libcall XProtocolBase XProtocolSetup 24 801
#pragma libcall XProtocolBase XProtocolSend 2A 801
#pragma libcall XProtocolBase XProtocolReceive 30 801
#pragma libcall XProtocolBase XProtocolHostMon 36 109804
#pragma libcall XProtocolBase XProtocolUserMon 3C 109804

/* End of Xpr_Lib.h source */

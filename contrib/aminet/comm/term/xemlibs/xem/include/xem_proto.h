
BOOL	XEmulatorSetup(struct XEM_IO *xem_io);
BOOL	XEmulatorOpenConsole(struct XEM_IO *xem_io);
VOID	XEmulatorCloseConsole(struct XEM_IO *xem_io);
VOID	XEmulatorCleanup(struct XEM_IO *xem_io);
VOID	XEmulatorWrite(struct XEM_IO *xem_io, UBYTE *string, LONG len);
BOOL	XEmulatorSignal(struct XEM_IO *xem_io, ULONG signal);
ULONG	XEmulatorHostMon(struct XEM_IO *xem_io, struct XEmulatorHostData *hd, ULONG actual);
ULONG	XEmulatorUserMon(struct XEM_IO *xem_io, UBYTE *retStr, ULONG maxLen, struct IntuiMessage *iMsg);
VOID	XEmulatorOptions(struct XEM_IO *xem_io);
VOID	XEmulatorClearConsole(struct XEM_IO *xem_io);
VOID	XEmulatorResetConsole(struct XEM_IO *xem_io);
VOID	XEmulatorResetTextStyles(struct XEM_IO *xem_io);
VOID	XEmulatorResetCharset(struct XEM_IO *xem_io);
ULONG	XEmulatorGetFreeMacroKeys(struct XEM_IO *xem_io, ULONG qualifier);
BOOL	XEmulatorMacroKeyFilter(struct XEM_IO *xem_io, struct List *keys);
LONG	XEmulatorInfo(struct XEM_IO *xem_io, ULONG type);
BOOL	XEmulatorPreferences(struct XEM_IO *xem_io, STRPTR fileName, ULONG mode);
VOID	XEmulatorNewSize(struct XEM_IO *xem_io, struct IBox *frame);

#ifdef LATTICE

/* Pragmas for SAS/Lattice-C V5.0 */

#pragma libcall XEmulatorBase XEmulatorSetup 1E 801
#pragma libcall XEmulatorBase XEmulatorOpenConsole 24 801
#pragma libcall XEmulatorBase XEmulatorCloseConsole 2A 801
#pragma libcall XEmulatorBase XEmulatorCleanup 30 801
#pragma libcall XEmulatorBase XEmulatorWrite 36 9803
#pragma libcall XEmulatorBase XEmulatorSignal 3C 802
#pragma libcall XEmulatorBase XEmulatorHostMon 42 9803
#pragma libcall XEmulatorBase XEmulatorUserMon 48 A09804
#pragma libcall XEmulatorBase XEmulatorOptions 4E 801
#pragma libcall XEmulatorBase XEmulatorClearConsole 54 801
#pragma libcall XEmulatorBase XEmulatorResetConsole 5A 801
#pragma libcall XEmulatorBase XEmulatorResetTextStyles 60 801
#pragma libcall XEmulatorBase XEmulatorResetCharset 66 801
#pragma libcall XEmulatorBase XEmulatorGetFreeMacroKeys 6c 802
#pragma libcall XEmulatorBase XEmulatorMacroKeyFilter 72 9802
#pragma libcall XEmulatorBase XEmulatorInfo 78 802
#pragma libcall XEmulatorBase XEmulatorPreferences 7E 9803
#pragma libcall XEmulatorBase XEmulatorNewSize 84 9802
#endif

#ifdef AZTEC_C

/* Pragmas for Manx Aztec-C V5.0 */

#pragma amicall(XEmulatorBase, 0x1E, XEmulatorSetup(a0))
#pragma amicall(XEmulatorBase, 0x24, XEmulatorOpenConsole(a0))
#pragma amicall(XEmulatorBase, 0x2A, XEmulatorCloseConsole(a0))
#pragma amicall(XEmulatorBase, 0x30, XEmulatorCleanup(a0))
#pragma amicall(XEmulatorBase, 0x36, XEmulatorWrite(a0,a1,d0))
#pragma amicall(XEmulatorBase, 0x3C, XEmulatorSignal(a0,d0))
#pragma amicall(XEmulatorBase, 0x42, XEmulatorHostMon(a0,a1,d0))
#pragma amicall(XEmulatorBase, 0x48, XEmulatorUserMon(a0,a1,d0,a2))
#pragma amicall(XEmulatorBase, 0x4E, XEmulatorOptions(a0))
#pragma amicall(XEmulatorBase, 0x54, XEmulatorClearConsole(a0))
#pragma amicall(XEmulatorBase, 0x5A, XEmulatorResetConsole(a0))
#pragma amicall(XEmulatorBase, 0x60, XEmulatorResetTextStyles(a0))
#pragma amicall(XEmulatorBase, 0x66, XEmulatorResetCharset(a0))
#pragma amicall(XEmulatorBase, 0x7C, XEmulatorGetFreeMacroKeys(a0,d0))
#pragma amicall(XEmulatorBase, 0x72, XEmulatorMacroKeyFilter(a0,a1))
#pragma amicall(XEmulatorBase, 0x78, XEmulatorInfo(a0,d0))
#pragma amicall(XEmulatorBase, 0x7E, XEmulatorPreferences(a0,a1,d0))
#pragma amicall(XEmulatorBase, 0x84, XEmulatorNewSize(a0,a1))
#endif

/* <EOB> */

// FormatDisk.h
// $Date$
// $Revision$


#ifndef FORMATDISK_H
#define	FORMATDISK_H

#define NDP_IconCreation	0x80000001

#define d1(x)		;
#define d2(x)		x;

//----------------------------------------------------------------------------

/// IDs of various file systems

#define FS_BSD_DISKLABEL_MSDOS_SWAP    0x42534401
#define FS_BSD_DISKLABEL_MSDOS_V6      0x42534402
#define FS_BSD_DISKLABEL_MSDOS_V7      0x42534403
#define FS_BSD_DISKLABEL_MSDOS_SYSV    0x42534404
#define FS_BSD_DISKLABEL_MSDOS_41      0x42534405
#define FS_BSD_DISKLABEL_MSDOS_8TH     0x42534406
#define FS_BSD_DISKLABEL_MSDOS_42      0x42534407
#define FS_BSD_DISKLABEL_MSDOS_MSDOS   0x42534408
#define FS_BSD_DISKLABEL_MSDOS_44LFS   0x42534409
#define FS_BSD_DISKLABEL_MSDOS_UNKNOWN 0x4253440a
#define FS_BSD_DISKLABEL_MSDOS_HPFS    0x4253440b
#define FS_BSD_DISKLABEL_MSDOS_ISO9660 0x4253440c
#define FS_BSD_DISKLABEL_MSDOS_BOOT    0x4253440d
#define FS_BSD_DISKLABEL_MSDOS_AFFS    0x4253440e
#define FS_BSD_DISKLABEL_MSDOS_HFS     0x4253440f

#define FS_OFS_LINUX_SWAP   0x4f534601
#define FS_OFS_BSD_V6       0x4f534602
#define FS_OFS_BSD_V7       0x4f534603
#define FS_OFS_BSD_SYSV     0x4f534604
#define FS_OFS_BSD_41       0x4f534605
#define FS_OFS_BSD_8TH      0x4f534606
#define FS_OFS_BSD_42       0x4f534607
#define FS_OFS_LINUX_NATIVE 0x4f534608
#define FS_OFS_BSD_44LFS    0x4f534609
#define FS_OFS_UNKNOWN      0x4f53460a
#define FS_OFS_HPFS         0x4f53460b
#define FS_OFS_ISO9660      0x4f53460c
#define FS_OFS_BOOT         0x4f53460d
#define FS_OFS_AFFS         0x4f53460e
#define FS_OFS_HFS          0x4f53460f
#define FS_OFS_ADVFS        0x4f534610

#define FS_SUN_EMPTY        0x53554e00
#define FS_SUN_BOOT         0x53554e01
#define FS_SUN_SUNOS_ROOT   0x53554e02
#define FS_SUN_SUNOS_SWAP   0x53554e03
#define FS_SUN_SUNOS_USR    0x53554e04
#define FS_SUN_WHOLE        0x53554e05
#define FS_SUN_SUNOS_STAND  0x53554e06
#define FS_SUN_SUNOS_VAR    0x53554e07
#define FS_SUN_SUNOS_HOME   0x53554e08
#define FS_SUN_LINUX_MINIX  0x53554e81
#define FS_SUN_LINUX_SWAP   0x53554e82
#define FS_SUN_LINUX_NATIVE 0x53554e83

#define FS_AMIGA_GENERIC_BOOT   0x424f4f55
#define FS_AMIGA_OFS            0x444f5300
#define FS_AMIGA_FFS            0x444f5301
#define FS_AMIGA_OFS_INTL       0x444f5302
#define FS_AMIGA_FFS_INTL       0x444f5303
#define FS_AMIGA_OFS_DC_INTL    0x444f5304
#define FS_AMIGA_FFS_DC_INTL    0x444f5405
#define FS_AMIGA_OFS_LNFS       0x444f5406
#define FS_AMIGA_FFS_LNFS       0x444f5407
#define FS_AMIGA_MUFS_FFS_INTL  0x6d754653
#define FS_AMIGA_MUFS_OFS       0x6d754600
#define FS_AMIGA_MUFS_FFS       0x6d754601
#define FS_AMIGA_MUFS_OFS_INTL  0x6d754602
#define FS_AMIGA_MUFS_FFS_INTL2 0x6d754603
#define FS_AMIGA_MUFS_OFS_DC    0x6d754604
#define FS_AMIGA_MUFS_FFS_DC    0x6d754605
#define FS_AMIGA_LINUX_NATIVE   0x4c4e5800
#define FS_AMIGA_LINUX_EXT2     0x45585432
#define FS_AMIGA_LINUX_SWAP     0x53574150
#define FS_AMIGA_LINUX_SWAP2    0x53575000
#define FS_AMIGA_LINUX_MINIX    0x4d4e5800
#define FS_AMIGA_AMIX_0         0x554e4900
#define FS_AMIGA_AMIX_1         0x554e4901
#define FS_AMIGA_NETBSD_ROOT    0x4e425207
#define FS_AMIGA_NETBSD_SWAP    0x4e425301
#define FS_AMIGA_NETBSD_OTHER   0x4e425507
#define FS_AMIGA_PFS0           0x50465300
#define FS_AMIGA_PFS1           0x50465301
#define FS_AMIGA_PFS2           0x50465302
#define FS_AMIGA_PFS3           0x50465303
#define FS_AMIGA_PDS2           0x50445300
#define FS_AMIGA_PDS3           0x50445303
#define FS_AMIGA_MUPFS          0x6d755046
#define FS_AMIGA_AFS            0x41465300
#define FS_AMIGA_AFS_EXP        0x41465301
#define FS_AMIGA_CDISO          0x43443031
#define FS_AMIGA_CDHSF          0x43443030
#define FS_AMIGA_CDDA           0x43444441
#define FS_AMIGA_CDRIVE         0x43444653
#define FS_AMIGA_ASIMCDFS       0x662dabac
#define FS_AMIGA_HFS            0x4d414300
#define FS_AMIGA_MSDOS          0x4d534400
#define FS_AMIGA_MSDOS_HF       0x4d534800
#define FS_AMIGA_BFFS           0x42464653
#define FS_AMIGA_SFS            0x53465300

#define FS_AMIGA_BAD            0x42414400
#define FS_AMIGA_NDOS           0x4e444f53
#define FS_AMIGA_KICK           0x4b49434b

#define FS_ATARI_GEMDOS       0x4147454d
#define FS_ATARI_GEMDOSBIG    0x4142474d
#define FS_ATARI_LINUX        0x414c4e58
#define FS_ATARI_LINUX_SWAP   0x41535750
#define FS_ATARI_LINUX_MINIX  0x414d4958
#define FS_ATARI_LINUX_MINIX2 0x414d4e58
#define FS_ATARI_HFS          0x414d4143
#define FS_ATARI_SYSV_UNIX    0x41554e58
#define FS_ATARI_RAW          0x41524157
#define FS_ATARI_EXTENDED     0x4158474d

#define FS_MAC_PARTITION_MAP    0x4d414301
#define FS_MAC_MACOS_DRIVER     0x4d414302
#define FS_MAC_MACOS_DRIVER_43  0x4d414303
#define FS_MAC_MACOS_HFS        0x4d414304
#define FS_MAC_MACOS_MFS        0x4d414305
#define FS_MAC_SCRATCH          0x4d414306
#define FS_MAC_PRODOS           0x4d414307
#define FS_MAC_FREE             0x4d414308
#define FS_MAC_LINUX_SWAP       0x4d414309
#define FS_MAC_AUX              0x4d41430a
#define FS_MAC_MSDOS            0x4d41430b
#define FS_MAC_MINIX            0x4d41430c
#define FS_MAC_AFFS             0x4d41430d
#define FS_MAC_LINUX_NATIVE     0x4d41430e
#define FS_MAC_NEWWORLD         0x4d41430f
#define FS_MAC_MACOS_ATA        0x4d414310
#define FS_MAC_MACOS_FW_DRIVER  0x4d414311
#define FS_MAC_MACOS_IOKIT      0x4d414312
#define FS_MAC_MACOS_PATCHES    0x4d414313
#define FS_MAC_MACOSX_BOOT      0x4d414314
#define FS_MAC_MACOSX_LOADER    0x4d414315
#define FS_MAC_UFS              0x4d414316

#define FS_ACORN_ADFS           0x41524d01
#define FS_ACORN_LINUX_MAP      0x41524d02
#define FS_ACORN_LINUX_EXT2     0x41524d03
#define FS_ACORN_LINUX_SWAP     0x41524d04
#define FS_ACORN_ADFS_ICS       0x41524d05
#define FS_ACORN_LINUX_EXT2_ICS 0x41524d06
#define FS_ACORN_LINUX_SWAP_ICS 0x41524d07

#define FS_SINCLAIR_QL5A 0x514c3541
#define FS_SINCLAIR_QL5B 0x514c3542

#define FS_SPECTRUM_DISCIPLE 0x5a585300
#define FS_SPECTRUM_UNIDOS   0x5a585301
#define FS_SPECTRUM_SAMDOS   0x5a585302
#define FS_SPECTRUM_OPUS     0x5a585303

#define FS_ARCHIMEDES_D 0x41524d44
#define FS_ARCHIMEDES_E 0x41524d45

#define FS_CPM 0x43504d02

#define FS_C64 0x31353431

//----------------------------------------------------------------------------

#ifdef __AROS__
#define KPrintF kprintf
#endif

extern int kprintf(CONST_STRPTR, ...);
extern int KPrintF(CONST_STRPTR, ...);

IPTR mui_getv(APTR, ULONG );

struct FormatDiskMUI_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

//----------------------------------------------------------------------------

#endif	/* FORMATDISK_H */

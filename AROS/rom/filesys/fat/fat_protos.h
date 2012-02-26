/*
 * fat.handler - FAT12/16/32 filesystem handler
 *
 * Copyright © 2006 Marek Szyprowski
 * Copyright © 2007-2011 The AROS Development Team
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the same terms as AROS itself.
 *
 * $Id$
 */

#ifndef FAT_HANDLER_PROTO_H
#define FAT_HANDLER_PROTO_H

/* fat */
LONG ReadFATSuper (struct FSSuper *s);
void FreeFATSuper(struct FSSuper *s);
LONG CompareFATSuper(struct FSSuper *s1, struct FSSuper *s2);

LONG GetVolumeIdentity(struct FSSuper *sb, struct VolumeIdentity *volume);

void CountFreeClusters(struct FSSuper *sb);
void AllocCluster(struct FSSuper *sb, ULONG cluster);
void FreeCluster(struct FSSuper *sb, ULONG cluster);

/* disk.c */
void ProcessDiskChange (struct Globals *glob);
void DoDiskInsert(struct Globals *glob);
BOOL AttemptDestroyVolume(struct FSSuper *sb);
void DoDiskRemove(struct Globals *glob);
#if 0
void SendVolumePacket(struct DosList *vol, ULONG action);
#endif

LONG InitDiskHandler(struct Globals *glob, struct FileSysStartupMsg *fssm);
void CleanupDiskHandler(struct Globals *glob);
void UpdateDisk(struct Globals *glob);
void Probe_64bit_support(struct Globals *glob);
ULONG AccessDisk(struct Globals *glob, BOOL do_write, ULONG num, ULONG nblocks, ULONG block_size, UBYTE *data);

/* info.c */
void FillDiskInfo (struct Globals *glob, struct InfoData *id);
 
/* packet.c */
void ProcessPackets(struct Globals *glob);
void ReplyPacket(struct DosPacket *pkt);

/* direntry.c */
LONG InitDirHandle(struct FSSuper *sb, ULONG cluster, struct DirHandle *dh, BOOL reuse);
LONG ReleaseDirHandle(struct DirHandle *dh);

LONG GetDirEntry(struct DirHandle *dh, ULONG index, struct DirEntry *de);
LONG GetNextDirEntry(struct DirHandle *dh, struct DirEntry *de);
LONG GetDirEntryByCluster(struct DirHandle *dh, ULONG cluster,
    struct DirEntry *de);

LONG GetDirEntryByName(struct DirHandle *dh, STRPTR name, ULONG namelen, struct DirEntry *de);
LONG GetDirEntryByPath(struct DirHandle *dh, STRPTR path, ULONG pathlen, struct DirEntry *de);

LONG GetParentDir(struct DirHandle *dh, struct DirEntry *de);

LONG UpdateDirEntry(struct DirEntry *de);

LONG AllocDirEntry(struct DirHandle *dh, ULONG gap, struct DirEntry *de);
LONG CreateDirEntry(struct DirHandle *dh, STRPTR name, ULONG namelen, UBYTE attr, ULONG cluster, struct DirEntry *de);
LONG DeleteDirEntry(struct DirEntry *de);

LONG FillFIB(struct ExtFileLock *fl, struct FileInfoBlock *fib);

/* names.c */
LONG GetDirEntryShortName(struct DirEntry *de, STRPTR name, ULONG *len);
LONG GetDirEntryLongName(struct DirEntry *de, STRPTR name, ULONG *len);
LONG SetDirEntryName(struct DirEntry *de, STRPTR name, ULONG len);
ULONG NumLongNameEntries(STRPTR name, ULONG len);

/* fat.c */
void ConvertFATDate(struct Globals *glob, UWORD date, UWORD time, struct DateStamp *ds);
void ConvertAROSDate(struct Globals *glob, struct timeval *tv, UWORD *date, UWORD *time);
LONG SetVolumeName(struct FSSuper *sb, UBYTE *name);
LONG FindFreeCluster(struct FSSuper *sb, ULONG *rcluster);

/* file.c */
LONG ReadFileChunk(struct IOHandle *ioh, ULONG file_pos, ULONG nwant, UBYTE *data, ULONG *nread);
LONG WriteFileChunk(struct IOHandle *ioh, ULONG file_pos, ULONG nwant, UBYTE *data, ULONG *nwritten);

/* ops.c */
LONG OpLockFile(struct ExtFileLock *dirlock, UBYTE *name, ULONG namelen, LONG access, struct ExtFileLock **filelock);
void OpUnlockFile(struct ExtFileLock *lock);
LONG OpCopyLock(struct ExtFileLock *lock, struct ExtFileLock **copy);
LONG OpLockParent(struct ExtFileLock *lock, struct ExtFileLock **parent);
LONG OpOpenFile(struct ExtFileLock *dirlock, UBYTE *name, ULONG namelen, LONG action, struct ExtFileLock **filelock);
LONG OpDeleteFile(struct ExtFileLock *dirlock, UBYTE *name, ULONG namelen);
LONG OpRenameFile(struct ExtFileLock *sdirlock, UBYTE *sname, ULONG snamelen, struct ExtFileLock *ddirlock, UBYTE *dname, ULONG dnamelen);
LONG OpCreateDir(struct ExtFileLock *dirlock, UBYTE *name, ULONG namelen, struct ExtFileLock **newdirlock);
LONG OpRead(struct ExtFileLock *lock, UBYTE *data, ULONG want, ULONG *read);
LONG OpWrite(struct ExtFileLock *lock, UBYTE *data, ULONG want, ULONG *written);
LONG OpSetFileSize(struct ExtFileLock *lock, LONG offset, LONG mode, LONG *newsize);
LONG OpSetProtect(struct ExtFileLock *lock, UBYTE *name, ULONG namelen, ULONG prot);
LONG OpSetDate(struct ExtFileLock *dirlock, UBYTE *name, ULONG namelen, struct DateStamp *ds);
LONG OpAddNotify(struct Globals *glob, struct NotifyRequest *nr);
LONG OpRemoveNotify(struct Globals *glob, struct NotifyRequest *nr);

/* lock.c */
LONG TestLock(struct ExtFileLock *fl);
LONG LockFileByName(struct ExtFileLock *fl, UBYTE *name, LONG namelen, LONG access, struct ExtFileLock **lock);
LONG LockFile(struct Globals *glob, ULONG cluster, ULONG entry, LONG access, struct ExtFileLock **lock);
LONG LockRoot(struct Globals *glob, LONG access, struct ExtFileLock **lock);
LONG CopyLock(struct ExtFileLock *fl, struct ExtFileLock **lock);
void FreeLock(struct ExtFileLock *fl);

/* notify.c */
void SendNotify(struct Globals *glob, struct NotifyRequest *nr);
void SendNotifyByLock(struct FSSuper *sb, struct GlobalLock *gl);
void SendNotifyByDirEntry(struct FSSuper *sb, struct DirEntry *de);
void ProcessNotify(struct Globals *glob); 

/* timer.c */
LONG InitTimer(struct Globals *glob);
void CleanupTimer(struct Globals *glob);
void RestartTimer(struct Globals *glob);
void HandleTimer(struct Globals *glob);

#endif

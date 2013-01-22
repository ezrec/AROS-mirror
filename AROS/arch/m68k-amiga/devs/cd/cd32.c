/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#define DEBUG 1
#include <aros/debug.h>

#include <aros/symbolsets.h>

#include <devices/cd.h>

#include <proto/exec.h>
#include <proto/alib.h>

#include <hardware/intbits.h>

#include "chinon.h"
#include "cd32.h"

#include "cd_intern.h"

static inline ULONG readl(ULONG addr)
{
    return *((volatile ULONG *)addr);
}

static inline VOID writel(ULONG value, ULONG addr)
{
    *((volatile ULONG *)addr) = value;
}

static inline UWORD readw(ULONG addr)
{
    return *((volatile UWORD *)addr);
}

static inline VOID writew(UWORD value, ULONG addr)
{
    *((volatile UWORD *)addr) = value;
}

static inline UBYTE readb(ULONG addr)
{
    return *((volatile UBYTE *)addr);
}

static inline VOID writeb(UBYTE value, ULONG addr)
{
    *((volatile UBYTE *)addr) = value;
}

struct CD32Mode1 {
    ULONG cs_Sector;
    UBYTE cs_Reserved[8];
    UBYTE cs_MinuteBCD;
    UBYTE cs_SecondBCD;
    UBYTE cs_FrameBCD;
    UBYTE cs_Mode;
    UBYTE cs_Data[2048];
    UBYTE cs_EDC[4];
    UBYTE cs_Reserved2[8];
    UBYTE cs_ECC[276];
};

struct CD32Unit {
    struct CD32Data {
        UBYTE Data[0xc00];
        UBYTE Error[0x400];
    } *cu_Data; /* x16 of these */
    struct CD32Misc {
        UBYTE Response[0x100];
        UBYTE Subcode[0x100];
        UBYTE Command[0x100];
        UBYTE Reserved[0x100];
    } *cu_Misc;
    struct Interrupt cu_Interrupt;
    LONG cu_SectorSize;
    UBYTE cu_TxHead, cu_RxHead;
    struct Task *cu_Task;
};

static void sec2msf(LONG sec, UBYTE *msf)
{
    msf[0] = sec / (60 * 75);
    sec = sec % (60 * 75);
    msf[1] = sec / 75;
    msf[2] = sec % 75;
}

static AROS_INTH1(CD32_Interrupt, struct CD32Unit *, cu)
{
    AROS_INTFUNC_INIT

    if (readl(AKIKO_CDINTREQ)) {
        writel(0, AKIKO_CDFLAG);
        writel(0, AKIKO_CDINTENA);

        Signal(cu->cu_Task, SIGF_SINGLE);

        return TRUE;
    }

    return FALSE;

    AROS_INTFUNC_EXIT
}

static LONG CD32_Cmd(struct CD32Unit *cu, UBYTE *cmd, LONG cmd_len, UBYTE *resp, LONG resp_len)
{
    UBYTE csum = 0;
    int i;

    for (i = 0; i < cmd_len; i++) {
        /* NOTE: We are relying on the fact that cu_TxHead is a
         *       UBYTE, so that it wraps at 0x100
         */
        cu->cu_Misc->Command[cu->cu_TxHead++] = cmd[i];
        csum += cmd[i];
    }

    cu->cu_Misc->Command[cu->cu_TxHead++] = ~csum;
    writeb(cu->cu_TxHead, AKIKO_CDTXCMP);

    cu->cu_RxHead = (cu->cu_RxHead + resp_len + 1) & 0xff;
    writeb((cu->cu_RxHead + resp_len + 1) & 0xff, AKIKO_CDRXCMP);

    SetSignal(0, SIGF_SINGLE);

    /* Just wait for the RX to complete of the status */
    writel(AKIKO_CDINT_DRIVERECV, AKIKO_CDINTENA);
    writel(AKIKO_CDFLAG_TXD | AKIKO_CDFLAG_RXD, AKIKO_CDFLAG);

    Wait(SIGF_SINGLE);

    csum = 0;
    for (i = 0; i < resp_len; i++) {
        /* NOTE: We are relying on the fact that cu_RxHead is a
         *       UBYTE, so that it wraps at 0x100
         */
        resp[i] = cu->cu_Misc->Response[cu->cu_RxHead++];
        csum += resp[i];
    }
    csum += cu->cu_Misc->Response[cu->cu_RxHead++];

    if (csum != 0) {
        D(bug("%s: Checksum failed on RX\n", __func__));
        return CDERR_NotSpecified;
    }

    if (resp[0] != cmd[0]) {
        D(bug("%s: Command mismatch\n", __func__));
        return CDERR_InvalidState;
    }

    return (resp[0] == 0) ? 0 : CDERR_NotSpecified;
}

static LONG CD32_CmdRead(struct CD32Unit *cu, UBYTE *cmd, LONG cmd_len, UBYTE *resp, LONG resp_len, LONG sectors)
{
    LONG err;
    UBYTE csum;
    int i;

    /* Start the read */
    err = CD32_Cmd(cu, cmd, cmd_len, resp, resp_len);
    if (err != CDERR_NotSpecified || ((resp[1] & 2) != 2))
        return err;

    /* Set up for DMA */
    resp_len = 2;
    writew((1 << sectors)-1, AKIKO_CDPBX);
    writeb((cu->cu_RxHead + resp_len + 1) & 0xff, AKIKO_CDRXCMP);

    SetSignal(0, SIGF_SINGLE);

    /* Wait for all sectors to come in */
    writel(AKIKO_CDINT_OVERFLOW, AKIKO_CDINTENA);
    writel(AKIKO_CDFLAG_RXD | AKIKO_CDFLAG_ENABLE | AKIKO_CDFLAG_PBX, AKIKO_CDFLAG);

    Wait(SIGF_SINGLE);

    csum = 0;
    for (i = 0; i < resp_len; i++) {
        /* NOTE: We are relying on the fact that cu_RxHead is a
         *       UBYTE, so that it wraps at 0x100
         */
        resp[i] = cu->cu_Misc->Response[cu->cu_RxHead++];
        csum += resp[i];
    }
    csum += cu->cu_Misc->Response[cu->cu_RxHead++];

    if (csum != 0) {
        D(bug("%s: Checksum failed on RX\n", __func__));
        return CDERR_NotSpecified;
    }

    if (resp[0] != cmd[0]) {
        D(bug("%s: Command mismatch\n", __func__));
        return CDERR_InvalidState;
    }

    return (resp[0] == 0) ? 0 : CDERR_NotSpecified;
}

static LONG CD32_DoIO(struct IOStdReq *io, APTR priv)
{
    struct CD32Unit *cu = priv;
    LONG off, err = CDERR_NOCMD;
    UBYTE cmd[16], res[16];
    LONG sect_start, sect_end, sect_len, len, actual;
    APTR buff;

    cu->cu_Task = FindTask(NULL);

    switch (io->io_Command) {
    case CD_RESET:
        cmd[0] = CHCD_RESET;
        err = CD32_Cmd(cu, cmd, 1, res, 1);
        break;
    case CD_READ:
        /* Convert offset and length to MSF */
        cmd[0] = CHCD_MULTI;
        sect_start = io->io_Offset / cu->cu_SectorSize;
        off = io->io_Offset % cu->cu_SectorSize;
        sect_end = (io->io_Offset + io->io_Length + cu->cu_SectorSize - 1) / cu->cu_SectorSize;
        buff = io->io_Data;
        len = io->io_Length;
        actual = 0;
       
        while (len > 0) {
            int i;

            sect_len = sect_start - sect_end;
            if (sect_len > 15)
                sect_len = 15;

            sec2msf(sect_start, &cmd[1]);
            sec2msf(sect_start + sect_len, &cmd[4]);
            cmd[7] = 0x80;  /* Data read */
            cmd[8] = 0x40;  /* 0x40 = 150 Frames/sec, 0x00 = 75 */
            cmd[9] = 0x00;
            cmd[10] = 0x00;  /* 0x04 to play audio */
            cmd[11] = 0x00;
            err = CD32_CmdRead(cu, cmd, 12, res, 2, sect_len);
            if (err != 0)
                break;

            for (i = 0; i < sect_len; i++) {
                LONG tocopy = cu->cu_SectorSize - off;
                if (tocopy > len)
                    tocopy = len;
                CopyMem(&cu->cu_Data[i].Data[16 + off], buff, tocopy);
                len -= tocopy;
                buff += tocopy;
                actual += tocopy;
                off = 0;
            }

            sect_start += sect_len;
        }
        if (err == 0)
            io->io_Actual = actual;
        break;
    case CD_GETGEOMETRY:
        if (io->io_Length >= sizeof(struct DriveGeometry)) {
            struct DriveGeometry *dg = (struct DriveGeometry *)io->io_Data;
            dg->dg_SectorSize = cu->cu_SectorSize;
            dg->dg_TotalSectors = 0;
            dg->dg_Cylinders = 1;
            dg->dg_CylSectors = dg->dg_TotalSectors;
            dg->dg_Heads = 1;
            dg->dg_TrackSectors = dg->dg_TotalSectors;
            dg->dg_BufMemType = MEMF_PUBLIC;
            dg->dg_DeviceType = DG_CDROM;
            dg->dg_Flags = DGF_REMOVABLE;
            dg->dg_Reserved = 0;
            io->io_Actual = sizeof(*dg);
            err = 0;
        } else {
            err = CDERR_BADLENGTH;
        }
        break;
    default:
        break;
    }

    return err;
}

static VOID CD32_Expunge(APTR priv)
{
    struct CD32Unit *cu = priv;
    RemIntServer(INTB_PORTS, &cu->cu_Interrupt);
    FreeMem(cu->cu_Misc, sizeof(struct CD32Misc));
    FreeMem(cu->cu_Data, sizeof(struct CD32Data) * 16);
    FreeVec(cu);
}

static const struct cdUnitOps CD32Ops = {
    .uo_Name = "CD32 (Akiko)",
    .uo_Expunge = CD32_Expunge,
    .uo_DoIO = CD32_DoIO,
};

static int CD32_InitLib(LIBBASETYPE *cb)
{
    LONG unit;
    struct CD32Unit *priv;

    if (readw(AKIKO_ID) != AKIKO_ID_MAGIC) {
        D(bug("%s: No Akiko detected\n", __func__));
        return 1;
    }

    /* Disable the CD32 interface for the moment */
    writel(0, AKIKO_CDFLAG);
    writel(0, AKIKO_CDINTENA);

    priv = AllocVec(sizeof(*priv), MEMF_ANY | MEMF_CLEAR);
    if (priv) {
        priv->cu_Misc = LibAllocAligned(sizeof(struct CD32Misc), MEMF_24BITDMA | MEMF_CLEAR, 1024);
        if (priv->cu_Misc) {
            priv->cu_Data = LibAllocAligned(sizeof(struct CD32Data) * 16, MEMF_24BITDMA | MEMF_CLEAR, 4096);
            if (priv->cu_Data) {
                unit = cdAddUnit(cb, &CD32Ops, priv);

                if (unit >= 0) {
                    priv->cu_Interrupt.is_Node.ln_Pri = 0;
                    priv->cu_Interrupt.is_Node.ln_Name = (STRPTR)CD32Ops.uo_Name;
                    priv->cu_Interrupt.is_Node.ln_Type = NT_INTERRUPT;
                    priv->cu_Interrupt.is_Data = priv;
                    priv->cu_Interrupt.is_Code = (VOID_FUNC)CD32_Interrupt;
                    AddIntServer(INTB_PORTS, &priv->cu_Interrupt);

                    writel((IPTR)priv->cu_Data, AKIKO_CDADRDATA);
                    writel((IPTR)priv->cu_Misc,  AKIKO_CDADRMISC);
                    priv->cu_TxHead = readb(AKIKO_CDTXINX);
                    priv->cu_RxHead = readb(AKIKO_CDRXINX);
                    writeb(priv->cu_TxHead, AKIKO_CDTXCMP);
                    writeb(priv->cu_RxHead, AKIKO_CDRXCMP);

                    D(bug("%s: Akiko as CD Unit %d\n", __func__, unit));
                    return 1;
                }

                FreeMem(priv->cu_Data, sizeof(struct CD32Data) * 16);
            }
            FreeMem(priv->cu_Misc, sizeof(struct CD32Misc));
        }
        FreeVec(priv);
    }

    return 0;
}

static int CD32_ExpungeLib(LIBBASETYPE *cb)
{
    /* Nothing to do. */
    return 0;
}


ADD2INITLIB(CD32_InitLib, 32);
ADD2EXPUNGELIB(CD32_ExpungeLib, 32);

/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <dos/filehandler.h>

#include "dopus.h"

#ifdef __SASC__
#include <i64.h>
#endif

void setupchangestate(void)
{
    int unit;
    struct IOStdReq *diskreq;

    disk_change_state=0;
    if (diskreq=(struct IOStdReq *)LCreateExtIO(general_port,sizeof(struct IOStdReq))) {
        for (unit=0;unit<4;unit++) {
            if (!(OpenDevice("trackdisk.device",unit,(struct IORequest *)diskreq,0))) {
                diskreq->io_Command=TD_CHANGESTATE;
                DoIO((struct IORequest *)diskreq);
                if (!diskreq->io_Actual) disk_change_state|=1<<unit;
                CloseDevice((struct IORequest *)diskreq);
            }
        }
        LDeleteExtIO((struct IORequest *)diskreq);
    }
}

#define ITEM_NUM 11

static short
    diskinfo_heads[]={
        STR_DISKINFO_DEVICE,
        STR_DISKINFO_NAME,
        STR_DISKINFO_SIZE,
        STR_DISKINFO_USED,
        STR_DISKINFO_FREE,
        STR_DISKINFO_PERCENT,
        STR_DISKINFO_DENSITY,
        STR_DISKINFO_ERRORS,
        STR_DISKINFO_STATUS,
        STR_DISKINFO_DATE,
        STR_DISKINFO_FILESYSTEM};

void dodiskinfo(path)
char *path;
{
    struct InfoData __aligned infodata;
    struct RastPort *dirp;
    long i;
#ifdef __SASC__
    bigint a;
#else
    unsigned long long a;
#endif
    int b,c,isd=-1,cx,cy,fail=0,xoffset,yoffset,titwidth;
    static char disktxt[11][60],buf[256],formstr[80],diskname[256],dname[10],buf1[40];
    float f;
    struct DateStamp ds;
    struct MsgPort *prt;
    struct Gadget contgad;
    char *gad_gads[2], *uscore,cont_key;
    BPTR lock;
    UBYTE old_change_state;
    ULONG class;
    UWORD code;
    struct DOpusRemember *key=NULL;

    if (!path || !path[0]) return;

    cx=scr_font[FONT_REQUEST]->tf_XSize;
    cy=scr_font[FONT_REQUEST]->tf_YSize;

    for (i=0;i<ITEM_NUM;i++) {
        disktxt[i][0]='-';
        disktxt[i][1]=0;
    }
    main_proc->pr_WindowPtr=(APTR)-1;
getnewdisk:
    strcpy(diskname,path);
    if (getroot(diskname,&ds)) {
        strcat(diskname,":");
        if ((prt=(struct MsgPort *)DeviceProc(diskname))) {
            if (lock=Lock(diskname,ACCESS_READ)) {
                Forbid();
                get_device_task(lock,disktxt[0],prt);

                for (i=0;i<4;i++) {
                    lsprintf(dname,"DF%ld:",i);
                    if (Stricmp(disktxt[0],dname)==0) {
                        isd=i;
                        break;
                    }
                }
                Permit();
                Info(lock,&infodata);
                UnLock(lock);
                switch (infodata.id_DiskType) {
                    case ID_UNREADABLE_DISK:
                        strcpy(disktxt[1],globstring[STR_UNREADABLE_DISK]);
                        break;
                    case ID_NOT_REALLY_DOS:
                        strcpy(disktxt[1],globstring[STR_NOT_A_DOS_DISK]);
                        break;
                    case ID_KICKSTART_DISK:
                        strcpy(disktxt[1],globstring[STR_KICKSTART_DISK]);
                        break;
                    default:
                        strcpy(buf,path);
                        strcpy(disktxt[1],diskname);
                        disktxt[1][(strlen(disktxt[1])-1)]=0;
#ifdef __SASC__
                        a = i64_uumul( infodata.id_NumBlocks, infodata.id_BytesPerBlock );
                        i64_itoa( a, buf1, 40 );
#else
                        a=((long long)infodata.id_NumBlocks)*infodata.id_BytesPerBlock;
                        sprintf(buf1,"%qd",a);
#endif
                        b=strlen(buf1);
                        lsprintf(buf,"%ld",(long int)infodata.id_NumBlocks);
                        lsprintf(formstr,"%%%lds %s; %%%ldld %s; %%s",
                            b,globstring[STR_DISKINFO_BYTES],
                            strlen(buf),globstring[STR_DISKINFO_BLOCKS]);
                        getsizestring(buf,a);
                        lsprintf(disktxt[2],formstr,buf1,infodata.id_NumBlocks,buf);
#ifdef __SASC__
                        a = i64_uumul( infodata.id_NumBlocksUsed, infodata.id_BytesPerBlock );
                        getsizestring(buf,a);
                        i64_itoa( a, buf1, 40 );
#else
                        a=((long long)infodata.id_NumBlocksUsed)*infodata.id_BytesPerBlock;
                        getsizestring(buf,a);
                        sprintf(buf1,"%qd",a);
#endif
                        lsprintf(disktxt[3],formstr,buf1,infodata.id_NumBlocksUsed,buf);
#ifdef __SASC__
                        a = i64_uumul( ( infodata.id_NumBlocks-infodata.id_NumBlocksUsed ), infodata.id_BytesPerBlock );
                        getsizestring(buf,a);
                        i64_itoa( a, buf1, 40 );
#else
                        a=(infodata.id_NumBlocks-infodata.id_NumBlocksUsed)*((long long)infodata.id_BytesPerBlock);
                        getsizestring(buf,a);
                        sprintf(buf1,"%qd",a);
#endif
                        lsprintf(disktxt[4],formstr,buf1,(infodata.id_NumBlocks-infodata.id_NumBlocksUsed),buf);

                        if (infodata.id_NumBlocks) f = ((infodata.id_NumBlocksUsed*100.0)/infodata.id_NumBlocks);
                        else f=100.0;
                        sprintf(buf,"%.1f",f); sprintf(buf1,"%.1f",100.0-f);
                        lsprintf(disktxt[5],"%s%% %s; %s%% %s",
                            buf,globstring[STR_DISKINFO_FULL],
                            buf1,globstring[STR_DISKINFO_SFREE]);
                        lsprintf(disktxt[6],"%ld %s",
                            (long int)infodata.id_BytesPerBlock,
                            globstring[STR_DISKINFO_BYTESPERBLOCK]);
                        lsprintf(disktxt[7],"%ld",(long int)infodata.id_NumSoftErrors);
                        switch (infodata.id_DiskState) {
                            case ID_VALIDATING:
                                strcpy(disktxt[8],globstring[STR_VALIDATING]);
                                break;
                            case ID_WRITE_PROTECTED:
                                strcpy(disktxt[8],globstring[STR_WRITE_PROTECTED]);
                                break;
                            default:
                                strcpy(disktxt[8],globstring[STR_READ_WRITE]);
                                break;
                        }
                        seedate(&ds,disktxt[9],0);
//#ifndef __AROS__
// DosType fix
                         {
                          struct DosList *dl;
                          struct FileSysStartupMsg *fssm;
                          struct DosEnvec *de;
                          char buf[32];

                          if (dl = LockDosList(LDF_DEVICES | LDF_READ))
                           {
                            strcpy(buf,disktxt[0]);
                            buf[strlen(buf)-1] = 0;
                            if (dl = FindDosEntry(dl,buf,LDF_DEVICES))
                             {
                              if (fssm = BADDR(((struct DeviceNode *)dl)->dn_Startup))
                               {
                                if (de = BADDR(fssm->fssm_Environ)) infodata.id_DiskType = de->de_DosType;
                               }
//                              else infodata.id_DiskType = 0x3F3F3F3F;
                             }
                            UnLockDosList(LDF_DEVICES | LDF_READ);
                           }
                         }
//#endif
// end
#ifndef __AROS__
/* old code
                        disktxt[10][0]=(char)((infodata.id_DiskType>>24)&0xff);
                        disktxt[10][1]=(char)((infodata.id_DiskType>>16)&0xff);
                        disktxt[10][2]=(char)((infodata.id_DiskType>>8)&0xff);
                        disktxt[10][3]=(char)(infodata.id_DiskType&0xff)+'0';
*/
// new code
                        *((ULONG *)disktxt[10]) = infodata.id_DiskType;
                        if (disktxt[10][3] < 32) disktxt[10][3] += '0';
// end
#else
                        disktxt[10][0]=(char)((infodata.id_DiskType>>24)&0xff);
                        disktxt[10][1]=(char)((infodata.id_DiskType>>16)&0xff);
                        disktxt[10][2]=(char)((infodata.id_DiskType>>8)&0xff);
                        disktxt[10][3]=(char)(infodata.id_DiskType&0xff);
                        if (disktxt[10][3] < 32) disktxt[10][3] += '0';
#endif
                        disktxt[10][4]=0;
                        switch (infodata.id_DiskType) {
                            case ID_DOS_DISK:
                                strcat(disktxt[10]," (OFS)");
                                break;
                            case ID_FFS_DISK:
                                strcat(disktxt[10]," (FFS)");
                                break;
                            case ID_INTER_DOS_DISK:
                                strcat(disktxt[10]," (I-OFS)");
                                break;
                            case ID_INTER_FFS_DISK:
                                strcat(disktxt[10]," (I-FFS)");
                                break;
                            case ID_FASTDIR_DOS_DISK:
                                strcat(disktxt[10]," (DC-OFS)");
                                break;
                            case ID_FASTDIR_FFS_DISK:
                                strcat(disktxt[10]," (DC-FFS)");
                                break;
                            case ID_MSDOS_DISK:
                                strcat(disktxt[10]," (MS-DOS)");
                                break;
                            case ID_SFS_BE_DISK:
                                strcat(disktxt[10]," (SFS)");
                                break;
#ifdef __AROS__
                            case AROS_MAKE_ID('F','A','T',0):
                                strcat(disktxt[10]," (FAT)");
                                break;
#endif
                        }
                        break;
                }
            }
            else {
                i=IoErr();
                doerror(i);
                switch (i) {
                    case ERROR_DEVICE_NOT_MOUNTED:
                        strcpy(disktxt[1],globstring[STR_DEVICE_NOT_MOUNTED]);
                        break;
                    case ERROR_NOT_A_DOS_DISK:
                        strcpy(disktxt[1],globstring[STR_NOT_A_DOS_DISK]);
                        break;
                    case ERROR_NO_DISK:
                        strcpy(disktxt[1],globstring[STR_NO_DISK_IN_DRIVE]);
                        break;
                    default:
                        lsprintf(disktxt[1],globstring[STR_DOS_ERROR_CODE],a);
                        break;
                }
            }
        }
        else fail=1;
    }
    else fail=1;

    if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
    if (fail) {
        doerror(-1);
        return;
    }

    b=0; titwidth=0;
    for (i=0;i<ITEM_NUM;i++) {
        if ((c=strlen(disktxt[i]))>b) b=c;
        if ((c=strlen(globstring[diskinfo_heads[i]]))>titwidth) titwidth=c;
    }

    disk_win.Width=((b+titwidth+3)*cx)+16;
    disk_win.Height=(ITEM_NUM*(cy+1))+19;
    if (config->generalscreenflags&SCR_GENERAL_REQDRAG) {
        disk_win.Flags=WFLG_RMBTRAP|WFLG_ACTIVATE|WFLG_DRAGBAR|WFLG_DEPTHGADGET;
        xoffset=Window->WScreen->WBorLeft;
        yoffset=Window->WScreen->WBorTop+Window->WScreen->Font->ta_YSize+1;
        disk_win.Width+=xoffset+Window->WScreen->WBorRight;
        disk_win.Height+=yoffset+Window->WScreen->WBorBottom;
        disk_win.Title="DiskInfo";
    }
    else {
        disk_win.Flags=WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_ACTIVATE;
        xoffset=2;
        yoffset=1;
        disk_win.Width+=4;
        disk_win.Height+=2;
        disk_win.Title=NULL;
    }
    centerwindow(&disk_win);

    contgad.NextGadget=NULL;
    contgad.Width=(strlen(globstring[STR_CONTINUE])+4)*cx;
    contgad.Height=cy+5;
    contgad.LeftEdge=(disk_win.Width-contgad.Width)/2;
    contgad.TopEdge=disk_win.Height-cy-10;
    contgad.Flags=GFLG_GADGHCOMP;
    contgad.Activation=GACT_RELVERIFY;
    contgad.GadgetType=GTYP_BOOLGADGET;
    contgad.GadgetRender=NULL;
    contgad.SelectRender=NULL;
    contgad.GadgetText=NULL;
    contgad.MutualExclude=0;
    contgad.SpecialInfo=NULL;
    contgad.GadgetID=0;

// StrCombine(buf,"_",globstring[STR_CONTINUE],256);
    gad_gads[0]=globstring[STR_CONTINUE]/*buf*/;
    gad_gads[1]=NULL;

    if (!(fontwindow=OpenWindow(&disk_win))) return;

    dirp=fontwindow->RPort;
    setupwindreq(fontwindow);
    SetFont(dirp,scr_font[FONT_REQUEST]);
    for (i=0;i<ITEM_NUM;i++) {
        Move(dirp,xoffset+8,yoffset+5+(i*cy)+scr_font[FONT_REQUEST]->tf_Baseline);
        Text(dirp,globstring[diskinfo_heads[i]],strlen(globstring[diskinfo_heads[i]]));
        Move(dirp,xoffset+8+(titwidth*cx),yoffset+5+(i*cy)+scr_font[FONT_REQUEST]->tf_Baseline);
        Text(dirp," : ",3);
        Text(dirp,disktxt[i],strlen(disktxt[i]));
    }
    AddGadgetBorders(&key,
        &contgad,1,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgets(fontwindow,
        &contgad,
        gad_gads,
        1,
        screen_pens[config->gadgettopcol].pen,
        screen_pens[config->gadgetbotcol].pen,1);
    i=0;

    if (uscore = strchr(globstring[STR_CONTINUE],'_')) cont_key = uscore[1];
    else cont_key = globstring[STR_CONTINUE][0];
    cont_key = ToLower(cont_key);

    FOREVER {
        while (IMsg=(struct IntuiMessage *)GetMsg(fontwindow->UserPort)) {
            class=IMsg->Class; code=IMsg->Code;
            ReplyMsg((struct Message *)IMsg);
            switch (class) {
                case IDCMP_DISKREMOVED:
                    setupchangestate();
                    break;
                case IDCMP_DISKINSERTED:
                    if (isd!=-1) {
                        old_change_state=disk_change_state;
                        setupchangestate();
                        if ((old_change_state&(1<<isd))!=(disk_change_state&(1<<isd))) {
                            i=2;
                            break;
                        }
                    }
                    break;
                case IDCMP_VANILLAKEY:
                    if (code!='\r' && ToLower(code)!=cont_key)
                        break;
                    SelectGadget(fontwindow,&contgad);
                case IDCMP_GADGETUP:
                    i=1;
                    break;
            }
        }
        if (i) break;
        Wait(1<<fontwindow->UserPort->mp_SigBit);
    }
    CloseWindow(fontwindow);
    fontwindow=NULL;
    LFreeRemember(&key);
    if (i==2) goto getnewdisk;
}

void get_device_task(lock,buffer,port)
BPTR lock;
char *buffer;
struct MsgPort *port;
{
    struct DeviceList *devlist,*dl;
    struct RootNode *rootnode;
    struct DosInfo *dosinfo;
    struct FileLock *lock2;

    rootnode=(struct RootNode *) DOSBase->dl_Root;
    dosinfo=(struct DosInfo *) BADDR(rootnode->rn_Info);
    devlist=(struct DeviceList *) BADDR(dosinfo->di_DevInfo);
    lock2=(struct FileLock *)BADDR(lock);
    dl=(struct DeviceList *)BADDR(lock2->fl_Volume);

    while (devlist) {
        if (devlist->dl_Type==DLT_DEVICE && devlist->dl_Task==dl->dl_Task) break;
        devlist=(struct DeviceList *) BADDR(devlist->dl_Next);
    }
    if (devlist) BtoCStr((BPTR)devlist->dl_Name,buffer,31);
    else strcpy(buffer,((struct Task *)port->mp_SigTask)->tc_Node.ln_Name);
    strcat(buffer,":");
}

#ifdef __SASC__
void getsizestring(buf,a)
char *buf;
bigint a;
{
    bigint temp1, temp2, temp3, quot, rema;

    temp1 = i64_atoi( "1024" );
    i64_udiv( a, temp1, &quot, &rema );

    a = quot;

    temp1 = i64_atoi( "1073741824" );
    temp2 = i64_atoi( "1048576" );
    temp3 = i64_atoi( "1024" );

    if ( i64_cmp( quot, temp1 ) == I64_GREATER )
    {
        lsprintf( buf, "HUGE" );
    }
    else if ( i64_cmp( quot, temp2 ) == I64_GREATER )
    {
        i64_udiv( a, temp2, &quot, &rema );

        sprintf(buf,"%.1fG", (double)quot.lo + ( (double)rema.lo / 1048576 ));
    }
    else if ( i64_cmp( quot, temp3 ) == I64_GREATER )
    {
        i64_udiv( a, temp3, &quot, &rema );

        sprintf(buf,"%.1fM", (double)quot.lo + ( (double)rema.lo / 1024 ));
    }
    else lsprintf( buf, "%ldK", (long)a.lo );
}
#else
void getsizestring(buf,a)
char *buf;
unsigned long long a;
{
    a/=1024;
    if (a>1073741824)   lsprintf(buf,"HUGE");
    else if (a>1048576) sprintf(buf,"%.1fG",(double)((double)a/1048576));
    else if (a>1024)    sprintf(buf,"%.1fM",(double)((double)a/1024));
    else                lsprintf(buf,"%ldK",(long)a);
}
#endif
/*
void getfloatstr(f,buf)
double f;
char *buf;
{
    int a,b,c,d;
    char buf1[20];

    a=(int)f; f-=a;
    b=(int)(f*100);
    c=(b/10)*10; d=b-c;
    if (d>4) c+=10;
    if (c==100) {
        c=0; ++a;
    }
    lsprintf(buf1,"%ld",c); buf1[1]=0;
    lsprintf(buf,"%ld.%s",a,buf1);
}
*/

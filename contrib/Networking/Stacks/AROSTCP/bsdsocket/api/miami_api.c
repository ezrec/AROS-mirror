//#include <clib/debug_protos.h>
#include <dos/dos.h>
//#include <emul/emulregs.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <exec/resident.h>
#include <exec/semaphores.h>
#include <sys/types.h>
#include <libraries/miami.h>
#include <proto/exec.h>
#include <proto/utility.h>

#define _GRP_H_
#define _PWD_H_
#define USE_INLINE_STDARG
//#include <proto/usergroup.h>

#include <conf.h>
#include <sys/malloc.h>
#include <sys/socket.h>
#include <kern/amiga_log.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_protos.h>
#include <net/pfil.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <syslog.h>
#include <version.h>

#include "amiga_api.h"
#include "gethtbynamadr.h"
#include "miami_api.h"

#include <proto/bsdsocket.h>

#undef SocketBase
#define SocketBase MiamiBase->_SocketBase
#define UserGroupBase MiamiBase->_UserGroupBase

extern ULONG Miami_UserFuncTable[];
extern struct ifnet *ifnet;

void __MiamiLIB_Cleanup(struct MiamiBase *MiamiBase)
{
  void *freestart;
  ULONG size;
  if (SocketBase)
    CloseLibrary(&SocketBase->libNode);
  freestart = (void *)((ULONG)MiamiBase - (ULONG)MiamiBase->Lib.lib_NegSize);
  size = MiamiBase->Lib.lib_NegSize + MiamiBase->Lib.lib_PosSize;
  FreeMem(freestart, size);
}

struct MiamiBase* MiamiLIB_Open(struct Library *MasterBase)
{
  struct MiamiBase *MiamiBase;
  WORD *i;

  D(kprintf("MiamiLIB_Open: 0x%08lx <%s> OpenCount %ld\n",
		     MasterBase,
		     MasterBase->lib_Node.ln_Name,
		     MasterBase->lib_OpenCnt));

  MiamiBase = (struct MiamiBase *)MakeLibrary(Miami_UserFuncTable,
					     (UWORD *)&Miami_initTable,
					     NULL,
					     sizeof(struct MiamiBase),
					     NULL);
  D(kprintf("Created user miami.library base: %08lx\n", MiamiBase);)
  if (MiamiBase) {
    for (i = (WORD *)((struct Library *)MiamiBase + 1); i < (WORD *)(MiamiBase + 1); i++)
      *i = 0L;
    MiamiBase->Lib.lib_OpenCnt = 1;
    SocketBase = OpenLibrary("bsdsocket.library", VERSION);
    if (SocketBase) {
	D(log(LOG_DEBUG,"miami.library opened: SocketBase = 0x%08lx, MiamiBase = 0x%08lx", (ULONG)SocketBase, (ULONG)MiamiBase);)
	return(MiamiBase);
    }
    D(else kprintf("Unable to open bsdsocket.library\n");)
    __MiamiLIB_Cleanup(MiamiBase);
  }
  return(NULL);
}

ULONG	MiamiLIB_Close(struct MiamiBase *MiamiBase)
{
VOID * freestart;
ULONG  size;

  D(kprintf("MiamiLIB_Close: 0x%08lx <%s> OpenCount %ld\n",
		      MiamiBase,
		      MasterMiamiBase->lib_Node.ln_Name,
		      MasterMiamiBase->lib_OpenCnt));

  if (MiamiBase->_UserGroupBase)
    CloseLibrary(MiamiBase->_UserGroupBase);
  __MiamiLIB_Cleanup(MiamiBase);
  MasterMiamiBase->lib_OpenCnt--;
  D(kprintf("MiamiLIB_Close: done\n"));

  return(0);
}

long MiamiSysCtl(void)
{
//	log(LOG_CRIT,"MiamiSysCtl() is not implemented");
	return ENOSYS;
}

void MiamiDisallowDNS(long value, struct MiamiBase *MiamiBase)
{
	DSYSCALLS(log(LOG_DEBUG,"MiamiDisallowDNS(%ld) called",value);)
	if (value)
		SocketBase->res_state.options |= AROSTCP_RES_DISABLED;
	else
		SocketBase->res_state.options &= ~AROSTCP_RES_DISABLED;
}

void *MiamiGetPid(void)
{
	return FindTask(NULL);
}

APTR MiamiPFAddHook(	struct Hook *hook, UBYTE *interface, struct TagItem *taglist)
{
	struct packet_filter_hook *pf = NULL;
	struct ifnet *ifp;

#ifdef ENABLE_PACKET_FILTER
	DPF(log(LOG_DEBUG,"MiamiPFAddHook(0x%08lx, %s) called", hook, interface);)

	ifp = ifunit(interface);
	DPF(log(LOG_DEBUG,"ifp = 0x%08lxn");)
	if (ifp) {
		pf = bsd_malloc(sizeof(struct packet_filter_hook), NULL, NULL);
		DPF(syslog(LOG_DEBUG,"Handle = 0x%08lx", pf);)
		if (pf) {
			pf->pfil_if = ifp;
			pf->pfil_hook = hook;
			pf->pfil_hooktype = GetTagData(MIAMITAG_HOOKTYPE, MIAMICPU_M68KREG, taglist);
			ObtainSemaphore(&pfil_list_lock);
			AddTail((struct List *)&pfil_list, (struct Node *)pf);
			ReleaseSemaphore(&pfil_list_lock);
			DPF(log(LOG_DEBUG,"Added packet filter hook:");)
			DPF(log(LOG_DEBUG,"Function: 0x%08lx", hook->h_Entry);)
			DPF(log(LOG_DEBUG,"CPU type: %lu", pf->pfil_hooktype);)
		}
	}
#else
	//log(LOG_CRIT,"Miami packet filter disabled", NULL);
#endif
	return pf;
}

void MiamiPFRemoveHook(struct Node *handle)
{

	DPF(log("MiamiPFRemoveHook(0x%08lx) called", handle);)
	if (handle) {
		ObtainSemaphore(&pfil_list_lock);
		Remove(handle);
		ReleaseSemaphore(&pfil_list_lock);
		bsd_free(handle, NULL);
	}
}

long MiamiGetHardwareLen(void)
{
//	log(LOG_CRIT,"MiamiGetHardwareLen() is not implemented");
	return NULL;
}

struct Library *MiamiOpenSSL(void)
{
	return NULL;
}

void MiamiCloseSSL(void)
{
}

long MiamiSetSocksConn(void)
{
//	log(LOG_CRIT,"MiamiSetSocksConn() is not implemented");
	return FALSE;
}

long MiamiIsOnline(char *name)
{
	struct ifnet *ifp;
	long online = FALSE;

	DSYSCALLS(log(LOG_DEBUG,"MiamiIsOnline(%s) called", (ULONG)(name ? name : "<NULL>"));)
	if (name && strcmp(name,"mi0")) {
		ifp = ifunit(name);
		if (ifp)
			online = (ifp->if_flags & IFF_UP) ? TRUE : FALSE;
	} else {
		for (ifp = ifnet; ifp; ifp = ifp->if_next) {
			DSYSCALLS(log(LOG_DEBUG,"Checking interface %s%u",ifp->if_name, ifp->if_unit);)
			if ((ifp->if_flags & (IFF_UP | IFF_LOOPBACK)) == IFF_UP) {
				online = TRUE;
				break;
			}
		}
	}
	DSYSCALLS(log(LOG_DEBUG,"MiamiIsOnline() result: %ld", online);)
	return online;
}

void MiamiOnOffline(char *name, int online, struct MiamiBase *MiamiBase)
{
	struct ifnet *ifp;

	ifp = ifunit(name);
	if (ifp)
		ifupdown(ifp, online);
}

char *inet_ntop(long family, char *addrptr, char *strptr, long len)
{
	DSYSCALLS(log(LOG_DEBUG,"inet_ntop(%ld) called",family);)
	if (family == AF_INET)
	{
		sprintf(strptr, "%u.%u.%u.%u", addrptr[0], addrptr[1], addrptr[2], addrptr[3]);
		return addrptr;
	} else {
		log(LOG_CRIT,"inet_ntop(): address family %ld is not implemented", family);
		return NULL;
	}
}

long Miami_inet_aton(char *strptr, struct in_addr *addrptr, struct MiamiBase *MiamiBase)
{
	DSYSCALLS(syslog(LOG_DEBUG,"inet_aton(%s) called", (ULONG)strptr);)
	return __inet_aton(strptr, addrptr);
}

long inet_pton(long family, char *strptr, struct in_addr *addrptr, struct MiamiBase *MiamiBase)
{
	DSYSCALLS(log(LOG_DEBUG,"inet_pton(%ld, %s) called", family, (ULONG)strptr);)
	if (family == AF_INET)
		return __inet_aton(strptr, addrptr);
	else {
		log(LOG_CRIT,"inet_pton(): address family %ld is not implemented", family);
		return 0;
	}
}

struct hostent *gethostbyname2(char *name, long family, struct MiamiBase *MiamiBase)
{
	DSYSCALLS(log(LOG_DEBUG,"gethostbyname2(%s, %ld) called", (ULONG)name, family);)
	if (family == AF_INET)
		return __gethostbyname(name, SocketBase);
	else {
		syslog(LOG_CRIT,"gethostbyname2(): address family %ld is not implemented", family);
		return NULL;
	}
}

char *ai_errors[] = {
	"No error",
	"Address family for host not supported",
	"Temporary failure in name resolution",
	"Invalid flags value",
	"Non-recoverable failure in name resolution",
	"Address family not supported",
	"Memory allocation failure",
	"No address associated with host",
	"Host nor service provided, or not known",
	"Service not supported for socket type",
	"Socket type not supported",
	"System error",
	"Unknown error"
};

char *gai_strerror(unsigned long error )
{
	DSYSCALLS(log(LOG_DEBUG,"gai_strerror(%ld) called", error);)
	if (error > 12)
		error = 12;
	return ai_errors[error];
}

void freeaddrinfo(void)
{
}

long getaddrinfo(struct MiamiBase *MiamiBase)
{
	log(LOG_CRIT,"getaddrinfo() is not implemented");
	writeErrnoValue(SocketBase, ENOSYS);
	return EAI_SYSTEM;
}

long getnameinfo(void)
{
	log(LOG_CRIT,"getnameinfo() is not implemented");
	return ENOSYS;
}

long MiamiSupportsIPV6(void)
{
	DSYSCALLS(log(LOG_DEBUG,"MiamiSupportsIPV6() called");)
	return FALSE;
}

long MiamiResGetOptions(struct MiamiBase *MiamiBase)
{
	DSYSCALLS(log(LOG_DEBUG,"MiamiResGetOptions() called, result: 0x%08lx", SocketBase.res_state.options);)
	return SocketBase->res_state.options;
}

void MiamiResSetOptions(long options, struct MiamiBase *MiamiBase)
{
	DSYSCALLS(log(LOG_DEBUG,"MiamiResSetOptions(0x%08lx) called", options);)
	SocketBase->res_state.options = options;
}

long sockatmark(void)
{
	log(LOG_CRIT,"sockatmark() is not implemented");
	return 0;
}

void MiamiSupportedCPUs(unsigned long *apis, unsigned long *callbacks, unsigned long *kernel)
{
	DSYSCALLS(log(LOG_DEBUG,"MiamiSupportedCPUs() called");)
	*apis = MIAMICPU_M68KREG;
	*callbacks = MIAMICPU_M68KREG;
	*kernel = MIAMICPU_PPCV4;
}

long MiamiGetFdCallback(fdCallback_t *cbptr, struct MiamiBase *MiamiBase)
{
	*cbptr = SocketBase->fdCallback;
	DSYSCALLS(log(LOG_DEBUG,"MiamiGetFdCallback() called, *cbptr = 0x%08lx", (ULONG)*cbptr);)
	return *cbptr ? MIAMICPU_M68KREG : 0;
}

long MiamiSetFdCallback(fdCallback_t cbptr, long cputype, struct MiamiBase *MiamiBase)
{
	DSYSCALLS(log(LOG_DEBUG,"MiamiSetFdCallback(0x%08lx, %ld) called", (ULONG)cbptr, cputype);)
	if (cputype == MIAMICPU_M68KREG) {
		SocketBase->fdCallback = cbptr;
		return TRUE;
	} else
		return FALSE;
}

void SetSysLogPort(void)
{	 
	DSYSCALLS(log(LOG_DEBUG,"SetSysLogPort() called");)
	ExtLogPort = FindPort("SysLog");
}

int Miami_sethostname(const char *name, size_t namelen)
{
	return sethostname(name, namelen);
}

LONG Miami_QueryInterfaceTagList(STRPTR interface_name, struct TagItem *tags, struct MiamiBase *MiamiBase)
{
	return QueryInterfaceTagList(interface_name, tags);
}

struct UserGroupCredentials *MiamiGetCredentials(struct MiamiBase *MiamiBase)
{
	/* We don't want to have this library at all, so we open it only if we really need it */
        if (!UserGroupBase)
			UserGroupBase = OpenLibrary("usergroup.library",4);

	DSYSCALLS(log(LOG_DEBUG,"MiamiGetCredentials(): UserGroupBase = 0x%08lx", (ULONG)UserGroupBase);)
#warning "TODO: uncomment the following lines once we have a working usergroups.library implemenetation"
//		if (UserGroupBase)
//		return getcredentials(NULL);
	else
		return NULL;
}


/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * 
 * Created: Sun Feb 14 18:35:48 1993 too
 * Last modified: Wed Feb 16 01:18:32 1994 jraja
 * 
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:21:39  henrik
 * lots of assembler in api/amiga_api.h that needs to be fixed
 * better not do it since my knowledge of assembler i almost zero
 * think one need to change adresses to something like eax,ebx or something?
 *
 * Revision 3.5  1994/03/22  08:41:36  jraja
 * Added calls to the libPtr->fdCallback to the appropriate places.
 *
 * Revision 3.4  1994/02/15  23:18:42  jraja
 * Changed sdFind() to return the sd via LONG * instead of ULONG * to be
 * consistent with API types.
 *
 * Revision 3.3  1994/01/07  15:40:29  too
 * Bug fixes after revision 3.1. Now tested.
 *
 * Revision 3.2  1994/01/06  13:39:11  too
 * Moved send and recv functions to amiga_sendrecv.c
 *
 * Revision 3.1  1994/01/04  14:32:29  too
 * Added function sdFind(). Revised socket() and accept() to use sdFind()
 * and socket usage bitmask.
 *
 * Revision 1.25  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 */

#include <conf.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/synch.h>
#include <sys/errno.h>
#include <sys/my-errno.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/execbase.h>
#include <proto/exec.h>

#include <api/amiga_api.h>
#include <api/amiga_libcallentry.h>
#include <api/sockargs.h>

#include <amitcp/socketbasetags.h>

#include <kern/uipc_socket_protos.h>
#include <kern/uipc_socket2_protos.h>

LONG SAVEDS RAF4(_socket,
		 struct SocketBase *, 	libPtr,		a6,
		 LONG,			domain,		d0,
		 LONG,			type,		d1,
		 LONG,			protocol,	d2)

#if 0
{
#endif
  
  struct socket *so;
  LONG fd, error;
/* Let's check out what's going wrong here, exactly 
  CHECK_TASK();
  */
    if (libPtr->thisTask != SysBase->ThisTask) 
      {
	struct Task * wTask = SysBase->ThisTask;
	log(LOG_CRIT, wrongTaskErrorFmt, wTask,
	    wTask->tc_Node.ln_Name,	libPtr->thisTask,
	    libPtr->thisTask->tc_Node.ln_Name); 
	return -1;	    
      }      


  if ( (error = sdFind(libPtr, &fd)) )
      goto Return;
  
  ObtainSyscallSemaphore(libPtr);
  error = socreate(domain, &so, type, protocol);
  ReleaseSyscallSemaphore(libPtr);

  if (! error) {
    /*
     * Tell the link library about the new fd
     */
    if (libPtr->fdCallback)
      error = libPtr->fdCallback(fd, FDCB_ALLOC);
    if (! error) {
      so->so_refcnt = 1;		/* reference count is pure AmiTCP addition */
      libPtr->dTable[fd] = so;
      FD_SET(fd, (fd_set *)(libPtr->dTable + libPtr->dTableSize));
    }
  }
  
 Return: API_STD_RETURN(error, fd);
}


LONG SAVEDS RAF4(_bind,
		 struct SocketBase *,	libPtr,		a6,
		 LONG,			s,		d0,
		 caddr_t,		name,		a0,
		 LONG,			namelen,	d1)
#if 0
{
#endif  

  struct socket *so;
  struct mbuf *nam;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);
  
  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;
  if ( (error = sockArgs(&nam, name, namelen, MT_SONAME)) )
    goto Return;
  error = sobind(so, nam);
  m_freem(nam);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG SAVEDS RAF3(_listen,
		 struct SocketBase *,	libPtr,		a6,
		 LONG,			s,		d0,
		 LONG,			backlog,	d1)
#if 0
{
#endif

  struct socket *so;
  LONG error;
  
  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);
  
  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;
  error = solisten(so, backlog);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);

}
    
LONG SAVEDS RAF4(_accept,
		 struct SocketBase *,	libPtr,		a6,
		 LONG,			s,		d0,
		 caddr_t,		name,		a0,
		 ULONG *,		anamelen,	a1)
#if 0
{
#endif

  struct socket *so;
  struct mbuf *nam;
  spl_t old_spl;
  LONG error, fd;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;

  old_spl = splnet();
  if ((so->so_options & SO_ACCEPTCONN) == 0) {
    error = EINVAL;
    goto Return_spl;
  }
  if ((so->so_state & SS_NBIO) && so->so_qlen == 0) {
    error = EWOULDBLOCK;
    goto Return_spl;
  }
  while (so->so_qlen == 0 && so->so_error == 0) 
    {
      if (so->so_state & SS_CANTRCVMORE) 
	{
	  so->so_error = ECONNABORTED;
	  break;
	}
      if ( (error = tsleep(libPtr, (caddr_t)&so->so_timeo, netcon, NULL)) )
	{
	  goto Return_spl;
	}
    }
  if (so->so_error) 
    {
      error = so->so_error;
      so->so_error = 0;
      goto Return_spl;
    }
   
  if ( (error = sdFind(libPtr, &fd)) )
    {
      goto Return_spl;
    }

  /*
   * Tell the link library about the new fd
   */
  if (libPtr->fdCallback)
    if ( (error = libPtr->fdCallback(fd, FDCB_ALLOC)) )
      goto Return_spl;

  {
    struct socket *aso = so->so_q;
    if (soqremque(aso, 1) == 0)
      panic("accept");
    so = aso;
  }	

  libPtr->dTable[fd] = so;
  FD_SET(fd, (fd_set *)(libPtr->dTable + libPtr->dTableSize));
  so->so_refcnt = 1;  /* pure AmiTCP addition */

  nam = m_get(M_WAIT, MT_SONAME);
  (void)soaccept(so, nam);  /* is this always successful */
  if (name) {
    if (*anamelen > nam->m_len)
      *anamelen = nam->m_len;
    /* SHOULD COPY OUT A CHAIN HERE */
    aligned_bcopy(mtod(nam, caddr_t), (caddr_t)name, (u_int)*anamelen);
  }
  m_freem(nam);

 Return_spl:
  splx(old_spl);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, fd);
}

LONG SAVEDS RAF4(_connect,
		 struct SocketBase *,	libPtr,		a6,
		 LONG,			s,		d0,
		 caddr_t,		name,		a0,
		 LONG,			namelen,	d1)
#if 0
{
#endif

  /*register*/ struct socket *so;
  struct mbuf *nam;
  LONG error;
  spl_t old_spl;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;
  if  ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) 
    {
      error = EALREADY;
      goto Return;
    }
  if ( (error = sockArgs(&nam, name, namelen, MT_SONAME)) )
    goto Return;
  error = soconnect(so, nam);
  if (error)
    goto bad;
  if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) 
    {
      m_freem(nam);
      error = EINPROGRESS;
      goto Return;
    }	
  old_spl = splnet();
  while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0)
    if ( (error = tsleep(libPtr,(caddr_t)&so->so_timeo, netcon, NULL)) )
      break;
  if (error == 0) 
    {
      error = so->so_error;
      so->so_error = 0;
    }
  splx(old_spl);
 bad:
  so->so_state &= ~SS_ISCONNECTING;
  m_freem(nam);
  if (error == ERESTART)
    error = EINTR;

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);

}

LONG SAVEDS RAF3(_shutdown,
		 struct SocketBase *,	libPtr,	a6,
		 LONG,			s,	d0,
		 LONG,			how,	d1)
#if 0
{
#endif

  struct socket *so;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;

  error = soshutdown(so, how);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG SAVEDS RAF6(_setsockopt,
		 struct SocketBase *,	libPtr,		a6,
		 LONG,			s,		d0,
		 LONG,			level,		d1,
		 LONG,			name,		d2,
		 caddr_t,		val,		a0,
		 ULONG,			valsize,	d3)
#if 0
{
#endif

  struct socket *so;
  struct mbuf *m = NULL;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;
  if (valsize > MLEN) { /* unsigned catches negative values */
    error = EINVAL;
    goto Return;
  }
  if (val) {
    m = m_get(M_WAIT, MT_SOOPTS);
    if (m == NULL) {
      error = ENOBUFS;
      goto Return;
    }
    bcopy(val, mtod(m, caddr_t), valsize); /* aligned ? */
    m->m_len = (int)valsize;
  }
  error = sosetopt(so, level, name, m);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}


LONG SAVEDS RAF6(_getsockopt,
		 struct SocketBase *,	libPtr,		a6,
		 LONG,			s,		d0,
		 LONG,			level,		d1,
		 LONG,			name,		d2,
		 caddr_t,		val,		a0,
		 ULONG *,		avalsize,	a1)
#if 0
{
#endif

  struct socket *so;
  struct mbuf *m = NULL;
  ULONG valsize, error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, s, &so)) )
    goto Return;
  
  if (val)
    valsize = *avalsize;
  else
    valsize = 0;
  
  if ((error = sogetopt(so, level, name, &m)) == 0
      && val && valsize && m != NULL) {
    if (valsize > m->m_len)  /* valsize is ULONG */
      valsize = m->m_len;
    bcopy(mtod(m, caddr_t), val, (u_int)valsize); /* aligned ? */
    *avalsize = valsize;
  }
  if (m != NULL)
    (void) m_free(m);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG SAVEDS RAF4(_getsockname,
		 struct SocketBase *,	libPtr,	a6,
		 LONG,			fdes,	d0,
		 caddr_t,		asa,	a0,
		 ULONG *,		alen,	a1)
#if 0
{
#endif

  /*register*/
  struct socket *so;
  struct mbuf *m;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, fdes, &so)) )
    goto Return;

  m = m_getclr(M_WAIT, MT_SONAME);    
  if (m == NULL) {
    error = ENOBUFS;
    goto Return;
  }
  if ( (error = (*so->so_proto->pr_usrreq)(so, PRU_SOCKADDR, 0, m, 0)) )
    goto bad;
  if (*alen > m->m_len)
    *alen = m->m_len;
  aligned_bcopy(mtod(m, caddr_t), (caddr_t)asa, (u_int)*alen);

 bad:
  m_freem(m);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG SAVEDS RAF4(_getpeername,
		 struct SocketBase *,	libPtr,	a6,
		 LONG,			fdes,	d0,
		 caddr_t,		asa,	a0,
		 ULONG *,		alen,	a1)
#if 0
{
#endif

  /*register*/
  struct socket *so;
  struct mbuf *m;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if ( (error = getSock(libPtr, fdes, &so)) )
    goto Return;

  if ((so->so_state & (SS_ISCONNECTED|SS_ISCONFIRMING)) == 0) {
    error = ENOTCONN;
    goto Return;
  }

  m = m_getclr(M_WAIT, MT_SONAME);
  if (m == NULL) {
    error = ENOBUFS;
    goto Return;
  }

  if ( (error = (*so->so_proto->pr_usrreq)(so, PRU_PEERADDR, 0, m, 0)) )
    goto bad;
  if (*alen > m->m_len)
    *alen = m->m_len;
  aligned_bcopy(mtod(m, caddr_t), (caddr_t)asa, (u_int)*alen);

 bad:
  m_freem(m);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG sockArgs(struct mbuf **mp,
		     caddr_t buf,	/* aligned */
		     LONG buflen,
		     LONG type)
{
  register struct mbuf *m;
  LONG error = 0;

  if ((u_int)buflen > MLEN)
    return (EINVAL);

  m = m_get(M_WAIT, type);
  if (m == NULL)
    return (ENOBUFS);
  m->m_len = buflen;

  aligned_bcopy(buf, mtod(m, caddr_t), (u_int)buflen);
  *mp = m;
  if (type == MT_SONAME)
    mtod(m, struct sockaddr *)->sa_len = buflen;

  return (error);
}

/*
 * sdFind replaces old fdAlloc. This version now looks for free socket
 * from socket usage bitmask stored right after descriptor table
 */

LONG sdFind(struct SocketBase * libPtr, LONG *fdp)
{
  int bit, moffset;
  ULONG * smaskp;
  int mlongs = (libPtr->dTableSize - 1) / NFDBITS + 1;

  moffset = 0, smaskp = (ULONG  *)(libPtr->dTable + libPtr->dTableSize);
  while (mlongs) {
    unsigned long cmask = *smaskp;
    unsigned long dmask = cmask + 1;

    if (dmask == 0) {
      mlongs--, smaskp++, moffset += 32;
      continue;	/* current  mask is full (cmask = 0xFFFFFFFF) */
    }
    cmask = ((cmask ^ dmask) >> 1) + 1; /* now only one bit set ! */

    bit = (cmask & 0xFFFF0000)? 16: 0;
    if (cmask & 0xFF00FF00) bit += 8;
    if (cmask & 0xF0F0F0F0) bit += 4;
    if (cmask & 0xCCCCCCCC) bit += 2;
    if (cmask & 0xAAAAAAAA) bit += 1;

    /*
     * Check if link library agrees with us on the next free fd...
     */
    if (libPtr->fdCallback)
      if (libPtr->fdCallback(moffset + bit, FDCB_CHECK)) {
	*smaskp |= cmask; /* mark this fd as used */
	continue; /* search for the next _bit_ */
      }

    if (moffset + bit >= libPtr->dTableSize)
      break;
    else {
      *fdp = moffset + bit;
      return 0;
    }
  }
  return (EMFILE);
}

/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
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

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>

#include <api/amiga_api.h>
#include <api/amiga_libcallentry.h>
#include <api/sockargs.h>

#include <bsdsocket/socketbasetags.h>

#include <kern/uipc_socket_protos.h>
#include <kern/uipc_socket2_protos.h>


LONG SAVEDS socket(
   REG(d0, LONG domain),
   REG(d1, LONG type),
   REG(d2, LONG protocol),
   REG(a6, struct SocketBase *libPtr))
{
  struct socket *so;
  LONG fd, error;

  CHECK_TASK();

  if (error = sdFind(libPtr, &fd))
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


LONG SAVEDS bind(
   REG(d0, LONG s),
   REG(a0, caddr_t name),
   REG(d1, LONG namelen),
   REG(a6, struct SocketBase *libPtr))
{
  struct socket *so;
  struct mbuf *nam;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);
  
  if (error = getSock(libPtr, s, &so))
    goto Return;
  if (error = sockArgs(&nam, name, namelen, MT_SONAME))
    goto Return;
  error = sobind(so, nam);
  m_freem(nam);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG SAVEDS listen(
   REG(d0, LONG s),
   REG(d1, LONG backlog),
   REG(a6, struct SocketBase *libPtr))
{
  struct socket *so;
  LONG error;
  
  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);
  
  if (error = getSock(libPtr, s, &so))
    goto Return;
  error = solisten(so, backlog);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);

}
    
LONG SAVEDS accept(
   REG(d0, LONG s),
   REG(a0, caddr_t name),
   REG(a1, ULONG *anamelen),
   REG(a6, struct SocketBase *libPtr))
{
  struct socket *so;
  struct mbuf *nam;
  spl_t old_spl;
  LONG error, fd;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, s, &so))
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
  while (so->so_qlen == 0 && so->so_error == 0) {
    if (so->so_state & SS_CANTRCVMORE) {
      so->so_error = ECONNABORTED;
      break;
    }
    if (error = tsleep(libPtr, (caddr_t)&so->so_timeo, netcon, NULL)) {
      goto Return_spl;
    }
  }
  if (so->so_error) {
    error = so->so_error;
    so->so_error = 0;
    goto Return_spl;
  }
   
  if (error = sdFind(libPtr, &fd)) {
    goto Return_spl;
  }

  /*
   * Tell the link library about the new fd
   */
  if (libPtr->fdCallback)
    if (error = libPtr->fdCallback(fd, FDCB_ALLOC))
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

LONG SAVEDS connect(
   REG(d0, LONG s),
   REG(a0, caddr_t name),
   REG(d1, LONG namelen),
   REG(a6, struct SocketBase *libPtr))
{
  /*register*/ struct socket *so;
  struct mbuf *nam;
  LONG error;
  spl_t old_spl;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, s, &so))
    goto Return;
  if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
    error = EALREADY;
    goto Return;
  }
  if (error = sockArgs(&nam, name, namelen, MT_SONAME))
    goto Return;
  error = soconnect(so, nam);
  if (error)
    goto bad;
  if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
    m_freem(nam);
    error = EINPROGRESS;
    goto Return;
  }	
  old_spl = splnet();
  while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0)
    if (error = tsleep(libPtr,(caddr_t)&so->so_timeo, netcon, NULL))
      break;
  if (error == 0) {
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

LONG SAVEDS shutdown(
   REG(d0, LONG s),
   REG(d1, LONG how),
   REG(a6, struct SocketBase *libPtr))
{
  struct socket *so;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, s, &so))
    goto Return;

  error = soshutdown(so, how);

 Return:
  ReleaseSyscallSemaphore(libPtr);
  API_STD_RETURN(error, 0);
}

LONG SAVEDS setsockopt(
   REG(d0, LONG s),
   REG(d1, LONG level),
   REG(d2, LONG name),
   REG(a0, caddr_t val),
   REG(d3, ULONG valsize),
   REG(a6, struct SocketBase *libPtr))
{
  struct socket *so;
  struct mbuf *m = NULL;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, s, &so))
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


LONG SAVEDS getsockopt(
   REG(d0, LONG s),
   REG(d1, LONG level),
   REG(d2, LONG name),
   REG(a0, caddr_t val),
   REG(a1, ULONG * avalsize),
   REG(a6, struct SocketBase * libPtr))
{
  struct socket *so;
  struct mbuf *m = NULL;
  ULONG valsize, error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, s, &so))
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

LONG SAVEDS getsockname(
   REG(d0, LONG fdes),
   REG(a0, caddr_t asa),
   REG(a1, ULONG * alen),
   REG(a6, struct SocketBase * libPtr))
{
  /*register*/
  struct socket *so;
  struct mbuf *m;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, fdes, &so))
    goto Return;

  m = m_getclr(M_WAIT, MT_SONAME);    
  if (m == NULL) {
    error = ENOBUFS;
    goto Return;
  }
  if (error = (*so->so_proto->pr_usrreq)(so, PRU_SOCKADDR, 0, m, 0))
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

LONG SAVEDS getpeername(
   REG(d0, LONG fdes),
   REG(a0, caddr_t asa),
   REG(a1, ULONG * alen),
   REG(a6, struct SocketBase * libPtr))
{
  struct socket *so;
  struct mbuf *m;
  LONG error;

  CHECK_TASK();
  ObtainSyscallSemaphore(libPtr);

  if (error = getSock(libPtr, fdes, &so))
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

  if (error = (*so->so_proto->pr_usrreq)(so, PRU_PEERADDR, 0, m, 0))
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

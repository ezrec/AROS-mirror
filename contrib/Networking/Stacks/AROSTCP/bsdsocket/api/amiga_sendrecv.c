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

#include <kern/uipc_socket_protos.h>
#include <kern/uipc_socket2_protos.h>
  
static LONG sendit(struct SocketBase * p,
		   LONG	s,
		   struct msghdr * mp,
		   LONG flags,
		   LONG * retsize);

static LONG recvit(struct SocketBase * p,
		   LONG s,
		   struct msghdr * mp,
		   LONG flags,
		   LONG * namelenp,
		   LONG * retsize);


LONG SAVEDS send(
   REG(d0, LONG s),
   REG(a0, caddr_t buf),
   REG(d1, LONG len),
   REG(d2, LONG flags),
   REG(a6, struct SocketBase *libPtr))
{
  struct msghdr msg;
  struct iovec aiov;
  LONG error, retval;
  
  CHECK_TASK();

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &aiov;
  msg.msg_iovlen = 1;
  msg.msg_control = 0;
  aiov.iov_base = buf;
  aiov.iov_len = len;
  
  ObtainSyscallSemaphore(libPtr);
  error = sendit(libPtr, s, &msg, flags, &retval);
  ReleaseSyscallSemaphore(libPtr);
  
  API_STD_RETURN(error, retval);
}

LONG SAVEDS sendto(
   REG(d0, LONG s),
   REG(a0, caddr_t buf),
   REG(d1, LONG len),
   REG(d2, LONG flags),
   REG(a1, caddr_t to),
   REG(d3, LONG tolen),
   REG(a6, struct SocketBase *libPtr))
{
  struct msghdr msg;
  struct iovec aiov;
  LONG error, retval;

  CHECK_TASK();

  msg.msg_name = to;
  msg.msg_namelen = tolen;
  msg.msg_iov = &aiov;
  msg.msg_iovlen = 1;
  msg.msg_control = 0;
  aiov.iov_base = buf;
  aiov.iov_len = len;

  ObtainSyscallSemaphore(libPtr);
  error = sendit(libPtr, s, &msg, flags, &retval);
  ReleaseSyscallSemaphore(libPtr);
  
  API_STD_RETURN(error, retval);
}

LONG SAVEDS sendmsg(
   REG(d0, LONG s),
   REG(a0, struct msghdr *msg_p),
   REG(d1, LONG flags),
   REG(a6, struct SocketBase *libPtr))
{
  LONG error, retval;

  CHECK_TASK();

  ObtainSyscallSemaphore(libPtr);
  error = sendit(libPtr, s, msg_p, flags, &retval);
  ReleaseSyscallSemaphore(libPtr);

  API_STD_RETURN(error, retval);
}

static LONG sendit(struct SocketBase * p,
		   LONG	s,
		   struct msghdr * mp,
		   LONG flags,
		   LONG * retsize)
{
  struct socket *so;
  struct uio auio;
  register int i;
  register struct iovec *iov;
  struct mbuf *to, *control;
  LONG len, error;

  if (error = getSock(p, s, &so))
    return (error);

  auio.uio_iov = mp->msg_iov;
  auio.uio_iovcnt = mp->msg_iovlen;
  auio.uio_procp = p;
  auio.uio_resid = 0;
  iov = mp->msg_iov;

  for(i = 0; i < mp->msg_iovlen; i++, iov++) {
    if (iov->iov_len < 0)
      return (EINVAL);
    if ((auio.uio_resid += iov->iov_len) < 0)
      return (EINVAL);
  }
  
  if (mp->msg_name) {
    if (error = sockArgs(&to, mp->msg_name, mp->msg_namelen, MT_SONAME))
      return (error);
  }
  else
    to = 0;
  
  if (mp->msg_control) {
    if (mp->msg_controllen < sizeof (struct cmsghdr)) {
      error = EINVAL;
      goto bad;
    }
    if (error = sockArgs(&control, mp->msg_control,
			 mp->msg_controllen, MT_CONTROL))
      goto bad;
  }
  else
    control = 0;

  len = auio.uio_resid;
  if (error = sosend(so, to, &auio, (struct mbuf *)0, control, flags)) {
    if (auio.uio_resid != len && (error == ERESTART || error == EINTR ||
				  error == EWOULDBLOCK))
      error = 0;
  }
  if (error == 0)
    *retsize = len - auio.uio_resid;

  /* sosend() frees control if allocated */
 bad:
  if (to)
    m_freem(to);

  return (error);
}

LONG SAVEDS recv(
   REG(d0, LONG s),
   REG(a0, caddr_t buf),
   REG(d1, LONG len),
   REG(d2, LONG flags),
   REG(a6, struct SocketBase *libPtr))
{
  struct msghdr msg;
  struct iovec aiov;
  LONG error, retval;

  CHECK_TASK();

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &aiov;
  msg.msg_iovlen = 1;
  msg.msg_control = 0;
  aiov.iov_base = buf;
  aiov.iov_len = len;

  ObtainSyscallSemaphore(libPtr);
  error = recvit(libPtr, s, &msg, flags, NULL, &retval);
  ReleaseSyscallSemaphore(libPtr);

  API_STD_RETURN(error, retval);
}

LONG SAVEDS recvfrom(
   REG(d0, LONG s),
   REG(a0, caddr_t buf),
   REG(d1, LONG len),
   REG(d2, LONG flags),
   REG(a1, caddr_t from),
   REG(a2, LONG * fromlenaddr),
   REG(a6, struct SocketBase *libPtr))
{
  struct msghdr	msg;
  struct iovec aiov;
  LONG error, retval;

  CHECK_TASK();

  if (fromlenaddr)
    msg.msg_namelen = *fromlenaddr;
  else
    msg.msg_namelen = 0;

  msg.msg_name = from;
  msg.msg_iov = &aiov;
  msg.msg_iovlen = 1;
  msg.msg_control = 0;
  aiov.iov_base = buf;
  aiov.iov_len = len;

  ObtainSyscallSemaphore(libPtr);
  error = recvit(libPtr, s, &msg, flags, fromlenaddr, &retval);
  ReleaseSyscallSemaphore(libPtr);

  API_STD_RETURN(error, retval);
}

LONG SAVEDS recvmsg(
   REG(d0, LONG s),
   REG(a0, struct msghdr *msg_p),
   REG(d1, LONG flags),
   REG(a6, struct SocketBase *libPtr))
{
  LONG error, retval;
  
  CHECK_TASK();

  ObtainSyscallSemaphore(libPtr);
  error = recvit(libPtr, s, msg_p, flags, NULL, &retval);
  ReleaseSyscallSemaphore(libPtr);

  API_STD_RETURN(error, retval);
}

static LONG recvit(struct SocketBase * p,
		   LONG s,
		   struct msghdr * mp,
		   LONG flags,
		   LONG * namelenp,
		   LONG * retsize)
{
  struct socket * so;
  struct uio auio;
  struct iovec *iov;
  register int i;
  ULONG len;
  LONG error;
  struct mbuf *from = 0, *control = 0;

  if (error = getSock(p, s, &so))
    return (error);

  auio.uio_iov = mp->msg_iov;
  auio.uio_iovcnt = mp->msg_iovlen;
  auio.uio_procp = p;
  auio.uio_resid = 0;

  iov = mp->msg_iov;
  for(i = 0; i < mp->msg_iovlen; i++, iov++) {
    if (iov->iov_len < 0)
      return (EINVAL);
    if ((auio.uio_resid += iov->iov_len) < 0)
      return (EINVAL);
  }
  len = auio.uio_resid;
  if (error = soreceive(so, &from, &auio,
			(struct mbuf **)0, &control, (int *)&flags))
    if (auio.uio_resid != len && (error == ERESTART || error == EINTR ||
				  error == EWOULDBLOCK))
      error = 0;
  
  if (error)
    goto out;
  
  *retsize = len - auio.uio_resid;
  
  if (mp->msg_name) {
    len = mp->msg_namelen;
    if (len <= 0 || from == 0)
      len = 0;
    else {
      if (len > from->m_len)
	len = from->m_len;
      aligned_bcopy(mtod(from, caddr_t), (caddr_t)mp->msg_name, (unsigned)len);
    }
    mp->msg_namelen = len;
    if (namelenp)
      *namelenp = len;
  }
  if (mp->msg_control) {
    len = mp->msg_controllen;
    if (len <= 0 || control == 0)
      len = 0;
    else {
      if (len >= control->m_len)
	len = control->m_len;
/*      else
	flags |= MSG_CTRUNC;  no syscall ever does something w/ mp->flags */
      
      aligned_bcopy(mtod(control, caddr_t),
		    (caddr_t)mp->msg_control, (unsigned)len);
    }
    mp->msg_controllen = len;
  }
 out:
  if (from)
    m_freem(from);
  if (control)
    m_freem(control);
  
  return (error);
}

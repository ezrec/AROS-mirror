/*
 * NETWORKING_HELPERS.C, Copyright (C) Stefano Crosara 2006
 *
 * Implements some functions that help in network programming
 *
*/

/* Amiga Includes */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#ifdef __AROS__
//#include <sys/net_types.h>
#include <proto/dos.h>
#include <proto/socket.h>
#include <string.h>
#endif

#ifdef __MORPHOS__
#include <sys/time.h>
#include <ppcinline/dos.h>
#include <ppcinline/socket.h>
#define select(a,b,c,d,e) WaitSelect(a,b,c,d,e,0)
extern struct Library *SocketBase;
#endif

#include <caf/caf_types.h>
#include <caf/networking_helpers.h>
#include <caf/caf_debugging.h>

//#define memset caf_memset_local

int g_caf_last_sockerr;

static void SetLastSockerr(int socket)
{
	int ret, optlen;
	
	optlen = sizeof(ret);
	getsockopt (socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
	g_caf_last_sockerr = ret;
}

int GetLastNetErr()
{
	return g_caf_last_sockerr;
}

bool SetBlocking(SOCKET socket, bool b_block)
{
	u_long opt;
	bool b_ret;
	
	g_caf_last_sockerr = 0;
	opt = (b_block == true) ? 0: 1;
	if (IoctlSocket(socket, FIONBIO, (char *) &opt) == -1) {
		b_ret = false;
	} else {
		b_ret = true;
	}
	
	return b_ret;
}

int ReadSocket(void *in_dest, SOCKET socket, int maxlen)
{
	char *p_dest = in_dest;
	int ret;
	
	g_caf_last_sockerr = 0;
	#ifdef DEBUG
	DebugOutput("Readsocket()\n");	
	#endif
	ret = recv(socket, p_dest, maxlen, 0);
	if (ret < 0) {
		// Socket error...
		#ifdef DEBUG
		DebugOutput("recv() error\n");
		#endif
		SetLastSockerr(socket);
		return false;
	}
	#ifdef DEBUG
	DebugOutput("Socket read()\n");
	#endif

    return true;
}

bool ReadSocketNB(void *in_dest, SOCKET socket, int maxlen, int wait_sec, int wait_usec)
{
	char *buffer = (char *) in_dest;
	int to_recv = maxlen, nbytes, pos, ret;
	
	g_caf_last_sockerr = 0;
	pos = 0;
	while (to_recv) {
 	    if((ret = CanReadSocket(socket, wait_sec, wait_usec)) == SOCKET_SELECT_OK) {
				// Riceve finché to_recv non va a zero, oppure errore
				nbytes = recv(socket, &buffer[pos], to_recv, 0);
				if (nbytes < 1) {
					// Socket error...
					SetLastSockerr(socket);
					return false;
				}
				
				// Toglie i byte mandati
				to_recv -= nbytes;	
				
				// Aggiugne i byte mandati
				pos += nbytes;
 		} else {
		  	   return false;
		}
	}
	
	return true;
}

bool WriteSocket(SOCKET socket, void *in_buffer, int len)
{
	char *buffer = (char *) in_buffer;
	int to_send = len, ret, pos;
	
	g_caf_last_sockerr = 0;
	pos = 0;
	while (to_send) {
		// Manda finché to_send non va a zero, oppure errore
		ret = send(socket, &buffer[pos], to_send, 0);
		if (ret < 1) {
			// Socket error...
			SetLastSockerr(socket);
			return false;
		}
		
		// Toglie i byte mandati
		to_send -= ret;	
		
		// Aggiugne i byte mandati
		pos += ret;
	}
	
	return true;
}

bool WriteSocketNB(SOCKET socket, void *in_buffer, int len, int wait_sec, int wait_usec)
{
	char *buffer = (char *) in_buffer;
	int to_send = len, nbytes, pos, ret;
	
	g_caf_last_sockerr = 0;
	pos = 0;
	while (to_send) {
 	    if((ret = CanWriteSocket(socket, wait_sec, wait_usec)) == SOCKET_SELECT_OK) {
				// Manda finché to_send non va a zero, oppure errore
				nbytes = send(socket, &buffer[pos], to_send, 0);
				if (nbytes < 1) {
					// Socket error...
					SetLastSockerr(socket);
					return false;
				}
				
				// Toglie i byte mandati
				to_send -= nbytes;	
				
				// Aggiugne i byte mandati
				pos += nbytes;
 		} else {
		  	   return false;
		}
	}
	
	return true;
}

int CanWriteSocket(SOCKET socket, int wait_sec, int wait_usec)
{
	struct fd_set fdwrite, fdexcept;
	struct timeval timeval;
	int ret;
	
	g_caf_last_sockerr = 0;
	
	FD_ZERO(&fdwrite);
	FD_SET(socket, &fdwrite);
	
	FD_ZERO(&fdexcept);
	FD_SET(socket, &fdexcept);
	
	timeval.tv_sec = wait_sec;
	timeval.tv_usec = wait_usec;
	
	ret = select(socket+1, 0, &fdwrite, 0 /*&fdexcept*/, &timeval);
	if ((ret == SOCKET_ERROR) /* || (FD_ISSET(socket, &fdexcept))*/) {
		/*
		if (FD_ISSET(socket, &fdexcept))
			DebugOutput("FDExcept() is set\n");
		*/
		
		SetLastSockerr(socket);
		#ifdef DEBUG
		DebugOutput("select() error\n");
		#endif
		return SOCKET_ERROR;
	}
	
	if (FD_ISSET(socket, &fdwrite)) {
		#ifdef DEBUG
		DebugOutput("Can write to socket\n");
		#endif
		return SOCKET_SELECT_OK;
	}
		
	return SOCKET_TIMEVAL_EXPIRED;
}

int CanReadSocket(SOCKET socket, int wait_sec, int wait_usec)
{
	struct fd_set fdread, fdexcept;
	struct timeval timeval;
	int ret;
	
	g_caf_last_sockerr = 0;
	FD_ZERO(&fdread);
	FD_SET(socket, &fdread);
	FD_ZERO(&fdexcept);
	FD_SET(socket, &fdexcept);
	timeval.tv_sec = wait_sec;
	timeval.tv_usec = wait_usec;
	
	#ifdef DEBUG
	DebugOutput("selecting()...\n");
	#endif
	ret = select(socket+1, &fdread, 0, 0 /*&fdexcept*/, &timeval);
//	ret = WaitSelect(socket, &fdread, 0, 0 /*&fdexcept*/, &timeval, 0);
	#ifdef DEBUG
	DebugOutput("seleted()\n");
	#endif
	if ((ret == SOCKET_ERROR) /* || (FD_ISSET(socket, &fdexcept))*/) {
//	if (ret < 0) {
		SetLastSockerr(socket);
		#ifdef DEBUG
		DebugOutput("select() error\n");
		#endif
		return SOCKET_ERROR;
	}
	
	if (FD_ISSET(socket, &fdread)) {
		#ifdef DEBUG
		DebugOutput("Can read from socket\n");
		#endif
		return SOCKET_SELECT_OK;
	}
	
	#ifdef DEBUG
	DebugOutput("SOCKET_TIMEVAL_EXPIRED\n");
	#endif
	return SOCKET_TIMEVAL_EXPIRED;
}


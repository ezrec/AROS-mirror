#ifndef AMITCP_BIGNAMES_H
#define AMITCP_BIGNAMES_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

/*
 * Writing standard bsd socket functions as capitalized letters was a
 * mistage which was changed in this release. However, someone may have
 * used those in their programs, and this file is written for their
 * convenience. Just include this file before other amitcp includes if
 * you have used Capitalized letters in your program code.
 */

#define AMITCP_BIG_NAMES	/* needed for pragmas */

#define socket Socket
#define bind Bind
#define listen Listen
#define accept Accept
#define connect Connect
#define send Send
#define sendto SendTo
#define sendmsg SendMsg
#define recv Recv
#define recvfrom RecvFrom
#define recvmsg RecvMsg
#define shutdown Shutdown
#define setsockopt SetSockOpt
#define getsockopt GetSockOpt
#define getsockname GetSockName
#define getpeername GetPeerName
#define inet_addr Inet_Addr
#define inet_network Inet_Network
#define gethostbyname GetHostByName
#define gethostbyaddr GetHostByAddr
#define getnetbyname GetNetByName
#define getnetbyaddr GetNetByAddr
#define getservbyname GetServByName
#define getservbyport GetServByPort
#define getprotobyname GetProtoByName
#define getprotobynumber GetProtoByNumber
#define getdtablesize GetDTableSize
#define gethostname GetHostName
#define gethostid GetHostId
#define vsyslog SyslogA
#define syslog Syslog

#endif /* !AMITCP_BIGNAMES_H */

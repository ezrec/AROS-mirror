/***************************************************************************
                          net.h  -  description
                             -------------------
    begin                : Sat Mar 9 2002
    copyright            : (C) 2001 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __NET_H
#define __NET_H

#ifdef NETWORK_ENABLED

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <signal.h>

/* nettalk stats */
//#define NET_INFO
/* display messages sent and received */
//#define NET_DEBUG
/* display raw direct talk on the stream */
//#define NET_RAW_DEBUG

/*
====================================================================
Initiate/finalize networking (handle internal data)
====================================================================
*/
void net_init();
void net_finalize();

/*
====================================================================
CONNECTION
====================================================================
*/

/*
====================================================================
Socket struct.
  socket:    socket handle
  is_server  true if socket is run as server
====================================================================
*/
enum { 
    NET_IP_SIZE = 64,
    NET_MSG_DATA_LIMIT = 1023,
    NET_RING_SIZE = NET_MSG_DATA_LIMIT + 1 /* type byte */
};
typedef struct {
    int socket;
    int is_server;
    int lost; /* becomes true if connection was lost */
    char remote_ip[NET_IP_SIZE]; /* ddd.ddd.ddd.ddd */
    /* ring buffer */
    char ring[NET_RING_SIZE]; /* used to buffer next messages (at least
                                 a single maximum size message) */
    int  ring_read_pos;
    int  ring_write_pos; /* write/read data to this position */
    int  ring_left; /* number of bytes that are waiting unhandled at
                       position 'ring_read_pos' */
    /* parser data for ring buffer */
    int write_id; /* if not -1 currently written message type */
    int write_dpos; /* msg_defs[write_id].data[write_dpos] is the 
                       datatype that must be written next */
    int read_id; /* dito */
    int read_dpos; /* msg_defs[read_id].data[read_dpos] is the datatype
                      that is currently read in */
    int read_dleft; /* bytes left for the current datatype to complete */
    int read_raw_length; /* true if current datatype is length of a raw section */
} Net_Socket;

/*
====================================================================
Create a local server socket at the given port. If 'block' is true 
the server will wait when accepting a connection.
====================================================================
*/
Net_Socket *net_open( int port, int block );
/*
====================================================================
Accept a connection and return the server's endpoint. If the server
is non-blocking this will wait until there is a connection 
requested by client.
====================================================================
*/
Net_Socket *net_accept( Net_Socket *server );
/*
====================================================================
Connect to a remote IP and return the client endpoint.
====================================================================
*/
Net_Socket *net_connect( char *host, int port );
/*
====================================================================
Close a socket and free it's memory.
====================================================================
*/
void net_close( Net_Socket *socket );

/*
====================================================================
MESSAGES
====================================================================
*/

/*
====================================================================
Message struct.
The first byte of 'data' contains the type so we can send the
whole message as one chunk to prevent latency. When the message
is read its 'data' array does not contain the type byte but to
keep compatibility it is added after completing the message.
(thus there is no change to the outside)
====================================================================
*/
typedef struct {
    int  type;
    char data[NET_MSG_DATA_LIMIT];
    int  length; 
    int  unpack_pos;  /* used to unpack data from 'data' */
    int  dpos; /* current datatype (checked with msg_defs) that
                  is either packed or unpacked */
} Net_Msg;

/*
====================================================================
Define the additional data bundle of a message. If this
function is not called, 'data' is NULL or empty the message
does not have any additional data.
Data is a string that may contain any of the following characters.
  b:    integer (1 byte)
  s:    integer (2 byte)
  i:    integer (4 byte)
  f:    float
  d:    double
  z:    zero-terminated string
  r:    raw data (<=64KB)
====================================================================
*/
int net_set_msg( int msg, char *data );

/*
====================================================================
Add type as first data element and set length to 1.
====================================================================
*/
void net_init_msg( Net_Msg *msg, int msg_type );
/*
====================================================================
Add data to the message as defined in net_set_msg() (this is 
checked). All functions return True on success or False if there
is no space left or if you wanted to add a wrong datatype.
====================================================================
*/
int net_pack_int8(  Net_Msg *msg, int i );
int net_pack_int16( Net_Msg *msg, int i );
int net_pack_int32( Net_Msg *msg, int i );
int net_pack_float( Net_Msg *msg, float f );
int net_pack_double( Net_Msg *msg, double d );
int net_pack_string( Net_Msg *msg, char *str );
int net_pack_printf( Net_Msg *msg, int limit, char *format, ... );
int net_pack_raw( Net_Msg *msg, void *data, int length );
/*
====================================================================
Build a complete message (but do not send it). The correctness
of passed data types cannot be checked due to C's limitation. It 
can only be assumed so be careful.
====================================================================
*/
int net_build_msg( Net_Msg *msg, int type, ... );
/*
====================================================================
Send a message and return True on success.
====================================================================
*/
int net_write_msg( Net_Socket *socket, Net_Msg *msg );
/*
====================================================================
Send an empty message. (does not contain additional data)
====================================================================
*/
int net_write_empty_msg( Net_Socket *socket, int type );

/*
====================================================================
Read a message and return True if one has been completely 
received. The message data is stored in 'msg' and unpacked with the
unpack-functions.
If 'timeout' is 0 net_read() checks for incoming data and returns
immediately else it will wait until a message was completely read.
Then 'timeout' is the number of milliseconds net_read() will
wait between incoming data returning False if timedout. This
means 'timeout' is the limit for the delay between incoming data
and not for the whole transmission!
====================================================================
*/
int net_read_msg( Net_Socket *socket, Net_Msg *msg, int timeout );

/*
====================================================================
Unpack message data. net_unpack_str() and net_unpack_raw()
do not return False when the data was truncated due to the limit.
In net_unpack_string() 'limit' includes the zero-termination.
====================================================================
*/
int net_unpack_int8( Net_Msg *msg, int *i );
int net_unpack_int16( Net_Msg *msg, int *i );
int net_unpack_int32( Net_Msg *msg, int *i );
int net_unpack_float( Net_Msg *msg, float *f );
int net_unpack_double( Net_Msg *msg, double *d );
int net_unpack_string( Net_Msg *msg, char *str, int limit );
int net_unpack_raw( Net_Msg *msg, void *ptr, int *length, int limit );

/*
====================================================================
Get/clear TCP error.
====================================================================
*/
char *net_get_error();
void net_clear_error();

#else

enum { 
    NET_IP_SIZE = 64,
    NET_MSG_DATA_LIMIT = 1023,
    NET_RING_SIZE = NET_MSG_DATA_LIMIT + 1 /* type byte */
};
typedef struct {
    int socket;
    int is_server;
    int lost; /* becomes true if connection was lost */
    char remote_ip[NET_IP_SIZE]; /* ddd.ddd.ddd.ddd */
    /* ring buffer */
    char ring[NET_RING_SIZE]; /* used to buffer next messages (at least
                                 a single maximum size message) */
    int  ring_read_pos;
    int  ring_write_pos; /* write/read data to this position */
    int  ring_left; /* number of bytes that are waiting unhandled at
                       position 'ring_read_pos' */
    /* parser data for ring buffer */
    int write_id; /* if not -1 currently written message type */
    int write_dpos; /* msg_defs[write_id].data[write_dpos] is the 
                       datatype that must be written next */
    int read_id; /* dito */
    int read_dpos; /* msg_defs[read_id].data[read_dpos] is the datatype
                      that is currently read in */
    int read_dleft; /* bytes left for the current datatype to complete */
    int read_raw_length; /* true if current datatype is length of a raw section */
} Net_Socket;

typedef struct {
    int  type;
    char data[NET_MSG_DATA_LIMIT];
    int  length;
    int  unpack_pos;  /* used to unpack data from 'data' */
    int  unpack_dpos; /* current datatype (checked with msg_defs)*/
} Net_Msg;

#endif

#endif

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

#ifdef NETWORK_ENABLED

#include <SDL_types.h>
 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdarg.h>
#include <time.h>
#include "tools.h"
#include "net.h" 

/*
====================================================================
Externals
====================================================================
*/
extern int errno;

/*
====================================================================
Locals
====================================================================
*/
static char read_buffer[NET_RING_SIZE]; /* used to read polled input
before putting it to a socket's 'ring' */
enum { MSG_TYPE_LIMIT = 256 }; /* for now we do only use one byte for the type */
static Net_Msg msg_defs[MSG_TYPE_LIMIT]; /* definitions set by net_set_msg() */
static Net_Msg cur_msg; /* currently read in message */
static char net_error[256];
static char *datatype_names[] = {
    "unknown",
    "int8",
    "int16",
    "int32",
    "float",
    "double",
    "string",
    "raw data"
};

#ifdef NET_DEBUG
char *msg_names[] = {
    "none", "error", "server info",
    "identify", "login okay", "chat",
    "update", "add user", "remove user", "add users",
    "add game", "remove game", "update game", "add games",
    "add channel", "remove channel", "add_channels", "challenge",
    "accept challenge", "reject challenge", "busy",
    "cancel challenge", "game exited", "disconnect",
    "ip address", "leveldata", "ready",
    "pause", "unpause", "benchmark", 
    "dm host data", "dm client data", "mp host data",
    "mp client data", "round over", "winner", "switch channel",
    "set channel", "init transfer", "cancel transfer",
    "accept set", "reject set", "older set", "whisper", 
    "open transfer", "close transfer", "transfer data",
    "game stats"
};
#endif

/*
====================================================================
Create an empty socket.
====================================================================
*/
static Net_Socket *socket_create_empty()
{
    Net_Socket *sock = calloc( 1, sizeof( Net_Socket ) );
    if ( sock == 0 )
        fprintf( stderr, "Out of memory.\n" );
    else {
        sock->socket = -1;
        sock->write_id = sock->read_id = -1;
    }
    return sock;
}

/*
====================================================================
Send raw binary data to this socket's endpoint and return true if
all bytes were transferred.
====================================================================
*/
static int net_write( Net_Socket *sock, void *data, int length )
{
    int sent, left = length;
    if ( sock->lost ) 
        return 0;
    errno = 0;
    if ( length == 0 ) return 1;
    while ( left > 0 ) {
        if ( ( sent = send( sock->socket, data, left, 0 ) ) == -1 )
            break;
        left -= sent;
        data += sent;
    } 
#ifdef NET_RAW_DEBUG
    printf( "      writing %i bytes to %s\n", length - left, sock->remote_ip );
#endif
    if ( errno == EPIPE )  {
        sock->lost = 1;
        strcpy( net_error, "Connection lost" );
    }
    else
        if ( left == length )
            sprintf( net_error, "Package lost (%i bytes)", length );
    return (left == 0);
}

/*
====================================================================
Read the next byte from the ring buffer.
====================================================================
*/
static int net_read_next_byte( Net_Socket *sock, char *byte )
{
    if ( sock->ring_left == 0 )
        return 0;
    *byte = sock->ring[sock->ring_read_pos++];
    if ( sock->ring_read_pos == NET_RING_SIZE )
        sock->ring_read_pos = 0;
    sock->ring_left--;
    return 1;
}

/*
====================================================================
(un)pack an integer consuming a specific number of bytes.
====================================================================
*/
static int net_pack_int( Net_Msg *msg, int i, int bytes )
{
    if ( msg->length + bytes > NET_MSG_DATA_LIMIT ) {
        sprintf( net_error, "pack_int%i failed: package full", bytes << 3 );
        return 0;
    }
    memcpy( msg->data + msg->length, &i, bytes );
    msg->length += bytes;
    return 1;
}
static int net_unpack_int( Net_Msg *msg, int *i, int bytes )
{
    if ( msg->unpack_pos > msg->length - bytes ) {
        sprintf( net_error, "unpack_int%i failed: stored data incomplete", bytes << 3 );
        return 0;
    }
    memcpy( i, msg->data + msg->unpack_pos, bytes );
    msg->unpack_pos += bytes;
    return 1;
}

/*
====================================================================
Query the ip of a socket by parsing the sockaddr struct.
====================================================================
*/
static int net_query_ip( struct sockaddr *addr, char *str, int limit )
{
    char *ptr;
    char aux[NET_IP_SIZE];
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;
//    struct sockaddr_un *sun;
    strcpy_lt( str, "undefined", limit );
    switch ( addr->sa_family ) {
        case AF_INET:
            sin = (struct sockaddr_in*)addr;
            if ( !inet_ntop( AF_INET, &sin->sin_addr, str, limit ) ) {
                sprintf( net_error, "net_query_ip: inet_ntop failed: %s\n", strerror( errno ) );
                return 0;
            }
#ifdef NET_INFO
            printf( "  AF_INET: %s\n", str );
#endif
            return 1;
        case AF_INET6:
            sin6 = (struct sockaddr_in6*)addr;
            if ( !inet_ntop( AF_INET6, &sin6->sin6_addr, aux, limit ) ) {
                sprintf( net_error, "net_query_ip: inet_ntop failed: %s\n", strerror( errno ) );
                return 0;
            }
            /* AF_INET6 features a ::ffff: at the beginning we'll have to cut this off */
            if ( aux[0] == ':' && aux[1] == ':' )
                if ( ( ptr = strchr( aux + 2, ':' ) ) )
                    strcpy_lt( str, ptr + 1, limit );
#ifdef NET_INFO
            printf( "  AF_INET6: %s\n", str );
#endif
            return 1;
/*        case AF_UNIX:
            sun = (struct sockaddr_un *)addr;
            if( sun->sun_path[0] == 0 ) {
                strcpy_lt( str, "unknown", limit );
                sprintf( net_error, "net_query_ip: unknown unix path\n" );
                return 0;
            }   
            else
                strcpy_lt( str, sun->sun_path, limit );
#ifdef NET_INFO
            printf( "  AF_UNIX: %s\n", str );
#endif
            return 1;*/
        default:
#ifdef NET_INFO
            printf( "  UNKNOWN\n" );
#endif
            strcpy_lt( str, "unknown", limit );
            return 0;
    }
}

/*
====================================================================
Query the datatype name belonging to the character.
====================================================================
*/
static char* net_query_datatype( char d )
{
    switch ( d ) {
        case 'b': return datatype_names[1];
        case 's': return datatype_names[2];
        case 'i': return datatype_names[3];
        case 'f': return datatype_names[4];
        case 'd': return datatype_names[5];
        case 'z': return datatype_names[6];
        case 'r': return datatype_names[7];
        default: return datatype_names[0];
    }
}

/*
====================================================================
Query the datatype length belonging to the character.
====================================================================
*/
static int net_query_dlength( char d )
{
    switch ( d ) {
        case 'b': return sizeof( Uint8 );
        case 's': return sizeof( Uint16 );
        case 'i': return sizeof( Uint32 );
        case 'f': return sizeof( float );
        case 'd': return sizeof( double );
        case 'z': return -1; /* terminated by null */
        case 'r': return -2; /* length + raw */
        default: return 0;
    }
}

/*
====================================================================
Publics
====================================================================
*/

/*
====================================================================
Initiate/finalize networking (handle internal data)
====================================================================
*/
void net_init()
{
    int i;
    signal( SIGPIPE, SIG_IGN );
    memset( &cur_msg, 0, sizeof( Net_Msg ) );
    memset( &msg_defs, 0, sizeof( msg_defs ) );
    for ( i = 0; i < MSG_TYPE_LIMIT; i++ )
        msg_defs[i].type = i;
    net_error[0] = 0;
}
void net_finalize()
{
}

/*
====================================================================
CONNECTION
====================================================================
*/

/*
====================================================================
Create a local server socket at the given port. If 'block' is true 
the server will wait when accepting a connection.
====================================================================
*/
Net_Socket *net_open( int port, int block )
{
    int yes = 1;
    char service[24];
    struct addrinfo hints, *info, *anchor;
    Net_Socket *server = socket_create_empty();
    if ( server == 0 ) return 0;
    /* get list of possible address infos */
    sprintf( service, "%d", port );
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ( getaddrinfo( 0, service, &hints, &info ) != 0 ) {
        sprintf( net_error, "Couldn't get address info list" );
        goto failure;
    }
    /* get socket, bind, listen */
    anchor = info;
    while ( info ) {
        if ( ( server->socket = socket( info->ai_family, info->ai_socktype, info->ai_protocol ) ) == -1 ) {
            /* try next address info */
            info = info->ai_next;
            continue;
        }
		setsockopt( server->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) );
        if ( bind( server->socket, info->ai_addr, info->ai_addrlen ) == -1 || listen( server->socket, 40 ) == -1 ) {
            close( server->socket ); server->socket = -1;
            info = info->ai_next;
            continue;
        }
        break;
    }
    freeaddrinfo( anchor );
    if ( server->socket == -1 ) {
        sprintf( net_error, "Couldn't open local server socket at port %d", port );
        goto failure;
    }
    /* non-blocking? */
    if ( !block )
        fcntl( server->socket, F_SETFL, O_NONBLOCK );
    /* return server */
    server->is_server = 1;
    return server;
failure:
    net_close( server );
    return 0;
}
/*
====================================================================
Accept a connection and return the server's endpoint. If the server
is non-blocking this will wait until there is a connection 
requested by client.
====================================================================
*/
Net_Socket *net_accept( Net_Socket *server )
{
    int okay = 1;
    struct sockaddr_in addr;
    struct sockaddr *name;
    int length = 128;
    Net_Socket *endpoint = socket_create_empty();
    if ( endpoint == 0 ) return 0;
    if ( !server->is_server ) {
        sprintf( net_error, "Only server sockets may accept connections" );
        goto failure;
    }
    /* accept */
    length = sizeof( addr );
    if ( ( endpoint->socket = accept( server->socket, (struct sockaddr*)&addr, &length ) ) == -1 )
        goto failure;
    /* get ips as string */
    if ( ( name = calloc( length, 1 ) ) ) {
        if ( getpeername( endpoint->socket, name, &length ) != -1 ) {
            if ( !net_query_ip( name, endpoint->remote_ip, NET_IP_SIZE ) )
                goto failure;
        }
        else {
            sprintf( net_error, "Couldn't get peer name\n" );
            goto failure;
        }
        free( name );
    }
    /* return server's endpoint to client */
    fcntl( endpoint->socket, F_SETFL, O_NONBLOCK );
    setsockopt( endpoint->socket, IPPROTO_TCP, TCP_NODELAY, &okay, sizeof( int ) );
    return endpoint;
failure:
    net_close( endpoint );
    return 0;
}
/*
====================================================================
Connect to a remote IP and return the client endpoint.
====================================================================
*/
Net_Socket *net_connect( char *host, int port )
{
    int okay = 1;
    char service[24];
    struct addrinfo hints, *info, *anchor;
    Net_Socket *client = socket_create_empty();
    if ( client == 0 ) return 0;
    /* get list of possible address infos */
    sprintf( service, "%d", port );
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ( getaddrinfo( host, service, &hints, &info ) != 0 ) {
        sprintf( net_error, "Couldn't get address info list" );
        goto failure;
    }
    /* get socket and connect */
    anchor = info;
    while ( info ) {
        if ( ( client->socket = socket( info->ai_family, info->ai_socktype, info->ai_protocol ) ) == -1 ) {
            /* try next address info */
            info = info->ai_next;
            continue;
        }
        if ( connect( client->socket, info->ai_addr, info->ai_addrlen ) == -1 ) {
            close( client->socket ); client->socket = -1;
            info = info->ai_next;
            continue;
        }
        break;
    }
    freeaddrinfo( anchor );
    if ( client->socket == -1 ) {
        sprintf( net_error, "Couldn't connect to %s at port %i", host, port );
        goto failure;
    }
    /* return client's endpoint to server */
    fcntl( client->socket, F_SETFL, O_NONBLOCK );
    setsockopt( client->socket, IPPROTO_TCP, TCP_NODELAY, &okay, sizeof( int ) );
    strcpy_lt( client->remote_ip, host, 15 );
    return client;
failure:
    net_close( client );
    return 0;
}
/*
====================================================================
Close a socket and free it's memory.
====================================================================
*/
void net_close( Net_Socket *sock )
{
    if ( sock ) {
        if ( sock->socket != -1 )
            close( sock->socket );
        free( sock );
    }
}

/*
====================================================================
MESSAGES
====================================================================
*/

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
  s:    null-terminated string
  r:    raw data (2 byte + data)
====================================================================
*/
int net_set_msg( int msg, char *data )
{
    int i, j;
    if ( msg >= MSG_TYPE_LIMIT ) return 0;
    if ( data && data[0] != 0 ) {
        for ( i = 0, j = 0; i < strlen( data ); i++ ) {
            if ( net_query_dlength( data[i] ) != 0 )
                msg_defs[msg].data[j++] = data[i];
            if ( j == NET_MSG_DATA_LIMIT )
                break;
        }
        msg_defs[msg].length = j;
    }
    else
        msg_defs[msg].length = 0;
    return 1;
}

/*
====================================================================
Add type as first data element and set length to 1.
====================================================================
*/
void net_init_msg( Net_Msg *msg, int msg_type )
{
    msg->type = msg_type;
    msg->data[0] = (unsigned char)msg_type;
    msg->length = 1;
    msg->dpos = 0;
}
/*
====================================================================
Add data to the message as defined in net_set_msg() (this is 
checked). All functions return True on success or False if there
is no space left or if you wanted to add a wrong datatype.
====================================================================
*/
int net_pack_int8(  Net_Msg *msg, int i )
{
#ifdef NET_DEBUG    
    printf( "    PACK INT8 '%i' to '%s'\n", i, msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_int8 failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'b' ) {
        sprintf( net_error, "pack_int8 failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    if ( !net_pack_int( msg, i, 1 ) )
        return 0;
    msg->dpos++;
    return 1;
}    
int net_pack_int16( Net_Msg *msg, int i )
{
#ifdef NET_DEBUG    
    printf( "    PACK INT16 '%i' to '%s'\n", i, msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_int16 failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 's' ) {
        sprintf( net_error, "pack_int16 failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    i = (int)htons( (Uint16)i );
    if ( !net_pack_int( msg, i, 2 ) )
        return 0;
    msg->dpos++;
    return 1;
}
int net_pack_int32( Net_Msg *msg, int i )
{
#ifdef NET_DEBUG    
    printf( "    PACK INT32 '%i' to '%s'\n", i, msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_int32 failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'i' ) {
        sprintf( net_error, "pack_int32 failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    i = (int)htonl( (Uint32)i );
    if ( !net_pack_int( msg, i, 4 ) )
        return 0;
    msg->dpos++;
    return 1;
}
int net_pack_float( Net_Msg *msg, float f )
{
#ifdef NET_DEBUG    
    printf( "    PACK FLOAT '%4.2f' to '%s'\n", f, msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_float failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'f' ) {
        sprintf( net_error, "pack_float failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    if ( msg->length + sizeof( float ) > NET_MSG_DATA_LIMIT ) {
        sprintf( net_error, "pack_float failed: package full" );
        return 0;
    }
    memcpy( msg->data + msg->length, &f, sizeof( float ) );
    msg->length += sizeof( float );
    msg->dpos++;
    return 1;
}
int net_pack_double( Net_Msg *msg, double d )
{
#ifdef NET_DEBUG    
    printf( "    PACK DOUBLE '%4.2f' to '%s'\n", d, msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_double failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'd' ) {
        sprintf( net_error, "pack_double failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    if ( msg->length + sizeof( double ) > NET_MSG_DATA_LIMIT ) {
        sprintf( net_error, "pack_double failed: package full" );
        return 0;
    }
    memcpy( msg->data + msg->length, &d, sizeof( double ) );
    msg->length += sizeof( double );
    msg->dpos++;
    return 1;
}
int net_pack_string( Net_Msg *msg, char *str )
{
#ifdef NET_DEBUG    
    printf( "    PACK STRING '%s' to '%s'\n", str, msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_string failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'z' ) {
        sprintf( net_error, "pack_string failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    if ( msg->length + strlen( str ) + 1 > NET_MSG_DATA_LIMIT ) {
        sprintf( net_error, "pack_string failed: package full" );
        return 0;
    }
    memcpy( msg->data + msg->length, str, strlen( str ) + 1 );
    msg->length += strlen( str ) + 1;
    msg->dpos++;
    return 1;
}
int net_pack_printf( Net_Msg *msg, int limit, char *format, ... )
{
    va_list args;
    char data[limit + 1];
    memset( data, 0, limit + 1 );
    va_start( args, format );
    vsnprintf( data, limit, format, args );
    va_end( args );
    return net_pack_string( msg, data );
}
int net_pack_raw( Net_Msg *msg, void *data, int _length )
{
    Uint16 length = (Uint16)_length;
#ifdef NET_DEBUG    
    printf( "    PACK RAW to '%s'\n", msg_names[msg->type] );
#endif
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "pack_raw failed: message is complete" );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'r' ) {
        sprintf( net_error, "pack_raw failed: %s expected by %i",
                 net_query_datatype( msg_defs[msg->type].data[msg->dpos] ),
                 msg->type );
        return 0;
    }
    if ( msg->length + length + 2 > NET_MSG_DATA_LIMIT ) {
        sprintf( net_error, "pack_raw failed: package full" );
        return 0;
    }
    memcpy( msg->data + msg->length, &length, 2 );
    memcpy( msg->data + msg->length + 2, data, length );
    msg->length += length + 2;
    msg->dpos++;
    return 1;
}
/*
====================================================================
Build a complete message (but do not send it). The correctness
of passed data types cannot be checked due to C's limitation. It 
can only be assumed correct so be careful.
====================================================================
*/
int net_build_msg( Net_Msg *msg, int type, ... )
{
    int length;
    char *s;
    int i;
    double d;
    float f;
    int pos;
    va_list args;
    va_start( args, type );
    net_init_msg( msg, type );
    for ( pos = 0; pos < msg_defs[type].length; pos++ )
        switch ( msg_defs[type].data[pos] ) {
            case 'b':
                i = va_arg( args, int );
                if ( !net_pack_int8( msg, i ) )
                    return 0;
                break;
            case 's':
                i = va_arg( args, int );
                if ( !net_pack_int16( msg, i ) )
                    return 0;
                break;
            case 'i':
                i = va_arg( args, int );
                if ( !net_pack_int32( msg, i ) )
                    return 0;
                break;
            case 'f':
                f = (float)va_arg( args, double );
                if ( !net_pack_float( msg, f ) )
                    return 0;
                break;
            case 'd':
                d = va_arg( args, double );
                if ( !net_pack_double( msg, d ) )
                    return 0;
                break;
            case 'z':
                s = va_arg( args, char * );
                if ( !net_pack_string( msg, s ) ) 
                    return 0;
                break;
            case 'r':
                s = va_arg( args, char * );
                /* get length */
                length = 0; memcpy( &length, s, sizeof( Uint16 ) );
                /* pack raw data */
                if ( !net_pack_raw( msg, s + 2, length ) )
                    return 0;
                break;
        }
    return 1;
}
/*
====================================================================
Send a message and return True on success.
====================================================================
*/
int net_write_msg( Net_Socket *socket, Net_Msg *msg )
{
#ifdef NET_DEBUG
    printf( "    SEND MSG '%s' to %s\n", msg_names[msg->type], socket->remote_ip );
#endif
    if ( !net_write( socket, msg->data, msg->length ) )
        return 0;
    return 1;
}
/*
====================================================================
Send an empty message. (does not contain additional data)
====================================================================
*/
int net_write_empty_msg( Net_Socket *socket, int type )
{
    Net_Msg msg;
    if ( msg_defs[type].length > 0 ) {
        sprintf( net_error, 
                 "net_write_empty_msg failed: message %i has additional data",
                 type );
        return 0;
    }
    net_init_msg( &msg, type );
    return net_write_msg( socket, &msg );
}

/*
====================================================================
Read a message and return True if one has been completely 
received. The message data is stored in 'msg' and unpacked with the
unpack-functions.
====================================================================
*/
int net_read_msg( Net_Socket *sock, Net_Msg *msg, int timeout )
{
    int length, i;
    struct pollfd pfd = { sock->socket, POLLIN, 0 };
    unsigned char byte;
    while ( 1 ) {
        /* check if there is data waiting (do only poll data if there is no more
           data to handle in the ring buffer */
        if ( sock->ring_left == 0 ) { 
            if ( poll( &pfd, 1, timeout ) == 1 ) {
                /* buffer at least next message ( */
                length = read( sock->socket, read_buffer, NET_RING_SIZE );
#ifdef NET_RAW_DEBUG
                printf( "      reading %i bytes from %s\n", length, sock->remote_ip );
#endif
                if ( length == -1 ) {
                    /* basically this is impossible as we used poll() */
                    sprintf( net_error, "read() failed though poll() checked True???" );
                    return 0;
                }
                if ( length == 0 ) {
                    /* poll told us there is data waiting but nothing was read
                       and no error occured --> connection is closed */
                    sock->lost = 1;
                    sprintf( net_error, "Connection lost" );
                    return 0;
                }
                sock->ring_left += length;
            }
            else {
                /* if no data is available stop reading */
                if ( timeout )
                    sprintf( net_error, "No data within %i ms: timeout", timeout );
                return 0;
            }
            /* transfer data to ring buffer */
            i = 0;
            while ( i < length ) {
                sock->ring[sock->ring_write_pos++] = read_buffer[i++];
                if ( sock->ring_write_pos == NET_RING_SIZE )
                    sock->ring_write_pos = 0;
            }
        }
        /* okay we have data so read in */
        if ( sock->read_id == -1 ) {
            /* prepare a new message */
            net_read_next_byte( sock, &byte );
            cur_msg.type = byte;
            cur_msg.length = 0;
            cur_msg.unpack_pos = 0;
            cur_msg.dpos = 0;
            /* if we have no additional data we're finished */
            if ( msg_defs[cur_msg.type].length == 0 )
                goto finalize;
            sock->read_id = byte;
            sock->read_dpos = 0;
            if ( msg_defs[sock->read_id].data[0] == 'r' ) {
                sock->read_dleft = sizeof( Uint16 ); /* length of raw section */
                sock->read_raw_length = 1;
            }
            else
                sock->read_dleft = net_query_dlength( msg_defs[sock->read_id].data[0] );
        }
        /* continue reading of a package */
        while ( net_read_next_byte( sock, &byte ) ) {
            if ( sock->read_dleft == -1 ) { 
                /* null-terminated string */
                cur_msg.data[cur_msg.length++] = byte;
                if ( byte == 0 ) {
                    /* next datatype */
                    sock->read_dpos++;
                    if ( sock->read_dpos == msg_defs[sock->read_id].length ) {
                        /* done! */
                        goto finalize;
                    }
                    if ( msg_defs[sock->read_id].data[sock->read_dpos] == 'r' ) {
                        sock->read_dleft = sizeof( Uint16 ); /* length of raw section */
                        sock->read_raw_length = 1;
                    }
                    else
                        sock->read_dleft = net_query_dlength( msg_defs[sock->read_id].data[sock->read_dpos] );
                }
            } else
            if ( sock->read_dleft > 0 ) {
                /* fixed size datatype */
                cur_msg.data[cur_msg.length++] = byte;
                if ( --sock->read_dleft == 0 ) {
                    /* check if this was just length of raw section */
                    if ( msg_defs[sock->read_id].data[sock->read_dpos] == 'r' && sock->read_raw_length )  {
                        memcpy( &sock->read_dleft, 
                                cur_msg.data + cur_msg.length - sizeof( Uint16 ), 
                                sizeof( Uint16  ) );
                        sock->read_raw_length = 0;
                        /* if this was an empty raw section go on */
                        if ( sock->read_dleft == 0 ) {
                            /* next datatype */
                            sock->read_dpos++;
                            if ( sock->read_dpos == msg_defs[sock->read_id].length ) {
                                /* done! */
                                goto finalize;
                            }
                            if ( msg_defs[sock->read_id].data[sock->read_dpos] == 'r' ) {
                                sock->read_dleft = sizeof( Uint16 ); /* length of raw section */
                                sock->read_raw_length = 1;
                            }
                            else
                                sock->read_dleft = net_query_dlength( msg_defs[sock->read_id].data[sock->read_dpos] );
                        }
                    }
                    else {
                        /* next datatype */
                        sock->read_dpos++;
                        if ( sock->read_dpos == msg_defs[sock->read_id].length ) {
                            /* done! */
                            goto finalize;
                        }
                        if ( msg_defs[sock->read_id].data[sock->read_dpos] == 'r' ) {
                            sock->read_dleft = sizeof( Uint16 ); /* length of raw section */
                            sock->read_raw_length = 1;
                        }
                        else
                            sock->read_dleft = net_query_dlength( msg_defs[sock->read_id].data[sock->read_dpos] );
                    }
                }
            }
        }
        /* if no timeout is set we did only poll */
        if ( timeout == 0 )
            return 0;
    }
    return 0;
finalize:
    /* add the type byte to data */
    msg->type = cur_msg.type;
    msg->length = cur_msg.length + 1;
    msg->data[0] = msg->type;
    msg->unpack_pos = 1;
    msg->dpos = 0;
    memcpy( msg->data + 1, cur_msg.data, cur_msg.length );
    sock->read_id = -1;
#ifdef NET_DEBUG
    printf( "    RECV MSG '%s' (%i bytes) from %s\n", msg_names[msg->type], msg->length, sock->remote_ip );
#endif
    return 1;
}
/*
====================================================================
Unpack message data. net_unpack_str() and net_unpack_raw()
do not return False when the data was truncated due to the limit.
====================================================================
*/
int net_unpack_int8( Net_Msg *msg, int *i )
{
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_int8 failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'b' ) {
        sprintf( net_error, "unpack_int8 failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    *i = 0;
    if ( !net_unpack_int( msg, i, 1 ) )
        return 0;
    msg->dpos++;
    return 1;
}
int net_unpack_int16( Net_Msg *msg, int *i )
{
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_int16 failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 's' ) {
        sprintf( net_error, "unpack_int16 failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    *i = 0;
    if ( !net_unpack_int( msg, i, 2 ) )
        return 0;
    *i = (Sint16)ntohs( (Uint16)*i );
    msg->dpos++;
    return 1;
}
int net_unpack_int32( Net_Msg *msg, int *i )
{
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_int32 failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'i' ) {
        sprintf( net_error, "unpack_int32 failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    *i = 0;
    if ( !net_unpack_int( msg, i, 4 ) )
        return 0;
    *i = (Sint32)ntohl( (Uint32)*i );
    msg->dpos++;
    return 1;
}
int net_unpack_float( Net_Msg *msg, float *f )
{
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_float failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'f' ) {
        sprintf( net_error, "unpack_float failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    if ( msg->unpack_pos > msg->length - sizeof( float ) ) {
        sprintf( net_error, "unpack_float failed: stored data incomplete" );
        return 0;
    }
    memcpy( f, msg->data + msg->unpack_pos, sizeof( float ) );
    msg->unpack_pos += sizeof( float );
    msg->dpos++;
    return 1;
}
int net_unpack_double( Net_Msg *msg, double *d )
{
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_double failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'd' ) {
        sprintf( net_error, "unpack_double failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    if ( msg->unpack_pos > msg->length - sizeof( double ) ) {
        sprintf( net_error, "unpack_double failed: stored data incomplete" );
        return 0;
    }
    memcpy( d, msg->data + msg->unpack_pos, sizeof( double ) );
    msg->unpack_pos += sizeof( double );
    msg->dpos++;
    return 1;
}
int net_unpack_string( Net_Msg *msg, char *str, int limit )
{
    int i = 0;
    char byte = 0;
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_string failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'z' ) {
        sprintf( net_error, "unpack_string failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    while ( msg->unpack_pos < msg->length ) {
        byte = msg->data[msg->unpack_pos++];
        if ( i < limit )
            str[i++] = byte;
        if ( byte == 0 )
            break;
    }
    str[limit-1] = 0; /* null-terminate for safety */
    msg->dpos++;
    return (byte == 0);
}
int net_unpack_raw( Net_Msg *msg, void *_ptr, int *length, int limit )
{
    int i = 0;
    char *ptr = _ptr;
    char byte = 0;
    if ( msg->dpos == msg_defs[msg->type].length ) {
        sprintf( net_error, "unpack_raw failed: message %i is completely unpacked", msg->type );
        return 0;
    }
    if ( msg_defs[msg->type].data[msg->dpos] != 'r' ) {
        sprintf( net_error, "unpack_raw failed: next datatype in message %i is '%s'", 
                 msg->type, net_query_datatype( msg_defs[msg->type].data[msg->dpos] ) );
        return 0;
    }
    *length = 0;
    if ( !net_unpack_int( msg, length, 2 ) )
        return 0;
    msg->dpos++;
    if ( *length <= 0 )
        return 1;
    while ( msg->unpack_pos < msg->length ) {
        byte = msg->data[msg->unpack_pos++];
        if ( i < limit )
            ptr[i++] = byte;
        if ( i == *length )
            break;
    }
    return (i == *length);
}

/*
====================================================================
Get/clear TCP error.
====================================================================
*/
char *net_get_error()
{
    if ( net_error[0] != 0 )
        return net_error;
    return 0;
}
void net_clear_error()
{
    net_error[0] = 0;
}

#endif

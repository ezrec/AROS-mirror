#include <exec/types.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

struct hostent *gethostbyname(const char *name)
{
return NULL;
}

int gethostname(char *name, size_t len)
{
snprintf(name,len-1,"AROS");
return 0;
}

int  select(int  n,  fd_set  *readfds,  fd_set  *writefds,
       fd_set *exceptfds, struct timeval *timeout)
{
return -1;
}

u_long inet_addr(const char *cp)
{
return 0;
}

int HTFTPLoad ( )
{
return 0;
}

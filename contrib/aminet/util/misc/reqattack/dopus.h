#ifndef MYDOPUS_H
#define MYDOPUS_H

struct MyBuffer
{
	struct MyBuffer *nextbuffer;
	char				 *buffer;
	ULONG				 buflen;
};

#endif
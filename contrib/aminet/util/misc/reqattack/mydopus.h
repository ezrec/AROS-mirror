#ifndef MYDOPUS_H
#define MYDOPUS_H

#ifndef MYGADGET_H
#include MYGADGET_H
#endif

struct MyBuffer
{

	char				 *buffer;
	ULONG				 buflen;
	APTR				 *history;
	struct MyGadget *gadget;
};

#endif

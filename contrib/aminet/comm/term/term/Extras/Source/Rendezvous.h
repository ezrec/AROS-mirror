/*
**	Rendezvous.h - External program interface for Amiga
**	               telecommunications software
**
**	Written by Olaf Barthel, freely distributable.
*/

#ifndef _COMPILER_H
#define REG(x) register __ ## x
#define ASM __asm
#endif	/* _COMPILER_H */

	/* Minimum rendezvous data structure revision. */

#define RENDEZVOUS_VERSION	0

	/* Returned by rs_Login(). */

typedef struct RendezvousData
{
	LONG			 rd_Version;		/* Version of this data structure. */
	struct Screen		*rd_Screen;		/* Pointer to terminal screen. */
	struct IOExtSer		 rd_ReadRequest,	/* Serial read request. */
				 rd_WriteRequest;	/* Serial write request. */

	struct List		 rd_UploadList,		/* List of files to upload. */
				 rd_DownloadList,	/* List of files to receive. */

				 rd_SentList,		/* List of files sent (uploaded). */
				 rd_ReceivedList;	/* List of files received (downloaded). */

	STRPTR			 rd_SendPath,		/* Path to look into for files to send. */
				 rd_ReceivePath,	/* Path to place files into when receiving. */

				 rd_Options;		/* Protocol options, command line parameters, etc. */
} RendezvousData;

	/* Rendezvous interface data, this is what FindSemaphore()
	 * returns.
	 */

struct RendezvousSemaphore
{
	struct SignalSemaphore	rs_Semaphore;		/* Link access. */

	LONG			rs_Version;		/* Data structure version. */

		/* Callback routines follow below. */

	RendezvousData *	(* ASM rs_Login)(REG(a0) struct MsgPort *ReadPort,REG(a1) struct MsgPort *WritePort,REG(a2) struct TagItem *TagList);
	VOID			(* ASM rs_Logoff)(REG(a0) RendezvousData *Data);
	struct Node *		(* ASM rs_NewNode)(REG(a0) STRPTR Name);
};

/***********************************************************************

About the rendezvous interface
==============================
`term' 4.0 provides an interface for other client program to gain control
over the serial driver. In order to do so, the following needs to be done
(code fragment follows):

   struct RendezvousSemaphore *Semaphore;

   Forbid();

      // Find the access semaphore, note that "TERM" can be any
      // other interface name, provided it is unique.

   if(Semaphore = (struct RendezvousSemaphore *)FindSemaphore("TERM"))
   {
      ObtainSemaphore(Semaphore);

      Permit();
   }
   else
      Permit();

Once the client has acquired the RendezvousSemaphore it can access the
data attached to it. Please note that the contents of the
RendezvousSemaphore are read-only. The rs_Version entry indicates which
entries and routines are available in this data structure. Please note
that the data structure may grow in the future.

To link to to `term', call the rs_Login() routine, such as shown
below:

   struct MsgPort        *ReadPort,
                         *WritePort;
   RendezvousData *Data;

   if(ReadPort = CreateMsgPort())
   {
      if(WritePort = CreateMsgPort())
      {
          if(Data = (*Semaphore -> rs_Login)(ReadPort,WritePort,NULL))
          {
             // Work with it.

             :
             :
             :

Make sure that you get what you want, rs_Login() may return NULL in case
of failure.


The RendezvousData structure
============================
Let's have a look at the contents of the RendezvousData structure:

1. rd_Version [read-only]

Just like the RendezvousSemaphore it includes a version entry (rd_Version)
to help you to find out which entries are present and which are not
(remember that this data structure may grow in the future).

2. rd_Screen [read-only]

Here you can find a pointer to the screen `term' uses or a NULL in case
`term' could not return a proper address. You can use this screen to
open your windows on it.

3. rd_ReadRequest, rd_WriteRequest

These are ready-to-use serial I/O requests for your program. The
rs_Login() call will have placed your ReadPort and WritePort pointers
in these requests for you to use.

4. rd_UploadList

This is a standard List with plain Nodes in it. Each node contains the
name of a file to upload. You are requested to process this list if
possible. When you are finished uploading a file, Remove() the
corresponding Node from this list and AddTail() it to the rd_SentList.

5. rd_DownloadList

This is another standard Listh with plain Nodes in it. Each of these
nodes contains the name of a file to receive, or to request from the
remote. You are requested to process this list if possible. When you
are finished receiving a file whose name could be found in the list,
Remove() the corresponding Node from this list and AddTail() it to
the rd_ReceivedList.

6. rd_SentList

After having sent a file whose name could not be found on the
rd_UploadList, allocate a Node using the rs_NewNode() call with
the name of the file as the parameter and AddTail() it to this list.

7. rd_ReceivedList

After having received a file whose name could not be found on the
rd_DownloadList, allocate a Node using the rs_NewNode() call with
the name of the file as the parameter and AddTail() it to this list.

8. rd_SendPath [read-only]

This gives the name of the directory to look for files to send. Please
note that this pointer may be NULL, ignore it in this case.

9. rd_ReceivePath [read-only]

This gives the name of the directory to receive files into. Please
note that this pointer may be NULL, ignore it in this case.

10. rd_Options [read-only]

You can find a list of options or command line parameters here. `term'
may place special data for you here. Please note that this pointer
may be NULL, ignore it in this case.


How to disconnect
=================
When the client is finished with whatever service it could provide,
it must make sure that no read or write request is still pending.
If the client made any changes to the serial parameters they should be
restored to their original state. Finally, the client must call
rs_Logoff() and release the semaphore as illustrated below:

   (*Semaphore -> rs_Logoff)(Data);

   ReleaseSemaphore((struct SignalSemaphore *)Semaphore);

After rs_Logoff() is called no further references may be made to the
RendezvousData structure. You may still call the rs_Login()+rs_Logoff()
pair as many times you like until you let go of the RendezvousSemaphore.


Future extensions & author's address
====================================
The rendezvous interface may grow in the future. If you need to make
changes to the specifications or add extensions you should register
them with me. If you need more information on how to implement a
client or a host interface feel free to ask.

My postal address is:

   Olaf Barthel
   Brabeckstrasse 35
   D-30559 Hannover

   Federal Republic of Germany

eMail:

   olsen@sourcery.han.de

***********************************************************************/

/****************************************************************************
*									    *
* Clean.c							    970319  *
*									    *
* Ensure that everything is cleaned up on exit				    *
*									    *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include "MyTypes.h"
#include "Clean.h"

extern struct GfxBase * GfxBase;

CleanNode FirstNode = {0};
CleanNode LastNode  = {0};

void InitCleanUp(void)
{
    /* Initialise Node Links */

    FirstNode.Next=&LastNode;
    FirstNode.Prev=&FirstNode;
    FirstNode.Type=CleanUpNone;
    FirstNode.Address=0;
    FirstNode.Size=0;

    LastNode.Next=&LastNode;
    LastNode.Prev=&FirstNode;
    LastNode.Type=CleanUpNone;
    LastNode.Address=0;
    LastNode.Size=0;
}

pCleanNode EnterCleanNode(int32 Type, void * Address, uint32 Size)
{
    /* Enter Node at the Top. */

    pCleanNode New;

    New=(pCleanNode)malloc(sizeof(CleanNode));

    if (New == 0)
    {
	CleanExit("Could not allocate CleanUpNode\n");
    }
    New->Next=FirstNode.Next;
    New->Prev=&FirstNode;
    New->Type=Type;
    New->Address=Address;
    New->Size=Size;
    FirstNode.Next->Prev=New;
    FirstNode.Next=New;

    return New;
}

void RemoveCleanNode(pCleanNode Node)
{
    /* Remove Node from Chain and clean up */

    Node->Prev->Next=Node->Next;
    Node->Next->Prev=Node->Prev;

    CleanUpNode(Node);
}

void CleanUpNodes(void)
{
    /* Clean up all Nodes */

    pCleanNode Node,Node2;

    Node=FirstNode.Next;

    while(Node->Next != Node->Next->Next)
    {
	Node2=Node;
	Node=Node->Next;
	CleanUpNode(Node2);
    }
}

void CleanUpNode(pCleanNode Node)
{
    /* Clean Up Node */

    switch (Node->Type)
    {
	case CleanUpLib : /* Close Library */
	{
	    CloseLibrary(Node->Address);
	    break;
	}
	case CleanUpScreen : /* Close Screen */
	{
	    CloseScreen(Node->Address);
	    break;
	}
	case CleanUpWindow : /* Close Window */
	{
	    CloseWindow(Node->Address);
	    break;
	}
	case CleanUpMem : /* Free Mem */
	{
	    free(Node->Address);
	    break;
	}
	case CleanUpBitMap : /* Free BitMap */
	{
	    FreeBitMap(Node->Address);
	    break;
	}
    }

    /* Free Node as well */

    free(Node);
}

void * MemAlloc(int32 Size)
{
    pCleanNode * Mem=0;

    Mem=(pCleanNode *)malloc(Size+sizeof(pCleanNode));

    if (!Mem)
    {
	printf ("malloc(%ld) failed.\n", Size);
	exit (10);
    }

    *Mem=EnterCleanNode(CleanUpMem,Mem,Size+sizeof(pCleanNode));

    return (void *)(Mem++);
}

void MemFree(void * Address)
{
    RemoveCleanNode((pCleanNode)(*((pCleanNode *)Address)));
}

void CleanExit(char * ErrorStr)
{
    printf("\n%s\n",ErrorStr);

    CleanUpNodes();

    exit(5);
}

void CheckCleanUp(void)
{
    pCleanNode Node;

    if (FirstNode.Next != FirstNode.Next->Next)
    {
	printf("   There was some Garbage left!\n");
	printf("(Please don`t mail me about it!)\n\n");

	Node=FirstNode.Next;

	while (Node != Node->Next)
	{
	    printf("Type %ld Address %ld Size %ld\n",Node->Type,(long)Node->Address,Node->Size);
	    Node=Node->Next;
	}

	CleanUpNodes();
    }
}


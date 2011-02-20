/* 
 *  @(#) $Header$
 * 
 *  BGUI library
 *  stkext.c
 *  Generic stack extension routines.
 *
 *  Copyright © 2011, The AROS Development Team.
 *  (C) Janne Jalkanen 1998
 *  (C) Copyright 1998 Manuel Lemos.
 *  (C) Copyright 1996-1997 Ian J. Einman.
 *  (C) Copyright 1993-1996 Jaba Development.
 *  (C) Copyright 1993-1996 Jan van den Baard.
 *  All Rights Reserved.
 *
 *  Converted by hand from stkext.asm
 * 
 */

#include <proto/intuition.h>
#include <proto/exec.h>

#define STKMARGIN	2048	// When should we start worrying about the stack?
#define STKSIZE		16384	// Allocate this much stack space
#define CTLBLOCK        sizeof(StackSwapStruct)

extern struct Task *InputDevice;
extern APTR InputStack;

/*****************************************************************************
 *
 *  EnsureStack allocates a handle, which must be returned back to RevertStack
 *
 *  Registers preserved: all, except d0 and sp
 */
APTR EnsureStack(VOID)
{
    struct Task *task;
    struct EasyStruct easy = {
    	.es_StructSize = sizeof(struct EasyStruct),
    	.es_Flags = 0,
    	.es_Title = "BGUI Stack Overflow",
    	.es_TextFormat = "The \"%s\" process is out of stack space.",
    	.es_GadgetFormat = "Suspend",
    };
//  Figure out our address
    task = FindTask(NULL);

//  Calculate current stack left and check if it is within
//  allowed limits

    if ((IPTR)&task >= ((IPTR)task->tc_SPLower + STKMARGIN))
    	return NULL;

// It's not safe to do on-the-fly stack extensions
// under AROS.
    for (;;)
    	EasyRequest(NULL, &easy, NULL, task->tc_Node.ln_Name);
}
 
/*****************************************************************************
 *
 *   This function returns the stack back to free memory pool.
 *   Safe to call with NULL args
 *
 *   Registers destroyed: d0/a0/a1/a7
 */

VOID RevertStack(APTR stack)
{
}

/*******************************************************************************
 *
 *   This function allocates memory for the input.device stack extension,
 *   which is allocated only during library initialization.
 */

VOID InitInputStack(VOID)
{
}

VOID FreeInputStack(VOID)
{
}

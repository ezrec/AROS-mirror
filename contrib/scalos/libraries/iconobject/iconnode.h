/*
 * Scalos/iconnode.h
 * iconnode.h
 *
 * $Date$
 * $Revision$
 *
 * Richard Drummond
 *
 * Module to handle nodes in library's internal
 * list of iconobject datatypes
 *
 */

#ifndef ICONNODE_H
#define ICONNODE_H

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef  EXEC_NODES_H
#include <exec/nodes.h>
#endif

#ifndef  INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

/*
 * iconobject.library maintains a list of all iconobject datatypes.
 * Each member has the following structure.
 */

struct IconNode
{
    struct Node	Node;

    Class 	*in_Class;        /* Pointer to the datatype's BOOPSI class */
    struct Library *in_LibBase;      /* Pointer to the datatype's library */
#ifdef __amigaos4__
    struct DTClassIFace *in_IFace;     /* Pointer to the datatype's interface */
#endif
    CONST_STRPTR in_Suffix;       /* Filename suffix for icon file's of this type */
    WORD         in_SuffixLen;    /* Length of the above */
};

#endif


// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <string.h>

#include "Scalos.h"
#include "TableClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** Table.scalos/--background *******************************************
*
*   Table class is used to identify what special infos an icon can have.
*   Nearly all attributes of this class are relative to the current selected
*   element, which can be selected using SCCM_Table_Entry.
*
*****************************************************************************
*/
// /

/****** Table.scalos/SCCA_Table_Attribute ***********************************
*
*  NAME
*   SCCA_Table_Attribute -- (V40) ..G (ULONG)
*
*  FUNCTION
*   With the attribute of a field you can find or remove the entry and it's
*   used to get information from an icon.
*
*   Example:
*       Attr: SCCA_DOSIcon_Size
*       Name: "Size"
*       Type: SCCV_Table_Type_Long
*       Flags: 0
*       Size: 4
*
*****************************************************************************
*/
// /
/****** Table.scalos/SCCA_Table_Name ****************************************
*
*  NAME
*   SCCA_Table_Name -- (V40) ..G (char *)
*
*  FUNCTION
*   That's the name of the field. It is already translated to the current
*   languange. It maybe using to identify the information for the user.
*
*****************************************************************************
*/
// /
/****** Table.scalos/SCCA_Table_Count ***************************************
*
*  NAME
*   SCCA_Table_Count -- (V40) ..G (ULONG)
*
*  FUNCTION
*   Getting this attribute will return the current number of fields in the
*   table.
*
*****************************************************************************
*/
// /
/****** Table.scalos/SCCA_Table_Type ****************************************
*
*  NAME
*   SCCA_Table_Type -- (V40) ..G (ULONG)
*
*  FUNCTION
*   This attribute gives you information about the datatype of this field.
*   See SCCM_Table_Add for more information.
*
*****************************************************************************
*/
// /
/****** Table.scalos/SCCA_Table_Flags ***************************************
*
*  NAME
*   SCCA_Table_Flags -- (V40) ..G (ULONG)
*
*  FUNCTION
*   Which flags has the field. Currently there are no flags defined.
*
*****************************************************************************
*/
// /
/****** Table.scalos/SCCA_Table_Size ****************************************
*
*  NAME
*   SCCA_Table_Size -- (V40) ..G (ULONG)
*
*  FUNCTION
*   This attribute is use to give size information of the field data. It's
*   majorly used for SCCV_Table_Type_Raw, but it will return a valid value
*   for each type.
*
*****************************************************************************
*/
// /

/** Init a table
*/
static ULONG Table_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct TableInst *inst)
{
        if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
        {
                NewList((struct List *) &inst->list);
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Exit a table
*/
static ULONG Table_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct TableInst *inst)
{
        FreeAllNodes(&inst->list);
        return(SC_DoSuperMethodA(cl,obj, (Msg) msg));
}
// /

/****** Table.scalos/SCCM_Table_Add *****************************************
*
*  NAME
*   SCCM_Table_Add
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Table_Add, ULONG Attr, char *Name, ULONG Type, ULONG Flags, ULONG Size);
*
*  FUNCTION
*   This methods addes the information of a new field to the table.
*
*  INPUTS
*   Attr - attribute to identify the field (e.g. SCCA_Icon_Name)
*   Name - the name of the field, translated to the current language
*   Type - datatype
*           SCCV_Table_Type_String - pointer to a zero terminated string
*           SCCV_Table_Type_Long   - long value
*           SCCV_Table_Type_Double - pointer to a double value
*           SCCV_Table_Type_Time   - pointer to 2 longs (secs,micros)
*           SCCV_Table_Type_Raw    - pointer to unknown data (check Size)
*   Flags - flags for the datatype, currently none available
*   Size - size of the Raw data. Must only be given if type is
*           SCCV_Table_Type_Raw.
*
*  RESULT
*   TRUE if adding was successfully, else FALSE.
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Table_Remove
*
*****************************************************************************
*/
static ULONG Table_Add(struct SC_Class *cl, Object *obj, struct SCCP_Table_Add *msg, struct TableInst *inst)
{
        struct TableNode *tn;

        if (tn = (struct TableNode *) AllocNode(&inst->list,sizeof(struct TableNode) + strlen(msg->Name)))
        {
                tn->Attr = msg->Attr;
                tn->Type = msg->Type;
                tn->Flags = msg->Flags;
                strcpy(&tn->Name,msg->Name);
                inst->count++;

                switch (tn->Type)
                {
                  case SCCV_Table_Type_String :
                        tn->Size = strlen(&tn->Name);
                        break;
                  case SCCV_Table_Type_Long :
                        tn->Size = sizeof(ULONG);
                        break;
                  case SCCV_Table_Type_Double :
                        tn->Size = sizeof(double);
                        break;
                  case SCCV_Table_Type_Date :
                        tn->Size = 2*sizeof(ULONG);
                        break;
                  case SCCV_Table_Type_Raw :
                        tn->Size = msg->Size;
                        break;
                }

                if (inst->curnode == NULL)
                        inst->curnode = tn;
                return(TRUE);
        }
        return(FALSE);
}
// /

/****** Table.scalos/SCCM_Table_Remove **************************************
*
*  NAME
*   SCCM_Table_Remove
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Table_Remove, ULONG Attr);
*
*  FUNCTION
*   Remove an entry identified by the ID from the table. If it was the
*   current entry, the previous entry will be selected.
*
*  INPUTS
*   Attr - attribute to identify the field (e.g. SCCA_Icon_Name)
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Table_Add
*
*****************************************************************************
*/
static void Table_Rem(struct SC_Class *cl, Object *obj, struct SCCP_Table_Remove *msg, struct TableInst *inst)
{
        struct MinNode *node;

        for (node = inst->list.mlh_Head; node->mln_Succ; node = node->mln_Succ)
        {
                if (((struct TableNode *) node)->Attr = msg->Attr)
                {
                        if (((ULONG) inst->curnode) == ((ULONG) node))
                        {
                                if ((inst->curnode = (struct TableNode *) node->mln_Pred)->node.mln_Pred == NULL)
                                {
                                        if (IsListEmpty((struct List *) &inst->list))
                                                inst->curnode = NULL;
                                        else
                                                inst->curnode = (struct TableNode *) inst->list.mlh_Head;
                                }
                        }

                        FreeNode(node);
                        inst->count--;
                        inst->curnode = NULL;
                        return;
                }
        }
}
// /

/****** Table.scalos/SCCM_Table_Entry ***************************************
*
*  NAME
*   SCCM_Table_Entry
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Table_Entry, ULONG Pos);
*
*  FUNCTION
*   This method selectes a current entry. All attributes are relative to this
*   entry except SCCA_Table_Count. Preselected is the first entry.
*
*  INPUTS
*   Pos - one of these values
*           SCCV_Table_Entry_First - select first entry
*           SCCV_Table_Entry_Last - select last entry
*           SCCV_Table_Entry_Next - select next entry
*           SCCV_Table_Entry_Previous - select previous entry
*
*  RESULT
*   TRUE if the new entry was successfully selected.
*
*  NOTES
*
*  SEE ALSO
*   SCCM_Table_Add, SCCM_Table_Remove
*
*****************************************************************************
*/
static ULONG Table_Entry(struct SC_Class *cl, Object *obj, struct SCCP_Table_Entry *msg, struct TableInst *inst)
{
        if (msg->Pos == SCCV_Table_Entry_Next)
        {
                if (inst->curnode)
                {
                        inst->curnode = (struct TableNode *) inst->curnode->node.mln_Succ;
                        if (inst->curnode->node.mln_Succ != 0)
                                return(TRUE);
                }
        }

        if (msg->Pos == SCCV_Table_Entry_Previous)
        {
                if (inst->curnode)
                {
                        inst->curnode = (struct TableNode *) inst->curnode->node.mln_Pred;
                        if (inst->curnode->node.mln_Pred != 0)
                                return(TRUE);
                }
        }

        if ((msg->Pos == SCCV_Table_Entry_First) && !(IsListEmpty((struct List *) &inst->list)))
        {
                inst->curnode = (struct TableNode *) inst->list.mlh_Head;
                return(TRUE);
        }

        if ((msg->Pos == SCCV_Table_Entry_Last) && !(IsListEmpty((struct List *) &inst->list)))
        {
                inst->curnode = (struct TableNode *) inst->list.mlh_Tail;
                return(TRUE);
        }

        return(FALSE);
};
// /

/** Get one attribute of all of the getable attributes
*/
static ULONG Table_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct TableInst *inst )
{
        if (inst->curnode)
        {
                if (msg->opg_AttrID == SCCA_Table_Attribute)
                {
                        *(msg->opg_Storage) = inst->curnode->Attr;
                        return(TRUE);
                }
                if (msg->opg_AttrID == SCCA_Table_Name)
                {
                        *(msg->opg_Storage) = (ULONG) &inst->curnode->Name;
                        return(TRUE);
                }
                if (msg->opg_AttrID == SCCA_Table_Type)
                {
                        *(msg->opg_Storage) = inst->curnode->Type;
                        return(TRUE);
                }
                if (msg->opg_AttrID == SCCA_Table_Flags)
                {
                        *(msg->opg_Storage) = inst->curnode->Flags;
                        return(TRUE);
                }
                if (msg->opg_AttrID == SCCA_Table_Size)
                {
                        *(msg->opg_Storage) = inst->curnode->Size;
                        return(TRUE);
                }
        }

        if (msg->opg_AttrID == SCCA_Table_Count)
        {
                *(msg->opg_Storage) = inst->count;
                return(TRUE);
        }

        return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData TableMethods[] =
{
        { SCCM_Table_Add,(ULONG) Table_Add, sizeof(struct SCCP_Table_Add), 0, NULL },
        { SCCM_Table_Remove,(ULONG) Table_Rem, sizeof(struct SCCP_Table_Remove), 0, NULL },
        { SCCM_Table_Entry,(ULONG) Table_Entry, sizeof(struct SCCP_Table_Entry), 0, NULL },
        { SCCM_Init,(ULONG) Table_Init, 0, 0, NULL },
        { SCCM_Exit,(ULONG) Table_Exit, 0, 0, NULL },
        { OM_GET,(ULONG) Table_Get, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};


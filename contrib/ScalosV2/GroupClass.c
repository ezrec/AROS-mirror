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
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/guigfx.h>
#include <guigfx/guigfx.h>

#include <clib/dos_protos.h>

#include "Scalos.h"
#include "GroupClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"
#include "scalos_protos.h"

#include "Debug.h"

#define EPS 100 // if blit area height or width are smaller than EPS than disable blit optimization

/****** Group.scalos/--background *******************************************
*
*   Group class is a class that creates an Area object.
*   It groups one or more Area class objects together like MUI does it, but
*   it sends a method asynchronously if the distination object runs on a
*   different thread than the Group object.
*   A class may be a vertical group (SCCV_Area_VGroup) or a horizontal group
*   (SCCV_Area_HGroup).
*   A VGroup displays its childs (which are Area objects as mentioned above)
*   from the top to the bottom of the group space and a HGroup displays its
*   childs from left to right.
*   E.g. the value for the tag SCCA_Window_Root normally is a Group object
*   to be able to have more than one Area object in a window.
*
*   *** Implementation of SCCM_Area_AskMinMax:
*
*   The values for the Group object for SCCM_Area_AskMinMax are calculated
*   as follows:
*
*   HGroup:
*   group maxheight = smallest maxheight of the childs
*   group minheight = biggest minheight of the childs
*   group defheight = adjusted average of the childs
*
*   group minwidth = sum of all minwidth of the childs
*   group maxwidth = sum of all maxwidth of the childs
*   group defwidth = sum of all defwidth of the childs
*
*
*   VGroup:
*   group maxheight = sum of all maxheight of the childs
*   group minheight = sum of all minheight of the childs
*   group defheight = sum of all defheight of the childs
*
*   group minwidth = biggest minwidth of the childs
*   group maxwidth = smallest maxwidth of the childs
*   group defwidth = adjusted average of the childs
*
*   This is like MUI does it.
*
*   *** Implementation of SCCM_Area_Show:
*
*   Remember!
*   The values of struct IBox that you pass for SCCM_Area_Show need to be
*   guaranteed to be reasonable, that is, they are in the range defined by
*   struct SC_MinMax so the group is displayable with these values.
*   If not, anything may happen.
*
*   SCCM_Area_Show takes the defwidth of a child as a weight for that child
*   for a HGroup, and defheights for a VGroup. The bigger that value is, the
*   more space the child will get compared to the other childs.
*   However, childs have a minimum and maximum size so that these weights can
*   get out of balance.
*   If all childs have a mimimum of 1 x 1 pixel and a maximum of
*   SCCV_Area_MaxSize x SCCV_Area_MaxSize pixels the sizes of the childs will
*   always perfectly match your weights.
*
*   So these default values have a different meaning to a Group object than
*   to a normal Area object.
*   
*****************************************************************************
*/
// /
/****** Group.scalos/SCCA_Group_Child ***************************************
*
*  NAME
*   SCCA_Group_Child -- (V40) I.. (Object *)
*
*  FUNCTION
*   This attribute may be set at initialization time to tell the group which
*   child objects belong to it. The child is added to the list of childs
*   using OM_ADDMEMBER. This list of childs is processed by SCCM_Area_Setup
*   of the group class so you may also change it before this method is called.
*
*  NOTES
*   A group can be created like this:
*
*           grobj1   = SC_NewObject(NULL,SCC_GROUP_NAME,
*                                   SCCA_Group_Child,btmobj3,
*                                   SCCA_Group_Child,grobj2,
*                                   TAG_DONE
*                                  );
*
*****************************************************************************
*/                                                              
// /
/****** Group.scalos/SCCA_Group_Type ****************************************
*
*  NAME
*   SCCA_Group_Type -- (V40) I.. (ULONG)
*
*  FUNCTION
*   This attribute may be set at initialization time to tell the group if it
*   should layout its childs horizontally (SCCV_Group_HGroup) or vertically
*   (SCCV_Group_VGroup).
*   Default is horizontally.
*
*  NOTES
*
*****************************************************************************
*/                                                              
// /

/** Alerttext Show
*/
UBYTE alerttext_Show[] =
{
                        10,
                        10,
                        10,
                        'P','a','n','i','c','!',' ','C','r','e','a','t','e','M','s','g','P','o','r','t','(',')',' ','f','a','i','l','e','d','.',' ','(','G','r','o','u','p','/','S','h','o','w',')','.',0,
                        1,
                        10,
                        10,
                        20,
                        'P','r','e','s','s',' ','l','e','f','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','c','o','n','t','i','n','u','e',' ','o','n',' ','y','o','u','r',' ','o','w','n',' ','r','i','s','k','!',0,
                        1,
                        10,
                        10,
                        30,
                        'P','r','e','s','s',' ','r','i','g','h','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','r','e','b','o','o','t','.',0,
                        0
};
// /
/** Alerttext AskMinMax
*/
UBYTE alerttext_AskMinMax[] =
{
                        10,
                        10,
                        10,
                        'P','a','n','i','c','!',' ','C','r','e','a','t','e','M','s','g','P','o','r','t','(',')',' ','f','a','i','l','e','d','.',' ','(','G','r','o','u','p','C','l','a','s','s','/','A','s','k','M','i','n','M','a','x',')','.',0,
                        1,
                        10,
                        10,
                        20,
                        'P','r','e','s','s',' ','l','e','f','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','c','o','n','t','i','n','u','e',' ','o','n',' ','y','o','u','r',' ','o','w','n',' ','r','i','s','k','!',0,
                        1,
                        10,
                        10,
                        30,
                        'P','r','e','s','s',' ','r','i','g','h','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','r','e','b','o','o','t','.',0,
                        0
 
};
// /
/** Alerttext Hide
*/
UBYTE alerttext_Hide[] =
{
                        10,
                        10,
                        10,
                        'P','a','n','i','c','!',' ','C','r','e','a','t','e','M','s','g','P','o','r','t','(',')',' ','f','a','i','l','e','d','.',' ','(','G','r','o','u','p','C','l','a','s','s','/','H','i','d','e',')','.',0,
                        1,
                        10,
                        10,
                        20,
                        'P','r','e','s','s',' ','l','e','f','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','c','o','n','t','i','n','u','e',' ','o','n',' ','y','o','u','r',' ','o','w','n',' ','r','i','s','k','!',0,
                        1,
                        10,
                        10,
                        30,
                        'P','r','e','s','s',' ','r','i','g','h','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','r','e','b','o','o','t','.',0,
                        0
};
// /
/** Alerttext Cleanup
*/
UBYTE alerttext_Cleanup[] =
{
                        10,
                        10,
                        10,
                        'P','a','n','i','c','!',' ','C','r','e','a','t','e','M','s','g','P','o','r','t','(',')',' ','f','a','i','l','e','d','.',' ','(','G','r','o','u','p','C','l','a','s','s','/','C','l','e','a','n','u','p',')','.',0,
                        1,
                        10,
                        10,
                        20,
                        'P','r','e','s','s',' ','l','e','f','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','c','o','n','t','i','n','u','e',' ','o','n',' ','y','o','u','r',' ','o','w','n',' ','r','i','s','k','!',0,
                        1,
                        10,
                        10,
                        30,
                        'P','r','e','s','s',' ','r','i','g','h','t',' ','m','o','u','s','e',' ','b','u','t','t','o','n',' ','t','o',' ','r','e','b','o','o','t','.',0,
                        0
};
// /
        
/** Init
*/
static ULONG Group_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct GroupInst *inst)
{
        /*
         * our object exists before we are called, so our instance data pointer is valid
         */
        DEBUG("Got Init\n");
        
        if (SC_DoSuperMethodA(cl,obj,(Msg) msg))
        {
                struct TagItem *tag;
                struct TagItem *taglist;
                struct TagItem **tags = &taglist;

                NewList((struct List *)&inst->childlist);

                inst->GroupType = GetTagData(SCCA_Group_Type,SCCV_Group_HGroup,msg->ops_AttrList);

                taglist = msg->ops_AttrList;
                while(tag = NextTagItem(tags))
                {
                        switch (tag->ti_Tag)
                        {
                                case SCCA_Group_Child:

                                        // add child to our branch of the classes tree
                                        // so that dispose will be called for every child in this list

                                        SC_DoSuperMethod(cl, obj, OM_ADDMEMBER, tag->ti_Data);

                                        break;
                        }
                }
                DEBUG("Init succeeded\n");

                return(TRUE);
        }
        return(FALSE);
}
// /

/** AskMinMax
*/
static void Group_AskMinMax(struct SC_Class *cl, Object *obj, struct SCCP_Area_AskMinMax *msg, struct GroupInst *inst)
{
        ULONG msgcntr = 0;
        struct ChildEntry *child;

        DEBUG("Got AskMinMax\n");

        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        // above our superclass(es) could have added some values for frames etc


        if(!(inst->childsreplyport = CreateMsgPort()))
        {
                if(DisplayAlert(RECOVERY_ALERT, alerttext_AskMinMax, 45))
                {
                        return;
                }
                else
                {
                        ColdReboot();
                }
        }
        
        /*
         * send AskMinMax method to all childs, asynchronous if possible
         */

        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                if (SC_DoMsgMethod(inst->childsreplyport, child->child, SCCM_Area_AskMinMax, &child->sizes))
                        msgcntr++;
                else    
                        SC_DoMethod(child->child, SCCM_Area_AskMinMax, &child->sizes);

        }
        
        for(;msgcntr;msgcntr--)
        {
                struct Message *msg;
        
                while (!(msg = GetMsg(inst->childsreplyport)))
                WaitPort(inst->childsreplyport);
        
                SC_FreeMsg((struct SC_Message *)msg);
        }

        if (inst->childsreplyport)
        {
                DeleteMsgPort(inst->childsreplyport);
                inst->childsreplyport = 0;
        }
                
        /*
         * the AskMinMax methods are done now
         *
         * it is guaranteed that the returned default values are in the range of the min and max values
         * now we calculate our minmax structure for the group object
         */

        /*
         * HGroup:
         * group maxheight = smallest maxheight of the childs
         * group minheight = biggest minheight of the childs
         * group defheight = adjusted average of the childs
         *
         * group minwidth = sum of all minwidth of the childs
         * group maxwidth = sum of all maxwidth of the childs
         * group defwidth = sum of all defwidth of the childs
         */

        if(inst->GroupType == SCCV_Group_HGroup)
        {
                msg->sizes->maxheight = SCCV_Area_MaxSize;
                msg->sizes->minheight = 0;
                msg->sizes->defheight = 0;
                msg->sizes->minwidth = 0;
                msg->sizes->maxwidth = 0;
                msg->sizes->defwidth = 0;

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        if(child->sizes.maxheight < msg->sizes->maxheight)
                                msg->sizes->maxheight = child->sizes.maxheight; 
                                
                        if(child->sizes.minheight > msg->sizes->minheight)
                                msg->sizes->minheight = child->sizes.minheight; 

                        msg->sizes->defheight += child->sizes.defheight;
                        
                        if(msg->sizes->defheight > SCCV_Area_MaxSize) msg->sizes->defheight = SCCV_Area_MaxSize;

#ifdef DEBUG_SERIAL_GROUP
                        DEBUG2("Child defwidth %ld defheight %ld\n",child->sizes.defwidth, child->sizes.defheight);
#endif
                        msg->sizes->minwidth += child->sizes.minwidth;
                        msg->sizes->maxwidth += child->sizes.maxwidth;
                        msg->sizes->defwidth += child->sizes.defwidth;
                        
                        if(msg->sizes->minwidth > SCCV_Area_MaxSize) msg->sizes->minwidth = SCCV_Area_MaxSize;
                        if(msg->sizes->maxwidth > SCCV_Area_MaxSize) msg->sizes->maxwidth = SCCV_Area_MaxSize;
                        if(msg->sizes->defwidth > SCCV_Area_MaxSize) msg->sizes->defwidth = SCCV_Area_MaxSize;
                }
                inst->GroupDefWidth = msg->sizes->defwidth; // for calculation of childpercent

                msg->sizes->defheight = msg->sizes->defheight/inst->NumChilds;
                
                /*
                 * we have to check if defheight is in the group range because the
                 * average that we calculate may be somewhere in the child ranges
                 * which is different to the group range
                 */

                if(msg->sizes->defheight > msg->sizes->maxheight)
                        msg->sizes->defheight = msg->sizes->maxheight;
                        
                if(msg->sizes->defheight < msg->sizes->minheight)
                        msg->sizes->defheight = msg->sizes->minheight;
        }
                        
        /*
         *
         * VGroup:
         * group maxheight = sum of all maxheight of the childs
         * group minheight = sum of all minheight of the childs
         * group defheight = sum of all defheight of the childs
         *
         * group minwidth = biggest minwidth of the childs
         * group maxwidth = smallest maxwidth of the childs
         * group defwidth = adjusted average of the childs
         */

        if(inst->GroupType == SCCV_Group_VGroup)
        {
                msg->sizes->maxheight = 0;
                msg->sizes->minheight = 0;
                msg->sizes->defheight = 0;
                msg->sizes->minwidth = 0;
                msg->sizes->maxwidth = SCCV_Area_MaxSize;
                msg->sizes->defwidth = 0;

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        if(child->sizes.maxwidth < msg->sizes->maxwidth)
                                msg->sizes->maxwidth = child->sizes.maxwidth; 
                                
                        if(child->sizes.minwidth > msg->sizes->minwidth)
                                msg->sizes->minwidth = child->sizes.minwidth; 

                        msg->sizes->defwidth += child->sizes.defwidth;
                        
                        if(msg->sizes->defwidth > SCCV_Area_MaxSize) msg->sizes->defwidth = SCCV_Area_MaxSize;
                                                
                        msg->sizes->minheight += child->sizes.minheight;
                        msg->sizes->maxheight += child->sizes.maxheight;
                        msg->sizes->defheight += child->sizes.defheight;
                        
                        if(msg->sizes->minheight > SCCV_Area_MaxSize) msg->sizes->minheight = SCCV_Area_MaxSize;
                        if(msg->sizes->maxheight > SCCV_Area_MaxSize) msg->sizes->maxheight = SCCV_Area_MaxSize;
                        if(msg->sizes->defheight > SCCV_Area_MaxSize) msg->sizes->defheight = SCCV_Area_MaxSize;
                }
                inst->GroupDefHeight = msg->sizes->defheight;

                msg->sizes->defwidth = msg->sizes->defwidth/inst->NumChilds;

                /*
                 * we have to check if defheight is in the group range because the
                 * average that we calculate may be somewhere in the child ranges
                 */

                if(msg->sizes->defwidth > msg->sizes->maxwidth)
                        msg->sizes->defwidth = msg->sizes->maxwidth;
                        
                if(msg->sizes->defwidth < msg->sizes->minwidth)
                        msg->sizes->defwidth = msg->sizes->minwidth;
        }

        DEBUG("AskMinMax finished\n");

        // no result, but a new struct SC_MinMax
}
// /

/** Show
*/
static void Group_Show(struct SC_Class *cl, Object *obj, struct SCCP_Area_Show *msg, struct GroupInst *inst)
{
        ULONG msgcntr = 0;
        struct ChildEntry *child;
        
        ULONG GroupHeight;
        ULONG GroupWidth;

        DEBUG("Got Show\n");

        DEBUG2("Group width %ld height %ld\n",msg->bounds->Width, msg->bounds->Height);
        DEBUG2("Group left %ld top %ld\n",msg->bounds->Left, msg->bounds->Top);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        /*
         * the values of struct IBox, that we get, are guaranteed to be reasonable
         * they are in the range defined by SC_MinMax so the group is displayable with these values
         */

        /*
         * save old values for draw optimization
         */
         
        if(msg->action == SCCV_Area_Action_Resize)
        {
                inst->action = SCCV_Area_Action_Resize;
        
                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        child->oldbounds.Left   = child->bounds.Left;
                        child->oldbounds.Top    = child->bounds.Top;
                        child->oldbounds.Height = child->bounds.Height;
                        child->oldbounds.Width  = child->bounds.Width;
                }
        }
        else
                inst->action = SCCV_Area_Action_OpenClose;
        
        if(inst->GroupType == SCCV_Group_HGroup)
        {
                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        /*
                         * calculate some width for the child according to the defwidth
                         */

                        // the percentage of this childs defwidth compared to all defwidths

                        double childpercent = (double)child->sizes.defwidth / (double)inst->GroupDefWidth;
                        
                        // msg->bounds->Width is the 100% width of the group
                        
                        child->bounds.Width = (UWORD) ((double) msg->bounds->Width * childpercent);
                        
#ifdef DEBUG_SERIAL_GROUP
                        DEBUG2("2 (unadjusted): Child %ld width %ld\n", child, child->bounds.Width);
#endif
                }

                /*
                 * now we need to adjust the group width and the width of all childs
                 * they should be equal
                 * we have to do this due to rounding problems
                 */

                GroupWidth = 0;

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        GroupWidth += child->bounds.Width;
                }
                
#ifdef DEBUG_SERIAL_GROUP
                DEBUG1("1a: Width of all childs %ld\n", GroupWidth);
                DEBUG1("1a: Group width %ld\n", msg->bounds->Width);
#endif      
                /*
                 * group is more wide than childs
                 */

                if(msg->bounds->Width > GroupWidth)
                {
                        ULONG diff;
                        
                        diff = msg->bounds->Width - GroupWidth;
                        
#ifdef DEBUG_SERIAL_GROUP
                        DEBUG1("3: diff: %ld\n", diff);
#endif
                        while(diff)
                        {
                                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                                {
                                        if( child->bounds.Width < child->sizes.maxwidth )
                                        {
                                                child->bounds.Width++;
                                                                        
#ifdef DEBUG_SERIAL_GROUP
                                                DEBUG1("Child %ld got one more pixel for width for group width adjustment\n", child);
#endif                              
                                                diff--;
                                                                        
                                                if(!diff) break;
                                        }
                                }
                        }           
                }
                
                /*
                 * childs are more wide than group
                 */

                if(GroupWidth > msg->bounds->Width)
                {
                        ULONG diff;
                        
                        diff = GroupWidth - msg->bounds->Width;
                        
#ifdef DEBUG_SERIAL_GROUP
                        DEBUG1("4: diff: %ld\n", diff);
#endif
                        while(diff)
                        {
                                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                                {
                                        if( child->bounds.Width > child->sizes.minwidth )
                                        {
                                                child->bounds.Width--;
                                                                        
#ifdef DEBUG_SERIAL_GROUP
                                                DEBUG1("Child %ld lost one more pixel for width for group width adjustment\n", child);
#endif                              
                                                diff--;
                                                                        
                                                if(!diff) break;
                                        }
                                }
                        }           
                }

#ifdef DEBUG_SERIAL_GROUP

                GroupWidth = 0;
        
                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        GroupWidth += child->bounds.Width;
                        // this sum should be exactly the group width
                }

                DEBUG1("1b: Width of all childs %ld\n", GroupWidth);
                DEBUG1("1b: Group width %ld\n", msg->bounds->Width);

#endif

                /*
                 * now we make sure that the bounds for each child are
                 * in the childs bounds range
                 */

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        if(child->bounds.Width > child->sizes.maxwidth)
                        {
                                // this child is calculated bigger than possible
                                // so we make its width smaller by putting it away to other
                                // childs that are not at their max width, pixel by pixel
                                // this will always work because in the worst case all
                                // childs will be at max width and nothing will be left
                                // to be put away.
                                // this happens when the group is at max and the childs defaults
                                // are bad
                                
                                // if this splitting must be done the weights get worse

                                ULONG diff;
                        
                                diff = child->bounds.Width - child->sizes.maxwidth;
                                
#ifdef DEBUG_SERIAL_GROUP
                                DEBUG1("1: diff: %ld\n", diff);
#endif              
                                while(diff)
                                {
                                        struct ChildEntry *otherchild;

                                        for (otherchild = (struct ChildEntry *)inst->childlist.mlh_Head; otherchild->node.mln_Succ; otherchild = (struct ChildEntry *)otherchild->node.mln_Succ)
                                        {
                                                if(otherchild != child)
                                                {
                                                        if( otherchild->bounds.Width < otherchild->sizes.maxwidth )
                                                        {
                                                                otherchild->bounds.Width++;
                                                                
#ifdef DEBUG_SERIAL_GROUP
                                                                DEBUG1("Child %ld got one more pixel for width\n", otherchild);
#endif                              
                                                                diff--;
                                                                
                                                                if(!diff) break;
                                                        }
                                                }
                                        }
                                }
                                child->bounds.Width = child->sizes.maxwidth;
                        }

                        if(child->bounds.Width < child->sizes.minwidth)
                        {
                                // this child is calculated smaller than possible
                                // so we make its width bigger by getting pixel space from other
                                // childs that are not at their min width, pixel by pixel
                                // this will always work because in the worst case all
                                // childs will be at min width and nothing will be left to get.
                                // this happens when the group is at min and the childs defaults
                                // are bad
                                
                                // if this splitting must be done the weights get worse

                                ULONG diff;
                        
                                diff = child->sizes.minwidth - child->bounds.Width;
                                
#ifdef DEBUG_SERIAL_GROUP
                                DEBUG1("2: diff: %ld\n", diff);
#endif
                                while(diff)
                                {
                                        struct ChildEntry *otherchild;

                                        for (otherchild = (struct ChildEntry *)inst->childlist.mlh_Head; otherchild->node.mln_Succ; otherchild = (struct ChildEntry *)otherchild->node.mln_Succ)
                                        {
                                                if(otherchild != child)
                                                {
                                                        if( otherchild->bounds.Width > otherchild->sizes.minwidth )
                                                        {
                                                                otherchild->bounds.Width--;
                                                                
#ifdef DEBUG_SERIAL_GROUP
                                                                DEBUG1("Child %ld lost one more pixel for width\n", otherchild);
#endif                              
                                                                diff--;
                                                                
                                                                if(!diff) break;
                                                        }
                                                }
                                        }
                                }
                                child->bounds.Width = child->sizes.minwidth;
                        }

                        // now this child has got some reasonable bounds.width
                        
                        /*
                         * now we calculate the height and top values
                         */

                        // if child height cannot be as big as the group height we have to add some space

                        if(msg->bounds->Height > child->sizes.maxheight)
                        {
                                WORD HeightDiff;            // difference of group height and child height

                                child->bounds.Height = child->sizes.maxheight;

                                if(HeightDiff = msg->bounds->Height - child->sizes.maxheight)
                                        child->bounds.Top = HeightDiff/2 + msg->bounds->Top;
                                        
                                        // upper and lower space have equal size
                        }
                        else
                        {
                                child->bounds.Height = msg->bounds->Height;
                                child->bounds.Top = msg->bounds->Top;
                        }

                }
                
                // now the childs have some reasonable Height and Top values
                
                /*
                 * Now we calculate the left values
                 */
                {
                        // SumLeft: space from absolute left of the group box to the next free left position for a child

                        WORD SumLeft = 0;
                
                        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                        {
                                child->bounds.Left = msg->bounds->Left + SumLeft;
                                SumLeft += child->bounds.Width;
                        
#ifdef DEBUG_SERIAL_GROUP
                                DEBUG2("Child %ld Left %ld\n", child, child->bounds.Left);
                                DEBUG2("4 (sending): Child %ld width %ld\n", child, child->bounds.Width);
#endif
                        }
                }
                
                // now the childs have some reasonable Left values
        }

/*---------------------------------------------------------------*/

        if(inst->GroupType == SCCV_Group_VGroup)
        {
                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        /*
                         * calculate some height for the child according to the defheight
                         */

                        // the percentage of this childs defheight compared to all defheights

                        double childpercent = (double)child->sizes.defheight / (double)inst->GroupDefHeight;
                        
                        // msg->bounds->Height is the 100% height of the group
                        child->bounds.Height = (UWORD) ((double) msg->bounds->Height * childpercent);
                        
#ifdef DEBUG_SERIAL_GROUP
                        DEBUG2("2 (unadjusted): Child %ld height %ld\n", child, child->bounds.Height);
#endif
                }

                /*
                 * now we need to adjust the group width and the width of all childs
                 * they should be equal
                 */

                GroupHeight = 0;

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        GroupHeight += child->bounds.Height;
                }
                
#ifdef DEBUG_SERIAL_GROUP
                DEBUG1("1a: Height of all childs %ld\n", GroupHeight);
                DEBUG1("1a: Group height %ld\n", msg->bounds->Height);
#endif
                /*
                 * group is taller than childs
                 */

                if(msg->bounds->Height > GroupHeight)
                {
                        ULONG diff;
                        
                        diff = msg->bounds->Height - GroupHeight;
                        
#ifdef DEBUG_SERIAL_GROUP
                        DEBUG1("3: diff: %ld\n", diff);
#endif
                        while(diff)
                        {
                                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                                {
                                        if( child->bounds.Height < child->sizes.maxheight )
                                        {
                                                child->bounds.Height++;
                                                                        
#ifdef DEBUG_SERIAL_GROUP
                                                DEBUG1("Child %ld got one more pixel for height for group height adjustment\n", child);
#endif                              
                                                diff--;
                                                                        
                                                if(!diff) break;
                                        }
                                }
                        }           
                }
                
                /*
                 * childs are taller than group
                 */

                if(GroupHeight > msg->bounds->Height)
                {
                        ULONG diff;
                        
                        diff = GroupHeight - msg->bounds->Height;
                        
#ifdef DEBUG_SERIAL_GROUP
                        DEBUG1("4: diff: %ld\n", diff);
#endif
                        while(diff)
                        {
                                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                                {
                                        if( child->bounds.Height > child->sizes.minheight )
                                        {
                                                child->bounds.Height--;
                                                                        
#ifdef DEBUG_SERIAL_GROUP
                                                DEBUG1("Child %ld lost one more pixel for height for group height adjustment\n", child);
#endif                              
                                                diff--;
                                                                        
                                                if(!diff) break;
                                        }
                                }
                        }           
                }

#ifdef DEBUG_SERIAL_GROUP

                GroupHeight = 0;
        
                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        GroupHeight += child->bounds.Height;
                        // this sum should be exactly the group height
                }

                DEBUG1("1b: Height of all childs %ld\n", GroupHeight);
                DEBUG1("1b: Group height %ld\n", msg->bounds->Height);

#endif

                /*
                 * now we make sure that the bounds for each child are
                 * in the childs bounds range
                 */

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        if(child->bounds.Height > child->sizes.maxheight)
                        {
                                // this child is calculated bigger than possible
                                // so we make its height smaller by putting it away to other
                                // childs that are not at their max height, pixel by pixel
                                // this will always work because in the worst case all
                                // childs will be at max height and nothing will be left
                                // to be put away.
                                // this happens when the group is at max and the childs defaults
                                // are bad
                                
                                // if this splitting must be done the weights get worse

                                ULONG diff;
                        
                                diff = child->bounds.Height - child->sizes.maxheight;
                                
#ifdef DEBUG_SERIAL_GROUP
                                DEBUG1("1: diff: %ld\n", diff);
#endif              
                                while(diff)
                                {
                                        struct ChildEntry *otherchild;

                                        for (otherchild = (struct ChildEntry *)inst->childlist.mlh_Head; otherchild->node.mln_Succ; otherchild = (struct ChildEntry *)otherchild->node.mln_Succ)
                                        {
                                                if(otherchild != child)
                                                {
                                                        if( otherchild->bounds.Height < otherchild->sizes.maxheight )
                                                        {
                                                                otherchild->bounds.Height++;
                                                                
#ifdef DEBUG_SERIAL_GROUP
                                                                DEBUG1("Child %ld got one more pixel for height\n", otherchild);
#endif                              
                                                                diff--;
                                                                
                                                                if(!diff) break;
                                                        }
                                                }
                                        }
                                }
                                child->bounds.Height = child->sizes.maxheight;
                        }

                        if(child->bounds.Height < child->sizes.minheight)
                        {
                                // this child is calculated smaller than possible
                                // so we make its height bigger by getting pixel space from other
                                // childs that are not at their min height, pixel by pixel
                                // this will always work because in the worst case all
                                // childs will be at min height and nothing will be left to get.
                                // this happens when the group is at min and the childs defaults
                                // are bad
                                
                                // if this splitting must be done the weights get worse

                                ULONG diff;
                        
                                diff = child->sizes.minheight - child->bounds.Height;
                                
#ifdef DEBUG_SERIAL_GROUP
                                DEBUG1("2: diff: %ld\n", diff);
#endif
                                while(diff)
                                {
                                        struct ChildEntry *otherchild;

                                        for (otherchild = (struct ChildEntry *)inst->childlist.mlh_Head; otherchild->node.mln_Succ; otherchild = (struct ChildEntry *)otherchild->node.mln_Succ)
                                        {
                                                if(otherchild != child)
                                                {
                                                        if( otherchild->bounds.Height > otherchild->sizes.minheight )
                                                        {
                                                                otherchild->bounds.Height--;
                                                                
#ifdef DEBUG_SERIAL_GROUP
                                                                DEBUG1("Child %ld lost one more pixel for Height\n", otherchild);
#endif                              
                                                                diff--;
                                                                
                                                                if(!diff) break;
                                                        }
                                                }
                                        }
                                }
                                child->bounds.Height = child->sizes.minheight;
                        }

                        // now this child has got some reasonable bounds.Height

                        /*
                         * now we calculate the width and left values
                         */
                                                
                        if(msg->bounds->Width > child->sizes.maxwidth)
                        {
                                WORD WidthDiff;         // difference of group width and child width

                                child->bounds.Width = child->sizes.maxwidth;

                                if(WidthDiff = msg->bounds->Width - child->sizes.maxwidth)
                                        child->bounds.Left = WidthDiff/2 + msg->bounds->Left;
                                        
                                        // upper and lower space have equal size
                        }
                        else
                        {
                                child->bounds.Width = msg->bounds->Width;
                                child->bounds.Left = msg->bounds->Left;
                        }

                }

                /*
                 * Now we calculate the top values
                 */
                {
                        // SumTop: space from absolute top of the group box to the next free top position for a child

                        WORD SumTop = 0;
                
                        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                        {
                                child->bounds.Top = msg->bounds->Top + SumTop;
                                SumTop += child->bounds.Height;
                                
#ifdef DEBUG_SERIAL_GROUP
                                DEBUG2("Child %ld Top %ld\n", child, child->bounds.Top);
                                DEBUG2("4 (sending): Child %ld height %ld\n", child, child->bounds.Height);
#endif
                        }
                }
        }

/*------------------------------------------------------------*/

        /*
         * Send the method to all children
         */

        if(!(inst->childsreplyport = CreateMsgPort()))
        {
                if(DisplayAlert(RECOVERY_ALERT, alerttext_Show, 45))
                {
                        return;
                }
                else
                {
                        ColdReboot();
                }
        }

        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                if (SC_DoMsgMethod(inst->childsreplyport,child->child, SCCM_Area_Show, msg->action, &child->bounds))
                        msgcntr++;
                else
                        SC_DoMethod(child->child, SCCM_Area_Show, msg->action, &child->bounds);
        }

        for(;msgcntr;msgcntr--)
        {
                struct Message *msg;
        
                while (!(msg = GetMsg(inst->childsreplyport)))
                WaitPort(inst->childsreplyport);
        
                SC_FreeMsg((struct SC_Message *)msg);
        }

        if (inst->childsreplyport)
        {
                DeleteMsgPort(inst->childsreplyport);
                inst->childsreplyport = 0;
        }

        DEBUG("Show finished\n");
}
// /

/** Hide
*/
static void Group_Hide(struct SC_Class *cl, Object *obj, struct SCCP_Area_Hide *msg, struct GroupInst *inst)
{
        struct ChildEntry *child;
        ULONG msgcntr = 0;

        DEBUG("Got Hide\n");

        /*
         * Send the method to all children
         */

        if(!(inst->childsreplyport = CreateMsgPort()))
        {
                if(DisplayAlert(RECOVERY_ALERT, alerttext_Hide, 45))
                {
                        return;
                }
                else
                {
                        ColdReboot();
                }
        }

        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                if (SC_DoMsgMethodA(inst->childsreplyport, child->child, (Msg) msg))
                        msgcntr++;
                else
                        SC_DoMethodA(child->child, (Msg) msg);
        }

        for(;msgcntr;msgcntr--)
        {
                struct Message *msg;
        
                while (!(msg = GetMsg(inst->childsreplyport)))
                WaitPort(inst->childsreplyport);
        
                SC_FreeMsg((struct SC_Message *)msg);
        }

        if (inst->childsreplyport)
        {
                DeleteMsgPort(inst->childsreplyport);
                inst->childsreplyport = 0;
        }

        SC_DoSuperMethodA(cl, obj, (Msg) msg); // at the end

        DEBUG("Hide finished\n");
}
// /

/** Draw
*/
static void Group_Draw(struct SC_Class *cl, Object *obj, struct SCCP_Area_Draw *msg, struct GroupInst *inst)
{
        struct ChildEntry   *child;
        struct Rectangle     rect1;
        struct Region       *region;

        DEBUG("Got Draw\n");

        SC_DoSuperMethodA(cl, obj, (Msg) msg); // SCCM_Draw from AreaClass has no return value
        
        /*
         * Send the method to all children
         */ 
        
        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
/*
                if( inst->action == SCCV_Area_Action_Resize )
                {
                        if( get(child->child, SCCA_Area_StaticContents) == TRUE )
                        {
                                LONG diffX, diffY;
                                WORD blitsrcx, blitsrcy; // for blit
                                        
                                diffX = child->bounds.Left - child->oldbounds.Left;
                                diffY = child->bounds.Top - child->oldbounds.Top;
                                
                                // schnittfläche

                                rect1.MinX = child->oldbounds.Left;
                                rect1.MaxX = child->oldbounds.Left + child->oldbounds.Width - 1;
                                rect1.MinY = child->oldbounds.Top;
                                rect1.MaxY = child->oldbounds.Top + child->oldbounds.Height - 1;
                                        
                                DEBUG2("old bounds: MinX %ld MaxX %ld\n", rect1.MinX, rect1.MaxX);
                                DEBUG2("old bounds: MinY %ld MaxY %ld\n", rect1.MinY, rect1.MaxY);

                                if(region = NewRegion())
                                {
                                        OrRectRegion( region, &rect1 );

                                        rect1.MinX = child->bounds.Left;
                                        rect1.MaxX = child->bounds.Left + child->bounds.Width - 1;
                                        rect1.MinY = child->bounds.Top;
                                        rect1.MaxY = child->bounds.Top + child->bounds.Height - 1;
                                        
                                        DEBUG2("new bounds: MinX %ld MaxX %ld\n", rect1.MinX, rect1.MaxX);
                                        DEBUG2("new bounds: MinY %ld MaxY %ld\n", rect1.MinY, rect1.MaxY);

                                        AndRectRegion( region, &rect1 );
                                
                                        blitsrcx = region->bounds.MinX;
                                        blitsrcy = region->bounds.MinY;
                                        
                                        // verschiebung auf newbounds
                                        
                                        region->bounds.MinX += diffX;
                                        region->bounds.MaxX += diffX;
                                        region->bounds.MinY += diffY;
                                        region->bounds.MaxY += diffY;
                                        
                                        if(region->RegionRectangle->Next != NULL) DEBUG("Hm, shit! Rectangle is not simple.\n");

                                        // schnitt von verschobener schnittfläche und newbounds rect = blitfläche (ziel)
                                        
                                        rect1.MinX = child->bounds.Left;
                                        rect1.MaxX = child->bounds.Left + child->bounds.Width - 1;
                                        rect1.MinY = child->bounds.Top;
                                        rect1.MaxY = child->bounds.Top + child->bounds.Height - 1;
                                        
                                        AndRectRegion( region, &rect1 );
                                
                                        DEBUG2("blit region: MinX %ld MaxX %ld\n", (ULONG) region->bounds.MinX, (ULONG) region->bounds.MaxX);
                                        DEBUG2("blit region: MinY %ld MaxY %ld\n", (ULONG) region->bounds.MinY, (ULONG) region->bounds.MaxY);
                                
                                        if(region->RegionRectangle)
                                        {
                                                DEBUG2("blit region: RegionRectangle MinX %ld MaxX %ld\n", region->RegionRectangle->bounds.MinX, region->RegionRectangle->bounds.MaxX);
                                                DEBUG2("blit region: RegionRectangle MinY %ld MaxY %ld\n", region->RegionRectangle->bounds.MinY, region->RegionRectangle->bounds.MaxY);
                                        }
                                        
                                        if(region->RegionRectangle->Next != NULL) DEBUG("Hm, shit! blit region rectangle is not simple.\n");

                                        // hier wird geblittet von left/top von erster schnittfläche mit width/height von blitfläche nach left/top von blitfläche

                                        if(((region->bounds.MaxX - region->bounds.MinX + 1) < EPS) || ((region->bounds.MaxY - region->bounds.MinY + 1) < EPS))
                                        {
                                                DisposeRegion(region);
                                                region = 0;

                                                SC_DoMethodA(child->child,(Msg) msg);
                                                
                                                DEBUG("Optimization skipped.\n");

                                                continue;
                                        }

                                        ClipBlit(scRenderInfo(obj)->window->RPort,              // srcrp
                                                         blitsrcx,                                      // srcx
                                                         blitsrcy,                                      // srcy
                                                         scRenderInfo(obj)->window->RPort,              // dstrp
                                                         region->bounds.MinX,                           // dstx
                                                         region->bounds.MinY,                           // dsty
                                                         region->bounds.MaxX - region->bounds.MinX + 1, // width
                                                         region->bounds.MaxY - region->bounds.MinY + 1, // height
                                                         ABNC | ABC);                                   // minterm
                                                 
                                        DEBUG4("blit: srcx %ld srcy %ld width %ld height %ld\n", region->bounds.MinX, region->bounds.MinY, region->bounds.MaxX - region->bounds.MinX + 1, region->bounds.MaxY - region->bounds.MinY + 1);
                                        DEBUG2("blit: dstx %ld dsty %ld\n", child->bounds.Left, child->bounds.Top);

                                        // blitfläche XOR newbounds rectangle = update fläche

                                        rect1.MinX = child->bounds.Left;
                                        rect1.MaxX = child->bounds.Left + child->bounds.Width - 1;
                                        rect1.MinY = child->bounds.Top;
                                        rect1.MaxY = child->bounds.Top + child->bounds.Height - 1;
                                        
                                        XorRectRegion( region, &rect1 );
                                
                                        // die clip region kann nicht das blit rectangle enthalten
                                        // die clip region ist der teil des newbounds rectangle der nicht im blit rectangle liegt
                                        
                                        // clip region fertig
                                        
                                        DEBUG2("clip region: MinX %ld MaxX %ld\n", region->bounds.MinX, region->bounds.MaxX);
                                        DEBUG2("clip region: MinY %ld MaxY %ld\n", region->bounds.MinY, region->bounds.MaxY);
                        
                                        if(region->RegionRectangle)
                                        {
                                                DEBUG2("clip region: RegionRectangle MinX %ld MaxX %ld\n", region->RegionRectangle->bounds.MinX, region->RegionRectangle->bounds.MaxX);
                                                DEBUG2("clip region: RegionRectangle MinY %ld MaxY %ld\n", region->RegionRectangle->bounds.MinY, region->RegionRectangle->bounds.MaxY);
                                        }
                                        
                                        if(region->RegionRectangle->Next != NULL) DEBUG("Hm, shit! clip region rectangle is not simple.\n");


                                        if(scRenderInfo(obj)->window->Flags & WFLG_SIMPLE_REFRESH)
                                        {
                                                // (newregion OR oldbounds_region AND layer->DamageList) verschoben OR clip region
                                                
//                      LONG diffX, diffY;
                                                struct Region *region1;
                                                
                                                DEBUG("Damage!\n");

                                                // schnittfläche
                                                
                                                rect1.MinX = child->oldbounds.Left;
                                                rect1.MaxX = child->oldbounds.Left + child->oldbounds.Width - 1;
                                                rect1.MinY = child->oldbounds.Top;
                                                rect1.MaxY = child->oldbounds.Top + child->oldbounds.Height - 1;

                                                if(region1 = NewRegion())
                                                {
                                                        OrRectRegion( region1, &rect1 );

                                                        AndRegionRegion( scRenderInfo(obj)->window->RPort->Layer->DamageList, region1 );
                                                
                                                        // verschiebung
                                        
//                          diffX = child->bounds.Left - child->oldbounds.Left;
//                          diffY = child->bounds.Top - child->oldbounds.Top;
                                                
                                                        region1->bounds.MinX += diffX;
                                                        region1->bounds.MaxX += diffX;
                                                        region1->bounds.MinY += diffY;
                                                        region1->bounds.MaxY += diffY;
                                        
                                                        if(region1->RegionRectangle->Next != NULL) DEBUG("Hm! damage rectangle is not simple.\n");

                                                        OrRegionRegion( region1, region );
                                                        
                                                        DisposeRegion(region1);
                                                }
                                        }


                                        SC_DoMethod(_scwinobj(obj),SCCM_Window_SetClipRegion,region);

                                        // draw

                                        SC_DoMethodA(child->child,(Msg) msg);
                                
                                        SC_DoMethod(_scwinobj(obj),SCCM_Window_ClearClipRegion);


                                        DisposeRegion(region);
                                        region = 0;
                                
                                        continue;
                                }
                        }

                }
*/

                SC_DoMethodA(child->child,(Msg) msg);
        }

        // oldbounds aller childs updaten für aufeinander folgende Group_Draw
                                
        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                child->oldbounds.Left   = child->bounds.Left;
                child->oldbounds.Top    = child->bounds.Top;
                child->oldbounds.Height = child->bounds.Height;
                child->oldbounds.Width  = child->bounds.Width;
        }

        DEBUG("Draw finished\n");
        
        inst->action = SCCV_Area_Action_OpenClose;
}
// /

/** Setup
*/
static ULONG Group_Setup(struct SC_Class *cl, Object *obj, struct SCCP_Area_Setup *msg, struct GroupInst *inst)
{
        struct ChildEntry *child;

        if (SC_DoSuperMethodA(cl, obj, (Msg) msg))
        {
                struct MinList *list;

                DEBUG("Got Setup\n");

                if (list = (struct MinList *) SC_DoMethod(obj,SCCM_LockObjectList,SCCV_LockShared))
                {
                        struct MinNode *node;

                        for (node = list->mlh_Head; node->mln_Succ; node = node->mln_Succ)
                        {
                                // alloc mem and add this mem as a node to the childlist

                                child = (struct ChildEntry *) AllocNode(&inst->childlist,sizeof(struct ChildEntry));

                                child->child = SCBASEOBJECT(node);

                                inst->NumChilds++;
                        }
                        SC_DoMethod(obj,SCCM_UnlockObjectList);
                }

                /*
                 * Send the method to all children
                 */

                if (!(inst->NumChilds))
                {
                        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Area_Cleanup);
                        return(FALSE);
                }

                for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
                {
                        SC_DoMethodA(child->child, (Msg) msg);
                }

                DEBUG("Setup finished\n");
                return(TRUE);
        }
        return(FALSE);
}
// /

/** Cleanup
*/
static void Group_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct GroupInst *inst)
{
        struct ChildEntry *child;
        ULONG msgcntr = 0;

        DEBUG("Got Setup\n");

        /*
         * Send the method to all children
         */

        if(!(inst->childsreplyport = CreateMsgPort()))
        {
                if(DisplayAlert(RECOVERY_ALERT, alerttext_Cleanup, 45))
                {
                        return;
                }
                else
                {
                        ColdReboot();
                }
        }

        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                if (SC_DoMsgMethodA(inst->childsreplyport, child->child, msg))
                        msgcntr++;
                else
                        SC_DoMethodA(child->child, msg);
        }

        for(;msgcntr;msgcntr--)
        {
                struct Message *msg;
        
                while (!(msg = GetMsg(inst->childsreplyport)))
                WaitPort(inst->childsreplyport);
        
                SC_FreeMsg((struct SC_Message *)msg);
        }

        if (inst->childsreplyport)
        {
                DeleteMsgPort(inst->childsreplyport);
                inst->childsreplyport = 0;
        }


        SC_DoSuperMethodA(cl, obj, (Msg) msg); // at the end

        DEBUG("Cleanup finished\n");
}
// /

/** BeginDraw
*/
static void Group_BeginDraw(struct SC_Class *cl, Object *obj, Msg msg, struct GroupInst *inst)
{
        struct ChildEntry *child;

        DEBUG("Got BeginDraw\n");

        SC_DoSuperMethodA(cl, obj, msg); // at the begining

        /*
         * Send the method to all children
         */
        
        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                SC_DoMethodA(child->child, msg);
        }

        DEBUG("BeginDraw finished\n");
}
// /

/** EndDraw
*/
static void Group_EndDraw(struct SC_Class *cl, Object *obj, Msg msg, struct GroupInst *inst)
{
        struct ChildEntry *child;

        DEBUG("Got EndDraw\n");

        /*
         * Send the method to all children
         */

        for (child = (struct ChildEntry *)inst->childlist.mlh_Head; child->node.mln_Succ; child = (struct ChildEntry *)child->node.mln_Succ)
        {
                SC_DoMethodA(child->child, msg);
        }

        SC_DoSuperMethodA(cl, obj, msg); // at the end

        DEBUG("EndDraw finished\n");
}
// /

/** Exit
*/
static void Group_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct GroupInst *inst)
{
        DEBUG("Got Exit\n");

        if (!IsMinListEmpty(&inst->childlist))
        {
                struct ChildEntry *child;
                struct ChildEntry *next_child;

                child=(struct ChildEntry *)(inst->childlist.mlh_Head);

                while (next_child=(struct ChildEntry *)(child->node.mln_Succ))
                {
                        FreeVec(child);
                        child=next_child;
                }
        }

        SC_DoSuperMethodA(cl, obj, msg);

        DEBUG("Exit finished\n");
}
// /

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData GroupMethods[] =
{
        { SCCM_Init,(ULONG)                     Group_Init,         0, 0, NULL },               // derived from RootClass
        { SCCM_Area_Setup,(ULONG)               Group_Setup,        0, 0, NULL },               // derived from AreaClass
        { SCCM_Area_Cleanup,(ULONG)             Group_Cleanup,      0, 0, NULL },               // derived from AreaClass
        { SCCM_Area_Draw,(ULONG)                Group_Draw,         0, 0, NULL },               // derived from AreaClass
        { SCCM_Area_AskMinMax,(ULONG)           Group_AskMinMax,    0, 0, NULL },               // derived from AreaClass
        { SCCM_Area_Show,(ULONG)                Group_Show,         0, 0, NULL },               // derived from AreaClass
        { SCCM_Area_Hide,(ULONG)                Group_Hide,         0, 0, NULL },               // derived from AreaClass
        { SCCM_Area_BeginDraw,(ULONG)           Group_BeginDraw,    0, 0, NULL },
        { SCCM_Area_EndDraw,(ULONG)             Group_EndDraw,      0, 0, NULL },
        { SCCM_Exit,(ULONG)                     Group_Exit,         0, 0, NULL },               // derived from RootClass
        { SCMETHOD_DONE,                        NULL,               0, 0, NULL }
};

/*
ich muss in SCCM_Area_Show checken, für ein child, daß SCCA_Area_SizeableAndNotScaleable ist,
wie die letzte und neue position ist. clip region ausrechnen für refresh bereich brauch ich nicht.

SCCA_Area_Draw:

drawtype == SCCV_Area_Draw_Update:

                                        ScrollWindowRaster(scRenderInfo(obj)->window, diffx, diffy,
                                                                           _scleft(obj), _sctop(obj), _scright(obj), _scbottom(obj));
weil ich über mich selbst blitten muss was nicht geht (verschiebung kleiner als objekt größe)
die reihenfolge richtig machen

dann das kleinere von beiden als blit size nehmen und blitten in SCCA_Area_Draw
d.h. in SCCM_Area_Show für SCCA_Area_NotScaleable objekte nicht SCCM_Area_Show
nicht weiterschicken
*/

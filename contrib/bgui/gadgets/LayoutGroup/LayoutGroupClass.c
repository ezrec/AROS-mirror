/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.8  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.7  2003/01/18 19:10:16  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.6  2000/08/11 08:09:14  chodorowski
 * Removed METHOD #define, already defined in bgui_compilerspecific.h.
 *
 * Revision 42.5  2000/08/09 10:17:24  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.4  2000/08/08 13:51:34  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/07/11 17:06:02  stegerg
 * struct Library *BGUIBase not struct BGUIBase *BGUIBase
 *
 * Revision 42.2  2000/07/09 03:05:08  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:07  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:20:52  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:41  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:44  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:31  mlemos
 * Initial revision.
 *
 *
 */

#ifdef __AROS__
extern struct Library * BGUIBase;
#endif

#ifdef DEBUG_GROUP_CLASS
void kprintf(char *format,...);
#define bug kprintf
#define D(x) (x)
#else
#define D(x) ;
#endif

#define DONT_FAKE_DIMENSIONS

#include <string.h>
#include <clib/alib_protos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/bgui.h>

#include <bgui/bgui_layoutgroup.h>

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

#include "include/olLayout.h"

struct oBGUIGroupClassData
{
	Class *NodeClass;
	struct Library *BGUILibraryBase;
};

typedef struct
{
	olLayoutDefinition LayoutDefinition;
	struct ElementData
	{
		Object *Object;
		olLayoutElementDefinition Element;
	}
	*Elements;
	BOOL GotConstraints;
	ULONG FrontPage;
}
GD;

static ULONG MultipleGet(Object *obj,ULONG attribute,...)
{
	struct ogpMGet mget;

	mget.MethodID=LGM_MGET;
	mget.ogpg_AttrList= (struct TagItem *)&attribute;
	return(DoMethodA(obj,(Msg)&mget));
}

METHOD(GroupClassLayout,struct bmLayout *,bml)
{
	GD *gd=INST_DATA(cl,obj);
	unsigned long member;
	struct ElementData *element;
#ifdef DEBUG_GROUP_CLASS
	Object **members;
#endif
	struct IBox *bounds;

	bml->bml_Flags|=BLF_CUSTOM_GROUP;
  if(DoSuperMethodA(cl, obj, (Msg)bml)==0)
	return(0);
	if(gd->GotConstraints==FALSE)
		return(FALSE);
	if(GetAttr(BT_InnerBox,obj,(ULONG *)&bounds)==FALSE)
		return(FALSE);
#ifdef DEBUG_GROUP_CLASS
	{
		UWORD member_count;

		if((members=(Object **)DoMethod(obj,GROUPM_OBTAINMEMBERS,GROMF_WAIT,&member_count))==NULL)
			return(FALSE);
	}
#endif
	gd->LayoutDefinition.Width=bounds->Width;
	gd->LayoutDefinition.Height=bounds->Height;
	olLayout(&gd->LayoutDefinition);
	for(element=gd->Elements,member=0;member<gd->LayoutDefinition.ElementCount;element++,member++)
	{
		if(gd->LayoutDefinition.LayoutType==olLayoutPaged
		&& gd->FrontPage!=member)
			continue;
		olLayoutElement(&element->Element);
		SetAttrs(element->Object,
			LGO_Left,element->Element.X+bounds->Left,
			LGO_Top,element->Element.Y+bounds->Top,
			LGO_Width,element->Element.Width,
			LGO_Height,element->Element.Height,
			TAG_DONE);
	}
#ifdef DEBUG_GROUP_CLASS
	DoMethod(obj,GROUPM_RELEASEMEMBERS,members);
#endif
	return(TRUE);
}
METHOD_END

METHOD(GroupClassDimensions,struct bmDimensions *,bmd)
{
	GD *gd=INST_DATA(cl,obj);
	ULONG member_count;

	bmd->bmd_Flags|=BDF_CUSTOM_GROUP;
	if(DoSuperMethodA(cl,obj,(Msg)bmd)==0)
		return(0);
	gd->GotConstraints=FALSE;
	if(GetAttr(GROUP_NumMembers,obj,&member_count)==FALSE)
		return(FALSE);
	if(gd->LayoutDefinition.LayoutType==olLayoutPaged
	&& gd->FrontPage>=member_count)
		gd->FrontPage=0;
	if(member_count)
	{
		Object **members;

		if(GetAttr(GROUP_Members,obj,(ULONG *)&members)==FALSE)
			return(FALSE);
		if(gd->LayoutDefinition.Elements)
		{
			if(gd->LayoutDefinition.ElementCount<member_count)
			{
				olLayoutElementDefinition **elements;

				if((elements=oBGUIPoolReallocate(gd->LayoutDefinition.AllocationContext,gd->LayoutDefinition.Elements,sizeof(*gd->LayoutDefinition.Elements)*member_count))==NULL)
					return(FALSE);
				gd->LayoutDefinition.Elements=elements;
			}
		}
		else
		{
			if((gd->LayoutDefinition.Elements=oBGUIPoolAllocate(gd->LayoutDefinition.AllocationContext,sizeof(*gd->LayoutDefinition.Elements)*member_count))==NULL)
				return(FALSE);
		}
		if(gd->Elements)
		{
			if(gd->LayoutDefinition.ElementCount<member_count)
			{
				struct ElementData *elements;

				if((elements=oBGUIPoolReallocate(gd->LayoutDefinition.AllocationContext,gd->Elements,sizeof(*gd->Elements)*member_count))==NULL)
					return(FALSE);
				gd->Elements=elements;
			}
		}
		else
		{
			if((gd->Elements=oBGUIPoolAllocate(gd->LayoutDefinition.AllocationContext,sizeof(*gd->Elements)*member_count))==NULL)
				return(FALSE);
		}
		{
			struct ElementData *element;

			for(element=gd->Elements,gd->LayoutDefinition.ElementCount=0;gd->LayoutDefinition.ElementCount<member_count;element++,gd->LayoutDefinition.ElementCount++)
			{
				{
					ULONG
						horizontal_weight,
						vertical_weight,
						width_weight,
						height_weight,
						left_weight,
						top_weight,
						right_weight,
						bottom_weight;

					gd->LayoutDefinition.Elements[gd->LayoutDefinition.ElementCount]= &element->Element;
					element->Object=members[gd->LayoutDefinition.ElementCount];
					if(MultipleGet(element->Object,
						LGNA_HorizontalWeight,&horizontal_weight,
						LGNA_VerticalWeight,&vertical_weight,
						LGNA_WidthWeight,&width_weight,
						LGNA_HeightWeight,&height_weight,
						LGNA_LeftWeight,&left_weight,
						LGNA_TopWeight,&top_weight,
						LGNA_RightWeight,&right_weight,
						LGNA_BottomWeight,&bottom_weight,
						LGNA_LeftOffset,&element->Element.LeftOffset,
						LGNA_RightOffset,&element->Element.RightOffset,
						LGNA_TopOffset,&element->Element.TopOffset,
						LGNA_BottomOffset,&element->Element.BottomOffset,
						TAG_END)==0)
						return(FALSE);
					element->Element.HorizontalWeight=horizontal_weight;
					element->Element.VerticalWeight=vertical_weight;
					element->Element.WidthWeight=width_weight;
					element->Element.HeightWeight=height_weight;
					element->Element.LeftWeight=left_weight;
					element->Element.TopWeight=top_weight;
					element->Element.RightWeight=right_weight;
					element->Element.BottomWeight=bottom_weight;
				}
				if(gd->LayoutDefinition.LayoutType==olLayoutTable)
				{
					if(MultipleGet(element->Object,
						LGNA_Row,&element->Element.Row,
						LGNA_Column,&element->Element.Column,
						LGNA_RowSpan,&element->Element.RowSpan,
						LGNA_ColumnSpan,&element->Element.ColumnSpan,
						TAG_END)==0)
						return(FALSE);
				}
#ifdef FAKE_DIMENSIONS
				element->Element.MinimumWidth=
				element->Element.MinimumHeight=
				element->Element.NominalWidth=
				element->Element.NominalHeight=1;
				element->Element.MaximumWidth=
				element->Element.MaximumHeight=0xFFFF;
#else
				{
					ULONG attribute;

					if(GetAttr(LGO_MinWidth,element->Object,&attribute)==FALSE)
						return(FALSE);
					element->Element.MinimumWidth=attribute;
					if(GetAttr(LGO_MinHeight,element->Object,&attribute)==FALSE)
						return(FALSE);
					element->Element.MinimumHeight=attribute;
					if(GetAttr(LGO_NomWidth,element->Object,&attribute)==FALSE)
						return(FALSE);
					element->Element.NominalWidth=attribute;
					if(GetAttr(LGO_NomHeight,element->Object,&attribute)==FALSE)
						return(FALSE);
					element->Element.NominalHeight=attribute;
					if(GetAttr(LGO_MaxWidth,element->Object,&attribute)==FALSE)
						return(FALSE);
					element->Element.MaximumWidth=attribute;
					if(GetAttr(LGO_MaxHeight,element->Object,&attribute)==FALSE)
						return(FALSE);
					element->Element.MaximumHeight=attribute;
				}
#endif
				if(gd->LayoutDefinition.LayoutType==olLayoutPaged)
				{
					Object *member;

					if(GetAttr(LGO_Object,element->Object,(ULONG *)&member)==FALSE
					|| DoMethod(member,BASE_INHIBIT,gd->FrontPage!=gd->LayoutDefinition.ElementCount)==FALSE)
						return(FALSE);
					D(bug("Member %lx, Count %lu, File %s, Line %lu\n",member,member_count,__FILE__,__LINE__));
				}
			}
			{
				ULONG attribute;

				if(GetAttr(BT_LeftOffset,obj,&attribute)==FALSE)
					return(FALSE);
				gd->LayoutDefinition.LeftOffset=attribute;
				if(GetAttr(BT_RightOffset,obj,&attribute)==FALSE)
					return(FALSE);
				gd->LayoutDefinition.RightOffset=attribute;
				if(GetAttr(BT_TopOffset,obj,&attribute)==FALSE)
					return(FALSE);
				gd->LayoutDefinition.TopOffset=attribute;
				if(GetAttr(BT_BottomOffset,obj,&attribute)==FALSE)
					return(FALSE);
				gd->LayoutDefinition.BottomOffset=attribute;
			}
		}
		if(olComputeMinimumSize(&gd->LayoutDefinition)==olFalse
		|| gd->LayoutDefinition.MinimumWidth>0xFFFF
		|| gd->LayoutDefinition.MinimumHeight>0xFFFF)
			return(FALSE);
	}
	else
	{
		gd->LayoutDefinition.ElementCount=0;
		gd->LayoutDefinition.MinimumWidth=
		gd->LayoutDefinition.MinimumHeight=0;
	}
	bmd->bmd_Extent->be_Nom.Width=(bmd->bmd_Extent->be_Min.Width+=gd->LayoutDefinition.MinimumWidth);
	bmd->bmd_Extent->be_Nom.Height=(bmd->bmd_Extent->be_Min.Height+=gd->LayoutDefinition.MinimumHeight);
	bmd->bmd_Extent->be_Max.Width=
	bmd->bmd_Extent->be_Max.Height=0xFFFF;
	gd->GotConstraints=TRUE;
	return(TRUE);
}
METHOD_END

#define SET_FAIL    0
#define SET_SUCCESS 1
#define SET_UNKNOWN 2

static ULONG GroupSet(Class *cl,Object *obj,struct opSet *ops)
{
	GD *gd;
	struct TagItem *tag,*tstate;
	ULONG success;
	BOOL refresh;

	gd=INST_DATA(cl,obj);
	for(refresh=FALSE,tstate=ops->ops_AttrList,success=SET_UNKNOWN;(tag=NextTagItem(&tstate));)
	{
		switch(tag->ti_Tag)
		{
			case LGA_LayoutType:
				if(ops->MethodID!=OM_NEW)
					return(SET_FAIL);
				switch(tag->ti_Data)
				{
					case LGT_HORIZONTAL:
						gd->LayoutDefinition.LayoutType=olLayoutHorizontal;
						break;
					case LGT_VERTICAL:
						gd->LayoutDefinition.LayoutType=olLayoutVertical;
						break;
					case LGT_PAGED:
						gd->LayoutDefinition.LayoutType=olLayoutPaged;
						break;
					case LGT_TABLE:
						gd->LayoutDefinition.LayoutType=olLayoutTable;
						break;
					default:
						return(SET_FAIL);
				}
				break;
			case LGA_FrontPage:
				if(gd->FrontPage!=tag->ti_Data)
				{
					if(ops->MethodID!=OM_NEW)
					{
						UWORD member_count;
						Object **members;

						if((members=(Object **)DoMethod(obj,GROUPM_OBTAINMEMBERS,GROMF_WAIT|GROMF_VISIBLE,&member_count)))
						{
							BOOL paged=TRUE;
							Object *member;
							
							D(bug("Member count %lu, Current %lu, New %lu\n",member_count,gd->FrontPage,tag->ti_Data));
							if(member_count<=tag->ti_Data)
								paged=FALSE;
							else
							{
								if(gd->FrontPage<member_count)
								{
									if(GetAttr(LGO_Object,members[gd->FrontPage],(ULONG *)&member)==FALSE)
										paged=FALSE;
									else
									{
										D(bug("Member %lx, File %s, Line %lu\n",member,__FILE__,__LINE__));
										if(DoMethod(member,BASE_INHIBIT,TRUE)==FALSE)
											paged=FALSE;
									}
								}
								if(paged)
								{
									if(GetAttr(LGO_Object,members[tag->ti_Data],(ULONG *)&member)==FALSE)
										paged=FALSE;
									else
									{
										D(bug("Member %lx, File %s, Line %lu\n",member,__FILE__,__LINE__));
										if(DoMethod(member,BASE_INHIBIT,FALSE)==FALSE)
											paged=FALSE;
									}
								}
							}
							DoMethod(obj,GROUPM_RELEASEMEMBERS,members);
							if(paged==FALSE)
							{
								D(bug("File %s, Line %lu\n",__FILE__,__LINE__));
								return(SET_FAIL);
							}
							refresh=TRUE;
						}
						else
							return(SET_FAIL);
					}
					gd->FrontPage=tag->ti_Data;
				}
				break;
			default:
				continue;
		}
		success=SET_SUCCESS;
		if(ops->MethodID!=OM_NEW
		&& refresh)
		{
			struct RastPort *rp;

			if((rp=ObtainGIRPort(ops->ops_GInfo)))
			{
				DoMethod(obj,GM_RENDER,ops->ops_GInfo,rp,GREDRAW_REDRAW);
				ReleaseGIRPort(rp);
			}
		}
	}
	return(success);
}

METHOD(GroupClassNew,struct opSet *,ops)
{
	if((obj=(Object *)DoSuperMethodA(cl,obj,(Msg)ops)))
	{
		GD *gd;

		gd=INST_DATA(cl,obj);
		olInitializeLayout(&gd->LayoutDefinition);
		gd->LayoutDefinition.Elements=NULL;
		gd->LayoutDefinition.ElementCount=0;
		gd->LayoutDefinition.LeftOffset=
		gd->LayoutDefinition.RightOffset=
		gd->LayoutDefinition.TopOffset=
		gd->LayoutDefinition.BottomOffset=2;
		gd->LayoutDefinition.LayoutType=olLayoutHorizontal;
		gd->Elements=NULL;
		gd->GotConstraints=FALSE;
		gd->FrontPage=(ULONG)~0;
		if((gd->LayoutDefinition.AllocationContext=oBGUICreatePool())==NULL
		|| GroupSet(cl,obj,ops)==SET_FAIL)
		{
			CoerceMethod(cl,obj,OM_DISPOSE);
			obj=NULL;
		}
	}
	return((ULONG)obj);
}
METHOD_END

METHOD(GroupClassDispose,Msg,msg)
{
	GD *gd=INST_DATA(cl,obj);

	olDisposeLayout(&gd->LayoutDefinition);
	if(gd->LayoutDefinition.Elements)
	{
		oBGUIPoolFree(gd->LayoutDefinition.AllocationContext,gd->LayoutDefinition.Elements);
		gd->LayoutDefinition.Elements=NULL;
	}
	if(gd->Elements)
	{
		oBGUIPoolFree(gd->LayoutDefinition.AllocationContext,gd->Elements);
		gd->Elements=NULL;
	}
	if(gd->LayoutDefinition.AllocationContext)
	{
		oBGUIDestroyPool(gd->LayoutDefinition.AllocationContext);
		gd->LayoutDefinition.AllocationContext=NULL;
	}
	return(DoSuperMethodA(cl,obj,msg));
}
METHOD_END

METHOD(GroupClassSet,struct opSet *,ops)
{
	BOOL success;

	success=DoSuperMethodA(cl,obj,(Msg)ops);
	switch(GroupSet(cl,obj,ops))
	{
		case SET_FAIL:
			success=FALSE;
			break;
		case SET_SUCCESS:
			success=TRUE;
			break;
	}
	return(success);
}
METHOD_END

METHOD(GroupClassGet,struct opGet *,opg)
{
	return(DoSuperMethodA(cl,obj,(Msg)opg));
}
METHOD_END

METHOD(GroupClassNewMember,struct opSet *,ops)
{
	return((ULONG)NewObjectA(((struct oBGUIGroupClassData *)cl->cl_UserData)->NodeClass,NULL,ops->ops_AttrList));
}
METHOD_END

METHOD(GroupClassAll, Msg,msg)
{
   ULONG rc;

   rc=DoSuperMethodA(cl, obj, msg);
   D(bug("BASE_INHIBIT %lx -> %lu\n",obj,rc));
   return rc;
}
METHOD_END

static DPFUNC ClassFunctions[] = {
	OM_NEW,          (FUNCPTR)GroupClassNew,
	OM_DISPOSE,      (FUNCPTR)GroupClassDispose,
	OM_SET,          (FUNCPTR)GroupClassSet,
	OM_GET,          (FUNCPTR)GroupClassGet,
	BASE_LAYOUT,     (FUNCPTR)GroupClassLayout,
	BASE_DIMENSIONS, (FUNCPTR)GroupClassDimensions,
	GROUPM_NEWMEMBER,(FUNCPTR)GroupClassNewMember,
	BASE_INHIBIT,    (FUNCPTR)GroupClassAll,
	DF_END,          NULL
};

typedef struct
{
	ULONG Row;
	ULONG Column;
	ULONG RowSpan;
	ULONG ColumnSpan;
	ULONG HorizontalWeight;
	ULONG VerticalWeight;
	ULONG LeftWeight;
	ULONG TopWeight;
	ULONG WidthWeight;
	ULONG HeightWeight;
	ULONG RightWeight;
	ULONG BottomWeight;
	ULONG LeftOffset;
	ULONG TopOffset;
	ULONG RightOffset;
	ULONG BottomOffset;
	ULONG LeftSpacing;
	ULONG TopSpacing;
	ULONG RightSpacing;
	ULONG BottomSpacing;
}
MD;

static BOOL SetGroupNodeAttributes(MD *md,struct opSet *ops)
{
	BOOL success;
	struct TagItem *tag,*tstate;

	for(tstate=ops->ops_AttrList,success=TRUE;success==TRUE && (tag=NextTagItem(&tstate));)
	{
		switch(tag->ti_Tag)
		{
			case LGNA_Row:
				md->Row=tag->ti_Data;
				break;
			case LGNA_Column:
				md->Column=tag->ti_Data;
				break;
			case LGNA_RowSpan:
				if(tag->ti_Data>0)
					md->RowSpan=tag->ti_Data;
				else
					success=FALSE;
				break;
			case LGNA_ColumnSpan:
				if(tag->ti_Data>0)
					md->ColumnSpan=tag->ti_Data;
				else
					success=FALSE;
				break;
			case LGNA_HorizontalWeight:
				md->HorizontalWeight=tag->ti_Data;
				break;
			case LGNA_VerticalWeight:
				md->VerticalWeight=tag->ti_Data;
				break;
			case LGNA_LeftWeight:
				md->LeftWeight=tag->ti_Data;
				break;
			case LGNA_TopWeight:
				md->TopWeight=tag->ti_Data;
				break;
			case LGNA_WidthWeight:
				md->WidthWeight=tag->ti_Data;
				break;
			case LGNA_HeightWeight:
				md->HeightWeight=tag->ti_Data;
				break;
			case LGNA_RightWeight:
				md->RightWeight=tag->ti_Data;
				break;
			case LGNA_BottomWeight:
				md->BottomWeight=tag->ti_Data;
				break;
			case LGNA_LeftOffset:
				md->LeftOffset=tag->ti_Data;
				break;
			case LGNA_TopOffset:
				md->TopOffset=tag->ti_Data;
				break;
			case LGNA_RightOffset:
				md->RightOffset=tag->ti_Data;
				break;
			case LGNA_BottomOffset:
				md->BottomOffset=tag->ti_Data;
				break;
			case LGNA_LeftSpacing:
				md->LeftSpacing=tag->ti_Data;
				break;
			case LGNA_TopSpacing:
				md->TopSpacing=tag->ti_Data;
				break;
			case LGNA_RightSpacing:
				md->RightSpacing=tag->ti_Data;
				break;
			case LGNA_BottomSpacing:
				md->BottomSpacing=tag->ti_Data;
				break;
		}
	}
	return(success);
}

METHOD(GroupNodeClassNew,struct opSet *,ops)
{
	if((obj=(Object *)DoSuperMethodA(cl,obj,(Msg)ops)))
	{
		MD *md;

		md=INST_DATA(cl,obj);
		md->Row=
		md->Column=
		md->RowSpan=
		md->ColumnSpan=1;
		md->HorizontalWeight=
		md->VerticalWeight=
		md->LeftWeight=
		md->TopWeight=
		md->WidthWeight=
		md->HeightWeight=
		md->RightWeight=
		md->BottomWeight=1;
		md->LeftOffset=
		md->TopOffset=
		md->RightOffset=
		md->BottomOffset=2;
		md->LeftSpacing=
		md->TopSpacing=
		md->RightSpacing=
		md->BottomSpacing=1;
		if(!SetGroupNodeAttributes(md,ops))
		{
			CoerceMethod(cl,obj,OM_DISPOSE);
			obj=NULL;
		}
	}
	return((ULONG)obj);
}
METHOD_END

static GetAttribute(MD *md,ULONG attribute,ULONG *store)
{
	switch(attribute)
	{
		case LGNA_Row:
			*store=md->Row;
			break;
		case LGNA_Column:
			*store=md->Column;
			break;
		case LGNA_RowSpan:
			*store=md->RowSpan;
			break;
		case LGNA_ColumnSpan:
			*store=md->ColumnSpan;
			break;
		case LGNA_HorizontalWeight:
			*store=md->HorizontalWeight;
			break;
		case LGNA_VerticalWeight:
			*store=md->VerticalWeight;
			break;
		case LGNA_LeftWeight:
			*store=md->LeftWeight;
			break;
		case LGNA_TopWeight:
			*store=md->TopWeight;
			break;
		case LGNA_WidthWeight:
			*store=md->WidthWeight;
			break;
		case LGNA_HeightWeight:
			*store=md->HeightWeight;
			break;
		case LGNA_RightWeight:
			*store=md->RightWeight;
			break;
		case LGNA_BottomWeight:
			*store=md->BottomWeight;
			break;
		case LGNA_LeftOffset:
			switch(md->LeftSpacing)
			{
				case 0:
					*store=0;
					break;
				case 1:
					*store=md->LeftOffset;
					break;
				case 2:
					*store=md->LeftOffset*2;
					break;
				default:
					*store=md->LeftOffset*md->LeftSpacing;
					break;
			}
			break;
		case LGNA_TopOffset:
			switch(md->TopSpacing)
			{
				case 0:
					*store=0;
					break;
				case 1:
					*store=md->TopOffset;
					break;
				case 2:
					*store=md->TopOffset*2;
					break;
				default:
					*store=md->TopOffset*md->TopSpacing;
					break;
			}
			break;
		case LGNA_RightOffset:
			switch(md->RightSpacing)
			{
				case 0:
					*store=0;
					break;
				case 1:
					*store=md->RightOffset;
					break;
				case 2:
					*store=md->RightOffset*2;
					break;
				default:
					*store=md->RightOffset*md->RightSpacing;
					break;
			}
			break;
		case LGNA_BottomOffset:
			switch(md->BottomSpacing)
			{
				case 0:
					*store=0;
					break;
				case 1:
					*store=md->BottomOffset;
					break;
				case 2:
					*store=md->BottomOffset*2;
					break;
				default:
					*store=md->BottomOffset*md->BottomSpacing;
					break;
			}
			break;
		case LGNA_LeftSpacing:
			*store=md->LeftSpacing;
			break;
		case LGNA_TopSpacing:
			*store=md->TopSpacing;
			break;
		case LGNA_RightSpacing:
			*store=md->RightSpacing;
			break;
		case LGNA_BottomSpacing:
			*store=md->BottomSpacing;
			break;
		default:
			return(FALSE);
	}
	return(TRUE);
}

METHOD(GroupNodeClassGet,struct opGet *,opg)
{
	return(GetAttribute(INST_DATA(cl,obj),opg->opg_AttrID,opg->opg_Storage) ? 1 : DoSuperMethodA(cl,obj,(Msg)opg));
}
METHOD_END

METHOD(GroupNodeClassMultipleGet,struct ogpMGet *,ogp)
{
	MD *md=INST_DATA(cl,obj);
	ULONG got;
	struct TagItem *tag,*tstate;

	for(got=0,tstate=ogp->ogpg_AttrList;(tag=NextTagItem(&tstate));got++)
	{
		if(!GetAttribute(md,tag->ti_Tag,(ULONG *)tag->ti_Data)
		&& !GetAttr(tag->ti_Tag,obj,(ULONG *)tag->ti_Data))
				return(0);
	}
	return(got);
}
METHOD_END

static DPFUNC NodeClassFunctions[] = {
	OM_NEW, (FUNCPTR)GroupNodeClassNew,
	OM_GET, (FUNCPTR)GroupNodeClassGet,
	LGM_MGET, (FUNCPTR)GroupNodeClassMultipleGet,
	DF_END, NULL
};


void __exit(int errcode)
{
}

extern UBYTE _LibName[]   = "bgui_layoutgroup.gadget";
extern UBYTE _LibID[]     = "\0$VER: bgui_layoutgroup.gadget 41.10 (3.5.00) ©1996 Manuel Lemos ©2000 BGUI Developers Team";
extern UWORD _LibVersion  = 41;
extern UWORD _LibRevision = 10;

/*--------------------------------LIBARY CODE FOLLOWS-----------------------------------*/

/*
 * The following code is used for the shared library.
 */
static Class *ClassBase = NULL;

/*
 * Initialize the class.
 */
SAVEDS ASM Class *BGUI_ClassInit(void)
{
	if((ClassBase=BGUI_MakeClass(
		CLASS_SuperClassBGUI,BGUI_GROUP_GADGET,
		CLASS_ObjectSize,sizeof(GD),
		CLASS_ClassSize,sizeof(struct oBGUIGroupClassData),
		CLASS_DFTable,ClassFunctions,
		TAG_DONE)))
	{
		Class *node_class;

		if((node_class=BGUI_MakeClass(
			CLASS_SuperClassBGUI,BGUI_GROUP_NODE,
			CLASS_ObjectSize,sizeof(MD),
			CLASS_DFTable,NodeClassFunctions,
			TAG_DONE)))
		{
			((struct oBGUIGroupClassData *)ClassBase->cl_UserData)->BGUILibraryBase=BGUIBase;
			((struct oBGUIGroupClassData *)ClassBase->cl_UserData)->NodeClass=node_class;
			return(ClassBase);
		}
		BGUI_FreeClass(ClassBase);
	}
	return(NULL);
}

/*
 * Called each time the library is closed. It simply closes
 * the required libraries and frees the class.
 */
SAVEDS ASM BOOL BGUI_ClassFree(void)
{
	if(ClassBase)
	{
		if(((struct oBGUIGroupClassData *)ClassBase->cl_UserData)->NodeClass)
		{
			if(BGUI_FreeClass(((struct oBGUIGroupClassData *)ClassBase->cl_UserData)->NodeClass)==FALSE)
				return(FALSE);
			((struct oBGUIGroupClassData *)ClassBase->cl_UserData)->NodeClass=NULL;
		}
		return(BGUI_FreeClass(ClassBase));
	}
	return(FALSE);
}

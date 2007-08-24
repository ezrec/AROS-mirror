#include "Private.h"

struct FS_Group_Tree                            { int32 MouseX, MouseY; };
struct FS_Group_Forward                         { uint32 Method; APTR MsgStart; };

/*** Methods ***************************************************************/

///Group_TreeUp
F_METHODM(uint32,Group_TreeUp,FS_Group_Tree)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	FAreaNode *node;
	
	if (Msg -> MouseX >= _x && Msg -> MouseX <= _x2 &&
		 Msg -> MouseY >= _y && Msg -> MouseY <= _y2)
	{
		uint32 rc;
		
		if (LOD -> PageData)
		{
			node = LOD -> PageData -> ActiveNode;

			if (node)
			{
				rc = F_DoA(node -> Object,Method,Msg);

				if (rc)
				{
					return rc;
				}
			}
		}
		else
		{
			for _each
			{
				rc = F_DoA(node -> Object,Method,Msg);

				if (rc)
				{
					return rc;
				}
			}
		}
		return F_SUPERDO();
	}
	return 0;
}
//+
///Group_Forward
F_METHODM(uint32,Group_Forward,FS_Group_Forward)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	_sub_declare_all;

	for _each
	{
		if (FF_Area_Group & _sub_flags)
		{
			F_DoA(node -> Object,Method,Msg);
		}
		else
		{
			F_DoA(node -> Object,Msg -> Method,&Msg -> MsgStart);
		}
	}

	return F_DoA(Obj,Msg -> Method,&Msg -> MsgStart);
}
//+
///Group_Propagate
F_METHOD(void,Group_Propagate)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	_sub_declare_all;

	for _each
	{
		F_OBJDO(node -> Object);
	}
	F_SUPERDO();
}
//+


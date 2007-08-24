#include "Private.h"

///code_family_create_node
F_HOOKM(APTR,code_family_create_node,FS_Family_CreateNode)
{
   FAreaPublic *pub = (FAreaPublic *) F_Get(Msg -> Object,FA_Area_PublicData);

//   F_Log(0,"create_family_node() Member %s{%08lx}",_classname(Msg -> Object),Msg -> Object);

   if (pub)
   {
      FAreaNode *node = F_NewP(CUD -> NodesPool,sizeof (FAreaNode));

      if (node)
      {
         node -> AreaPublic = pub;

         return node;
      }
   }
   return NULL;
}
//+
///code_family_delete_node
F_HOOKM(void,code_family_delete_node,FS_Family_DeleteNode)
{
//   F_Log(0,"delete_family_node() Node 0x%08lx - Member %s{%08lx}",Msg -> Node,_classname(Msg -> Node -> Object),Msg -> Node -> Object);

   F_Dispose(Msg -> Node);
}
//+

/*** methods ***************************************************************/

///Group_AddMember
F_METHODM(int32,Group_AddMember,FS_AddMember)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_Log(0,"HEAD 0x%08lx - TAIL 0x%08lx - NEW %s{0x%08lx}",LOD -> ChildList.Head,LOD -> ChildList.Tail,_classname(Msg -> Object),Msg -> Object);

    if (F_OBJDO(LOD -> Family))
    {
/*
      if (!(FF_Area_CanShow & _flags))
      {
         F_Log(0,"HIDDEN_GROUP_ADD >> %s{%08lx}",_classname(Msg -> Object),Msg -> Object);

         F_Set(Msg -> Object,FA_Hidden,TRUE);
      }
*/
        if (_render)
        {
            if (F_Do(Msg -> Object,FM_Setup,_render))
            {
                if (!F_Get(Obj,FA_Hidden) && !F_Get(Msg -> Object,FA_Hidden))
                {
                   F_Do(Msg -> Object,FM_Show);
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}
//+
///Group_RemMember
F_METHODM(uint32,Group_RemMember,FS_RemMember)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
       F_Do(Msg -> Member,FM_Hide);
       F_Do(Msg -> Member,FM_Cleanup);
    }

    return F_OBJDO(LOD -> Family);
}
//+


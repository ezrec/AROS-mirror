LIBRARY 'feelin.fake',0,0,'' IS
         fe_CreatePoolA(D0,A0),
         fe_DeletePool(A0),
         fe_reserved(),
         fe_reserved(),
         fe_reserved(),
         fe_reserved(),
         fe_reserved(),
         fe_New(D0),
         fe_NewP(A0,D0),
         fe_Dispose(A1),
         fe_reserved(),
         fe_OPool(A0),
         fe_SPool(A0),
         fe_RPool(A0),
         fe_reserved(),

         fe_LinkTail(A0,A1),
         fe_LinkHead(A0,A1),
         fe_LinkInsert(A0,A1,A2),
         fe_LinkMove(A0,A1,A2),
         fe_LinkRemove(A0,A1),
         fe_LinkMember(A0,A1),
         fe_NextNode(A0),

         fe_StrFmtA(A0,A1,A2),
         fe_StrNewA(A0,A1,A2),
         fe_StrLen(A0),
         fe_StrCmp(A0,A1,D1),

         fe_LogA(D0,A0,A1),
         fe_AlertA(A0,A1,A2),

         fe_HashCreate(D0),
         fe_HashDelete(A0),
         fe_HashFind(A0,A1,D1,A2),
         fe_HashAddLink(A0,A1),
         fe_HashRemLink(A0,A1),
         fe_HashAdd(A0,A1,D0,A2),
         fe_HashRem(A0,A1,D0),

         fe_DynamicCreate(A0),
         fe_DynamicDelete(A0),
         fe_DynamicFindAttribute(A0,A1,A2),
         fe_DynamicFindMethod(A0,A1,A2),
         fe_DynamicFindID(A0),
         fe_DynamicResolveTable(A0),
         fe_DynamicAddAutoTable(A0),
         fe_DynamicRemAutoTable(A0),
         fe_DynamicNTI(A0,A1,A2),
         fe_DynamicFTI(D0,A0),
         fe_DynamicGTD(D0,A1),

         fe_FindClass(A0),
         fe_OpenClass(A0),
         fe_CloseClass(A0),
         fe_CreateClassA(A0,A1),
         fe_DeleteClass(A0),

         fe_DoA(A0,D0,A1),
         fe_ClassDoA(A2,A0,D0,A1),
         fe_SuperDoA(A2,A0,D0,A1),

         fe_NewObjA(A0,A1),
         fe_MakeObjA(D0,A0),
         fe_DisposeObj(A0),

         fe_Get(A0,D1),
         fe_Set(A0,D1,D2),

         fe_Draw(A0,D0),
         fe_Layout(A0,D0,D1,D2,D3,D4),
         fe_Erase(A0,D0,D1,D2,D3,D4),

         fe_SharedFind(A0),
         fe_SharedCreate(A0,A1),
         fe_SharedDelete(A0),
         fe_SharedOpen(A0),
         fe_SharedClose(A0)
         
PROC main() IS EMPTY

PROC fe_CreatePoolA(itemsize,tags) IS EMPTY
PROC fe_DeletePool(pool) IS EMPTY
PROC fe_NewP(pool,size) IS EMPTY
PROC fe_New(size) IS EMPTY
PROC fe_Dispose(mem) IS EMPTY
PROC fe_DisposeP(pool,mem) IS EMPTY
PROC fe_OPool(pool) IS EMPTY
PROC fe_SPool(pool) IS EMPTY
PROC fe_RPool(pool) IS EMPTY

PROC fe_LinkTail(list,node) IS EMPTY
PROC fe_LinkHead(list,node) IS EMPTY
PROC fe_LinkInsert(list,node,prev) IS EMPTY
PROC fe_LinkMove(list,node,prev) IS EMPTY
PROC fe_LinkRemove(list,node) IS EMPTY
PROC fe_LinkMember(list,node) IS EMPTY
PROC fe_NextNode(objptrptr) IS EMPTY

PROC fe_StrFmtA(buf,fmt,args) IS EMPTY
PROC fe_StrNewA(valptr,fmt,args) IS EMPTY
PROC fe_StrLen(str) IS EMPTY
PROC fe_StrCmp(str1,str2,len) IS EMPTY

PROC fe_LogA(level,format,args) IS EMPTY
PROC fe_AlertA(title,body,args) IS EMPTY

PROC fe_HashCreate(size) IS EMPTY
PROC fe_HashDelete(table) IS EMPTY
PROC fe_HashFind(table,key,keylength,hashptr) IS EMPTY
PROC fe_HashAddLink(table,link) IS EMPTY
PROC fe_HashRemLink(table,link) IS EMPTY
PROC fe_HashAdd(table,key,keylength,data) IS EMPTY
PROC fe_HashRem(table,key,keylength) IS EMPTY

PROC fe_DynamicCreate(class) IS EMPTY
PROC fe_DynamicDelete(class) IS EMPTY
PROC fe_DynamicFindAttribute(name,fromclass,realclass) IS EMPTY
PROC fe_DynamicFindMethod(name,fromclass,realclass) IS EMPTY
PROC fe_DynamicFindID(string)  IS EMPTY
PROC fe_DynamicResolveTable(table)  IS EMPTY
PROC fe_DynamicAddAutoTable(table) IS EMPTY
PROC fe_DynamicRemAutoTable(table) IS EMPTY
PROC fe_DynamicNTI(taglistptrptr,tagitemptrptr,class) IS EMPTY
PROC fe_DynamicFTI(attr,taglist) IS EMPTY
PROC fe_DynamicGTD(attr,taglist) IS EMPTY

PROC fe_FindClass(id) IS EMPTY
PROC fe_OpenClass(id) IS EMPTY
PROC fe_CloseClass(class) IS EMPTY
PROC fe_CreateClassA(name,tags) IS EMPTY
PROC fe_DeleteClass(class) IS EMPTY

PROC fe_DoA(obj,method,args) IS EMPTY
PROC fe_ClassDoA(class,obj,method,args) IS EMPTY
PROC fe_SuperDoA(class,obj,method,args) IS EMPTY

PROC fe_NewObjA(id,taglist) IS EMPTY
PROC fe_MakeObjA(type,params) IS EMPTY
PROC fe_DisposeObj(obj) IS EMPTY

PROC fe_Get(obj,attr) IS EMPTY
PROC fe_Set(obj,attr,data) IS EMPTY
PROC fe_Draw(obj,flags) IS EMPTY
PROC fe_Layout(obj,x,y,w,h,f) IS EMPTY
PROC fe_Erase(obj,x1,y1,x2,y2,flags) IS EMPTY

PROC fe_SharedFind(name) IS EMPTY
PROC fe_SharedCreate(obj,name) IS EMPTY
PROC fe_SharedDelete(obj) IS EMPTY
PROC fe_SharedOpen(name) IS EMPTY
PROC fe_SharedClose(obj) IS EMPTY

PROC fe_reserved() IS EMPTY

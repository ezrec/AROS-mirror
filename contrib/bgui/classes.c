/*
 * @(#) $Header$
 *
 * BGUI library
 * classes.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.9  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.8  2003/01/18 19:09:55  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.7  2000/08/09 11:45:57  chodorowski
 * Removed a lot of #ifdefs that disabled the AROS_LIB* macros when not building on AROS. This is now handled in contrib/bgui/include/bgui_compilerspecific.h.
 *
 * Revision 42.6  2000/07/06 16:42:03  stegerg
 * the function ForwardMsg relied on gpInput->gpht_Mouse.X/Y and
 * gpHitTest.gpht_Mouse.X/Y being of type WORD. For example
 * it used LONG reads to backup this values and a WORD * pointer
 * to change them. Does not work with AROS where these are of
 * type STACKWORD and not WORD.
 *
 * This caused the problems with the gadgets which could not be
 * activated, or only activated by clicking somewhere outside.
 *
 * Revision 42.5  2000/06/01 01:41:37  bergers
 * Only 2 linker problems left: stch_l & stcu_d. Somebody might want to replace them (embraced by #ifdef __AROS__), please.
 *
 * Revision 42.4  2000/05/31 01:23:10  bergers
 * Changes to make BGUI compilable and linkable.
 *
 * Revision 42.3  2000/05/29 00:40:23  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.2  2000/05/15 19:27:00  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:46  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:08:37  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:03  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.12  1999/08/09 23:01:25  mlemos
 * Optimized method lookup by using a 256 positions hash table and a simple
 * UBYTE conversion as hash function.
 *
 * Revision 41.10.2.11  1999/07/29 00:42:12  mlemos
 * Added a call MarkFreedClass at the end of a successful call to
 * BGUI_FreeClass.
 *
 * Revision 41.10.2.10  1999/01/06 19:18:18  mlemos
 * Prevented division by 0 in the class statistics when no methods were
 * called.
 *
 * Revision 41.10.2.9  1998/12/14 19:56:13  mlemos
 * Replaced the binary search method lookup by hash table method lookup.
 * Added debug code to produce statistics about method lookup on dispatching.
 *
 * Revision 41.10.2.8  1998/12/14 18:36:25  mlemos
 * Added support for method inheritance to speed up method dispatching.
 *
 * Revision 41.10.2.7  1998/12/14 04:59:47  mlemos
 * Replaced the assembly method dispatcher by a C dispatcher that uses binary
 * search as method lookup.
 *
 * Revision 41.10.2.6  1998/11/13 18:08:10  mlemos
 * Fixed mistaken default value for BaseInfo screen pointer in AllocBaseInfo.
 *
 * Revision 41.10.2.5  1998/10/18 18:22:59  mlemos
 * Ensured that the allocated Baseinfo structure is cleared when it is not
 * being copied from another BaseInfo structure.
 *
 * Revision 41.10.2.4  1998/10/12 01:21:54  mlemos
 * Added the support for class constructor and destructor methods.
 *
 * Revision 41.10.2.3  1998/10/01 04:24:40  mlemos
 * Made the class data store also BGUI global data pointer.
 *
 * Revision 41.10.2.2  1998/03/02 23:48:09  mlemos
 * Switched vector allocation functions calls to BGUI allocation functions.
 *
 * Revision 41.10.2.1  1998/03/01 15:32:02  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10  1998/02/25 21:11:47  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:51  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

#ifdef __AROS__
#else
#include <dos.h>
#endif

#ifdef __AROS__
#warning If something is wrong with the dispatcherfunction then look here!
//#if 0
typedef AROS_UFP4(ULONG,* ClassMethodDispatcher,
		AROS_UFPA(Class *, cl, A0),
		AROS_UFPA(Object *, obj, A2),
		AROS_UFPA(Msg, msg, A1),
		AROS_UFPA(APTR, global_data, A4));
//#endif

//typedef ClassMethodDispatcher (void *);

#else
typedef ASM ULONG (*ClassMethodDispatcher)(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg, REG(a4) APTR global_data);
#endif

typedef struct SortedMethod
{
   ULONG MethodID;
#ifdef __AROS__
   ULONG (*DispatcherFunction)(Class *, Object *, Msg, APTR);
#else
   ClassMethodDispatcher DispatcherFunction;
#endif
   Class *Class;
   APTR GlobalData;
   struct SortedMethod *NextHashedMethod;
   APTR Padding[3];
}
SortedMethod;

typedef struct
{
   DPFUNC *ClassMethodFunctions;
#ifdef __AROS__
   ULONG (*ClassDispatcher)(Class *, Object *, Msg, APTR);
#else
   ClassMethodDispatcher ClassDispatcher;
#endif
   APTR BGUIGlobalData;
   APTR ClassGlobalData;
   SortedMethod *MethodFunctions;
   ULONG MethodCount;
   SortedMethod **MethodHashTable;
#ifdef DEBUG_BGUI
   ULONG MethodLookups;
   ULONG MethodComparisions;
   ULONG MethodColisions;
   APTR Padding[6];
#else
   APTR Padding;
#endif
}
BGUIClassData;

#define METHOD_HASH_TABLE_SIZE 256
#define MethodHashValue(method) ((UBYTE)(method))

makeproto UBYTE *RootClass   = "rootclass";
makeproto UBYTE *ImageClass  = "imageclass";
makeproto UBYTE *GadgetClass = "gadgetclass";
makeproto UBYTE *PropGClass  = "propgclass";
makeproto UBYTE *StrGClass   = "strgclass";

makeproto Class *BGUI_MakeClass(ULONG tag, ...)
{
   return BGUI_MakeClassA((struct TagItem *)&tag);
}

//static ASM ULONG ClassCallDispatcher(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg, REG(a4) APTR global_data)
static ASM REGFUNC4(ULONG, ClassCallDispatcher,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg),
	REGPARAM(A4, APTR, global_data))
{
   BGUIClassData *class_data;
   DPFUNC *class_methods;

   if(cl
   && (class_data=(BGUIClassData *)cl->cl_UserData)
   && (class_methods=class_data[-1].ClassMethodFunctions))
   {
      for(;class_methods->df_MethodID!=DF_END;class_methods++)
	 if(class_methods->df_MethodID==msg->MethodID)
#ifdef __AROS__
	   return AROS_UFC4(ULONG, class_methods->df_Func,
		     AROS_UFCA(Class *, cl, A0),
		     AROS_UFCA(Object *, obj, A1),
		     AROS_UFCA(Msg, msg, A2),
		     AROS_UFCA(APTR, global_data, A4));
#else
	   return(((ClassMethodDispatcher)class_methods->df_Func)(cl,obj,msg,global_data));
#endif
   }
   switch(msg->MethodID)
   {
      case OM_NEW:
      case OM_DISPOSE:
	 return(1);
   }
   return(0);
}
REGFUNC_END

struct CallData
{
#ifdef __AROS__
   ULONG (*dispatcher)(Class *, Object *, Msg, APTR);
#else
   ClassMethodDispatcher dispatcher;
#endif
   Class *cl;
   Object *obj;
   Msg msg;
   APTR global_data;
};

//static ULONG ASM CallMethod(REG(a3) struct CallData *call_data)
static ASM REGFUNC1(ULONG, CallMethod,
	REGPARAM(A3, struct CallData *, call_data))
{
   register APTR stack;
   register ULONG result;

#ifdef __AROS__
#warning Commented EnsureStack
#else
   stack=EnsureStack();
#endif
   result=(*call_data->dispatcher)(call_data->cl,call_data->obj,call_data->msg,call_data->global_data);
#ifdef __AROS__
#else
   RevertStack(stack);
#endif
   return(result);
}
REGFUNC_END

static int CompareMethods(const void *method_1, const void *method_2)
{
   return(((SortedMethod *)method_1)->MethodID==((SortedMethod *)method_2)->MethodID ? 0 : (((SortedMethod *)method_1)->MethodID>((SortedMethod *)method_2)->MethodID ? 1 : -1));
}

static SortedMethod *LookupMethod(BGUIClassData *class_data,ULONG method)
{
   SortedMethod *method_found;
   size_t lower,upper,index;

#ifdef DEBUG_BGUI
   class_data->MethodLookups++;
#endif
   for(lower=0,upper=class_data->MethodCount;lower<upper;)
   {
#ifdef DEBUG_BGUI
      class_data->MethodComparisions++;
#endif
      index=(lower+upper)/2;
      method_found=class_data->MethodFunctions+index;
      if(method==method_found->MethodID)
	 return(method_found);
      if(method>method_found->MethodID)
	 lower=index+1;
      else
	 upper=index;
   }
#ifdef DEBUG_BGUI
   class_data->MethodComparisions++;
#endif
   return(NULL);
}

//makeproto SAVEDS ASM ULONG __GCD( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg)
makeproto SAVEDS ASM REGFUNC3(ULONG, __GCD,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
   struct CallData call_data;
   BGUIClassData *class_data;
   SortedMethod *method_found;

   if(cl==NULL
   || (class_data=((BGUIClassData *)cl->cl_UserData))==NULL
   || ((--class_data)->MethodFunctions)==NULL)
   {
      D(bug("BGUI method dispatcher was called to handle an invalid class!!\n"));
      return(0);
   }
#ifdef DEBUG_BGUI
   class_data->MethodLookups++;
#endif
   for(method_found=class_data->MethodHashTable[MethodHashValue(msg->MethodID)];method_found && method_found->MethodID!=msg->MethodID;method_found=method_found->NextHashedMethod)
   {
#ifdef DEBUG_BGUI
      class_data->MethodComparisions++;
#endif
   }
#ifdef DEBUG_BGUI
   class_data->MethodComparisions++;
#endif
   if(method_found)
   {
      call_data.cl=method_found->Class;
#ifdef __AROS__
      call_data.dispatcher=method_found->DispatcherFunction;
#else
      call_data.dispatcher=(ClassMethodDispatcher)method_found->DispatcherFunction;
#endif
      call_data.global_data=method_found->GlobalData;
   }
   else
   {
#ifdef __AROS__
      call_data.dispatcher=(call_data.cl=cl->cl_Super)->cl_Dispatcher.h_Entry;
#else
      call_data.dispatcher=(ClassMethodDispatcher)(call_data.cl=cl->cl_Super)->cl_Dispatcher.h_Entry;
#endif
      call_data.global_data=class_data->BGUIGlobalData;
   }
   call_data.obj=obj;
   call_data.msg=msg;
   return(CallMethod(&call_data));
}
REGFUNC_END

/*
 * Setup a class.
 */
#ifdef __AROS__
makearosproto
AROS_LH1(Class *, BGUI_MakeClassA,
    AROS_LHA(struct TagItem *, tags, A0),
    struct Library *, BGUIBase, 24, BGUI)
#else
makeproto ASM Class *BGUI_MakeClassA(REG(a0) struct TagItem *tags)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

#ifdef __AROS__
#else
   ULONG  old_a4 = (ULONG)getreg(REG_A4);
#endif
   ULONG  SuperClass, SuperClass_ID, Class_ID, Flags, ClassSize, ObjectSize;
   BGUIClassData *ClassData;
   Class *cl=NULL;

#ifdef __AROS__
#else
   geta4();
#endif

   if ((SuperClass = GetTagData(CLASS_SuperClassBGUI, (ULONG)~0, tags)) == (ULONG)~0)
      SuperClass = GetTagData(CLASS_SuperClass, NULL, tags);
   else
      SuperClass = (ULONG)BGUI_GetClassPtr(SuperClass);

   if (SuperClass)
      SuperClass_ID = NULL;
   else
      SuperClass_ID = GetTagData(CLASS_SuperClassID, (ULONG)"rootclass", tags);

   Class_ID   = GetTagData(CLASS_ClassID, NULL, tags);
   Flags      = GetTagData(CLASS_Flags,      0, tags);
   ClassSize  = GetTagData(CLASS_ClassSize,  0, tags);
   ObjectSize = GetTagData(CLASS_ObjectSize, 0, tags);

   if (ClassData = BGUI_AllocPoolMem(ClassSize + sizeof(BGUIClassData)))
   {
      if (cl = MakeClass((UBYTE *)Class_ID, (UBYTE *)SuperClass_ID, (Class *)SuperClass, ObjectSize, Flags))
      {
	 DPFUNC *method_functions;

	 cl->cl_UserData                 = (LONG)(ClassData+1);
	 cl->cl_Dispatcher.h_Entry       = (HOOKFUNC)GetTagData(CLASS_Dispatcher, (ULONG)__GCD, tags);
	 ClassData->ClassMethodFunctions = (DPFUNC *)GetTagData(CLASS_ClassDFTable, NULL, tags);
#ifdef __AROS__
	 ClassData->ClassDispatcher      = GetTagData(CLASS_ClassDispatcher, (ULONG)ClassCallDispatcher, tags);
#else
	 ClassData->ClassDispatcher      = (ClassMethodDispatcher)GetTagData(CLASS_ClassDispatcher, (ULONG)ClassCallDispatcher, tags);
#endif

#ifdef __AROS__
#warning Missing code here!
#else
	 ClassData->BGUIGlobalData       = (APTR)getreg(REG_A4);
	 ClassData->ClassGlobalData      = (APTR)old_a4;
#endif

	 if((method_functions=(DPFUNC *)GetTagData(CLASS_DFTable, NULL, tags)))
	 {
	    {
	       DPFUNC *methods;

	       for(ClassData->MethodCount=0,methods=method_functions;methods->df_MethodID!=DF_END;ClassData->MethodCount++,methods++);
	    }
	    if((ClassData->MethodFunctions=BGUI_AllocPoolMem(sizeof(*ClassData->MethodFunctions)*ClassData->MethodCount)))
	    {
	       ULONG method;

	       for(method=0;method<ClassData->MethodCount;method++)
	       {
		  ClassData->MethodFunctions[method].MethodID=method_functions[method].df_MethodID;
#ifdef __AROS__
		  ClassData->MethodFunctions[method].DispatcherFunction=method_functions[method].df_Func;
#else
		  ClassData->MethodFunctions[method].DispatcherFunction=(ClassMethodDispatcher)method_functions[method].df_Func;
#endif
		  ClassData->MethodFunctions[method].Class=cl;
		  ClassData->MethodFunctions[method].GlobalData=ClassData->ClassGlobalData;
	       }
	       qsort(ClassData->MethodFunctions,ClassData->MethodCount,sizeof(*ClassData->MethodFunctions),CompareMethods);
	       if(cl->cl_Super->cl_Dispatcher.h_Entry==__GCD)
	       {
		  BGUIClassData *super_class_data;
		  ULONG new_methods;

		  super_class_data=(((BGUIClassData *)cl->cl_Super->cl_UserData)-1);
		  for(new_methods=ClassData->MethodCount,method=0;method<super_class_data->MethodCount;method++)
		  {
		     if(LookupMethod(ClassData,super_class_data->MethodFunctions[method].MethodID)==NULL)
		     {
			SortedMethod *new_sorted_methods;

			if((new_sorted_methods=BGUI_AllocPoolMem(sizeof(*new_sorted_methods)*(new_methods+1)))==NULL)
			{
			   cl->cl_UserData=NULL;
			   FreeClass(cl);
			   cl=NULL;
			   break;
			}
			memcpy(new_sorted_methods,ClassData->MethodFunctions,sizeof(*new_sorted_methods)*new_methods);
			BGUI_FreePoolMem(ClassData->MethodFunctions);
			ClassData->MethodFunctions=new_sorted_methods;
			ClassData->MethodFunctions[new_methods]=super_class_data->MethodFunctions[method];
			new_methods++;
		     }
		  }
		  ClassData->MethodCount=new_methods;
		  qsort(ClassData->MethodFunctions,ClassData->MethodCount,sizeof(*ClassData->MethodFunctions),CompareMethods);
	       }
	       if(cl)
	       {
		  if((ClassData->MethodHashTable=BGUI_AllocPoolMem(sizeof(*ClassData->MethodHashTable)*METHOD_HASH_TABLE_SIZE)))
		  {
		     ULONG method;

		     memset(ClassData->MethodHashTable,'\0',sizeof(*ClassData->MethodHashTable)*METHOD_HASH_TABLE_SIZE);
#ifdef DEBUG_BGUI
		     ClassData->MethodColisions=0;
#endif
		     for(method=0;method<ClassData->MethodCount;method++)
		     {
			SortedMethod **method_hash;

			method_hash= ClassData->MethodHashTable+MethodHashValue(ClassData->MethodFunctions[method].MethodID);
#ifdef DEBUG_BGUI
			if(*method_hash)
			   ClassData->MethodColisions++;
#endif
			ClassData->MethodFunctions[method].NextHashedMethod= *method_hash;
			*method_hash= &ClassData->MethodFunctions[method];
		     }
#ifdef DEBUG_BGUI
		     ClassData->MethodLookups=ClassData->MethodComparisions=0;
#endif
		  }
		  else
		  {
		     cl->cl_UserData=NULL;
		     FreeClass(cl);
		     cl=NULL;
		  }
	       }
	    }
	    else
	    {
	       cl->cl_UserData=NULL;
	       FreeClass(cl);
	       cl=NULL;
	    }
	 }
	 else
	 {
	    ClassData->MethodFunctions=NULL;
	    ClassData->MethodHashTable=NULL;
	 }
	 if(cl
	 && ClassData->ClassDispatcher)
	 {
	    struct opSet msg;

	    msg.MethodID=OM_NEW;
	    msg.ops_AttrList=NULL;
	    msg.ops_GInfo=NULL;
	    if(!ClassData->ClassDispatcher(cl,NULL,(Msg)&msg,ClassData->ClassGlobalData))
	    {
	       cl->cl_UserData=NULL;
	       FreeClass(cl);
	       cl=NULL;
	    }
	 }
      }
      if(cl==NULL)
      {
	 if(ClassData->MethodFunctions)
	    BGUI_FreePoolMem(ClassData->MethodFunctions);
	 BGUI_FreePoolMem(ClassData);
      }
   };
#ifdef __AROS__
#else
   putreg(REG_A4, (LONG)old_a4);
#endif
   return cl;

   AROS_LIBFUNC_EXIT
}

#ifdef __AROS__
makearosproto
AROS_LH1(BOOL, BGUI_FreeClass,
    AROS_LHA(Class *, cl, A0),
    struct Library *, BGUIBase, 25, BGUI)
#else
makeproto SAVEDS ASM BOOL BGUI_FreeClass(REG(a0) Class *cl)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   if (cl)
   {
      if(cl->cl_UserData)
      {
	 BGUIClassData *ClassData;

	 ClassData=((BGUIClassData *)cl->cl_UserData) - 1;
	 if(ClassData->ClassDispatcher)
	 {
	    ULONG msg=OM_DISPOSE;

	    if(!ClassData->ClassDispatcher(cl,NULL,(Msg)&msg,ClassData->ClassGlobalData))
	       return FALSE;
	 }
	 if(ClassData->MethodFunctions)
	 {
	    D(bug("Methods %lu, Lookups %lu, Comparisions %lu, Misses %lu (%lu%%), Colisions %lu (%lu%%)\n",ClassData->MethodCount,ClassData->MethodLookups,ClassData->MethodComparisions,ClassData->MethodComparisions-ClassData->MethodLookups,ClassData->MethodComparisions ? (ClassData->MethodComparisions-ClassData->MethodLookups)*100/ClassData->MethodComparisions : 0,ClassData->MethodColisions,ClassData->MethodCount ? ClassData->MethodColisions*100/ClassData->MethodCount : 0));
	    BGUI_FreePoolMem(ClassData->MethodFunctions);
	    BGUI_FreePoolMem(ClassData->MethodHashTable);
	 }
	 BGUI_FreePoolMem(ClassData);
	 cl->cl_UserData=NULL;
      }
      if(FreeClass(cl))
      {
	 MarkFreedClass(cl);
	 return(TRUE);
      }
   };
   return FALSE;

   AROS_LIBFUNC_EXIT
}

//makeproto ULONG ASM BGUI_GetAttrChart(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct rmAttr *ra)
makeproto ASM REGFUNC3(ULONG, BGUI_GetAttrChart,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct rmAttr *, ra))
{
   ULONG           flags = ra->ra_Flags;
   struct TagItem *attr  = ra->ra_Attr;
   ULONG           data;
   UBYTE          *dataspace;
   ULONG           rc;

   if (rc = AsmCoerceMethod(cl, obj, RM_GETATTRFLAGS, attr, flags))
   {
      if (!(rc & RAF_NOGET))
      {
	 dataspace = (UBYTE *)INST_DATA(cl, obj) + ((rc >> 16) & 0x07FF);

	 if (rc & RAF_BOOL)
	 {
	    data = (*dataspace >> ((rc >> 27) & 0x07)) & 1;
	    if (rc & RAF_SIGNED) data = !data;
	 }
	 else
	 {
	    switch (rc & (RAF_SIGNED|RAF_BYTE|RAF_WORD|RAF_LONG|RAF_ADDR))
	    {
	    case RAF_BYTE:
	       data = (ULONG)(*(UBYTE *)dataspace);
	       break;
	    case RAF_BYTE|RAF_SIGNED:
	       data = (LONG)(*(BYTE *)dataspace);
	       break;
	    case RAF_WORD:
	       data = (ULONG)(*(UWORD *)dataspace);
	       break;
	    case RAF_WORD|RAF_SIGNED:
	       data = (LONG)(*(WORD *)dataspace);
	       break;
	    case RAF_LONG:
	       data = (ULONG)(*(ULONG *)dataspace);
	       break;
	    case RAF_LONG|RAF_SIGNED:
	       data = (LONG)(*(LONG *)dataspace);
	       break;
	    case RAF_ADDR:
	       data = (ULONG)dataspace;
	       break;
	    case RAF_NOP:
	       goto no_get;
	    };
	 };
	 *((ULONG *)(attr->ti_Data)) = data;
      }
      no_get:
      if (rc & RAF_CUSTOM)
      {
	 AsmCoerceMethod(cl, obj, RM_GETCUSTOM, attr, flags);
      };
      if (rc & RAF_SUPER)
      {
	 rc |= AsmDoSuperMethod(cl, obj, RM_GET, attr, flags);
      };
      rc = (rc & 0xFFFF) | RAF_UNDERSTOOD;
   }
   else
   {
      /*
       * Let the superclass have a go at it.
       */
      rc = AsmDoSuperMethodA(cl, obj, (Msg)ra);
   };
   return rc;
}
REGFUNC_END

//makeproto ULONG ASM BGUI_SetAttrChart(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct rmAttr *ra)
makeproto ASM REGFUNC3(ULONG, BGUI_SetAttrChart,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct rmAttr *, ra))
{
   ULONG           flags = ra->ra_Flags;
   struct TagItem *attr  = ra->ra_Attr;
   ULONG           data;
   UBYTE          *dataspace, flag;
   ULONG           rc;

   if (rc = AsmCoerceMethod(cl, obj, RM_GETATTRFLAGS, attr, flags))
   {
      if ((rc & RAF_NOSET) || ((rc & RAF_NOUPDATE)  && (flags & RAF_UPDATE))
			   || ((rc & RAF_NOINTERIM) && (flags & RAF_INTERIM))
			   || ((rc & RAF_INITIAL)  && !(flags & RAF_INITIAL)))
      {
	 rc &= ~RAF_SUPER;
      }
      else
      {
	 if (flags & RAF_INITIAL) rc &= ~RAF_SUPER;

	 dataspace = (UBYTE *)INST_DATA(cl, obj) + ((rc >> 16) & 0x07FF);
	 data      = attr->ti_Data;

	 if (rc & RAF_BOOL)
	 {
	    flag = 1 << ((rc >> 27) & 0x07);
	    if (rc & RAF_SIGNED) data = !data;

	    if (data) *dataspace |=  flag;
	    else      *dataspace &= ~flag;
	 }
	 else
	 {
	    switch (rc & (RAF_SIGNED|RAF_BYTE|RAF_WORD|RAF_LONG|RAF_ADDR))
	    {
	    case RAF_BYTE:
	    case RAF_BYTE|RAF_SIGNED:
	       *((UBYTE *)dataspace) = (UBYTE)data;
	       break;
	    case RAF_WORD:
	    case RAF_WORD|RAF_SIGNED:
	       *((UWORD *)dataspace) = (UWORD)data;
	       break;
	    case RAF_LONG:
	    case RAF_LONG|RAF_SIGNED:
	       *((ULONG *)dataspace) = (ULONG)data;
	       break;
	    case RAF_NOP:
	       break;
	    };
	 };
      };
      if (rc & RAF_CUSTOM)
      {
	 AsmCoerceMethod(cl, obj, RM_SETCUSTOM, attr, flags);
      };
      if (rc & RAF_SUPER)
      {
	 rc |= AsmDoSuperMethod(cl, obj, RM_SET, attr, flags);
      };
      rc = (rc & 0xFFFF) | RAF_UNDERSTOOD;
   }
   else
   {
      /*
       * Let the superclass have a go at it.
       */
      rc = AsmDoSuperMethod(cl, obj, RM_SET, attr, flags);
   };
   return rc;
}
REGFUNC_END

makeproto ULONG BGUI_PackStructureTag(UBYTE *dataspace, ULONG *pt, ULONG tag, ULONG data)
{
   #ifdef ENHANCED

   struct TagItem tags[2];

   tags[0].ti_Tag  = tag;
   tags[0].ti_Data = data;
   tags[1].ti_Tag  = TAG_DONE;

   return (ULONG)PackStructureTags((APTR)dataspace, pt, tags);

   #else

   ULONG type;
   UBYTE flag;
   ULONG base = *pt++;

   while (type = *pt++)
   {
      if (type == 0xFFFFFFFF)
      {
	 base = *pt++;
	 continue;
      };
      if (tag == (base + ((type >> 16) & 0x03FF) ))
      {
	 if (type & PKCTRL_UNPACKONLY) return 0;

	 dataspace += (type & 0x1FFF);

	 switch (type & 0x18000000)
	 {
	 case PKCTRL_BIT:
	    flag = 1 << ((type >> 13) & 0x0007);
	    if (type & PSTF_SIGNED) data = !data;
	    if (data) *dataspace |= flag;
	    else      *dataspace &= ~flag;
	    break;
	 case PKCTRL_UBYTE:
	    *((UBYTE *)dataspace) = (UBYTE)data;
	    break;
	 case PKCTRL_UWORD:
	    *((UWORD *)dataspace) = (UWORD)data;
	    break;
	 case PKCTRL_ULONG:
	    *((ULONG *)dataspace) = (ULONG)data;
	    break;
	 };
	 return 1;
      };
   };
   return 0;

   #endif
}

makeproto ULONG BGUI_UnpackStructureTag(UBYTE *dataspace, ULONG *pt, ULONG tag, ULONG *storage)
{
   #ifdef ENHANCED

   struct TagItem tags[2];

   tags[0].ti_Tag  = tag;
   tags[0].ti_Data = (ULONG)storage;
   tags[1].ti_Tag  = TAG_DONE;

   return (ULONG)UnpackStructureTags((APTR)dataspace, pt, tags);

   #else

   ULONG type, data;
   ULONG base = *pt++;

   while (type = *pt++)
   {
      if (type == 0xFFFFFFFF)
      {
	 base = *pt++;
	 continue;
      };
      if (tag == (base + ((type >> 16) & 0x03FF) ))
      {
	 if (type & PKCTRL_PACKONLY) return 0;

	 dataspace += (type & 0x1FFF);

	 switch (type & 0x98000000)
	 {
	 case PKCTRL_UBYTE:
	    data = (ULONG)(*((UBYTE *)dataspace));
	    break;
	 case PKCTRL_UWORD:
	    data = (ULONG)(*((UWORD *)dataspace));
	    break;
	 case PKCTRL_ULONG:
	    data = (ULONG)(*((ULONG *)dataspace));
	    break;
	 case PKCTRL_BYTE:
	    data = (LONG)(*((BYTE *)dataspace));
	    break;
	 case PKCTRL_WORD:
	    data = (LONG)(*((WORD *)dataspace));
	    break;
	 case PKCTRL_LONG:
	    data = (LONG)(*((LONG *)dataspace));
	    break;
	 case PKCTRL_BIT:
	 case PKCTRL_FLIPBIT:
	    data = (*dataspace & (1 << ((type >> 13) & 0x0007))) ? ~0 : 0;
	    if (type & PSTF_SIGNED) data = ~data;
	    break;
	 };
	 *storage = data;
	 return 1;
      };
   };
   return 0;

   #endif
}

#ifdef __AROS__
makearosproto
AROS_LH3(ULONG, BGUI_PackStructureTags,
    AROS_LHA(APTR, pack, A0),
    AROS_LHA(ULONG *, packTable, A1),
    AROS_LHA(struct TagItem *, tagList, A2),
    struct Library *, BGUIBase, 26, BGUI)
#else
makeproto SAVEDS ULONG ASM BGUI_PackStructureTags(REG(a0) APTR pack, REG(a1) ULONG *packTable, REG(a2) struct TagItem *tagList)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   #ifdef ENHANCED

   return PackStructureTags(pack, packTable, tagList);

   #else

   struct TagItem *tstate = tagList, *tag;
   ULONG rc = 0;

   while (tag = NextTagItem(&tstate))
   {
      rc += BGUI_PackStructureTag((UBYTE *)pack, packTable, tag->ti_Tag, tag->ti_Data);
   };
   return rc;

   #endif
   
   AROS_LIBFUNC_EXIT
}

#ifdef __AROS__
makearosproto
AROS_LH3(ULONG, BGUI_UnpackStructureTags,
    AROS_LHA(APTR, pack, A0),
    AROS_LHA(ULONG *, packTable, A1),
    AROS_LHA(struct TagItem *, tagList, A2),
    struct Library *, BGUIBase, 27, BGUI)
#else
makeproto SAVEDS ULONG ASM BGUI_UnpackStructureTags(REG(a0) APTR pack, REG(a1) ULONG *packTable, REG(a2) struct TagItem *tagList)
#endif
{
   AROS_LIBFUNC_INIT
   AROS_LIBBASE_EXT_DECL(struct Library *,BGUIBase)

   #ifdef ENHANCED

   return UnpackStructureTags(pack, packTable, tagList);

   #else

   struct TagItem *tstate = tagList, *tag;
   ULONG rc = 0;

   while (tag = NextTagItem(&tstate))
   {
      rc += BGUI_UnpackStructureTag((UBYTE *)pack, packTable, tag->ti_Tag, (ULONG *)tag->ti_Data);
   };
   return rc;

   #endif

   AROS_LIBFUNC_EXIT
}

/*
 * Quick GetAttr();
 */
//makeproto ASM ULONG Get_Attr(REG(a0) Object *obj, REG(d0) ULONG attr, REG(a1) void *storage)
makeproto ASM REGFUNC3(ULONG, Get_Attr,
	REGPARAM(A0, Object *, obj),
	REGPARAM(D0, ULONG, attr),
	REGPARAM(A1, void *, storage))
{
   return AsmDoMethod(obj, OM_GET, attr, storage);
}
REGFUNC_END

//makeproto ASM ULONG Get_SuperAttr(REG(a2) Class *cl, REG(a0) Object *obj, REG(d0) ULONG attr, REG(a1) void *storage)
makeproto ASM REGFUNC4(ULONG, Get_SuperAttr,
	REGPARAM(A2, Class *, cl),
	REGPARAM(A0, Object *, obj),
	REGPARAM(D0, ULONG, attr),
	REGPARAM(A1, void *, storage))
{
   return AsmDoSuperMethod(cl, obj, OM_GET, attr, storage);
}
REGFUNC_END

makeproto ULONG NewSuperObject(Class *cl, Object *obj, struct TagItem *tags)
{
   return AsmDoSuperMethod(cl, obj, OM_NEW, (ULONG)tags, NULL);
}

/*
 * Like SetAttrs();
 */
makeproto ULONG DoSetMethodNG(Object *obj, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_SET, (struct TagItem *)&tag1, NULL);
   else     return 0;
}

/*
 * Like SetAttrs();
 */
makeproto ULONG DoSuperSetMethodNG(Class *cl, Object *obj, Tag tag1, ...)
{
   if (obj) return AsmDoSuperMethod(cl, obj, OM_SET, (struct TagItem *)&tag1, NULL);
   else     return 0;
}

/*
 * Call the OM_SET method.
 */
makeproto ULONG DoSetMethod(Object *obj, struct GadgetInfo *ginfo, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_SET, (struct TagItem *)&tag1, ginfo);
   else     return 0;
}

/*
 * Call the OM_SET method of the superclass.
 */
makeproto ULONG DoSuperSetMethod(Class *cl, Object *obj, struct GadgetInfo *ginfo, Tag tag1, ...)
{
   if (obj) return AsmDoSuperMethod(cl, obj, OM_SET, (struct TagItem *)&tag1, ginfo);
   else     return 0;
}

/*
 * Call the OM_UPDATE method.
 */
makeproto ULONG DoUpdateMethod(Object *obj, struct GadgetInfo *ginfo, ULONG flags, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_UPDATE, (struct TagItem *)&tag1, ginfo, flags);
   else     return 0;
}

/*
 * Call the OM_NOTIFY method.
 */
makeproto ULONG DoNotifyMethod(Object *obj, struct GadgetInfo *ginfo, ULONG flags, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_NOTIFY, (struct TagItem *)&tag1, ginfo, flags);
   else     return 0;
}

/*
 * Call the GM_RENDER method.
 */

//makeproto ASM ULONG DoRenderMethod(REG(a0) Object *obj, REG(a1) struct GadgetInfo *ginfo, REG(d0) ULONG redraw)
makeproto ASM REGFUNC3(ULONG, DoRenderMethod,
	REGPARAM(A0, Object *, obj),
	REGPARAM(A1, struct GadgetInfo *, ginfo),
	REGPARAM(D0, ULONG, redraw))
{
   struct RastPort   *rp;
   ULONG              rc = 0;

   if (obj && (rp = BGUI_ObtainGIRPort(ginfo)))
   {
      rc = AsmDoMethod(obj, GM_RENDER, ginfo, rp, redraw);
      ReleaseGIRPort(rp);
   }
   return rc;
}
REGFUNC_END

/*
 * Forward certain types of messages with modifications.
 */
//makeproto ASM ULONG ForwardMsg(REG(a0) Object *s, REG(a1) Object *d, REG(a2) Msg msg)
makeproto ASM REGFUNC3(ULONG, ForwardMsg,
	REGPARAM(A0, Object *, s),
	REGPARAM(A1, Object *, d),
	REGPARAM(A2, Msg, msg))
{
#ifdef __AROS__
   #define MOUSEWORD STACKWORD
#else
   #define MOUSEWORD WORD
#endif
   MOUSEWORD      *mousex = NULL, *mousey = NULL, old_mousex, old_mousey;
   ULONG          rc;
   struct IBox   *b1 = GADGETBOX(s);
   struct IBox   *b2 = GADGETBOX(d);
   
   /*
    * Get address of mouse coordinates in message.
    */
   switch (msg->MethodID)
   {
   case GM_HITTEST:
   case GM_HELPTEST:
      mousex = &((struct gpHitTest *)msg)->gpht_Mouse.X;
      mousey = &((struct gpHitTest *)msg)->gpht_Mouse.Y;
      break;
   case GM_GOACTIVE:
   case GM_HANDLEINPUT:
      mousex = &((struct gpInput *)msg)->gpi_Mouse.X;
      mousey = &((struct gpInput *)msg)->gpi_Mouse.Y;
      break;
   };
   if (!mousex) return 0;

   /*
    * Store the coordinates.
    */
   old_mousex = *mousex;
   old_mousey = *mousey;

   Get_Attr(s, BT_OuterBox, (ULONG *)&b1);
   Get_Attr(d, BT_OuterBox, (ULONG *)&b2);

   /*
    * Adjust the coordinates to be relative to the destination object.
    */
   *mousex += b1->Left - b2->Left;
   *mousey += b1->Top  - b2->Top;

   /*
    * Send the message to the destination object.
    */
   rc = AsmDoMethodA(d, msg);

   /*
    * Put the coordinates back to what they were originally.
    */
   *mousex = old_mousex;
   *mousey = old_mousey;

   return rc;
}
REGFUNC_END

#define BI_FREE_RP  1
#define BI_FREE_DRI 2

#ifdef DEBUG_BGUI
makeproto struct BaseInfo *AllocBaseInfoDebug(STRPTR file, ULONG line,ULONG tag1, ...)
{
   return BGUI_AllocBaseInfoDebugA((struct TagItem *)&tag1,file,line);
}
#else
makeproto struct BaseInfo *AllocBaseInfo(ULONG tag1, ...)
{
   return BGUI_AllocBaseInfoA((struct TagItem *)&tag1);
}
#endif

#ifdef DEBUG_BGUI
//makeproto SAVEDS ASM struct BaseInfo *BGUI_AllocBaseInfoDebugA(REG(a0) struct TagItem *tags,REG(a1) STRPTR file, REG(d0) ULONG line)
makeproto SAVEDS ASM REGFUNC3(struct BaseInfo *, BGUI_AllocBaseInfoDebugA,
	REGPARAM(A0, struct TagItem *, tags),
	REGPARAM(A1, STRPTR, file),
	REGPARAM(D0, ULONG, line))
#else
//makeproto SAVEDS ASM struct BaseInfo *BGUI_AllocBaseInfoA(REG(a0) struct TagItem *tags)
makeproto SAVEDS ASM REGFUNC1(struct BaseInfo *, BGUI_AllocBaseInfoA,
	REGPARAM(A0, struct TagItem *, tags))
#endif
{
   struct BaseInfo   *bi, *bi2;
   struct GadgetInfo *gi = NULL;
   ULONG             *flags;

#ifdef DEBUG_BGUI
   if (bi = BGUI_AllocPoolMemDebug(sizeof(struct BaseInfo) + sizeof(ULONG),file,line))
#else
   if (bi = BGUI_AllocPoolMem(sizeof(struct BaseInfo) + sizeof(ULONG)))
#endif
   {
      flags = (ULONG *)(bi + 1);

      if (bi2 = (struct BaseInfo *)GetTagData(BI_BaseInfo, NULL, tags))
      {
	 *bi = *bi2;
      }
      else
	memset(bi,'\0',sizeof(*bi));

      if (gi = (struct GadgetInfo *)GetTagData(BI_GadgetInfo, (ULONG)gi, tags))
      {
	 *((struct GadgetInfo *)bi) = *gi;
      };

      bi->bi_DrInfo  = (struct DrawInfo *)GetTagData(BI_DrawInfo, (ULONG)bi->bi_DrInfo, tags);
      bi->bi_RPort   = (struct RastPort *)GetTagData(BI_RastPort, (ULONG)bi->bi_RPort,  tags);
      bi->bi_IScreen = (struct Screen *)  GetTagData(BI_Screen,   (ULONG)bi->bi_IScreen, tags);
      bi->bi_Pens    = (UWORD *)          GetTagData(BI_Pens,     (ULONG)bi->bi_Pens,   tags);

      if (gi && !bi->bi_RPort)
      {
	 if (bi->bi_RPort = ObtainGIRPort(gi))
	 {
	    *flags |= BI_FREE_RP;
	 };
      };

      if (bi->bi_IScreen && !bi->bi_DrInfo)
      {
	 if (bi->bi_DrInfo = GetScreenDrawInfo(bi->bi_IScreen))
	 {
	    *flags |= BI_FREE_DRI;
	 };
      };

      if (!bi->bi_Pens)
      {
	 if (bi->bi_DrInfo) bi->bi_Pens = bi->bi_DrInfo->dri_Pens;
	 else               bi->bi_Pens = DefDriPens;
      };
   }
   return bi;
}
REGFUNC_END

#ifdef DEBUG_BGUI
makeproto void FreeBaseInfoDebug(struct BaseInfo *bi, STRPTR file, ULONG line)
{
#else
makeproto void FreeBaseInfo(struct BaseInfo *bi)
{
#endif
   ULONG *flags = (ULONG *)(bi + 1);

   if (*flags & BI_FREE_RP)  ReleaseGIRPort(bi->bi_RPort);
   if (*flags & BI_FREE_DRI) FreeScreenDrawInfo(bi->bi_IScreen, bi->bi_DrInfo);

#ifdef DEBUG_BGUI
   BGUI_FreePoolMemDebug(bi,file,line);
#else
   BGUI_FreePoolMem(bi);
#endif
}


-> rexxbgui.library version 4.0
-> Date: June 24, 1999
-> Copyright © Jilles Tjoelker 1999
->
-> New in V4.0:
-> * Version number bumped because of new functions.
-> * Added image buttons.
-> * Added a simple file requester function.
-> * Added a function to set up listview control with the arrow keys.
-> * Bug fixed: I once disabled CYC_Active but forgot to enable it again.
->
-> New in V3.0:
-> * Version number bumped because of new functions.
-> * Added separators and cycle gadgets.
-> * Added version testing functions bguirexxversion() and bguiversion().
->
-> New in V2.0:
-> * Version number bumped because of new functions.
-> * Added menus.
-> * Added progress bar (suggested by Ingo Musquinier) and indicator.
-> * Bug fixed: bguiwindow() did not get the argument count correctly. This
->   caused problems, because ARexx does not clear extra argument slots but
->   instead leaves in them what values happen to be contained in them from
->   previous function calls.
->
-> New in V1.1:
-> * New functions: bguiwinbusy(), bguiwinready(), bguipages(), bguimx(),
->   bguiactivategadget(), bguilist(), bguiaddcondit(), bguilistvaddentry(),
->   bguilistvcommand(), bguilistvreplacesel()
-> * New argument for bgui[hv]group(), equal, to force equal width or height
->   of the objects in the group
-> * New argument for bguiwindow(), pos, to open the window on another position
->   than centred on the screen
-> * Some nonfunctional tags removed
-> * New tags: BT_#?, PAGE_#?, MX_#?, TAG_IGNORE
-> * Better documentation
-> * Bug fixed: the library could be flushed from memory when a bguiopen() was
->   still outstanding.
-> * A control-C break is now accepted immediately in bguiwinwaitevent() and
->   bguiwait() and passed to ARexx.
-> * Bug fixed: BGUI 41.10 took the command name, which is always equal to "", for
->   the prefs filename. Fixed by temporarily setting the pr_Cli pointer to zero.
->
-> Look in the example scripts for more information.
-> General ARexx limitation: functions can only have up to 15 arguments.

		/* ESCAPE SEQUENCES
		   ^^^ in bold */
-> Escape sequences can be used in most texts, including labels, frame titles
-> info object contents and listview entries. Frame titles may have shadow.
-> Listview entries and labels within objects can be text on background or
-> filltext on fill; when you change the colours it cannot be restored.
-> ESC='1b'x or just use the Esc key to put it in a string constant.
-> 
-> ESC||ESC no operation; used to separate sequences ending in a digit
->          from normal digits.
-> ESC'u'   underlined on (do not use for indicating a hotkey)
-> ESC'b'   bold on
-> ESC'i'   italics on
-> ESC'z'   underscore on (can indicate the hotkey for a gadget, only visual
->          though, use BT_Key to make it work)
-> ESC'n'   normal text
-> ESC'd'n  set DrawInfo pen #n (2=text, 3=shine, 4=shadow, 5=selected gadget
->          fill, 6=text on fill, 7=background, 8=important text)
-> ESC'p'n  set colour #n
-> ESC'D'n  set DrawInfo pen #n as background
-> ESC'P'n  set colour #n as background
-> ESC's'   shadow on (additionally use ESC'd3')
-> ESC'-'ch turn something off (ch=u/b/i/s/z)
-> ESC'c'   centre this and the following lines
-> ESC'r'   right-justify this and the following lines
-> ESC'l'   left-justify this and the following lines
-> '0a'x    new line. This does not work for listview entries and frame titles.

		/* FUNCTIONS
*/

OPT OSVERSION=37,LARGE

MODULE 'rexx/storage','rexxsyslib','tools/rexxvar','rexx/rxslib',
       'exec/nodes','exec/lists','exec/ports','dos/dos','icon',
       'exec/tasks','exec/libraries','exec/memory','amigalib/lists',
       'utility','dos/dosextens','utility/tagitem','tools/boopsi',
       'intuition/intuition','intuition/gadgetclass','bgui',
       'libraries/bgui','libraries/gadtools','workbench/workbench',
       'libraries/asl','utility/hooks','tools/installhook',
       'devices/inputevent','*strchr','*stricmp'

LIBRARY 'rexxbgui.library',4,0,'rexxbgui.library 4.0 (24.6.99)' IS
	query(A0)

OBJECT idcmphook OF hook
	window_object
	listview
	doarrowmessages
	enter_id
ENDOBJECT

OBJECT tasknode
	task:PTR TO tc
	bguibase:PTR TO lib
	lastid
	stringspace:mlh -> MinList of stringbuffer
	objects:mlh -> MinList of objnode
	images:mlh -> MinList of imagenode
	filereq
	chipmem:PTR TO memblock
	idcmphook:idcmphook
ENDOBJECT

OBJECT objnode
	node:mln
	obj
ENDOBJECT

OBJECT imagenode
	node:mln
	dobj:PTR TO diskobject
ENDOBJECT

OBJECT memblock
	next:PTR TO memblock
ENDOBJECT

CONST QUANTUM=4000

OBJECT stringbuffer
	node:mln
	ptr:LONG
	space[QUANTUM]:ARRAY
ENDOBJECT

ENUM TAG_INT,TAG_STRING,TAG_OBJECT,TAG_MENU,TAG_MAX

OBJECT taginfo
	name:PTR TO CHAR
	value:LONG
	type:LONG
ENDOBJECT

DEF templist[160]:LIST,tempstr[256]:STRING,tasknode=NIL:PTR TO tasknode
DEF rexxbguibase:PTR TO lib

PROC idcmphook_func(h:PTR TO idcmphook,wd_obj,imsg:PTR TO intuimessage)
	DEF code,qual,wnd,reportid=0,ridflags=0
	IF h.window_object<>wd_obj THEN RETURN
	GetAttr(WINDOW_Window,wd_obj,{wnd})
	code:=imsg.code; qual:=imsg.qualifier
	IF imsg.class=IDCMP_RAWKEY
		IF (code=67) OR (code=68)
			reportid:=h.enter_id
		ELSEIF code=$4C -> Arrow up
      IF imsg.qualifier AND (IEQUALIFIER_LSHIFT OR IEQUALIFIER_RSHIFT)
        SetGadgetAttrsA(h.listview,wnd,NIL,[LISTV_Select,LISTV_Select_Page_Up,0])
      ELSEIF imsg.qualifier AND (IEQUALIFIER_LALT OR IEQUALIFIER_RALT)
        SetGadgetAttrsA(h.listview,wnd,NIL,[LISTV_Select,LISTV_Select_First,0])
      ELSE
        SetGadgetAttrsA(h.listview,wnd,NIL,[LISTV_Select,LISTV_Select_Previous,0])
      ENDIF
      reportid:=h.doarrowmessages
    ELSEIF code=$4D -> Arrow down
      IF imsg.qualifier AND (IEQUALIFIER_LSHIFT OR IEQUALIFIER_RSHIFT)
        SetGadgetAttrsA(h.listview,wnd,NIL,[LISTV_Select,LISTV_Select_Page_Down,0])
      ELSEIF imsg.qualifier AND (IEQUALIFIER_LALT OR IEQUALIFIER_RALT)
        SetGadgetAttrsA(h.listview,wnd,NIL,[LISTV_Select,LISTV_Select_Last,0])
      ELSE
        SetGadgetAttrsA(h.listview,wnd,NIL,[LISTV_Select,LISTV_Select_Next,0])
      ENDIF
      reportid:=h.doarrowmessages
		ENDIF
	ENDIF
	ListCopy(templist,[WM_REPORT_ID])
	listadd2(templist,reportid,ridflags)
	domethod(wd_obj,templist)
ENDPROC

PROC query(rm:PTR TO rexxmsg) HANDLE
	DEF rc=1,result=NIL,q:PTR TO CHAR,wnd:PTR TO window
	DEF funcname
	DEF obj,n,i,plong:PTR TO LONG
	DEF dos:PTR TO doslibrary,temp2str[80]:STRING,c
	DEF t:PTR TO taginfo,flags

	MOVE.L A6,rexxbguibase

	StrCopy(tempstr,rm.args[0],4)
	IF stricmp(tempstr,'bgui') THEN Raise(0)

	dos:=dosbase
	utilitybase:=dos.utilitybase
	rexxsysbase:=rm.libbase
	funcname:=rm.args[0]+4
	IF funcname[]="_" THEN funcname++

	IF stricmp(funcname,'open')=0
		/* success=bguiopen([taskname],[noerrorifopenfail])
									0					1								2					*/
		-> This function opens bgui.library. Version 41 is required.
		-> taskname indicates the name of the prefs file, default "ARexx".
		-> Predefined are: ID.WIN(CLOSE|NOMORE|INACTIVE|ACTIVE).
		-> If this function has not yet been called, all functions starting with
		-> 'bgui' cause "Error return from function".
		n:=rm.action AND $F
		IF tasknode:=findtasknode(rm)
			rc:=12
			Raise(0)
		ENDIF
		IF n>=1
			tasknode:=addtasknode(rm,rm.args[1])
		ELSE
			tasknode:=addtasknode(rm,NIL)
		ENDIF
		rc:=setRexxVar(rm,'ID.WINCLOSE','65536',STRLEN) OR
				setRexxVar(rm,'ID.WINNOMORE','131072',STRLEN) OR
				setRexxVar(rm,'ID.WININACTIVE','196608',STRLEN) OR
				setRexxVar(rm,'ID.WINACTIVE','262144',STRLEN)
		IF rc<>0
			freetasknode(rm,tasknode)
			tasknode:=0
		ENDIF
		IF tasknode
			result:=CreateArgstring('1',STRLEN)
		ELSE
			IF n>=2 AND rm.args[2]
				result:=CreateArgstring('0',STRLEN)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF (tasknode:=findtasknode(rm))=NIL
		rc:=12
	ELSEIF stricmp(funcname,'wait')=0
		/* x=bguiwait()
						0			*/
		-> Wait for most events; Control-C is passed trough to ARexx
		-> (signal on break_c).
		i:=Wait($FFFF0000 OR SIGBREAKF_CTRL_C)
		IF i AND SIGBREAKF_CTRL_C
			SetSignal(SIGBREAKF_CTRL_C,SIGBREAKF_CTRL_C)
		ENDIF
		rc:=0
		result:=CreateArgstring('1',STRLEN)
	ELSEIF stricmp(funcname,'winevent')=0
		/* id=bguiwinevent(winobj,[destvar])
							0						1				2		 */
		-> This function does not wait for events.
		IF rm.action AND $F<1
			rc:=17 -> Wrong number of arguments
		ELSE
			installhook(tasknode.idcmphook,{idcmphook_func})
			obj:=Long(rm.args[1])
			rc:=domethod(obj,[WM_HANDLEIDCMP])
			StringF(tempstr,'\d',rc)
			result:=CreateArgstring(tempstr,EstrLen(tempstr))
			rc:=0
			IF rm.args[2]
				rc:=setRexxVar(rm,rm.args[2],tempstr,EstrLen(tempstr))
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'winwaitevent')=0
		/* id=bguiwaitwinevent(winobj,[destvar])
									0						1				2		 */
		-> This function is much more efficient than the bguiwinevent()/bguiwait()
		-> combination.
		IF rm.action AND $F<1
			rc:=17 -> Wrong number of arguments
		ELSE
			installhook(tasknode.idcmphook,{idcmphook_func})
			obj:=Long(rm.args[1])
			i:=0
			WHILE (rc:=domethod(obj,[WM_HANDLEIDCMP]))=WMHI_NOMORE
				i:=Wait($FFFF0000 OR SIGBREAKF_CTRL_C)
				EXIT i AND SIGBREAKF_CTRL_C
			ENDWHILE
			IF i AND SIGBREAKF_CTRL_C
				SetSignal(SIGBREAKF_CTRL_C,SIGBREAKF_CTRL_C)
			ENDIF
			StringF(tempstr,'\d',rc)
			result:=CreateArgstring(tempstr,EstrLen(tempstr))
			rc:=0
			IF rm.args[2]
				rc:=setRexxVar(rm,rm.args[2],tempstr,EstrLen(tempstr))
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'winopen')=0
		/* success=bguiwinopen(winobj)
										0					1	 */
		-> This function must be used to open a window. When a window object is created,
		-> it does not open automatically, unlike some other GUI engines' behaviour.
		-> This enables you to do things like bguiwintabcycleorder() in time.
		IF rm.action AND $F<>1 OR (rm.args[1]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			obj:=Long(rm.args[1])
			rc:=0
			IF domethod(obj,[WM_OPEN])
				result:=CreateArgstring('1',STRLEN)
			ELSE
				result:=CreateArgstring('0',STRLEN)
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'winclose')=0
		/* x=bguiwinclose(winobj)
							0					1		*/
		IF rm.action AND $F<>1 OR (rm.args[1]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			rc:=0
			obj:=Long(rm.args[1])
			domethod(obj,[WM_CLOSE])
			result:=CreateArgstring('1',STRLEN)
		ENDIF
	ELSEIF stricmp(funcname,'winbusy')=0
		/* success=bguiwinbusy(winobj)
										0					1	 */
		-> Use this function to set a busy pointer and zero-size requester that blocks
		-> input on a window. It nests, i.e. the window will not be unblocked until
		-> one bguiwinready() has been done for each bguiwinbusy() executed.
		IF rm.action AND $F<>1 OR (rm.args[1]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			obj:=Long(rm.args[1])
			rc:=domethod(obj,[WM_SLEEP])
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'winready')=0
		/* success=bguiwinready(winobj)
										0					1		*/
		IF rm.action AND $F<>1 OR (rm.args[1]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			obj:=Long(rm.args[1])
			rc:=domethod(obj,[WM_WAKEUP])
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'window')=0
		/* obj=bguiwindow(title,mastergroup,[scalew],[scaleh],[refwin],[scrname],[pos],[menu])
								0				1				2						3				4					5				 6			 7			8	 */
		-> scale[wh]: 0% = minimum size, 100% = full screen
		-> Precede scalew and/or scaleh with a minus sign to lock the dimension.
		-> Locking both removes the size gadget.
		-> refwin: the new window object will open on the same screen as refwin
		-> pos 'S' centre on screen
		->     'M' centre on mouse pointer
		->     'T' top-left of the screen
		->     'W' centre on window
		->     'B' l t w h position in screen (pixel) coordinates
		->     'R' l t w h centre window on this box
		->     '*' (anywhere) do not activate window upon opening (doesn't really
		->         fit here but this saves arguments)
		-> menu: concatenate bguimenu() results
		rc,result:=createwindow(rm,templist,tempstr)
	ELSEIF stricmp(funcname,'layout')=0
		/* info=bguilayout(tag,value,[tag],[value],...)
								0				1		2			3				4				*/
		-> Concatenate the return value of this to an object. It does not work for
		-> VarSpace members.
		IF (n:=rm.action AND $F)<2 OR Odd(n)
			rc:=17
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSE
			rc:=0
			ListCopy(templist,[-1])
			FOR q:=1 TO n-1 STEP 2
				t:=findtag(rm.args[q])
				SELECT TAG_MAX OF t.type
					CASE TAG_INT; listadd2(templist,t.value,Val(rm.args[q+1]))
					DEFAULT; rc:=18 -> Invalid argument to function
				ENDSELECT
				EXIT rc<>0
			ENDFOR
			IF rc=0
				ListAdd(templist,[TAG_DONE])
				result:=CreateArgstring(templist,ListLen(templist)*4)
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'varspace')=0
		/* info=bguivarspace(weight)
									0					1	 */
		-> A weight controlled spacing in a group; 1<weight<1023
		-> bguilayout() cannot be used on this.
		IF (rm.action AND $F)<>1
			rc:=17
		ELSEIF rm.args[1]=NIL
			rc:=17 -> Wrong number of arguments
		ELSEIF (q:=Val(rm.args[1]))<=0
			rc:=18 -> Invalid argument to function
		ELSEIF q>=1024
			rc:=18 -> Invalid argument to function
		ELSE
			rc:=0
			result:=CreateArgstring({q},SIZEOF LONG)
		ENDIF
	ELSEIF stricmp(funcname,'hgroup')=0 OR ((q:=stricmp(funcname,'vgroup'))=0)
		/* obj=bgui[hv]group(objects,[spacing],[offset],[title],[equal])
								0						1				2					3				4		 		 5	 */
		-> Double use of objects or forgotten '||' is caught -> error 12.
		-> spacing/offset -1 narrow
		->                -2 normal (default, unlike groupclass itself)
		->                -3 wide
		-> offset 'F' frame (required for a title)
		-> equal 'W' all objects the same width
		->       'H' all objects the same height
		->       'S' all objects the same width and height
		IF q=0
			ListCopy(templist,[GROUP_Style,GRSTYLE_VERTICAL])
		ELSE
			SetList(templist,0)
		ENDIF
		IF (n:=rm.action AND $F)<1
			rc:=17
		ELSEIF rm.args[1]=NIL
			rc:=17
		ELSE
			IF rm.args[2] AND (n>=2)
				listadd2(templist,GROUP_Spacing,Val(rm.args[2]))
			ELSE
				listadd2(templist,GROUP_Spacing,GRSPACE_NORMAL)
			ENDIF
			IF rm.args[3] AND (n>=3)
				IF Char(rm.args[3])="f" OR (Char(rm.args[3])="F")
					ListAdd(templist,[
						GROUP_HorizOffset,GRSPACE_NORMAL,
						GROUP_VertOffset,GRSPACE_NORMAL,
						FRM_Type,FRTYPE_DEFAULT
					])
				ELSE
					listadd2(templist,GROUP_HorizOffset,Val(rm.args[3]))
					listadd2(templist,GROUP_VertOffset,Val(rm.args[3]))
				ENDIF
			ENDIF
			IF rm.args[4] AND (n>=4)
				listadd2(templist,FRM_Title,addstring(rm.args[4]))
			ENDIF
			IF rm.args[5] AND (n>=5)
				IF (c:=Char(rm.args[5]))="w" OR (c="W")
					ListAdd(templist,[GROUP_EqualWidth,TRUE])
				ELSEIF c="h" OR (c="H")
					ListAdd(templist,[GROUP_EqualHeight,TRUE])
				ELSEIF c="s" OR (c="S")
					ListAdd(templist,[GROUP_EqualWidth,TRUE,GROUP_EqualHeight,TRUE])
				ENDIF
			ENDIF
			rc:=0
			plong:=rm.args[1]
			q:=plong+LengthArgstring(rm.args[1])
			WHILE q-plong>=4
				obj:=plong[]
				IF obj>0 AND (obj<1024)
					listadd2(templist,GROUP_SpaceObject,obj)
				ELSEIF obj=-1
					IF (n:=ListLen(templist))>=4 AND (templist[n-1]=0) AND (templist[n-2]=0)
						SetList(templist,n-2)
						plong++
						WHILE q-plong>=4
							EXIT plong[]=0
							listadd1(templist,plong[])
							plong++
						ENDWHILE
						ListAdd(templist,[TAG_END,0])
					ELSE
						WHILE q-plong>=4
							EXIT plong[]=0
							plong++
						ENDWHILE
					ENDIF
				ELSEIF removeobjnode(obj)
					listadd2(templist,GROUP_Member,obj)
					ListAdd(templist,[TAG_END,0])
				ELSE
					rc:=18 -> Invalid argument to function
				ENDIF
				plong++
			ENDWHILE
			ListAdd(templist,[TAG_DONE])
			obj:=NIL
			IF obj:=BgUI_NewObjectA(BGUI_GROUP_GADGET,templist)
				IF rc
					DisposeObject(obj)
				ELSE
					addobjnode(obj)
					result:=CreateArgstring({obj},4)
				ENDIF
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'pages')=0
		/* obj=bguipages(name,objects)
							 0			1			 2	 */
		-> This function only creates OBJ.name, not ID.name.
		-> Do not set PAGE_Active values which are out of range!
		-> The usage of pages increases the number of objects that can be in a window
		-> and therefore object creation time. The BGUI prefs program is an example of
		-> this!
		IF rm.action AND $F<>2 OR (rm.args[1]=NIL) OR (rm.args[2]=NIL)
			rc:=17
		ELSE
			ListCopy(templist,[TAG_IGNORE,0])
			rc:=0
			plong:=rm.args[2]
			q:=plong+LengthArgstring(rm.args[2])
			WHILE q-plong>=4
				obj:=plong[]
				IF removeobjnode(obj)
					listadd2(templist,PAGE_Member,obj)
				ELSE
					rc:=18 -> Invalid argument to function
				ENDIF
				plong++
			ENDWHILE
			ListAdd(templist,[TAG_DONE])
			obj:=NIL
			IF obj:=BgUI_NewObjectA(BGUI_PAGE_GADGET,templist)
				IF rc
					DisposeObject(obj)
				ELSE
					addobjnode(obj)
					StringF(tempstr,'OBJ.\s',rm.args[1])
					UpperStr(tempstr)
					rc:=setRexxVar(rm,tempstr,{obj},4)
					result:=CreateArgstring({obj},4)
				ENDIF
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'button')=0
		/* obj=bguibutton(name,label)
								0				1		 2	*/
		-> As other "real" gadget creation functions, this one sets both ID.name
		-> (compare with bguiwinevent() or bguiwinwaitevent() result) and OBJ.name (for
		-> bguiget() and similar functions).
		->
		-> Prefix the keyboard shortcut with '_' in label. This also applies to other
		-> gadgets. Take care that all shortcuts are different! BGUI does not check
		-> this (MUI doesn't either). This type of bug is present in much software.
		IF rm.action AND $F<>2
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			listadd2(templist,LAB_Label,addstring(rm.args[2]))
			listadd1(templist,TAG_DONE)
			IF obj:=BgUI_NewObjectA(BGUI_BUTTON_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'toggle')=0
		/* obj=bguitoggle(name,label,[state])
								0				1		 2			3		*/
		IF (n:=rm.action AND $F)<2 OR (n>3)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			listadd2(templist,LAB_Label,addstring(rm.args[2]))
			IF rm.args[3]
				listadd2(templist,GA_SELECTED,Val(rm.args[3]))
			ELSE
				ListAdd(templist,[GA_SELECTED,FALSE])
			ENDIF
			ListAdd(templist,[GA_TOGGLESELECT,TRUE,TAG_DONE])
			IF obj:=BgUI_NewObjectA(BGUI_BUTTON_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'checkbox')=0
		/* obj=bguicheckbox(name,[label],[state])
								0					1			2				3		*/
		IF (n:=rm.action AND $F)<2 OR (n>3)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			IF rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			IF rm.args[3]
				listadd2(templist,GA_SELECTED,Val(rm.args[3]))
			ELSE
				ListAdd(templist,[GA_SELECTED,FALSE])
			ENDIF
			ListAdd(templist,[TAG_DONE])
			IF obj:=BgUI_NewObjectA(BGUI_CHECKBOX_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'ibutton')=0
		/* obj=bguiibutton(name,itype,idata,[label],[lplace])
								0				1			2			3			 4				5		*/
		-> Image button.
		->
		-> itype 'B' BGUI built-in
		->       'I' icon
		->       'M' bitmap image
		->       'V' vector (not yet implemented)
		->       '-' (anywhere) no frame
		->       '*' (anywhere) no spacing between frame and image
		-> idata (itype='B') number, 'F'=getfile, 'P'=popup, 'D'=getpath
		->       (itype='I') filename without .info
		->       (itype='M') one or two results of bguiimage() (normal & selected)
		->       (itype='V') string
		-> lplace 'L' left (default)
		->        'R' right
		->        'A' above
		->        'B' below
		rc,result:=createibutton(rm,tempstr,templist)
	ELSEIF stricmp(funcname,'info')=0
		/* obj=bguiinfo(name,[label],contents)
							0				1			2				3		 */
		-> Non-clickable information object.
		IF rm.action AND $F<>3
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			listadd2(templist,INFO_FixTextWidth,TRUE)
			listadd2(templist,FRM_Type,FRTYPE_DEFAULT)
			IF rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			listadd2(templist,INFO_TextFormat,q:=rm.args[3])
			n:=1; WHILE c:=q[]++ DO IF c="\n" THEN n++
			listadd2(templist,INFO_MinLines,n)
			listadd1(templist,TAG_DONE)
			IF obj:=BgUI_NewObjectA(BGUI_INFO_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'string')=0
		/* obj=bguistring(name,[label],[contents],maxchars)
								0				1			2					3					4		*/
		IF rm.action AND $F<>4
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[4]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			IF rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			IF rm.args[3]
				listadd2(templist,STRINGA_TEXTVAL,rm.args[3])
			ENDIF
			listadd2(templist,STRINGA_MAXCHARS,Val(rm.args[4]))
			listadd1(templist,TAG_DONE)
			IF obj:=BgUI_NewObjectA(BGUI_STRING_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'integer')=0
		/* obj=bguiinteger(name,[label],[contents],[maxchars],[min],[max])
								0				1			2					3						4				5			6	 */
		-> It is not guaranteed, that min<=bguiget(int,STRINGA_LongVal)<=max.
		IF (n:=rm.action AND $F)>6
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			IF n>=2
				IF rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			ENDIF
			IF n>=3 AND rm.args[3]
				listadd2(templist,STRINGA_LONGVAL,Val(rm.args[3]))
			ELSE
				listadd2(templist,STRINGA_LONGVAL,0)
			ENDIF
			IF n>=4 AND rm.args[4]
				listadd2(templist,STRINGA_MAXCHARS,Val(rm.args[4]))
			ELSE
				listadd2(templist,STRINGA_MAXCHARS,12)
			ENDIF
			IF n>=5 AND rm.args[5]
				listadd2(templist,STRINGA_IntegerMin,Val(rm.args[5]))
			ELSE
				listadd2(templist,STRINGA_IntegerMin,$80000000)
			ENDIF
			IF n>=6 AND rm.args[6]
				listadd2(templist,STRINGA_IntegerMax,Val(rm.args[6]))
			ELSE
				listadd2(templist,STRINGA_IntegerMax,$7FFFFFFF)
			ENDIF
			listadd1(templist,TAG_DONE)
			IF obj:=BgUI_NewObjectA(BGUI_STRING_GADGET,templist)
				IF rm.args[3]=NIL OR (n<3)
					GetAttr(STRINGA_TEXTVAL,obj,{q})
					SetGadgetAttrsA(obj,NIL,NIL,[STRINGA_LONGVAL,0,TAG_DONE])
					q[]:=0
				ENDIF
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'listview')=0
		/* obj=bguilistview(name,[label],[stem],[type],[ncolumns])
								0					1			2				3			4					5		 */
		-> type 'R' read-only
		->      'S' select (default)
		->      'D' drag'n'drop
		-> In multi-column listview entries, the columns are separated by a tab
		-> character. Attributes set with escape sequences do not continue from
		-> one column to the next.
		-> Optionally, you can append column weights to ncolumns, e.g. '3 10 45 45' to
		-> create one small column and two wider ones.
		IF (n:=rm.action AND $F)<1 OR (n>5)
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			IF n>=2 AND rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			IF n>=4 AND (q:=rm.args[4])
				c:=q[]
				IF c="R" OR (c="r")
					ListAdd(templist,[LISTV_ReadOnly,TRUE])
				ELSEIF c="D" OR (c="d")
					ListAdd(templist,[LISTV_ShowDropSpot,TRUE,BT_DragObject,TRUE,BT_DropObject,TRUE])
				ENDIF
			ENDIF
			IF n>=5 AND (q:=rm.args[5])
				listadd2(templist,LISTV_Columns,Val(q,{c}))
				ListAdd(templist,[LISTV_DragColumns,TRUE])
				q:=q+c
				IF q[]>"9" OR (q[]<"0") THEN q++
				plong:=tempstr
				WHILE c
					plong[]:=Val(q,{c})
					EXIT c=0
					q:=q+c
					IF q[]>"9" OR (q[]<"0") THEN q++
					plong++
				ENDWHILE
				IF plong<>tempstr THEN listadd2(templist,LISTV_ColumnWeights,tempstr)
			ENDIF
			listadd1(templist,TAG_DONE)
			IF obj:=BgUI_NewObjectA(BGUI_LISTVIEW_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
				IF n>=3 AND (q:=rm.args[3])
					StringF(tempstr,'\s.COUNT',q)
					UpperStr(tempstr)
					IF getRexxVar(rm,tempstr,{n})=0
						n:=Val(n)
						FOR i:=0 TO n-1
							StringF(tempstr,'\s.\d',q,i)
							UpperStr(tempstr)
							IF getRexxVar(rm,tempstr,{c})=0
								ListCopy(templist,[LVM_ADDSINGLE,NIL])
								listadd1(templist,c)
								ListAdd(templist,[LVAP_TAIL,0])
								domethod(obj,templist)
							ENDIF
						ENDFOR
					ENDIF
				ENDIF
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'mx')=0
		/* obj=bguimx(name,[label],stem,[type])
							0			1			2			3			4		*/
		-> type 'R' radio buttons (default)
		->      'T' tabs
		->      'U' upside down tabs
		-> The number of entries is limited to 63 (it must fit on the screen, so it's not
		-> much of a limitation)
		IF (n:=rm.action AND $F)<3 OR (n>4)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[3]=NIL)
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			ListAdd(templist,[LAB_NoPlaceIn,TRUE])
			IF n>=2 AND rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			IF n>=4 AND (q:=rm.args[4])
				c:=q[]
				IF c="T" OR (c="t")
					ListAdd(templist,[MX_TabsObject,TRUE])
				ELSEIF c="U" OR (c="u")
					ListAdd(templist,[MX_TabsObject,TRUE,MX_TabsUpsideDown,TRUE])
				ELSE
					ListAdd(templist,[GROUP_Style,GRSTYLE_VERTICAL])
				ENDIF
			ENDIF
			q:=rm.args[3]
			StringF(temp2str,'\s.COUNT',q)
			UpperStr(temp2str)
			plong:=tempstr
			IF getRexxVar(rm,temp2str,{n})=0
				n:=Bounds(Val(n),0,63)
				FOR i:=0 TO n-1
					StringF(temp2str,'\s.\d',q,i)
					UpperStr(temp2str)
					IF getRexxVar(rm,temp2str,{c})=0
						plong[]++:=addstring(c)
					ENDIF
				ENDFOR
				IF plong=tempstr THEN plong[]++:=addstring('\eb\ei\euNO LABELS')
				plong[]:=NIL
				listadd2(templist,MX_Labels,tempstr)
				listadd1(templist,TAG_DONE)
				IF obj:=BgUI_NewObjectA(BGUI_MX_GADGET,templist)
					addobjnode(obj)
					StringF(tempstr,'OBJ.\s',rm.args[1])
					UpperStr(tempstr)
					rc:=setRexxVar(rm,tempstr,{obj},4)
					result:=CreateArgstring({obj},4)
				ELSE
					rc:=12
				ENDIF
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'cycle')=0
		/* obj=bguicycle(name,[label],stem,[type])
							 0			1			 2			3			4	 */
		-> type 'P' force popup regardless of BGUI prefs
		->      'N' popup if enabled in BGUI prefs (default)
		-> Pop on active setting is taken from BGUI prefs
		-> The number of entries is limited to 63 (BGUI popup cycles truncate a list that
		-> doesn't fit on the screen, so it's not much of a limitation)
		IF (n:=rm.action AND $F)<3 OR (n>4)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[3]=NIL)
		ELSE
			SetList(templist,0)
			StringF(tempstr,'ID.\s',rm.args[1])
			UpperStr(tempstr)
			tasknode.lastid:=tasknode.lastid+1
			q:=tasknode.lastid
			setRexxVar(rm,tempstr,StringF(temp2str,'\d',q),EstrLen(temp2str))
			listadd2(templist,GA_ID,q)
			ListAdd(templist,[LAB_NoPlaceIn,TRUE])
			IF n>=2 AND rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			IF n>=4 AND (q:=rm.args[4])
				c:=q[]
				IF c="P" OR (c="p")
					ListAdd(templist,[CYC_Popup,TRUE])
				ENDIF
			ENDIF
			q:=rm.args[3]
			StringF(temp2str,'\s.COUNT',q)
			UpperStr(temp2str)
			plong:=tempstr
			IF getRexxVar(rm,temp2str,{n})=0
				n:=Bounds(Val(n),0,63)
				FOR i:=0 TO n-1
					StringF(temp2str,'\s.\d',q,i)
					UpperStr(temp2str)
					IF getRexxVar(rm,temp2str,{c})=0
						plong[]++:=addstring(c)
					ENDIF
				ENDFOR
				IF plong=tempstr THEN plong[]++:=addstring('\eb\ei\euNO LABELS')
				plong[]:=NIL
				listadd2(templist,CYC_Labels,tempstr)
				listadd1(templist,TAG_DONE)
				IF obj:=BgUI_NewObjectA(BGUI_CYCLE_GADGET,templist)
					addobjnode(obj)
					StringF(tempstr,'OBJ.\s',rm.args[1])
					UpperStr(tempstr)
					rc:=setRexxVar(rm,tempstr,{obj},4)
					result:=CreateArgstring({obj},4)
				ELSE
					rc:=12
				ENDIF
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'progress')=0
		/* obj=bguiprogress(name,[label],min,max,[format],[type])
								0					1			2			3		4			5				6		*/
		-> format: C-style format string for text in the gadget. Use %ld as a
		-> placeholder for the number. Other percent signs must be doubled. Other
		-> possibilities: "%03ld" forces a field size of three digits and pads on the
		-> left with zeroes. "%lD" formats the number according to the language selected
		-> and works only if locale.library is installed. "%lx" creates a hexadecimal
		-> format.
		-> BGUI 41.10 draws the text in the last colour used (if done=max then the
		-> progress bar colour else the background colour) because of a bug, therefore
		-> the format string should begin with ESC'd2'ESC||ESC (not necessarily 2).
		-> ESC'D'n is also recommended if the text should be always visible on two
		-> colour screens or screens with strange pens (e.g. non-newlook (no 3D look)).
		-> If you don't want the problems of a format string, then you can use
		-> bguiaddmap() to put the number in an indicator next to the progress bar.
		-> type 'H' horizontal
		->      'V' vertical
		-> The amount done is set to zero or the minimum.
		IF (n:=rm.action AND $F)<4 OR (n>6)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[3]=NIL) OR (rm.args[4]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			IF rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
			listadd2(templist,PROGRESS_Min,Val(rm.args[3]))
			listadd2(templist,PROGRESS_Max,Val(rm.args[4]))
			IF n>=5 AND rm.args[5]
				listadd2(templist,PROGRESS_FormatString,addstring(rm.args[5]))
			ENDIF
			IF n>=6 AND rm.args[6]
				IF (c:=Char(rm.args[6]))="V" OR (c="v")
					ListAdd(templist,[PROGRESS_Vertical,TRUE])
				ENDIF
			ENDIF
			ListAdd(templist,[TAG_DONE])
			IF obj:=BgUI_NewObjectA(BGUI_PROGRESS_GADGET,templist)
				addobjnode(obj)
				StringF(tempstr,'OBJ.\s',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,{obj},4)
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'indicator')=0
		/* obj=bguiindicator(name,[label],min,max,[level],[justify],[format])
								 0				1			 2		 3	 4		 5				6		 			7		*/
		-> Textual level indication gadget.
		-> justify 'L' left (default)
		->         'C' centre
		->         'R' right
		-> format: the same thing as in bguiprogress() except that setting colours is
		-> not necessary and that it has a default "%ld".
		rc,result:=createindicator(rm,templist)
	ELSEIF stricmp(funcname,'hseparator')=0
		/* obj=bguihseparator([title])
									0					 1	 */
		-> Horizontal separator line with optional title.
		-> Use in vertical groups only.
		-> This function does not take a name argument because modifying the object is not
		-> supported by RexxBGUI and not useful anyway.
		IF (n:=rm.action AND $F)>1
			rc:=17 -> Wrong number of arguments
		ELSE
			SetList(templist,0)
			listadd2(templist,SEP_Horiz,TRUE)
			IF n>=1 AND rm.args[1] THEN listadd2(templist,SEP_Title,addstring(rm.args[1]))
			listadd1(templist,TAG_DONE)
			IF obj:=BgUI_NewObjectA(BGUI_SEPARATOR_GADGET,templist)
				addobjnode(obj)
				rc:=0
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'vseparator')=0
		/* obj=bguivseparator()
									0				*/
		-> Vertical separator line.
		-> Use in horizontal groups only.
		-> This function does not take a name argument because modifying the object is not
		-> supported by RexxBGUI and not useful anyway.
		IF rm.action AND $F
			rc:=17 -> Wrong number of arguments
		ELSE
			IF obj:=BgUI_NewObjectA(BGUI_SEPARATOR_GADGET,[SEP_Horiz,FALSE,TAG_DONE])
				addobjnode(obj)
				rc:=0
				result:=CreateArgstring({obj},4)
			ELSE
				rc:=12
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'get')=0
		/* res=bguiget(obj,attr)
							0			1		2	 */
		-> Error descriptions here:
		-> "Invalid keyword" if you supplied an attribute that RexxBGUI does not know -
		-> see "tags" further on in this file for which attributes are available.
		-> "Error return from function" if the obj parameter was not OK or if the
		-> object did not understand the attribute, for example an attribute of another
		-> gadget class or one that is not gettable;
		-> "Wrong number of arguments" if rm->rm_Action & 0xF is not 2 or if
		-> rm_Args[1] or rm_Args[2] is equal to zero.
		IF rm.action AND $F<>2 OR (rm.args[1]=NIL) OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSEIF (t:=findtag(rm.args[2]))=NIL
			rc:=33 -> Invalid keyword
		ELSE
			rc:=0
			SELECT TAG_MAX OF t.type
				CASE TAG_INT
					IF GetAttr(t.value,Long(rm.args[1]),{q})
						StringF(tempstr,'\d',q)
						result:=CreateArgstring(tempstr,EstrLen(tempstr))
					ELSE
						rc:=18 -> Invalid argument to function
					ENDIF
				CASE TAG_STRING
					IF GetAttr(t.value,Long(rm.args[1]),{q})
						IF q
							result:=CreateArgstring(q,StrLen(q))
						ELSE
							result:=CreateArgstring('',0)
						ENDIF
					ELSE
						rc:=18 -> Invalid argument to function
					ENDIF
				CASE TAG_OBJECT
					IF GetAttr(t.value,Long(rm.args[1]),{q})
						result:=CreateArgstring({q},4)
					ELSE
						rc:=18
					ENDIF
				DEFAULT; rc:=18 -> Invalid argument to function
			ENDSELECT
		ENDIF
	ELSEIF stricmp(funcname,'set')=0
		/* x=bguiset(obj,[refwin],tag,value,[tag],[value],...)
						0			1			2			 3		4			5			 6			 */
		-> refwin: window object in which the gadget is located; necessary if you want
		-> visual updating. Specifying another window causes strange results...
		-> If you need to set more than 6 tags, split it out over multiple calls and
		-> omit refwin in all possibly except the last. Put at least one attribute
		-> causing visual update in the last call, if you want any update.
		IF (n:=rm.action AND $F)<4 OR Odd(n) OR (rm.args[1]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			wnd:=NIL
			IF rm.args[2]
				IF LengthArgstring(rm.args[2])=4
					GetAttr(WINDOW_Window,Long(rm.args[2]),{wnd})
				ENDIF
			ENDIF
			obj:=Long(rm.args[1])
			SetList(templist,0)
			rc:=0
			FOR q:=3 TO n-1 STEP 2
				t:=findtag(rm.args[q])
				IF t=NIL THEN rc:=33 -> Invalid keyword
				EXIT t=NIL
				SELECT TAG_MAX OF t.type
					CASE TAG_INT; listadd2(templist,t.value,Val(rm.args[q+1]))
					CASE TAG_STRING
						IF t.value=BT_Key
							PutChar(obj+40,Char(rm.args[q+1]))
							PutChar(obj+41,0)
							listadd2(templist,t.value,obj+40)
						ELSE
							listadd2(templist,t.value,rm.args[q+1])
						ENDIF
					CASE TAG_OBJECT
						IF LengthArgstring(rm.args[q+1])=4
							listadd2(templist,t.value,Long(rm.args[q+1]))
						ELSE
							rc:=18
						ENDIF
					DEFAULT; rc:=18 -> Invalid argument to function
				ENDSELECT
				EXIT rc<>0
			ENDFOR
			IF rc=0
				ListAdd(templist,[TAG_DONE])
				rc:=SetGadgetAttrsA(obj,wnd,NIL,templist)
				StringF(tempstr,'\d',rc)
				result:=CreateArgstring(tempstr,EstrLen(tempstr))
				rc:=0
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'req')=0
		/* gad=bguireq(text,gadgets,[title],[refwin],[scrname])
							0			1			2				 3				4				 5		*/
		-> RequestChoice/rtEZRequest() like requester. Use '|' to separate the gadgets.
		-> An asterisk adds return as a shortcut for the button it stands before and
		-> escape for the last one. Underscores can be used to prefix keyboard shortcuts.
		-> The window specified by refwin will automatically get a busypointer.
		-> Title defaults to refwin's title, or "BGUI Request" or its localized
		-> equivalent if wndobj is not given.
		-> Scrname does not work.
		-> gad: 1 for the first, 2 for the second, etc. and 0 for the last gadget.
		IF (n:=rm.action AND $F)<2
			rc:=17 -> Wrong number of arguments
		ELSE
			IF n>=5 AND rm.args[5]
				obj:=NIL
				obj:=LockPubScreen(rm.args[5])
				templist[]:=rm.args[1]
				rc:=bguireq(NIL,n>=3 AND rm.args[3],'\s',rm.args[2],templist,obj BUT NIL)
				IF obj THEN UnlockPubScreen(rm.args[5],obj)
			ELSE
				q:=NIL
				IF n>=4 AND rm.args[4]
					IF LengthArgstring(rm.args[4])=4
						obj:=Long(rm.args[4])
						GetAttr(WINDOW_Window,obj,{q})
					ENDIF
				ENDIF
				templist[]:=rm.args[1]
				rc:=bguireq(q,n>=3 AND rm.args[3],'\s',rm.args[2],templist)
			ENDIF
			StringF(tempstr,'\d',rc)
			result:=CreateArgstring(tempstr,EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'addmap')=0
		/* success=bguiaddmap(srcobj,dstobj,sattr,dattr)
										0				1				2			3			4	 */
		-> This function connects two objects so srcobj will update dstobj's dattr
		-> when its sattr changes. This happens even without bguiwinevent().
		->
		-> Example usages:
		-> Listview with connected string gadget:
		-> call bguiaddmap(obj.listv,obj.str,LISTV_Entry,STRINGA_TextVal)
		-> Listview to select a page (#of listview entries must be #of pages!!!):
		-> call bguiaddmap(obj.listv,obj.pages,LISTV_EntryNumber,PAGE_Active)
		->
		-> The number of attributes that can cause notification (sattr) or be updated
		-> (dattr) is more limited than the number that can be used with bguiget() or
		-> bguiset(). If notification does not work, you'll have to fall back to the
		-> latter mechanism.
		->
		-> There is no "initial" notification.
		->
		-> Notification of LGO attributes may cause a crash. Using bguiset() is safer.
		IF rm.action AND $F<>4
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL) OR (rm.args[3]=NIL) OR (rm.args[4]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSEIF LengthArgstring(rm.args[2])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[BASE_ADDMAP])
			listadd1(templist,Long(rm.args[2]))
			listadd1(templist,templist+12)
			rc:=18 -> Invalid argument to function
			IF t:=findtag(rm.args[3])
				listadd1(templist,t.value)
				IF t:=findtag(rm.args[4])
					listadd1(templist,t.value)
					ListAdd(templist,[TAG_DONE])
					rc:=domethod(Long(rm.args[1]),templist)
					result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
					rc:=0
				ENDIF
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'addcondit')=0
		/* success=bguiaddcondit(srcobj,dstobj,iftag,ifval,thtag,thval,eltag,elval)
										0						1			2			 3		 4		 5		 6		 7		 8	*/
		-> This function connects two objects so srcobj will update dstobj depending
		-> on its ifattr. This is more or less equivalent to doing this when srcobj's
		-> iftag changes:
		-> IF bguiget(srcobj,iftag)=ifval THEN CALL bguiset(dstobj,thtag,thval),
		->   ELSE CALL bguiset(dstobj,eltag,elval)
		-> This happens even without bguiwinevent(). See also bguiaddmap().
		->
		-> Integer tags only.
		->
		-> Always test boolean tags for 0 and never for 1.
		->
		-> Example usage:
		-> Yes/No setting that makes a choice impossible:
		-> call bguiaddcondit(obj.check,obj.mx,GA_Selected,0,MX_EnableButton,0,
		-> ,MX_DisableButton,0)
		->
		-> MX_DisableButton,n will never change MX_Active. You have to check separately
		-> if the selection is still possible.
		->
		-> If you init the checkbox to on, the first MX button will not be enabled
		-> initially. If you init the checkbox to off and set it to on after this call,
		-> it will be OK.
		IF rm.action AND $F<>8
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL) OR (rm.args[3]=NIL) OR (rm.args[4]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[5]=NIL OR (rm.args[6]=NIL) OR (rm.args[7]=NIL) OR (rm.args[8]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSEIF LengthArgstring(rm.args[2])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[BASE_ADDCONDITIONAL])
			listadd1(templist,Long(rm.args[2]))
			rc:=18 -> Invalid argument to function
			IF t:=findtag(rm.args[3])
				listadd2(templist,t.value,Val(rm.args[4]))
				IF t:=findtag(rm.args[5])
					listadd2(templist,t.value,Val(rm.args[6]))
					IF t:=findtag(rm.args[7])
						listadd2(templist,t.value,Val(rm.args[8]))
						rc:=domethod(Long(rm.args[1]),templist)
						result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
						rc:=0
					ENDIF
				ENDIF
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'listvgetentries')=0
		/* count=bguilistvgetentries(listvobj,stemname,[type])
												0						1					2				3	 */
		-> type 'A' all
		->      'S' selected only
		IF (n:=rm.action AND $F)<2 OR (rm.args[1]=NIL) OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			rc:=0
			obj:=Long(rm.args[1])
			q:=rm.args[2]
			flags:=0
			IF rm.args[3] AND (n>=3)
				IF (c:=Char(rm.args[3]))="S" OR (c="s") THEN flags:=LVGEF_SELECTED
			ENDIF
			n:=0
			ListCopy(templist,[LVM_FIRSTENTRY,NIL])
			listadd1(templist,flags)
			i:=domethod(obj,templist)
			templist[]:=LVM_NEXTENTRY
			WHILE i
				StringF(tempstr,'\s.\d',q,n)
				UpperStr(tempstr)
				rc:=12
				EXIT setRexxVar(rm,tempstr,i,StrLen(i))
				rc:=0
				n++
				templist[1]:=i
				i:=domethod(obj,templist)
			ENDWHILE
			IF rc=0
				StringF(temp2str,'\s.COUNT',q)
				UpperStr(temp2str)
				IF setRexxVar(rm,temp2str,StringF(tempstr,'\d',n),EstrLen(tempstr))
					rc:=12
				ELSE
					rc:=0
					result:=CreateArgstring(tempstr,EstrLen(tempstr))
				ENDIF
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'listvaddentry')=0
		/* success=bguilistvaddentry(listvobj,[refwin],entry,[where],[flags])
											0							1					2				3			4				5		*/
		-> where 'H' head
		->       'T' tail
		->       'S' sorted
		->       number add before entry n
		-> flags 'S' select
		->       'M' multiselect
		->       '+' make visible by scrolling list if necessary
		->       '-' do not make visible
		-> 'S' or 'M' makes visible automatically unless disabled with '-'
		IF (n:=rm.action AND $F)<3 OR (rm.args[1]=NIL) OR (rm.args[3]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			rc:=0
			wnd:=NIL
			IF rm.args[2]
				IF LengthArgstring(rm.args[2])=4
					GetAttr(WINDOW_Window,Long(rm.args[2]),{wnd})
				ENDIF
			ENDIF
			ListCopy(templist,[LVM_ADDSINGLE,NIL])
			listadd1(templist,rm.args[3])
			ListAdd(templist,[LVAP_TAIL])
			IF n>=4 AND rm.args[4]
				IF (c:=Char(rm.args[4]))>="0" AND (c<="9") OR (c="%") OR (c="$")
					ListCopy(templist,[LVM_INSERTSINGLE,NIL])
					listadd1(templist,Val(rm.args[4]))
					listadd1(templist,rm.args[3])
				ELSE
					ListCopy(templist,[LVM_ADDSINGLE,NIL])
					listadd1(templist,rm.args[3])
					IF c="H" OR (c="h")
						ListAdd(templist,[LVAP_HEAD])
					ELSEIF c="S" OR (c="s")
						ListAdd(templist,[LVAP_SORTED])
					ELSE
						ListAdd(templist,[LVAP_TAIL])
					ENDIF
				ENDIF
			ENDIF
			flags:=0
			IF n>=5 AND rm.args[5]
				IF (c:=Char(rm.args[5]))="s" OR (c="S")
					flags:=flags OR LVASF_SELECT
				ELSEIF c="m" OR (c="M")
					flags:=flags OR LVASF_MULTISELECT
				ENDIF
				IF strchr(rm.args[5],"+")
					flags:=flags OR LVASF_MAKEVISIBLE
				ELSEIF strchr(rm.args[5],"-")
					flags:=flags OR LVASF_NOT_VISIBLE
				ENDIF
			ENDIF
			listadd1(templist,flags)
			rc:=domethod(Long(rm.args[1]),templist)
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
			IF wnd
				ListCopy(templist,[LVM_REFRESH,NIL])
				BgUI_DoGadgetMethodA(Long(rm.args[1]),wnd,NIL,templist)
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'listvcommand')=0
		/* rc=bguilistvcommand(listvobj,[refwin],command)
								0							1					2				3		*/
		-> command 'clear' delete all entries
		->         'refresh' refresh listview and scroller
		->         'sort'
		->         'redraw' redraw all entries only
		->         'remselected' also selects new entry
		IF (n:=rm.action AND $F)<>3 OR (rm.args[1]=NIL) OR (rm.args[3]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			rc:=0
			wnd:=NIL
			IF rm.args[2]
				IF LengthArgstring(rm.args[2])=4
					GetAttr(WINDOW_Window,Long(rm.args[2]),{wnd})
				ENDIF
			ENDIF
			c:=rm.args[3]
			IF stricmp(c,'clear')=0
				i:=LVM_CLEAR
			ELSEIF stricmp(c,'refresh')=0
				i:=LVM_REFRESH
			ELSEIF stricmp(c,'sort')=0
				i:=LVM_SORT
			ELSEIF stricmp(c,'redraw')=0
				i:=LVM_REDRAW
			ELSEIF stricmp(c,'remselected')=0
				i:=LVM_REMSELECTED
			ELSE
				rc:=33 -> Invalid keyword
			ENDIF
			IF rc=0
				SetList(templist,0)
				listadd2(templist,i,NIL)
				rc:=BgUI_DoGadgetMethodA(Long(rm.args[1]),wnd,NIL,templist)
				result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
				rc:=0
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'listvreplacesel')=0
		/* success=bguilistvreplacesel(listvobj,[refwin],new)
												0							1					2			3	*/
		-> Use this function to replace the first selected entry of a listview by another.
		-> If no entry is selected, a new one is added and selected.
		IF (n:=rm.action AND $F)<>3 OR (rm.args[1]=NIL) OR (rm.args[3]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			rc:=0
			wnd:=NIL
			IF rm.args[2]
				IF LengthArgstring(rm.args[2])=4
					GetAttr(WINDOW_Window,Long(rm.args[2]),{wnd})
				ENDIF
			ENDIF
			obj:=Long(rm.args[1])
			IF i:=domethod(obj,[LVM_FIRSTENTRY,NIL,LVGEF_SELECTED])
				ListCopy(templist,[LVM_REPLACE,NIL])
				listadd1(templist,i)
				listadd1(templist,rm.args[3])
				rc:=BgUI_DoGadgetMethodA(obj,wnd,NIL,templist)
				result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
				rc:=0
			ELSE
				ListCopy(templist,[LVM_ADDSINGLE,NIL])
				listadd1(templist,rm.args[3])
				ListAdd(templist,[LVAP_TAIL,LVASF_SELECT])
				rc:=domethod(obj,templist)
				result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
				rc:=0
				IF wnd
					ListCopy(templist,[LVM_REFRESH,NIL])
					BgUI_DoGadgetMethodA(Long(rm.args[1]),wnd,NIL,templist)
				ENDIF
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'wintabcycleorder')=0
		/* success=bguiwintabcycleorder(winobj,objects)
												0							1				2		*/
		-> RexxBGUI can only check the validity of the objects array to a limited
		-> extent: if a multiple of four spaces is intermixed wrong object pointers
		-> are used and a crash can be expected if tab is pressed in the first gadget.
		IF rm.action AND $F<>2
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSEIF LengthArgstring(rm.args[2]) AND 3
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[WM_TABCYCLE_ORDER])
			plong:=rm.args[2]
			n:=LengthArgstring(plong)/4
			FOR q:=0 TO n-1
				SetGadgetAttrsA(plong[q],NIL,NIL,[GA_TABCYCLE,TRUE,TAG_DONE])
				listadd1(templist,plong[q])
			ENDFOR
			ListAdd(templist,[NIL])
			obj:=Long(rm.args[1])
			rc:=domethod(obj,templist)
			StringF(tempstr,'\d',rc)
			result:=CreateArgstring(tempstr,EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'image')=0
		/* image=bguiimage(left,top,width,height,depth,data,planepick,planeonoff)
								 0			1		 2		3			4			 5		6				7					8			*/
		-> Creates an Image structure and returns the pointer. Use this for the idata
		-> parameter of bguiibutton() if itype='M'.
		->
		-> Create these function calls with the MakeImage program.
		rc,result:=createimage(rm)
	ELSEIF stricmp(funcname,'menu')=0
		/* nm=bguimenu(type,[label],[commkey],[flags],[mutualexclude],[id])
							0			1			 2				3				 4					 5					6	*/
		-> Creates a NewMenu structure. Concatenate some of these and pass the result
		-> to bguiwindow(). If IDs (bguiwinevent()) are to be shared between gadgets
		-> and menuitems, create the gadgets first. Do not give a checkbox and a
		-> CHECKIT|TOGGLE item the same ID as it will be impossible to distinguish
    -> between them.
		->
		-> type '1' menu title
		->      '2' item
		->      '3' subitem
		-> Omitting label gives a separator bar (automatically disabled).
		-> commkey: RAmiga-... shortcut
		-> flags 'DISABLED'
		->       'CHECKIT' can be checked (not unchecked)
		->       'CHECKED' has checkmark
		->       'TOGGLE' specify CHECKIT too
		-> mutualexclude: selecting this CHECKIT item deselects these items
		->                (bit #x means item #x)
		-> id: may be either a number or a name of a variable
		->     number: selecting the item generates this ID
		->     variable: generate a new ID and store it in the variable
		->     Therefore you can write bguimenu(...,id.quit) and it will
		->     automatically share its ID with a gadget's, if it is present.
		->
		-> Example:
		-> menu=bguimenu(1,'Project')||,
		->       bguimenu(2,'About...','?',,,id.about)||,
		->       bguimenu(2)||,
		->       bguimenu(2,'Quit','Q',,,id.quit)||,
		->      bguimenu(1,'Testing')||,
		->       bguimenu(2,'Have your cake',,'CHECKIT CHECKED','10'b,id.hyc)||,
		->       bguimenu(2,'Eat it too',,'CHECKIT','01'b,id.eit)||,
		->       bguimenu(2,'On or off???',,'CHECKIT TOGGLE',,id.ooo)||,
		->       bguimenu(2)||,
		->       bguimenu(2,'Subitems')||,
		->        bguimenu(3,'Subitem #1','1',,,id.sub1)||,
		->        bguimenu(3,'Subitem #2','2',,,id.sub2)||,
		->        bguimenu(3,'Subitem #3','3','DISABLED',,id.sub3)
		IF (n:=rm.action AND $F)>6 OR (n<1)
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL
			rc:=17 -> Wrong number of arguments
		ELSE
			rc,result:=makenm(rm,tempstr)
		ENDIF
	ELSEIF stricmp(funcname,'windisablemenu')=0
		/* success=bguiwindisablemenu(winobj,id,state)
												0						1			2		3	 */
		IF rm.action AND $F<>3
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL) OR (rm.args[3]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[WM_DISABLEMENU])
			listadd2(templist,Val(rm.args[2]),Val(rm.args[3]))
			rc:=domethod(Long(rm.args[1]),templist)
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'wincheckitem')=0
		/* success=bguiwincheckitem(winobj,id,state)
											0						1			2		3	 */
		IF rm.action AND $F<>3
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL) OR (rm.args[3]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[WM_CHECKITEM])
			listadd2(templist,Val(rm.args[2]),Val(rm.args[3]))
			rc:=domethod(Long(rm.args[1]),templist)
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'winmenudisabled')=0
		/* rc=bguiwinmenudisabled(winobj,id)
									0							1		 2 */
		-> rc 1 disabled
		->    0 enabled
		->    -1 unknown menu/no menus at all attached to window
		IF rm.action AND $F<>2
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[WM_MENUDISABLED])
			listadd1(templist,Val(rm.args[2]))
			rc:=domethod(Long(rm.args[1]),templist)
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'winitemchecked')=0
		/* rc=bguiwinitemchecked(winobj,id)
									0							1		2	*/
		-> rc 1 checked
		->    0 not checked
		->    -1 unknown menu/no menus at all attached to window
		IF rm.action AND $F<>2
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			ListCopy(templist,[WM_ITEMCHECKED])
			listadd1(templist,Val(rm.args[2]))
			rc:=domethod(Long(rm.args[1]),templist)
			result:=CreateArgstring(StringF(tempstr,'\d',rc),EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'activategadget')=0
		/* success=bguiactivategadget(gadget,refwin)
												0						1				2	 */
		-> This call may fail, although that's usually not a severe problem...
		-> RexxBGUI can only check the validity of the objects to a limited extent.
		IF rm.action AND $F<>2
			rc:=17 -> Wrong number of arguments
		ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSEIF LengthArgstring(rm.args[1])<>4
			rc:=18 -> Invalid argument to function
		ELSEIF LengthArgstring(rm.args[2])<>4
			rc:=18 -> Invalid argument to function
		ELSE
			GetAttr(WINDOW_Window,Long(rm.args[2]),{wnd})
			rc:=ActivateGadget(Long(rm.args[1]),wnd,NIL)
			StringF(tempstr,'\d',rc)
			result:=CreateArgstring(tempstr,EstrLen(tempstr))
			rc:=0
		ENDIF
	ELSEIF stricmp(funcname,'close')=0
		/* x=bguiclose()
						 0		 */
		-> This MUST be called before exiting the script.
		freetasknode(rm,tasknode)
		rc:=0
		result:=CreateArgstring('1',STRLEN)
	ELSEIF stricmp(funcname,'readentries')=0
		/* success=bguireadentries(filename,stemname)
											0						1					2		*/
		-> When using READLN(), EOF cannot be distinguished from an empty line.
		-> Therefore this function is present. It reads lines from a file to a stem
		-> in a format suitable for the listview gadget.
		IF (n:=rm.action AND $F)<>2 OR (rm.args[1]=NIL) OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSE
			rc:=0
			IF obj:=Open(rm.args[1],OLDFILE)
				q:=rm.args[2]
				n:=0
				WHILE Fgets(obj,tempstr,StrMax(tempstr))
					IF i:=strchr(tempstr,"\n") THEN i[]:=0
					StringF(temp2str,'\s.\d',q,n)
					UpperStr(temp2str)
					rc:=12
					EXIT setRexxVar(rm,temp2str,tempstr,StrLen(tempstr))
					rc:=0
					n++
				ENDWHILE
				Close(obj)
				IF rc=0
					StringF(temp2str,'\s.COUNT',q)
					UpperStr(temp2str)
					rc:=0
					IF setRexxVar(rm,temp2str,StringF(tempstr,'\d',n),EstrLen(tempstr))
						result:=CreateArgstring('0',1)
					ELSE
						result:=CreateArgstring('1',1)
					ENDIF
				ELSE
					rc:=0
					result:=CreateArgstring('0',1)
				ENDIF
			ELSE
				rc:=0
				result:=CreateArgstring('0',1)
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'list')=0
		/* stemname=bguilist(stemname,entry0,[entry1],...)
										0				1				2				3				 */
		-> This function allows you to place small lists (<=14 entries) for listviews
		-> directly in the gadget creation function, a bit like in E [e0,e1,...].
		-> It sets stemname.COUNT and stemname.x (0<=x<stemname.COUNT)
		->
		-> Example:
		-> ...
		-> bguilistview('switcher',,bguilist('pnames','Single-Select','Multi-Select'))||,
		-> ...
		-> drop pnames. /* not really necessary */
		IF (n:=rm.action AND $F)<2 OR (rm.args[1]=NIL) OR (rm.args[2]=NIL)
			rc:=17 -> Wrong number of arguments
		ELSE
			FOR i:=0 TO n-2
				rc:=12
				EXIT rm.args[i]=NIL
				StringF(tempstr,'\s.\d',rm.args[1],i)
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,rm.args[i+2],LengthArgstring(rm.args[i+2]))
				EXIT rc
			ENDFOR
			IF rc=0
				StringF(tempstr,'\s.COUNT',rm.args[1])
				UpperStr(tempstr)
				rc:=setRexxVar(rm,tempstr,StringF(temp2str,'\d',n-1),EstrLen(temp2str))
				IF rc=0 THEN result:=CreateArgstring(rm.args[1],LengthArgstring(rm.args[1]))
			ENDIF
		ENDIF
	ELSEIF stricmp(funcname,'listvcontrol')=0
		/* x=bguilistvcontrol([listvobj],[winobj],[arrows_id],[enter_id])
								0							1					2					3						4			*/
		-> This function will cause the arrow keys up and down to control the the
		-> listview. They do only in the window specified. You can receive events for
		-> the arrow keys and for return and enter keys. You can specify either a number
		-> or a variable name which will receive a newly generated ID.
		-> Leaving out all arguments turns off this feature. If an argument is left out,
		-> (but there are arguments at all), that setting is not changed.
		-> Never ever set winobj if listvobj is invalid.
		-> Initially, winobj is zero and the other values are undefined.
		rc,result:=setlistvcontrol(rm,tempstr)
	ELSEIF stricmp(funcname,'filereq')=0
		/* path=bguifilereq([path],[title],[winobj],[flags],[patvar])
								0					1				2				3		 		 4	 			5		*/
		-> Subsequent calls will remember the path, the pattern and the requester's
		-> position and size, but not the title, the window object pointer and the flags.
		-> winobj: window object pointer. The requester will open on the same screen and
		-> the window will be locked like bguiwinbusy().
		-> flags 'DOSAVEMODE'
		->       'DOPATTERNS' pattern gadget; a pattern may be used without it
		->       'DRAWERSONLY'
		->       'FILTERDRAWERS'
		->       'REJECTICONS'
		-> patvar: name of a variable that contains the pattern. When the requester is
		-> closed, the possibly changed pattern is stored in it. If the variable does not
		-> exist, the argument is used literally as a pattern, but the changed pattern is
		-> not stored in any variable.
		-> If the requester was cancelled, the function returns the empty string, else it
		-> returns the path (the drawer and file concatenated with a slash in between if
		-> necessary).
		rc,result:=dofrq(rm,templist,tempstr)
	ELSEIF stricmp(funcname,'error')=0
		/* x=bguierror(code)
						 0			1	 */
		-> This function causes an ARexx error, e.g. bguierror(12) causes "Error return
		-> from function".
		IF rm.action AND $F<>1
			rc:=17 -> Wrong number of arguments
		ELSE
			rc:=Val(rm.args[1])
		ENDIF
	ELSEIF stricmp(funcname,'rexxversion')=0
		/* result=bguirexxversion([version],[revision])
										 0						1					2			*/
		-> Syntax #1: available=bguirexxversion(version,[revision])
		-> Syntax #2: version=bguirexxversion() /* could return '3.0' */
		IF rm.action AND $F>2
			rc:=17 -> Wrong number of arguments
		ELSE
			i:=-1; q:=0
			IF n>=1 AND rm.args[1] THEN i:=Val(rm.args[1])
			IF n>=2 AND rm.args[2] THEN q:=Val(rm.args[2])
			rc,result:=checkversion(rexxbguibase,i,q,tempstr)
		ENDIF
	ELSEIF stricmp(funcname,'version')=0
		/* result=bguiversion([version],[revision])
									0						1					2			*/
		-> Syntax #1: available=bguiversion(version,[revision])
		-> Syntax #2: version=bguiversion() /* could return '41.8' */
		IF rm.action AND $F>2
			rc:=17 -> Wrong number of arguments
		ELSE
			i:=-1; q:=0
			IF n>=1 AND rm.args[1] THEN i:=Val(rm.args[1])
			IF n>=2 AND rm.args[2] THEN q:=Val(rm.args[2])
			rc,result:=checkversion(bguibase,i,q,tempstr)
		ENDIF
	ENDIF
EXCEPT DO
	IF rc=-1 THEN rc:=12
	MOVEA.L result,A0
ENDPROC rc

PROC checkversion(lib:PTR TO lib,ver,rev,tempstr /* scratch */)
	IF ver<>-1
		IF ver<lib.version OR (ver=lib.version AND (rev<=lib.revision))
			RETURN 0,CreateArgstring('1',1)
		ELSE
			RETURN 0,CreateArgstring('0',1)
		ENDIF
	ELSE
		StringF(tempstr,'\d.\d',lib.version,lib.revision)
		RETURN 0,CreateArgstring(tempstr,EstrLen(tempstr))
	ENDIF
ENDPROC

PROC setlistvcontrol(rm:PTR TO rexxmsg,tempstr:PTR TO CHAR /* scratch */)
	DEF n,s,h:PTR TO idcmphook,i,r
	h:=tasknode.idcmphook
	IF (n:=rm.action AND $F)>4 THEN RETURN 17 -> Wrong number of arguments
	IF n=0 THEN h.window_object:=NIL
	IF n>=1 AND (s:=rm.args[1])
		IF LengthArgstring(s)=4
			h.listview:=Long(s)
		ENDIF
	ENDIF
	IF n>=2 AND (s:=rm.args[2])
		IF LengthArgstring(s)=4
			h.window_object:=Long(s)
		ENDIF
	ENDIF
	IF n>=3 AND (s:=rm.args[3])
		i,r:=Val(s)
		IF r=0
			tasknode.lastid:=tasknode.lastid+1
			i:=tasknode.lastid
			IF setRexxVar(rm,s,StringF(tempstr,'\d',i),EstrLen(tempstr)) THEN RETURN 12
		ENDIF
		h.doarrowmessages:=i
	ENDIF
	IF n>=4 AND (s:=rm.args[4])
		i,r:=Val(s)
		IF r=0
			tasknode.lastid:=tasknode.lastid+1
			i:=tasknode.lastid
			IF setRexxVar(rm,s,StringF(tempstr,'\d',i),EstrLen(tempstr)) THEN RETURN 12
		ENDIF
		h.enter_id:=i
	ENDIF
ENDPROC 0,CreateArgstring('1',1)

PROC dofrq(rm:PTR TO rexxmsg,templist:PTR TO LONG /* scratch */,
		tempstr:PTR TO CHAR /* scratch */)
	DEF rc,result,n,frq,obj,s,flags1,flags2,i,varexisted=FALSE,lockedwinobj=NIL
	IF tasknode.filereq=NIL
		tasknode.filereq:=BgUI_NewObjectA(BGUI_FILEREQ_OBJECT,NIL)
		IF tasknode.filereq=NIL THEN RETURN 12
	ENDIF
	frq:=tasknode.filereq
	n:=rm.action AND $F
	IF n>=2 AND rm.args[2]
		setattr(frq,ASLFR_TITLETEXT,rm.args[2])
	ELSE
		setattr(frq,ASLFR_TITLETEXT,NIL)
	ENDIF
	SetAttrsA(frq,[ASLFR_WINDOW,NIL,TAG_DONE])
	IF n>=3 AND rm.args[3]
		IF LengthArgstring(rm.args[3])=4
			IF lockedwinobj:=Long(rm.args[3])
				domethod(lockedwinobj,[WM_SLEEP])
				GetAttr(WINDOW_Window,lockedwinobj,{obj})
				setattr(frq,ASLFR_WINDOW,obj)
			ENDIF
		ENDIF
	ENDIF
	flags1:=0; flags2:=0
	IF s:=rm.args[4] AND (n>=4)
		WHILE s[]
			StrCopy(tempstr,'')
			WHILE s[]>="A" AND (s[]<="z") DO StrAdd(tempstr,s,1) BUT s++
			IF stricmp(tempstr,'dosavemode')=0
				flags1:=flags1 OR FRF_DOSAVEMODE
			ELSEIF stricmp(tempstr,'dopatterns')=0
				flags1:=flags1 OR FRF_DOPATTERNS
			ELSEIF stricmp(tempstr,'drawersonly')=0
				flags2:=flags2 OR FRF_DRAWERSONLY
			ELSEIF stricmp(tempstr,'filterdrawers')=0
				flags2:=flags2 OR FRF_FILTERDRAWERS
			ELSEIF stricmp(tempstr,'rejecticons')=0
				flags2:=flags2 OR FRF_REJECTICONS
			ENDIF
			WHILE s[]<"A" OR (s[]>"z") AND s[] DO s++
		ENDWHILE
	ENDIF
	setattr(frq,ASLFR_FLAGS1,flags1)
	setattr(frq,ASLFR_FLAGS2,flags2)
	IF n>=5 AND (s:=rm.args[5])
		StrCopy(tempstr,s); UpperStr(tempstr); i:=NIL
		IF getRexxVar(rm,tempstr,{i})=0 AND i
			setattr(frq,ASLFR_INITIALPATTERN,i)
			varexisted:=TRUE
		ELSE
			setattr(frq,ASLFR_INITIALPATTERN,s)
		ENDIF
	ENDIF
	IF n>=1 AND (s:=rm.args[1])
		IF flags2 AND FRF_DRAWERSONLY
			setattr(frq,FILEREQ_Drawer,s)
		ELSE
			setattr(frq,FILEREQ_File,FilePart(s))
			StrCopy(tempstr,s)
			PutChar(PathPart(tempstr),0)
			setattr(frq,FILEREQ_Drawer,tempstr)
		ENDIF
	ENDIF
	rc:=0
	IF domethod(frq,[ASLM_DOREQUEST])=ASLREQ_OK
		GetAttr(IF flags2 AND FRF_DRAWERSONLY THEN FILEREQ_Drawer ELSE FILEREQ_Path,frq,{s})
		result:=CreateArgstring(s,StrLen(s))
	ELSE
		result:=CreateArgstring('',0)
	ENDIF
	IF varexisted
		StrCopy(tempstr,rm.args[5]); UpperStr(tempstr)
		GetAttr(FILEREQ_Pattern,frq,{i})
		setRexxVar(rm,tempstr,i,StrLen(i))
	ENDIF
	IF lockedwinobj THEN domethod(lockedwinobj,[WM_WAKEUP])
ENDPROC rc,result

PROC setattr(obj,attr,val)
	DEF t[3]:ARRAY OF LONG
	t[0]:=attr
	t[1]:=val
	t[2]:=0
	SetAttrsA(obj,t)
ENDPROC

PROC createibutton(rm:PTR TO rexxmsg,tempstr /* stratch */,
		templist:PTR TO LONG /* scratch */)
	DEF rc,result,n,obj,c,i,temp2str[80]:STRING,im:PTR TO imagenode
	IF (n:=rm.action AND $F)<3 OR (n>5)
		RETURN 17 -> Wrong number of arguments
	ELSEIF rm.args[1]=NIL OR (rm.args[2]=NIL) OR (rm.args[3]=NIL)
		RETURN 17 -> Wrong number of arguments
	ENDIF
	SetList(templist,0)
	IF (c:=Char(rm.args[2]))="B" OR (c="b")
		i:=Val(rm.args[3])
		IF i=0
			c:=Char(rm.args[3])
			IF c="f" OR (c="F")
				i:=BUILTIN_GETFILE
			ELSEIF c="p" OR (c="P")
				i:=BUILTIN_POPUP
			ELSEIF c="d" OR (c="D")
				i:=BUILTIN_GETPATH
			ENDIF
		ENDIF
		listadd2(templist,VIT_BuiltIn,i)
	ELSEIF c="I" OR (c="i")
		IF (im:=addicon(rm.args[3]))=NIL THEN RETURN 12
		listadd2(templist,BUTTON_Image,im.dobj.gadget.gadgetrender)
		IF i:=im.dobj.gadget.selectrender
			listadd2(templist,BUTTON_SelectedImage,i)
		ENDIF
	ELSEIF c="M" OR (c="m")
		IF (i:=LengthArgstring(rm.args[3]))=4
			listadd2(templist,BUTTON_Image,Long(rm.args[3]))
		ELSEIF i=8
			listadd2(templist,BUTTON_Image,Long(rm.args[3]))
			listadd2(templist,BUTTON_SelectedImage,Long(rm.args[3]+4))
		ELSE
		ENDIF
	ELSEIF c="V" OR (c="v")
		RETURN 12
	ELSE
		RETURN 12
	ENDIF
	IF strchr(rm.args[2],"-")
		ListAdd(templist,[FRM_Type,FRTYPE_NONE,BUTTON_EncloseImage,TRUE,FRM_EdgesOnly,TRUE])
	ENDIF
	IF strchr(rm.args[2],"*")
		ListAdd(templist,[BUTTON_EncloseImage,TRUE])
	ENDIF
	IF n>=4 AND rm.args[4]
		listadd2(templist,LAB_Label,addstring(rm.args[4]))
		i:=PLACE_LEFT
		IF n>=5 AND rm.args[5]
			IF (c:=Char(rm.args[5]))="l" OR (c="L")
				i:=PLACE_LEFT
			ELSEIF c="r" OR (c="R")
				i:=PLACE_RIGHT
			ELSEIF c="a" OR (c="A")
				i:=PLACE_ABOVE
			ELSEIF c="b" OR (c="B")
				i:=PLACE_BELOW
			ENDIF
		ENDIF
		listadd2(templist,LAB_Place,i)
	ENDIF
	StringF(tempstr,'ID.\s',rm.args[1])
	UpperStr(tempstr)
	tasknode.lastid:=tasknode.lastid+1
	i:=tasknode.lastid
	setRexxVar(rm,tempstr,StringF(temp2str,'\d',i),EstrLen(temp2str))
	listadd2(templist,GA_ID,i)
	ListAdd(templist,[TAG_DONE])
	IF obj:=BgUI_NewObjectA(BGUI_BUTTON_GADGET,templist)
		addobjnode(obj)
		StringF(tempstr,'OBJ.\s',rm.args[1])
		UpperStr(tempstr)
		rc:=setRexxVar(rm,tempstr,{obj},4)
		result:=CreateArgstring({obj},4)
	ELSE
		rc:=12
	ENDIF
ENDPROC rc,result

PROC createindicator(rm:PTR TO rexxmsg,templist:PTR TO LONG /* scratch */)
	DEF n,c,q,obj,rc=1,result
	IF (n:=rm.action AND $F)<4 OR (n>7)
		rc:=17 -> Wrong number of arguments
	ELSEIF rm.args[1]=NIL OR (rm.args[3]=NIL) OR (rm.args[4]=NIL)
		rc:=17 -> Wrong number of arguments
	ELSE
		ListCopy(templist,[FRM_Type,FRTYPE_NONE])
		IF rm.args[2] THEN listadd2(templist,LAB_Label,addstring(rm.args[2]))
		listadd2(templist,INDIC_Min,Val(rm.args[3]))
		listadd2(templist,INDIC_Max,Val(rm.args[4]))
		IF n>=5 AND rm.args[5]
			listadd2(templist,INDIC_Level,Val(rm.args[5]))
		ENDIF
		c:=0
		IF n>=6 AND rm.args[6]
			IF (c:=Char(rm.args[6]))="l" OR (c="L")
				ListAdd(templist,[INDIC_Justification,IDJ_LEFT])
			ELSEIF c="c" OR (c="C")
				ListAdd(templist,[INDIC_Justification,IDJ_CENTER])
			ELSEIF c="r" OR (c="R")
				ListAdd(templist,[INDIC_Justification,IDJ_RIGHT])
			ENDIF
		ENDIF
		q:=NIL
		IF n>=7 AND rm.args[7]
			listadd2(templist,INDIC_FormatString,q:=addstring(rm.args[7]))
		ENDIF
		ListAdd(templist,[TAG_DONE])
		IF obj:=BgUI_NewObjectA(BGUI_INDICATOR_GADGET,templist)
			IF q
				/* 41.10: INDIC_FormatString -S--U */
				ListCopy(templist,[INDIC_FormatString])
				listadd1(templist,q)
				/* OM_SET INDIC_FormatString loses INDIC_Justification */
				IF c="l" OR (c="L")
					ListAdd(templist,[INDIC_Justification,IDJ_LEFT])
				ELSEIF c="c" OR (c="C")
					ListAdd(templist,[INDIC_Justification,IDJ_CENTER])
				ELSEIF c="r" OR (c="R")
					ListAdd(templist,[INDIC_Justification,IDJ_RIGHT])
				ENDIF
				ListAdd(templist,[TAG_DONE])
				SetGadgetAttrsA(obj,NIL,NIL,templist)
			ENDIF
			addobjnode(obj)
			StringF(tempstr,'OBJ.\s',rm.args[1])
			UpperStr(tempstr)
			rc:=setRexxVar(rm,tempstr,{obj},4)
			result:=CreateArgstring({obj},4)
		ELSE
			rc:=12
		ENDIF
	ENDIF
ENDPROC rc,result

PROC createwindow(
		rm:PTR TO rexxmsg,
		templist:PTR TO LONG /* scratch */,
		tempstr:PTR TO LONG /* scratch */
)
	DEF rc=1,result,lock,obj,n,q,c,i,plong:PTR TO LONG,pint:PTR TO INT,
			wnd:PTR TO window
	IF (n:=rm.action AND $F)<2
		rc:=17 -> Wrong number of arguments
	ELSE
		SetList(templist,0)
		obj:=Long(rm.args[2])
		IF removeobjnode(obj)=0 THEN RETURN 12
		listadd2(templist,WINDOW_MasterGroup,obj)
		listadd2(templist,WINDOW_Title,addstring(rm.args[1]))
		listadd2(templist,WINDOW_AutoAspect,TRUE)
		listadd2(templist,WINDOW_CloseOnEsc,TRUE)
		listadd2(templist,WINDOW_AutoKeyLabel,TRUE)
    listadd2(templist,WINDOW_IDCMPHookBits,IDCMP_RAWKEY)
    listadd2(templist,WINDOW_IDCMPHook,tasknode.idcmphook)
		lock:=0
		IF n>=3 AND (q:=rm.args[3])
			rc:=Abs(Val(q))
			IF q[]<>"-"
				IF rc THEN listadd2(templist,WINDOW_ScaleWidth,rc)
			ELSE
				IF rc THEN listadd2(templist,WINDOW_ScaleWidth,rc)
				listadd2(templist,WINDOW_LockWidth,TRUE)
				lock:=lock OR 1
			ENDIF
		ENDIF
		IF n>=4 AND (q:=rm.args[4])
			rc:=Abs(Val(q))
			IF q[]<>"-"
				IF rc THEN listadd2(templist,WINDOW_ScaleHeight,rc)
			ELSE
				IF rc THEN listadd2(templist,WINDOW_ScaleHeight,rc)
				listadd2(templist,WINDOW_LockHeight,TRUE)
				lock:=lock OR 2
			ENDIF
		ENDIF
		IF lock=3 THEN ListAdd(templist,[WINDOW_SizeGadget,FALSE,WINDOW_TitleZip,TRUE])
		wnd:=NIL; obj:=NIL
		IF rm.args[5] AND (n>=5)
			IF LengthArgstring(rm.args[5])=4
				obj:=Long(rm.args[5])
				GetAttr(WINDOW_Window,obj,{wnd})
				listadd2(templist,WINDOW_Screen,wnd.wscreen)
			ENDIF
		ELSEIF rm.args[6] AND (n>=6)
			listadd2(templist,WINDOW_PubScreenName,addstring(rm.args[6]))
		ENDIF
		IF n>=7 AND rm.args[7]
			c:=Char(rm.args[7])
			IF c="S" OR (c="s")
				ListAdd(templist,[WINDOW_Position,POS_CENTERSCREEN])
			ELSEIF c="m" OR (c="M")
				ListAdd(templist,[WINDOW_Position,POS_CENTERMOUSE])
			ELSEIF c="t" OR (c="T")
				ListAdd(templist,[WINDOW_Position,POS_TOPLEFT])
			ELSEIF c="w" OR (c="W") AND wnd
				listadd2(templist,WINDOW_PosRelBox,wnd+4)
			ELSEIF c="b" OR (c="B")
				q:=rm.args[7]+1
				WHILE q[]>"9" OR (q[]<"0") AND (q[]) DO q++
				pint:=tempstr; i:=0
				PutLong(tempstr,0); PutLong(tempstr+4,0)
				WHILE c
					pint[]:=Val(q,{c})
					EXIT c=0
					q:=q+c
					IF q[]>"9" OR (q[]<"0") THEN q++
					pint++; i++
				ENDWHILE
				pint:=alloceven(8); CopyMem(tempstr,pint,8)
				listadd2(templist,WINDOW_Bounds,pint)
			ELSEIF c="r" OR (c="R")
				q:=rm.args[7]+1
				WHILE q[]>"9" OR (q[]<"0") AND (q[]) DO q++
				pint:=tempstr; i:=0
				PutLong(tempstr,0); PutLong(tempstr+4,0)
				WHILE c
					pint[]:=Val(q,{c})
					EXIT c=0
					q:=q+c
					IF q[]>"9" OR (q[]<"0") THEN q++
					pint++; i++
				ENDWHILE
				pint:=alloceven(8); CopyMem(tempstr,pint,8)
				listadd2(templist,WINDOW_PosRelBox,pint)
			ENDIF
			IF strchr(rm.args[7],"*")
				ListAdd(templist,[WINDOW_Activate,FALSE])
			ENDIF
		ENDIF
		IF rm.action AND $F>=8 AND (q:=rm.args[8])
			IF LengthArgstring(q)>SIZEOF newmenu AND Char(q)=1 THEN listadd2(templist,WINDOW_MenuStrip,q)
		ENDIF
		listadd1(templist,TAG_DONE)
		rc:=12
		IF obj:=BgUI_NewObjectA(BGUI_WINDOW_OBJECT,templist)
			addobjnode(obj)
			rc:=0
			result:=CreateArgstring({obj},4)
		ELSE
			rc:=12
		ENDIF
	ENDIF
ENDPROC rc,result

PROC createimage(rm:PTR TO rexxmsg)
	DEF n,i,w,h,d,sz,data,im:PTR TO image
	IF (n:=rm.action AND $F)<>8 THEN RETURN 17
	FOR i:=1 TO 8 DO IF rm.args[i]=NIL THEN RETURN 17
	w:=Val(rm.args[3])
	h:=Val(rm.args[4])
	d:=Val(rm.args[5])
	sz:=w+15/16*2*h*d
	IF LengthArgstring(rm.args[6])<>sz THEN RETURN 12
	data:=allocchip(sz)
	IF data=NIL THEN RETURN 12
	CopyMem(rm.args[6],data,sz)
	IF (im:=alloceven(SIZEOF image))=NIL THEN RETURN 12
	im.leftedge:=Val(rm.args[1])
	im.topedge:=Val(rm.args[2])
	im.width:=w
	im.height:=h
	im.depth:=d
	im.imagedata:=data
	im.planepick:=Val(rm.args[7])
	im.planeonoff:=Val(rm.args[8])
	im.nextimage:=NIL
ENDPROC 0,CreateArgstring({im},SIZEOF LONG)

PROC makenm(rm:PTR TO rexxmsg,tempstr /* scratch */)
	DEF i,s,nm:newmenu,n,len
	/* 1<=Nargs<=6, rm.args[1]<>NIL */
	n:=rm.action AND $F
	i:=Val(rm.args[1]); IF i<=0 OR (i>3) THEN RETURN 18 -> Invalid argument to function
	nm.type:=i

	nm.flags:=0
	IF s:=rm.args[4] AND (n>=4)
		WHILE s[]
			StrCopy(tempstr,'')
			WHILE s[]>="A" AND (s[]<="z") DO StrAdd(tempstr,s,1) BUT s++
			IF stricmp(tempstr,'disabled')=0
				nm.flags:=nm.flags OR (IF i=1 THEN NM_MENUDISABLED ELSE NM_ITEMDISABLED)
			ELSEIF stricmp(tempstr,'checkit')=0
				nm.flags:=nm.flags OR CHECKIT
			ELSEIF stricmp(tempstr,'checked')=0
				nm.flags:=nm.flags OR CHECKED
			ELSEIF stricmp(tempstr,'toggle')=0
				nm.flags:=nm.flags OR MENUTOGGLE
			ENDIF
			WHILE s[]<"A" OR (s[]>"z") AND s[] DO s++
		ENDWHILE
	ENDIF
	i:=0
	IF s:=rm.args[5] AND (n>=5)
		IF (len:=LengthArgstring(s))<=4
			IF len
				CopyMem(s,{i}+4-len,len)
			ENDIF
		ELSE
			RETURN 18
		ENDIF
	ENDIF
	nm.mutualexclude:=i
	IF s:=rm.args[6] AND (n>=6)
		IF (i:=Val(s))=0
			tasknode.lastid:=tasknode.lastid+1
			i:=tasknode.lastid
			IF setRexxVar(rm,s,StringF(tempstr,'\d',i),EstrLen(tempstr)) THEN RETURN 12
		ENDIF
	ENDIF
	nm.userdata:=i
	nm.label:=NIL
	IF rm.args[2] AND (n>=2)
		nm.label:=addstring(rm.args[2])
	ENDIF
	IF nm.label=NIL THEN nm.label:=NM_BARLABEL
	IF rm.args[3] AND (n>=3)
		nm.commkey:=addstring(rm.args[3])
	ELSE
		nm.commkey:=NIL
	ENDIF
	s:=CreateArgstring(nm,SIZEOF newmenu)
	IF s=NIL THEN RETURN 12 ELSE RETURN 0,s
ENDPROC

PROC bguireq(wnd,title,text,gadgets,args=NIL,screen=NIL)
	DEF rq:bguiRequest
	rq.flags:=BREQF_AUTO_ASPECT OR
		(IF wnd THEN BREQF_LOCKWINDOW ELSE 0) OR
		(IF InStr(gadgets,'*')<>-1 THEN BREQF_FAST_KEYS ELSE 0) OR
		(IF gadgets[0]="%" AND (gadgets[1]="[") THEN BREQF_XEN_BUTTONS ELSE 0) OR
		(IF gadgets[0]="%" AND (gadgets[1]="]") THEN BREQF_FUZZ_BUTTONS ELSE 0)
	rq.title:=title
	rq.gadgetFormat:=IF gadgets[0]<>"%" OR 
		(gadgets[1]<>"[" AND (gadgets[1]<>"]")) THEN gadgets ELSE gadgets+2
	rq.textFormat:=text
	rq.reqPos:=POS_CENTERMOUSE
	rq.textAttr:=NIL
	rq.underscore:="_"
	rq.reserved0:=0
	rq.screen:=screen
	rq.reserved1:=0
ENDPROC BgUI_RequestA(wnd,rq,args)

PROC findtag(name)
	DEF first[10]:STRING,q,next,t=NIL:PTR TO taginfo
	StrCopy(first,name,q:=InStr(name,'_'))
	IF q<=1 THEN RETURN 0
	IF q>=10 THEN RETURN 0
	next:=name+q+1

	IF FALSE
		/* TAGS: General information
		*/
		-> Types:
		-> INT=integer, may be boolean (0 means false, anything else means true)
		-> STRING=string
		-> OBJECT=4-byte pointer
		->
		-> Applicability: ISGNU
		-> I=Init (not applicable for RexxBGUI)
		-> S=Set (bguiset())
		-> G=Get (bguiget())
		-> N=Notify (bguiaddmap() sattr/bguiaddcondit() ifattr)
		-> U=Update (bguiaddmap() dattr/bguiaddcondit() thattr/elattr)
		-> -=not possible
		-> ?=not known to me, try it
	ELSEIF stricmp(first,'WINDOW')=0
		t:=[
->			'Activate',$800F06F1,TAG_INT,
->			'AutoAspect',$800F0718,TAG_INT,
->			'AutoKeyLabel',$800F0725,TAG_INT,
->			'Backdrop',$800F06F6,TAG_INT,
->			'Borderless',$800F06F5,TAG_INT,
->			'CloseGadget',$800F06ED,TAG_INT,
->			'CloseOnEsc',$800F071A,TAG_INT,
->			'DepthGadget',$800F06EE,TAG_INT,
->			'DragBar',$800F06EB,TAG_INT,
->			'HelpFile',$800F070F,TAG_STRING,
->			'HelpLine',$800F0711,TAG_INT,
->			'HelpNode',$800F0710,TAG_STRING,
->			'HelpText',$800F0716,TAG_STRING,
->			'LockHeight',$800F06E5,TAG_INT,
->			'LockWidth',$800F06E4,TAG_INT,
->			'MenuStrip',$800F0703,TAG_MENU,
->			'NoBufferRP',$800F0717,TAG_INT,
->			'NoVerify',$800F071D,TAG_INT,
->			'Position',$800F06E1,TAG_INT,
->			'PreBufferRP',$800F072B,TAG_INT,
->			'PubScreenName',$800F0706,TAG_STRING,
->			'RMBTrap',$800F06F2,TAG_INT,
->			'ReportMouse',$800F06F4,TAG_INT,
->			'ScaleHeight',$800F06E3,TAG_INT,
->			'ScaleWidth',$800F06E2,TAG_INT,
->			'ScreenTitle',$800F0702,TAG_STRING,
->			'ShowTitle',$800F06F7,TAG_INT,
->			'SizeBottom',$800F06EF,TAG_INT,
->			'SizeGadget',$800F06EC,TAG_INT,
->			'SizeRight',$800F06F0,TAG_INT,
->			'SmartRefresh',$800F06F3,TAG_INT,
->			'Title',$800F0701,TAG_STRING,
->			'TitleZip',$800F0724,TAG_INT,
->			'ToolTicks',$800F071F,TAG_INT,
->			'UniqueID',$800F0714,TAG_INT,
			'Window',$800F0715,TAG_OBJECT, -> --G--
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'INFO')=0
		t:=[
			'TextFormat',$800F0281,TAG_STRING, -> IS--U
			'MinLines',$800F0283,TAG_INT,
			'FixTextWidth',$800F0284,TAG_INT,
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'STRINGA')=0
		t:=[
			'TextVal',STRINGA_TEXTVAL,TAG_STRING, -> ISGNU makes string gadget
			'LongVal',STRINGA_LONGVAL,TAG_INT, -> ISGNU makes integer gadget
			'MaxChars',STRINGA_MAXCHARS,TAG_INT, -> I-?--
			'BufferPos',STRINGA_BUFFERPOS,TAG_INT, -> IS??? cursor position
			'DispPos',STRINGA_DISPPOS,TAG_INT, -> IS??? first char visible
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'LGO')=0
		t:=[
			'FixWidth',LGO_FixWidth,TAG_INT,
			'FixHeight',LGO_FixHeight,TAG_INT,
			'Weight',LGO_Weight,TAG_INT,
			'FixMinWidth',LGO_FixMinWidth,TAG_INT,
			'FixMinHeight',LGO_FixMinHeight,TAG_INT,
			'Align',LGO_Align,TAG_INT,
			'NoAlign',LGO_NoAlign,TAG_INT,
			'FixAspect',LGO_FixAspect,TAG_INT, -> 65536*x+y 41.8 only
			'Visible',LGO_Visible,TAG_INT,
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'LISTV')=0
		t:=[
			'Top',LISTV_Top,TAG_INT, -> ISG-U
			'ReadOnly',LISTV_ReadOnly,TAG_INT, -> ISG?? S/U will not change frame
			'MultiSelect',LISTV_MultiSelect,TAG_INT, -> IS?-U
			'Select',LISTV_Select,TAG_INT, -> -S--U
			'MakeVisible',LISTV_MakeVisible,TAG_INT, -> -S--U
			'Entry',LISTV_Entry,TAG_STRING, -> ---N-
			'EntryNumber',LISTV_EntryNumber,TAG_INT, -> ---N-
			'LastClicked',LISTV_LastClicked,TAG_STRING, -> --G--
			'LastClickedNum',LISTV_LastClickedNum,TAG_INT, -> --G--
			'NewPosition',LISTV_NewPosition,TAG_INT, -> ---N-
			'NumEntries',LISTV_NumEntries,TAG_INT, -> --G--
			'MinEntriesShown',LISTV_MinEntriesShown,TAG_INT, -> ISG??
			'SelectMulti',LISTV_SelectMulti,TAG_INT, -> -S--U
			'SelectNotVisible',LISTV_SelectNotVisible,TAG_INT, -> -S--U
			'SelectMultiNotVisible',LISTV_SelectMultiNotVisible,TAG_INT, -> -S--U
			'MultiSelectNoShift',LISTV_MultiSelectNoShift,TAG_INT, -> IS--U
			'Deselect',LISTV_Deselect,TAG_INT, -> -S--U
			'DropSpot',LISTV_DropSpot,TAG_INT, -> --G--
			'ShowDropSpot',LISTV_ShowDropSpot,TAG_INT, -> IS---
			'Columns',LISTV_Columns,TAG_INT, -> I-G--
			'DragColumns',LISTV_DragColumns,TAG_INT, -> ISG-U
			'Titles',LISTV_Titles,TAG_STRING, -> I-G-U 41.8 Enforcer hits
			'Titles!',LISTV_Titles,TAG_OBJECT, -> ISG-U static string problem
			'PreClear',LISTV_PreClear,TAG_INT, -> ISG--
			'LastColumn',LISTV_LastColumn,TAG_INT, -> --G--
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'GA')=0
		t:=[
			'Disabled',GA_DISABLED,TAG_INT, -> IS?-U
			'Selected',GA_SELECTED,TAG_INT, -> ISGNU toggles/checkboxes only
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'BT')=0
		t:=[
			'LabelClick',BT_LabelClick,TAG_INT, -> IS---
			'DragObject',BT_DragObject,TAG_INT, -> ISG--
			'DropObject',BT_DropObject,TAG_INT, -> ISG--
			'Key',BT_Key,TAG_STRING, -> ISG-- set after creating window object only
			'RawKey',BT_RawKey,TAG_INT, -> ISG-- set after creating window object only
			'Qualifier',BT_Qualifier,TAG_INT, -> ISG-- set after creating window object only
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'PAGE')=0
		t:=[
			'Active',PAGE_Active,TAG_INT, -> ISG?U
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'MX')=0
		t:=[
			'Active',MX_Active,TAG_INT, -> ISGNU
			'DisableButton',MX_DisableButton,TAG_INT, -> IS--U
			'EnableButton',MX_EnableButton,TAG_INT, -> IS--U
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'CYC')=0
		t:=[
			'Active',CYC_Active,TAG_INT, -> ISGNU
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'PROGRESS')=0
		t:=[
			'Min',PROGRESS_Min,TAG_INT, -> IS---
			'Max',PROGRESS_Max,TAG_INT, -> IS---
			'Done',PROGRESS_Done,TAG_INT, -> ISGNU
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'INDIC')=0
		t:=[
			'Level',INDIC_Level,TAG_INT, -> IS--U
			0,$0,0
		]:taginfo
	ELSEIF stricmp(first,'TAG')=0
		t:=[
			'IGNORE',1,TAG_INT, -> IS--U does nothing!
			0,$0,0
		]:taginfo
	ENDIF
	IF t=NIL THEN RETURN NIL
	WHILE t[].name
		IF stricmp(t[].name,next)=0
			RETURN t
		ENDIF
		t++
	ENDWHILE
ENDPROC NIL

PROC listadd1(list,e)
	DEF l[1]:LIST
	ListCopy(l,[0])
	l[]:=e
	ListAdd(list,l)
ENDPROC

PROC listadd2(list,e1,e2)
	DEF l[2]:LIST
	ListCopy(l,[0,0])
	l[]:=e1
	l[1]:=e2
	ListAdd(list,l)
ENDPROC

PROC addobjnode(obj)
	DEF on=NIL:PTR TO objnode
	on:=AllocVec(SIZEOF objnode,0)
	IF on=NIL THEN RETURN NIL
	on.obj:=obj
	AddTail(tasknode.objects,on)
ENDPROC on

PROC removeobjnode(obj)
	DEF on:PTR TO objnode
	on:=tasknode.objects.head
	WHILE on::mln.succ
		IF on.obj=obj
			Remove(on)
			FreeVec(on)
			RETURN TRUE
		ENDIF
		on:=on::mln.succ
	ENDWHILE
ENDPROC FALSE

PROC findtasknode(rm:PTR TO rexxmsg)
	DEF p=NIL,tn:PTR TO tasknode
	IF getRexxVar(rm,'bguitasknode',{p}) THEN RETURN NIL
	IF p=NIL THEN RETURN NIL
	IF tn:=Long(p)
		bguibase:=tn.bguibase
		RETURN tn
	ELSE
		RETURN NIL
	ENDIF
ENDPROC

PROC addtasknode(rm:PTR TO rexxmsg,name) HANDLE
	DEF tn=NIL:PTR TO tasknode,tc:PTR TO tc,oldname,cli
	tc:=FindTask(NIL)
	IF name=NIL THEN name:=tc::ln.name
	oldname:=tc::ln.name
	tc::ln.name:=name

	cli:=tc::process.cli
	tc::process.cli:=0
	bguibase:=OpenLibrary('bgui.library',41)
	tc::process.cli:=cli

	tc::ln.name:=oldname
	IF bguibase=NIL THEN Raise("LIB")
	tn:=AllocVec(SIZEOF tasknode,MEMF_PUBLIC)
	IF tn=NIL THEN Raise("MEM")
	tn.task:=tc
	tn.bguibase:=bguibase
	tn.lastid:=0
	newList(tn.stringspace)
	newList(tn.objects)
	newList(tn.images)
	tn.filereq:=NIL
	tn.chipmem:=NIL
	tn.idcmphook.window_object:=NIL
	IF setRexxVar(rm,'bguitasknode',{tn},4) THEN Raise(1)
	rexxbguibase.opencnt:=rexxbguibase.opencnt+1
	RETURN tn
EXCEPT DO
	IF tn THEN FreeVec(tn)
	IF bguibase THEN CloseLibrary(bguibase)
	bguibase:=NIL
ENDPROC NIL

PROC freetasknode(rm:PTR TO rexxmsg,tn:PTR TO tasknode)
	DEF o:PTR TO objnode,n:PTR TO stringbuffer,im:PTR TO imagenode
	DEF head
	WHILE n:=RemHead(tn.stringspace) DO FreeVec(n)
	WHILE o:=RemHead(tn.objects)
		DisposeObject(o.obj)
		FreeVec(o)
	ENDWHILE
	IF iconbase:=OpenLibrary('icon.library',0)
		WHILE im:=RemHead(tn.images)
			FreeDiskObject(im.dobj)
			FreeVec(im)
		ENDWHILE
		CloseLibrary(iconbase); iconbase:=NIL
	ENDIF
	IF tn.filereq THEN DisposeObject(tn.filereq)
	head:=tn.chipmem
	o:=head
	WHILE head
		head:=o::mln.succ
		FreeVec(o)
		o:=head
	ENDWHILE
	CloseLibrary(tn.bguibase)
	FreeVec(tn)
	rexxbguibase.opencnt:=rexxbguibase.opencnt-1
	setRexxVar(rm,'bguitasknode',[0],4)
ENDPROC

PROC addicon(filename) HANDLE
	DEF im=NIL:PTR TO imagenode,dobj=NIL
	im:=AllocVec(SIZEOF imagenode,0)
	IF im=NIL THEN RETURN NIL
	IF iconbase:=OpenLibrary('icon.library',0)
		dobj:=GetDiskObject(filename)
		CloseLibrary(iconbase); iconbase:=NIL
	ENDIF
	IF dobj=NIL THEN Raise(1)
	im.dobj:=dobj
	AddTail(tasknode.images,im)
	RETURN im
EXCEPT DO
	IF im THEN FreeVec(im)
ENDPROC NIL

PROC addstring(old)
	DEF n:PTR TO stringbuffer,len,end,p
	len:=StrLen(old)
	IF len>=QUANTUM THEN RETURN NIL
	n:=tasknode.stringspace.head
	WHILE n.node.succ
		end:=n.space+QUANTUM
		IF (p:=n.ptr)+len+1<end
			CopyMem(old,p,len+1)
			n.ptr:=p+len+1
			RETURN p
		ENDIF
		n:=n.node.succ
	ENDWHILE
	IF (n:=AllocVec(SIZEOF stringbuffer,MEMF_PUBLIC))=NIL THEN RETURN NIL
	p:=n.space
	CopyMem(old,p,len+1)
	n.ptr:=n.space+len+1
	AddTail(tasknode.stringspace,n)
ENDPROC p

PROC alloceven(len)
	DEF n:PTR TO stringbuffer,end,p
	len++
	IF len>=QUANTUM THEN RETURN NIL
	n:=tasknode.stringspace.head
	WHILE n.node.succ
		end:=n.space+QUANTUM
		IF (p:=n.ptr)+len<end
			n.ptr:=p+len
			RETURN p+1 AND $FFFFFFFE
		ENDIF
		n:=n.node.succ
	ENDWHILE
	IF (n:=AllocVec(SIZEOF stringbuffer,MEMF_PUBLIC))=NIL THEN RETURN NIL
	p:=n.space
	n.ptr:=n.space+len
	AddTail(tasknode.stringspace,n)
ENDPROC p

PROC allocchip(len)
	DEF m:PTR TO memblock
	m:=AllocVec(len+4,MEMF_CHIP OR MEMF_PUBLIC) /* SIZEOF memblock */
	m.next:=tasknode.chipmem
	tasknode.chipmem:=m
ENDPROC m+4

PROC main() IS 0

/* PROC close() IS 0 */

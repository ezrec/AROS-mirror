OPT PREPROCESS,NOWARN

MODULE  'exec/nodes','exec/interrupts','exec/io','exec/ports','exec/memory',
		'devices/input','devices/inputevent',
		'graphics/gfxmacros','graphics/rastport','graphics/text','graphics/gfx','graphics/clip','graphics/layers',
		'layers',
		'intuition/intuition','intuition/screens','intuition/intuitionbase',
		'utility','utility/tagitem',
		'feelin','libraries/feelin','a4'

LIBRARY  FC_Menu,3,00,'FC_Menu Class 3.00 (25-02-02) by Olivier Laviale (lotan9@aol.com)' IS fcc_Query(A0)

PROC main() IS sys_SGlob()
CONST IECODE_ENTER   = $43,
	  IECODE_RETURN  = $44,
	  IECODE_ESC     = $45,
	  IECODE_UP      = $4C,
	  IECODE_DOWN    = $4D,
	  IECODE_RIGHT   = $4E,
	  IECODE_LEFT    = $4F,

	  IECODE_LShift_d = $60,  IECODE_LShift_u = $E0,
	  IECODE_RShift_d = $61,  IECODE_RShift_u = $E1,
	  IECODE_LAlt_d   = $64,  IECODE_LAlt_u   = $E4,
	  IECODE_RAlt_d   = $65,  IECODE_RAlt_u   = $E5,
	  IECODE_LAmiga_d = $66,  IECODE_LAmiga_u = $E6,
	  IECODE_RAmiga_d = $67,  IECODE_RAmiga_u = $E7

CONST FF_Menu_Multiple     = FF_Custom_1,
	  FF_Menu_KeyMouse     = FF_Custom_2

OBJECT menuInputMessage OF mn
   class:CHAR
   code:INT
   qualifier:INT
ENDOBJECT

#define _maleft(o)   o::feelinMenu.menuarea.x1
#define _maright(o)  o::feelinMenu.menuarea.x2
#define _matop(o)    o::feelinMenu.menuarea.y1
#define _mabottom(o) o::feelinMenu.menuarea.y2
#define _rootdata(o) o::feelinMenu.render.rootdata
#define _rootmenu(o) _rootdata(o).rootmenu
#define _mainmenu(o) _rootdata(o).mainmenu
#define _openmenu(o) _rootdata(o).openmenu

CONST FV_Menu_Shadow = 4

->ENDPROC

PROC fcc_Query(feelin:PTR TO feelinbase)
   feelinbase  := feelin
   utilitybase := feelin.utility
   layersbase  := feelin.layers

   SetStdOut(feelin.console)

   RETURN  [FA_SuperID,    FC_Item,
			FA_DataSize,   SIZEOF feelinMenu - SIZEOF feelinItem,
			FA_Dispatcher, {fcc_Dispatcher},
			NIL]
ENDPROC
PROC fcc_Dispatcher(cl=A2,self=A0:PTR TO feelinMenu,method=D0,args=A1:PTR TO LONG)
   sys_RGlob()

->   SetStdRast(intuitionbase.activescreen.rastport)

   SELECT method
	  CASE FM_New     ; RETURN menu_New(cl,self,args)
	  CASE FM_Dispose ; menu_Dispose(self)  ; F_SuperDoA(cl,self,method,args)
	  CASE FM_Set     ; menu_Set(self,args) ; F_SuperDoA(cl,self,method,args)
	  CASE FM_Get     ; F_SuperDoA(cl,self,method,args) ; menu_Get(self,args)

	  CASE FM_Setup   ; menu_Setup(cl,self,args)
->      CASE FM_Cleanup ; menu_Cleanup(cl,self)
	  CASE FM_Draw    ; menu_Draw(cl,self)

	  CASE FM_Menu_Create      ; RETURN menu_Create(self,args::FS_CreateMenu.menuowner,args::FS_CreateMenu.render)
	  CASE FM_Menu_Open        ; RETURN menu_Open(self)
	  CASE FM_Menu_Close       ; menu_Close(self)
	  CASE FM_Menu_HandleEvent ; RETURN menu_HandleEvent(self,args[])
	  CASE FM_Menu_CreateRootData
		 IF self.parent
			menu_DeleteRootData(self)
			RETURN F_DoA(self.parent,method,args)
		 ELSE
			RETURN menu_CreateRootData(self,args[])
		 ENDIF

	  DEFAULT
		 IF method == [FR_Family_Mthd TO FR_Family_Mthd + 63]
			RETURN F_DoA(self.family,method,args)
		 ELSE
			RETURN F_SuperDoA(cl,self,method,args)
		 ENDIF
   ENDSELECT
ENDPROC

->-- Methods --<-------------------------------------------------------------

PROC menu_New(cl,self:PTR TO feelinMenu,tags)
   IF self.menurender := F_New(SIZEOF feelinMenuRender)
	  IF F_NewObjA(FC_Family,[FA_Tool_Owner,self, TAG_MORE,tags,NIL])
		 RETURN F_SuperDoA(cl,self,FM_New,tags)
	  ENDIF
   ENDIF
ENDPROC
PROC menu_Dispose(self:PTR TO feelinMenu)
   self.family := F_DisposeObj(self.family)
   menu_DeleteRootData(self)
   self.menurender := F_Dispose(self.menurender)
ENDPROC
PROC menu_Set(self:PTR TO feelinMenu,tags)
   DEF item:PTR TO tagitem

   WHILE item := NextTagItem({tags})
	  SELECT item.tag
		 CASE FA_Family
			self.family := item.data

		 CASE FA_Menu_ActiveItem
			IF self.active <> item.data
			   F_DoA(self.active,FM_Menu_Close,NIL)
			   F_Set(self.active,FA_Item_Active,FALSE)
			   self.active := item.data
			   F_Set(self.active,FA_Item_Active,TRUE)
			   F_DoA(self.active,FM_Menu_Open,NIL)
			ENDIF

		 CASE FA_Menu_TriggerSelected ; priv_AddSelected(self.menurender.rootdata,item.data)
	  ENDSELECT
   ENDWHILE
ENDPROC
PROC menu_Get(self:PTR TO feelinMenu,tags)
   DEF item:PTR TO tagitem,save

   WHILE item := NextTagItem({tags}) ; save := item.data
	  SELECT item.tag
		 CASE FA_Menu            ; ^save := self
		 CASE FA_Menu_Active     ; ^save := priv_WhichMenu(_mainmenu(self),_scr(self).mousex,_scr(self).mousey)
		 CASE FA_Menu_ActiveItem ; ^save := self.active
		 CASE FA_Menu_OpenedMenu ; ^save := _openmenu(self)
		 CASE FA_Menu_Selected   ; ^save := IF self.menurender.rootdata THEN self.menurender.rootdata.selected.head ELSE NIL
		 CASE FA_Menu_Owner      ; ^save := IF self.menurender.rootdata THEN self.menurender.rootdata.menuowner ELSE NIL
	  ENDSELECT
   ENDWHILE
ENDPROC

PROC menu_Setup(cl,self:PTR TO feelinMenu,args:PTR TO FS_Setup)
   DEF mr:PTR TO feelinMenuRender,
	   rd:PTR TO feelinMenuRootData, next,obj

   IF F_SuperDoA(cl,self,FM_Setup,args)
	  IF rd := args.render::feelinMenuRender.rootdata
		 mr          := self.menurender
		 mr.context  := args.render.context
		 mr.rport    := NIL
		 mr.pens     := F_Get(args.render.context,FA_Display_Scheme)
		 mr.rootdata := rd

->         WriteF('[33mMenu.Setup[0m - [1mSelf[0m 0x\h - [1mRootData[0m 0x\h - [1mRender[0m 0x\h - [1mMenuRender[0m 0x\h\n',self,mr.rootdata,self.render,self.menurender)

		 next := self.family.head
		 WHILE obj := F_NextNode({next}) DO F_DoA(obj,FM_Setup,{mr})
	  ENDIF
   ENDIF
ENDPROC
PROC menu_Draw(cl,self:PTR TO feelinMenu)
   DEF rp:PTR TO rastport

   F_SuperDoA(cl,self,FM_Draw,NIL)

   rp := _rp(self)
   _APen(self.render.pens[IF FF_Item_Active AND self.flags THEN FV_Pen_Shine ELSE FV_Pen_Dark])
   _DrMd(RP_JAM1)
   _Move(_x2(self)-10,_y(self)+2 + rp.font.baseline)
   _Text('»',1)
   _DrMd(RP_JAM2)
ENDPROC

PROC menu_Create(self:PTR TO feelinMenu,owner:PTR TO feelinObject,render:PTR TO feelinRender)
   DEF rd:PTR TO feelinMenuRootData, inputPort:PTR TO mp,   msg,
	   mp:PTR TO mp,                 inputReq:PTR TO iostd

   IF render = NIL THEN RETURN F_AlertA('Menu.Create','FeelinRender struct MUST by provided',NIL)
   IF (rd := F_DoA(self,FM_Menu_CreateRootData,{render})) = NIL THEN RETURN F_AlertA('Menu.Create','Unable to access RootMenuData',NIL)

   ->WriteF('[33mMenu.Create[0m - [1mRootData[0m 0x\h - [1mRootMenu[0m 0x\h - [1mMainMenu[0m 0x\h\n',rd,rd.rootmenu,self)

   rd.mainmenu  := self
   rd.openmenu  := NIL
   rd.menuowner := owner

   F_DoA(rd.rootmenu,FM_Set,[
					 FA_Menu_TriggerSelected, NIL,
					 FA_NoNotify,             TRUE, TAG_DONE])

   IF mp := CreateMsgPort()
	  IF inputPort := CreateMsgPort()
		 IF inputReq := CreateIORequest(inputPort,SIZEOF iostd)
			IF OpenDevice('input.device',NIL,inputReq,NIL) = NIL
			   rd.menuport          := mp
			   rd.inputhandler.data := self

			   IF menu_Open(self)
				  inputReq.data    := rd.inputhandler
				  inputReq.command := IND_ADDHANDLER
				  DoIO(inputReq)

				  priv_RootHandle(rd,render)

				  inputReq.data    := rd.inputhandler
				  inputReq.command := IND_REMHANDLER
				  DoIO(inputReq)

				  menu_Close(self)
			   ENDIF

			   CloseDevice(inputReq)
			ELSE
			   F_AlertA('Menu.Create','Unable to Open input.device',NIL)
			ENDIF
			DeleteIORequest(inputReq)
		 ELSE
			F_AlertA('Menu.Create','Unable to Create IO Request',NIL)
		 ENDIF
		 DeleteMsgPort(inputPort)
	  ELSE
		 F_AlertA('Menu.Create','Unable to Create Input Port',NIL)
	  ENDIF

	  rd.menuport := NIL
	  WHILE msg := GetMsg(mp) DO F_Dispose(msg)
	  DeleteMsgPort(mp)
   ELSE
	  F_AlertA('Menu.Create','Unable to Create Message Port',NIL)
   ENDIF

   rd.mainmenu := NIL
   IF rd.selected.head THEN F_Set(render.application,FA_Menu_Action,self)
ENDPROC
PROC menu_Open(self:PTR TO feelinMenu)
   DEF head:PTR TO LONG,  tab,               item:PTR TO feelinItem,
	   scr:PTR TO screen, rp:PTR TO rastport,
	   bmp:PTR TO bitmap, svd:PTR TO bitmap, lay:PTR TO layer,
	   rd:PTR TO feelinMenuRootData,
	   w=NIL,h=NIL,x,y

   IF (rd := self.render.rootdata) = NIL THEN RETURN F_AlertA('Menu.Open','Unable to access MenuRootData',NIL)
   scr  := F_Get(self.render.context,FA_Display_Screen)
   head := self.family.head
   
/* 1: Calcul de la taille du Menu */

   tab := head

   WHILE item := F_NextNode({tab})
	  F_DoA(item,FM_AskMinMax,NIL)
	  w := Max(w,_minw(item))
	  h += _minh(item)
   ENDWHILE

   w += 2 ; h += 2 -> Bordure

->   WriteF('[33mMenu.Open[0m - [1mW[0m \d[3] - [1mH[0m \d[3] - [1mScr[0m 0x\h - [1mMse[0m \d[3]:\d[3] - [1mParent[0m 0x\h\n',w,h,scr,scr.mousex,scr.mousey,self.parent)

   IF self <> rd.mainmenu
	  x := _maright(self.parent) - 10
	  y := _matop(self.parent) + _y(self) - (h / 2 - (_y2(self) - _y(self) + 1 / 2))
   ELSE
	  x := scr.mousex - (w / 2)
	  y := scr.mousey - (h / 2)
   ENDIF

   x := Max(x,NIL) ; IF x + w + FV_Menu_Shadow - 1 > scr.width  THEN x := scr.width  - w - FV_Menu_Shadow ; self.menuarea.x2 := x + w - 1 ; self.menuarea.x1 := x
   y := Max(y,NIL) ; IF y + h + FV_Menu_Shadow - 1 > scr.height THEN y := scr.height - h - FV_Menu_Shadow ; self.menuarea.y2 := y + h - 1 ; self.menuarea.y1 := y

   tab := head
	 y := 1

   WHILE item := F_NextNode({tab})
	  _x(item) := 1 ; _w(item) := w - 2
	  _y(item) := y ; _h(item) := _minh(item) ; y += _minh(item)
   ENDWHILE

	y := self.menuarea.y1

   IF bmp := AllocBitMap(w+FV_Menu_Shadow,h+FV_Menu_Shadow,scr.rastport.bitmap.depth,BMF_INTERLEAVED,scr.rastport.bitmap)
	  IF svd := AllocBitMap(w+FV_Menu_Shadow,h+FV_Menu_Shadow,scr.rastport.bitmap.depth,BMF_INTERLEAVED,scr.rastport.bitmap)
		 BltBitMap(scr.rastport.bitmap,x,y,svd,0,0,w+FV_Menu_Shadow,h+FV_Menu_Shadow,$0C0,ALL,NIL)
		 BltBitMap(scr.rastport.bitmap,x,y,bmp,0,0,w+FV_Menu_Shadow,h+FV_Menu_Shadow,$0C0,ALL,NIL)

		 IF lay := CreateUpfrontLayer(rd.layerinfo,scr.rastport.bitmap,x,y,self.menuarea.x2+FV_Menu_Shadow - 1,self.menuarea.y2+FV_Menu_Shadow - 1,LAYERSUPER,bmp)
			self.bitmap := bmp ; self.menurender.rport := lay.rp
			self.saved  := svd ; rd.openmenu := self
			self.layer  := lay ; rp := lay.rp

			SetAfPt(rp,[$AAAA5555,$AAAA5555],2)
			_DrMd(RP_JAM1)
			_APen(self.render.pens[FV_Pen_Dark])
			_BPen(0)
			_Boxf(FV_Menu_Shadow, FV_Menu_Shadow, w + FV_Menu_Shadow - 1, h + FV_Menu_Shadow - 1)
			_DrMd(RP_JAM2)
			SetAfPt(rp,[$FFFFFFFF,$FFFFFFFF],2)

			_APen(self.render.pens[FV_Pen_Dark])
			_Move(0,0) ; _Draw(w-1,0) ; _Draw(w-1,h-1) ; _Draw(0,h-1) ; _Draw(0,0)

			tab := head
			WHILE item := F_NextNode({tab}) DO F_Draw(item,FF_Draw_Object)

			SyncSBitMap(lay)

			RETURN self
		 ENDIF
		 FreeBitMap(svd)
	  ENDIF
	  FreeBitMap(bmp)
   ENDIF

   F_AlertA('Menu.Open','Failed to Open Menu',NIL)
   F_DoA(self,FM_Menu_Close,NIL)
ENDPROC
PROC menu_Close(self:PTR TO feelinMenu)
   DEF mr:PTR TO feelinMenuRender,rd:PTR TO feelinMenuRootData,
	   x1,y1,w,h,lay:PTR TO layer,svd:PTR TO bitmap,bmp:PTR TO bitmap

   x1 := self.menuarea.x1 ; w := self.menuarea.x2 - x1 + 1
   y1 := self.menuarea.y1 ; h := self.menuarea.y2 - y1 + 1

   mr := self.menurender
   mr.rport := NIL

   IF rd := mr.rootdata
	  IF rd.openmenu = self
		 rd.openmenu := IF self <> rd.mainmenu THEN self.parent ELSE NIL
	  ENDIF
   ENDIF

   IF lay := self.layer  ; self.layer := NIL
	  F_Set(self,FA_Menu_ActiveItem,NIL)
	  DeleteLayer(NIL,lay)
   ENDIF

   IF svd := self.saved  ; self.saved := NIL
	  BltBitMapRastPort(svd,0,0,mr.context.screen.rastport,x1,y1,w+FV_Menu_Shadow,h+FV_Menu_Shadow,$0C0)
	  FreeBitMap(svd)
   ENDIF

   IF bmp := self.bitmap ; self.bitmap := NIL
	  FreeBitMap(bmp)
   ENDIF
ENDPROC
PROC menu_HandleEvent(self:PTR TO feelinMenu,mhe:PTR TO feelinMenuHandleEvent)
   DEF next,item:PTR TO feelinItem

->   WriteF('[33mMenu.HandleEvent[0m - [1mMenu[0m 0x\h - [1mTitle[0m \s\n',self,IF self.title THEN self.title ELSE 'None')

   next := self.family.head

   IF mhe.class = IDCMP_MOUSEMOVE
	  IF (mhe.mousex == [_maleft(self) TO _maright(self)]) AND (mhe.mousey == [_matop(self) TO _mabottom(self)])
		 WHILE item := F_NextNode({next})
			IF item.title
			   IF (mhe.mousex - _maleft(self)) == [_x(item) TO _x2(item)] AND (mhe.mousey - _matop(self)) == [_y(item) TO _y2(item)]
				  F_Set(self,FA_Menu_ActiveItem,item) ; RETURN
			   ENDIF
			ENDIF
		 ENDWHILE
	  ELSE
		 IF F_Get(self.active,FA_Menu) = NIL THEN F_Set(self,FA_Menu_ActiveItem,NIL)
		 IF self <> _mainmenu(self) THEN F_DoA(self.parent,FM_Menu_HandleEvent,{mhe})
	  ENDIF
   ELSEIF mhe.class = IDCMP_MOUSEBUTTONS
	  WHILE item := F_NextNode({next})
		 IF item.title
			IF (mhe.mousex - _maleft(self)) == [_x(item) TO _x2(item)] AND (mhe.mousey - _matop(self)) == [_y(item) TO _y2(item)]
			   IF F_Get(item,FA_Menu) = NIL THEN RETURN item
			   F_Set(self,FA_Menu_ActiveItem,item)
			ENDIF
		 ENDIF
	  ENDWHILE

   ELSEIF mhe.class = IDCMP_RAWKEY
	  SELECT mhe.key
		 CASE IECODE_UP   ; priv_Cycle(self,1)
		 CASE IECODE_DOWN ; priv_Cycle(self,0)
		 CASE IECODE_LEFT ; IF self <> _mainmenu(self) THEN F_DoA(self,FM_Menu_Close,NIL)
		 DEFAULT
			IF mhe.key == [IECODE_RETURN, IECODE_ENTER, IECODE_RIGHT]
			   IF item := self.active
				  IF F_Get(item,FA_Menu)
					 self.active := NIL
					 F_Set(self,FA_Menu_ActiveItem,item)
				  ELSEIF mhe.key <> IECODE_RIGHT
					 RETURN item
				  ENDIF
			   ELSE
				  F_Set(self,FA_Menu_ActiveItem,self.family.head)
			   ENDIF
			ENDIF
	  ENDSELECT
   ENDIF
ENDPROC

PROC menu_CreateRootData(self:PTR TO feelinMenu,render:PTR TO feelinRender)
   DEF rd:PTR TO feelinMenuRootData, td:PTR TO feelinTextDisplay,
	   ih:PTR TO is,                 mr:PTR TO feelinMenuRender,  next,obj

   mr := self.menurender
   self.render := mr

   IF rd := self.rootdata
->      WriteF('[33mMenu.CreateRD[0m - [1mRoot[0m 0x\h - [1mData[0m 0x\h\n',self,rd)
	  
	  mr.window := NIL
	  mr.rport  := NIL

	  RETURN rd
   ELSE
	  IF rd := F_New(SIZEOF feelinMenuRootData)
		 IF td := F_NewObjA(FC_TextDisplay,NIL)
			IF ih := F_New(SIZEOF is)
			   ih.code := {__FakeGetEvent}
			   ih.pri  := 100

			   rd.rootmenu       := self
			   rd.textobject     := td
			   rd.layerinfo      := render.context.screen.layerinfo
			   rd.inputhandler   := ih

			   mr.application    := render.application
			   mr.context        := render.context
			   mr.rootdata       := rd
			   mr.pens           := render.context.scheme

			   self.rootdata     := rd

->               WriteF('[33mMenu.CreateRD[0m - [1mRoot[0m 0x\h - [1mData[0m 0x\h - [1mContext[0m 0x\h - [1mPens[0m 0x\h\n',rd.rootmenu,rd,mr.context,mr.pens)

			   F_Set(td,FA_TextDisplay_Font,render.context.screen.rastport.font)

			   next := self.family.head
			   WHILE obj := F_NextNode({next}) DO F_DoA(obj,FM_Setup,{mr})

			   RETURN rd
			ENDIF
			F_DisposeObj(td)
		 ENDIF
		 F_Dispose(rd)
	  ENDIF
   ENDIF
ENDPROC
PROC menu_DeleteRootData(self:PTR TO feelinMenu)
   DEF rd:PTR TO feelinMenuRootData

   IF rd := self.rootdata
	  F_DoA(self,FM_Set,[FA_Menu_TriggerSelected, NIL,
						 FA_NoNotify,             TRUE, NIL])

	  rd.inputhandler   := F_Dispose(rd.inputhandler)
	  rd.textobject     := F_DisposeObj(rd.textobject)
	  self.rootdata     := F_Dispose(rd)
   ENDIF
ENDPROC

->-- Private --<-------------------------------------------------------------

PROC priv_RootHandle(rd:PTR TO feelinMenuRootData,render:PTR TO feelinRender)
   DEF scr:PTR TO screen,
	   msg:PTR TO menuInputMessage,       item:PTR TO feelinItem,
	   mhe:PTR TO feelinMenuHandleEvent,  menu:PTR TO feelinMenu, endloop=FALSE

   scr := render.context.screen

   IF mhe := F_New(SIZEOF feelinMenuHandleEvent)
	  REPEAT
		 item := NIL
		 menu := rd.openmenu
		 WaitPort(rd.menuport)

		 WHILE msg := GetMsg(rd.menuport)
/*
			Colour(2,1)
			TextF(30,38,'  Menu.RootHandle() - Msg 0x\h - Menu 0x\h - RootData 0x\h  ',msg,menu,rd)
*/
			IF msg.class = IECLASS_RAWMOUSE
			   mhe.mousex := scr.mousex
			   mhe.mousey := scr.mousey

			   IF msg.code = IECODE_NOBUTTON
				  mhe.class := IDCMP_MOUSEMOVE
			   ELSEIF msg.code == [IECODE_LBUTTON,IECODE_RBUTTON]
				  mhe.class := IDCMP_MOUSEBUTTONS
				  menu := priv_WhichMenu(rd.mainmenu,scr.mousex,scr.mousey)
				  IF menu = NIL THEN endloop := TRUE
->               ELSEIF msg.code = (IECODE_LBUTTON + IECODE_UP_PREFIX)
->                  menu := NIL ; endloop := TRUE
/*               ELSEIF msg.code = IECODE_RBUTTON
				  F_Set(menu,FA_Menu_TriggerSelected,NIL) ; endloop := TRUE*/
			   ENDIF
			ELSEIF msg.class = IECLASS_RAWKEY
			   IF msg.code = IECODE_ESC
				  F_Set(menu,FA_Menu_TriggerSelected,NIL) ; endloop := TRUE
			   ELSE
				  mhe.class := IDCMP_RAWKEY
				  mhe.key   := msg.code
			   ENDIF
			ENDIF

			IF endloop = FALSE
			   IF item := F_DoA(menu,FM_Menu_HandleEvent,{mhe})
				  F_Set(rd.rootmenu,FA_Menu_TriggerSelected,item)
			   ENDIF
			ENDIF

			F_Dispose(msg)
		 ENDWHILE

		 IF FF_Menu_Multiple AND rd.mainmenu.flags
			endloop := FALSE
		 ELSEIF item
			endloop := TRUE
		 ENDIF
	  UNTIL endloop
	  
	  F_Dispose(mhe)
   ENDIF
ENDPROC
PROC priv_ReadInputs(self:PTR TO feelinMenu,ie:PTR TO inputevent)
   DEF msg:PTR TO menuInputMessage

   sys_RGlob()

   IF ie.class <> IECLASS_TIMER
	  IF msg := F_New(SIZEOF menuInputMessage)
		 msg.type       := NT_MESSAGE
		 msg.length     := SIZEOF menuInputMessage
		 msg.class      := ie.class
		 msg.code       := ie.code
		 msg.qualifier  := ie.qualifier

		 IF ie.class = IECLASS_RAWKEY
			IF ie.code == [IECODE_LAmiga_d, IECODE_RAmiga_d]
			   self.flags := self.flags OR FF_Menu_KeyMouse
			ELSEIF ie.code == [IECODE_LShift_d, IECODE_RShift_d]
			   self.flags := self.flags OR FF_Menu_Multiple
			ELSEIF ie.code == [IECODE_LAmiga_u, IECODE_RAmiga_u]
			   self.flags := self.flags - (self.flags AND FF_Menu_KeyMouse)
			ELSEIF ie.code == [IECODE_LShift_u, IECODE_RShift_u]
			   self.flags := self.flags - (self.flags AND FF_Menu_Multiple)
			ENDIF
		 ENDIF

		 IF FF_Menu_KeyMouse AND self.flags
			msg.class := IECLASS_RAWMOUSE

			IF ie.code == [IECODE_LEFT, IECODE_RIGHT, IECODE_UP, IECODE_DOWN]
			   msg.code := IECODE_NOBUTTON
			ELSEIF ie.code = IECODE_LAlt_d
			   msg.code := IECODE_LBUTTON
			ELSEIF ie.code = IECODE_RAlt_d
			   msg.code := IECODE_RBUTTON
			ENDIF
		 ENDIF

		 PutMsg(_rootdata(self).menuport,msg)
	  ENDIF
   ENDIF

   IF ie.class = IECLASS_RAWMOUSE
	  IF ie.code <> IECODE_NOBUTTON THEN RETURN NIL
   ELSEIF ie.class = IECLASS_RAWKEY
	  IF FF_Menu_KeyMouse AND self.flags = NIL THEN RETURN NIL
   ENDIF
ENDPROC ie
PROC priv_WhichMenu(self:PTR TO feelinMenu,msex,msey)
   DEF next,obj:PTR TO feelinObject,rc

   ->WriteF('[33mMenu.Which()[0m - [1mMenu[0m 0x\h - [1mMse[0m \d[4],\d[4] - [1mTitle[0m \s\n',self,msex,msey,IF self.title THEN self.title ELSE 'None')

   IF self.layer
	  next := self.family.head

	  WHILE obj := F_NextNode({next})
		 IF F_Get(obj,FA_Menu)
			IF rc := priv_WhichMenu(obj,msex,msey) THEN RETURN rc
		 ENDIF
	  ENDWHILE

	  IF msex == [_maleft(self) TO _maright(self)] AND msey == [_matop(self) TO _mabottom(self)]
		 RETURN self
	  ENDIF
   ENDIF
ENDPROC
PROC priv_Cycle(self:PTR TO feelinMenu,direction)
   DEF item:PTR TO feelinItem

   IF direction
	  item := IF self.active THEN self.active.prev ELSE NIL
	  IF item = NIL THEN item := self.family.tail
	  WHILE item.title = NIL DO item := item.prev
   ELSE
	  item := IF self.active THEN self.active.next ELSE NIL
	  IF item = NIL THEN item := self.family.head
	  WHILE item.title = NIL DO item := item.next
   ENDIF

   IF item
	  F_Set(self.active,FA_Item_Active,FALSE) ; self.active := item
	  F_Set(self.active,FA_Item_Active,TRUE)
   ENDIF
ENDPROC
PROC priv_AddSelected(rd:PTR TO feelinMenuRootData,item:PTR TO feelinObject)
   DEF pool:PTR TO CHAR,sn:PTR TO FS_SelectedNode

   IF rd
	  pool := rd.selectedpool

	  IF item
		 IF pool = NIL
			pool := CreatePool(MEMF_CLEAR,SIZEOF FS_SelectedNode + 4 * 32,SIZEOF FS_SelectedNode + 4)
			rd.selectedpool := pool
		 ENDIF

		 IF pool
			IF sn := AllocPooled(pool,SIZEOF FS_SelectedNode)
			   sn.object := item
			   F_LinkTail(rd.selected,sn)
			ENDIF
		 ELSE
			F_AlertA('Menu.AddSelected','Unable to create SelectedPool',NIL)
		 ENDIF
	  ELSEIF pool
		 rd.selectedpool  := NIL
		 rd.selected.head := NIL
		 rd.selected.tail := NIL ; DeletePool(pool)
	  ENDIF
   ENDIF
ENDPROC

->PROC
__FakeGetEvent:
   MOVEM.L  A4-A6,-(A7)
   MOVE.L   A1,-(A7)
   MOVE.L   A0,-(A7)

   BSR      priv_ReadInputs

   MOVE.L   (A7)+,A0
   MOVE.L   (A7)+,A1
   MOVEM.L  (A7)+,A4-A6
   RTS
->ENDPROC
/*PROC Historique

   0.00 - 12.04.01

	  C'est parti pour une nouvelle classe. Celle-ci va  de  pair  avec  sa
	  superclasse  Item.fcc  puisque  les  menus peuvent être également des
	  items.

	  De grands projets pour cette classe. Je veux de beaux menus  au  look
	  totalement configurable. Et surtout je veux pouvoir laisser certaines
	  partie en flotement. A ce sujet il ne faudra  pas  oublier  d'ignorer
	  les menus (en tant qu'items) lors de la création d'un surmenu.

   1.00 - 13.04.01

	  Réecriture de nombreuses méthodes, épurage intensif  du  code...  les
	  premières heures c'est toujours pareil ;-).

   1.02 - 14.04.01

	  Modification de quelques mécanismes  qui  me  permettront  d'utiliser
	  n'importe quel menu en tant que Menu Racine.

   1.04 - 15.04.01

	  Lorsque l'attribut FA_Menu_TriggerSelected est  modifié,  ou  demandé
	  celui-ci  est  envoyé  au  parent  du  menu. Ainsi, seul le véritable
	  MenuRacine conserve l'attribut. Il est donc  à  présent  possible  de
	  créer un tableau d'items sélectionnées (touche shift).

   1.06 - 16.04.01

	  L'attribut FA_Menu_TriggerSelected a à présent une nouvelle fonction.
	  Il  est  toujours  envoyé  à  l'objet  parent  mais  sert à présent à
	  construire  un  tableau  de  pointer  reflétant  la  ou   les   items
	  sélectionées.   Ce  tableau  n'est  par  contre  accessible  que  par
	  l'attribut FA_Menu_Selected. Cet attribut n'est d'ailleurs envoyé  en
	  notification  qu'une  fois  le Menu Racine refermé à la condition que
	  l'attribut ne soit pas NIL.

	  De plus, modifier l'attribut FA_Menu_TriggerSelected  en  NIL  libère
	  tout le tableau mettant ainsi l'attribut FA_Menu_Selected à NIL.

	  Les menus appraraissent à présent en se déroulant.

   2.00 - 24.09.01

	  Très grosse mise à jour.  Beaucoup  de  modifications  apportées  aux
	  structures.

	  Les menus n'apparaissent plus en se déroulant car je  n'utilise  plus
	  les  ancienne  méthodes  d'affichage  qui  par contre me permettaient
	  cette fantaisie... peut-être plus tard...

	  Nouveau champ 'MainMenu' dans  la  structure  FeelinMenuRootData.  La
	  structure  FeelinMenuRootData n'est allouée que par le véritable Menu
	  Racine et non pas par le menu qui reçoit la méthode Menu.Create(). Le
	  menu  recevant  la  méthode Menu.Create() sera référencé en temps que
	  Menu Principal (MainMenu).

	  Mise à jour de la méthode Menu.HandleEvent() qui pouvait  tourner  en
	  boucle  dans  certaines  circonstances  avec  pour résult un plantage
	  assuré. Lorsqu'un Menu reçoit la méthode Menu.HandleEvent() et  qu'il
	  n'est  pas/plus le menu actif, je ne cherche pas à savoir quel est le
	  menu actif (en utilisant la fonction Priv.WhichMenu()), je passe tout
	  simplement  la  méthode  à  l'objet parent (en m'assurant que le menu
	  courrant n'est pas le Menu Principal).

	  La fonction Priv.WhichMenu() n'est plus utilisée que  pour  localiser
	  le  pointeur  de la souris lorsqu'un de ses bouttons est pressé. Avec
	  l'ancienne méthode de localisation utilisée  dans  Menu.HandleEvent()
	  la  fonction  était  appelée  pour chaque mouvement de souris, autant
	  dire que cela coutait beaucoup d'énergie.

	  Nouvelle utilisation des layers et plus  particulièrement  des  Super
	  BitMaps.  A  présent  les rafraichissments sont gérés automatiquement
	  par le système. Je peux ainsi utiliser les fonctions SyncSBitMap() et
	  CopySBitMap()  qui  ne demandent aucune mise en place au lieu de tout
	  les BltBitMap() que j'utilisais à la place.

	  La structure FeelinMenuRender n'est plus partagée  avec  les  enfants
	  par  la  méthode Item.Setup(), mais tout simplement par l'utilisation
	  de l'attribut privé Item.Render et de la  méthode  Object.Set().  Les
	  objets  Menu ont un traitement spécial de cet attribut similaire à ce
	  qu'ils fesaient avant lors de la méthode Item.Setup().  Les  méthodes
	  Item.Setup() et Item.Cleanup() sont a présent obsolètes.

	  L'attribut FA_Menu_Selected n'est modifié qu'après  la  fermeture  de
	  tous  les  menus  (si  rd.selected <> NIL), il pouvait arriver qu'une
	  notification se mette en place  alors  que  le  menu  était  toujours
	  ouvert.

   3.00 (25-02-02)

	  La   méthode   Menu.Create()   demande   maintenant   une   structure
	  FeelinRender  pour  créer  le  menu,  cela  m'évite de créer un objet
	  DisplayContext en plus je suis certain de faire  les  choses  au  bon
	  endroit.

	  Nouvelle méthode Menu.CreateRootData() plus sûre : si le  menu  n'est
	  plus  le  menu  racine  il  détruit sa structure RootData et passe la
	  méthode au parent. Suppression de l'attribut FA_Item_Render  qui  est
	  obsolète, je suis revenu à Menu.Setup() plus conventionnel.

	  Je ne crée plus de Layer_Info, j'utilise à présent celui de  l'écran,
	  comme  cela  doit se faire. Comme ça il n'y a plus rien qui s'affiche
	  par dessus les menus (e.g. l'horloge de MCP).

	  Au lieu d'appliquer les  notifications  au  menu  lui  même,  je  les
	  appliques à présent au client.

ENDPROC*/

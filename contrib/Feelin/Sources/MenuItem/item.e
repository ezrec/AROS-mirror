OPT PREPROCESS,NOWARN

MODULE  'graphics/rastport','graphics/text',
		'intuition/intuition','intuition/screens','intuition/intuitionbase',
		'utility','utility/tagitem',
		'feelin','libraries/feelin','a4'

LIBRARY  FC_Item,0,0,'FC_Item 0.0 (25-03-01) by Olivier Laviale (lotan9@aol.com)' IS fcc_Query(A0)

PROC main() IS sys_SGlob()

CONST FV_Item_Inner_Left   = 5,
	  FV_Item_Inner_Right  = 5,
	  FV_Item_Inner_Top    = 2,
	  FV_Item_Inner_Bottom = 2
->ENDPROC

PROC fcc_Query(feelin:PTR TO feelinbase)
   feelinbase  := feelin
   utilitybase := feelin.utility

   SetStdOut(feelin.console)

   RETURN  [FA_SuperID,    FC_Object,
			FA_DataSize,   SIZEOF feelinItem - SIZEOF feelinObject,
			FA_Dispatcher, {fcc_Dispatcher},
			NIL]
ENDPROC
PROC fcc_Dispatcher(cl=A2,self=A0:PTR TO feelinItem,method=D0,args=A1:PTR TO LONG)
   sys_RGlob()

   SELECT method
	  CASE FM_Set       ; item_Set(self,args) ; F_SuperDoA(cl,self,method,args)
	  CASE FM_Get       ; F_SuperDoA(cl,self,method,args) ; item_Get(self,args)

	  CASE FM_Setup     ; RETURN self.render := args::FS_Setup.render
	  CASE FM_AskMinMax ; item_AskMinMax(self)
	  CASE FM_Draw      ; item_Draw(self)
	  DEFAULT           ; RETURN F_SuperDoA(cl,self,method,args)
   ENDSELECT
ENDPROC

PROC item_Set(self:PTR TO feelinItem,tags)
   DEF item:PTR TO tagitem

   WHILE item := NextTagItem({tags})
	  SELECT item.tag
		 CASE FA_Item_Title         ; self.title := item.data
		 CASE FA_Item_Short         ; self.short := item.data
		 CASE FA_Item_CommandString ; priv_Change_Flags(self,item.data,FF_Item_CommandString)
		 CASE FA_Item_Sensitive     ; priv_Change_Flags(self,item.data,FF_Item_Sensitive)
		 CASE FA_Item_Active        ; priv_Change_Flags(self,item.data,FF_Item_Active)
									  F_Draw(self,FF_Draw_Object)
									  IF self.render THEN IF self.render.rootdata THEN F_Set(self.render.rootdata.rootmenu,FA_Menu_TriggerActive,self)
		 CASE FA_Item_Selected      ; priv_Change_Flags(self,item.data,FF_Item_Selected)
		 CASE FA_Item_Check         ; priv_Change_Flags(self,item.data,FF_Item_Check)
		 CASE FA_Item_Checked       ; priv_Change_Flags(self,item.data,FF_Item_Checked)
		 CASE FA_Item_Toggle        ; priv_Change_Flags(self,item.data,FF_Item_Toggle)
		 CASE FA_Item_Enabled       ; priv_Change_Flags(self,item.data,FF_Item_Enabled)
		 CASE FA_Item_Exclude
	  ENDSELECT
   ENDWHILE
ENDPROC
PROC item_Get(self:PTR TO feelinItem,tags)
   DEF item:PTR TO tagitem,save

   WHILE item := NextTagItem({tags}) ; save := item.data
	  SELECT item.tag
		 CASE FA_Item_Title         ; ^save := self.title
		 CASE FA_Item_Short         ; ^save := self.short
		 CASE FA_Item_CommandString ; ^save := NIL <> (self.flags AND FF_Item_CommandString)
		 CASE FA_Item_Sensitive     ; ^save := NIL <> (self.flags AND FF_Item_Sensitive)
		 CASE FA_Item_Active        ; ^save := NIL <> (self.flags AND FF_Item_Active)
		 CASE FA_Item_Selected      ; ^save := NIL <> (self.flags AND FF_Item_Selected)
		 CASE FA_Item_Check         ; ^save := NIL <> (self.flags AND FF_Item_Check)
		 CASE FA_Item_Checked       ; ^save := NIL <> (self.flags AND FF_Item_Checked)
		 CASE FA_Item_Toggle        ; ^save := NIL <> (self.flags AND FF_Item_Toggle)
		 CASE FA_Item_Enabled       ; ^save := NIL <> (self.flags AND FF_Item_Enabled)
		 CASE FA_Item_Exclude
	  ENDSELECT
   ENDWHILE
ENDPROC

PROC item_AskMinMax(self:PTR TO feelinItem)
   DEF td:PTR TO feelinTextDisplay

->   WriteF('[33mItem.AskMinMax[0m - [1mItem[0m 0x\h - [1mRender[0m 0x\h - [1mRootData[0m 0x\h - [1mTitle[0m \a\s\a\n',self,self.render,self.render.rootdata,IF self.title THEN self.title ELSE 'None')

   IF self.title
	  IF td := self.render.rootdata.textobject
		 F_Set(td,FA_Text,self.title)
->         WriteF('[33mItem.AskMinMax[0m - TD 0x\h - Font 0x\h\n',td,td.font)

->         F_DoA(td,FM_TextDisplay_Size,[NIL])

		 _minw(self) := td.nWidth  + 20 + FV_Item_Inner_Left + FV_Item_Inner_Right
		 _minh(self) := td.nHeight      + FV_Item_Inner_Top + FV_Item_Inner_Bottom

->         WriteF('[33mItem.AskMinMax[0m - MinW \d - MinH \d\n',_minw(self),_minh(self))
	  ENDIF
   ELSE
	  _minw(self) := 10
	  _minh(self) := 1
   ENDIF
ENDPROC
PROC item_Draw(self:PTR TO feelinItem)
   DEF rp,x1,x2,y1,y2,
	   td:PTR TO feelinTextDisplay,
	   td_draw:FS_TextDisplay_Draw,td_rect:feelinRect

->   WriteF('[33mItem.Draw[0m - [1mSelf[0m $\h - [1mRPort[0m $\h - \d[3]:\d[3] / \d[3]:\d[3]\n',self,_rp(self),_x1(self),_y1(self),_x2(self),_y2(self))

   rp := _rp(self) ; td := self.render.rootdata.textobject
   x1 :=  _x(self) ; x2 := _x2(self)
   y1 :=  _y(self) ; y2 := _y2(self)

   IF self.title = NIL
	  _APen(self.render.pens[FV_Pen_Dark])
	  _Move(x1,y1) ; _Draw(x2,y1)
   ELSE
	  _APen(self.render.pens[IF FF_Item_Active AND self.flags THEN FV_Pen_Dark ELSE FV_Pen_Shine])
	  _Boxf(x1,y1,x2,y2)

	  IF td
		 F_DoA(td,FM_Set,[
				  FA_Text,          self.title,
				  FA_Text_PreParse, IF FF_Item_Active AND self.flags THEN '`<1>' ELSE '`<0>',
				  NIL])

		 td_rect.x2  := x2
		 td_rect.x1  := x1 + FV_Item_Inner_Left
		 td_rect.y2  := y2
		 td_rect.y1  := y1 + FV_Item_Inner_Top

		 td_draw.rect   := td_rect
		 td_draw.render := self.render

		 F_DoA(td,FM_TextDisplay_Draw,td_draw)
	  ENDIF
   ENDIF
ENDPROC

PROC priv_Change_Flags(self:PTR TO feelinItem,do,bits)
   self.flags := IF do THEN self.flags OR bits ELSE self.flags - (self.flags AND bits)
ENDPROC

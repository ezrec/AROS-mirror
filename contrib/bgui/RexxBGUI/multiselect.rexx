/* REXX script for rexxbgui.library: MultiSelect example */

if ~show('l','rexxbgui.library') then do
	if ~addlib('rexxbgui.library',0,-30) then do
		exit(20)
	end
	else nop
end
else nop

call bguiopen() /* causes error 12 if it did not work */
/* supply a second argument for a '0' return code instead of an ARexx
   error */

signal on syntax /* important: bguiclose() MUST be called */
signal on halt
signal on break_c

esc='1B'x
lf='0A'x
cr='0D'x
grspace.narrow=-1
grspace.normal=-2
grspace.wide=-3

/*
lst.count=26
do i=0 for lst.count
	lst.i=esc'c'd2c(i+65)||d2c(i+97)
end
*/
i=0
lst.i='cThis listview object has multi-'; i=i+1
lst.i='cselection turned on. You can'; i=i+1
lst.i='cmulti-select the items by holding'; i=i+1
lst.i='cdown the SHIFT-key while clicking'; i=i+1
lst.i='con the different items or by clicking'; i=i+1
lst.i='con an entry and dragging the mouse'; i=i+1
lst.i='cup or down.'; i=i+1
lst.i='c'; i=i+1
lst.i='cIf you check the No SHIFT checkbox'; i=i+1
lst.i='cyou can multi-select the items without'; i=i+1
lst.i='cusing the SHIFT key'; i=i+1
lst.count=i

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbARexx-b conversion'lf'of bMultiSelect-b example',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguivgroup(,
		bguilistview('listv',,'LST','S')||,
		bguihgroup(,
			bguibutton('all','_All')||,
			bguibutton('none','N_one'),
		,0)||bguilayout(LGO_Align,1,LGO_FixMinHeight,1),
	,0)||bguilayout(LGO_NoAlign,1)||,
	bguihgroup(,
		bguicheckbox('ms','_Multiple selection',1)||bguilayout(LGO_FixMinWidth,1)||,
		bguivarspace(50)||,
		bguicheckbox('nsm','_No SHIFT',0)||bguilayout(LGO_FixMinWidth,1),
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguibutton('show','_Show')||,
		bguivarspace(50)||,
		bguibutton('quit','_Quit'),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

call bguiaddmap(obj.ms,obj.listv,GA_Selected,LISTV_MultiSelect)
call bguiaddmap(obj.nsm,obj.listv,GA_Selected,LISTV_MultiSelectNoShift)

call bguiset(obj.listv,,LISTV_MultiSelect,1)

a=bguiwindow('Multi-Selection Demo',g,20,30,,arg(1))

if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
		when id=id.show then do
			/*
			text=bguiget(obj.listv,LISTV_LastClicked)lf||,
				bguiget(obj.listv,LISTV_LastClickedNum)
			call bguireq('Results are:'lf||text,'*_OK',,a)
			*/
			if bguilistvgetentries(obj.listv,'RES','S')>0 then do
				do i=0 for res.count
					say substr(res.i,3)
				end
			end; else do
				say 'No selections made!'
			end
		end
		when id=id.ms then do
			if bguiget(obj.ms,GA_Selected)=0 then do
				call bguiset(obj.nsm,a,GA_Disabled,1)
				call bguiset(obj.all,a,GA_Disabled,1)
			end; else do
				call bguiset(obj.nsm,a,GA_Disabled,0)
				call bguiset(obj.all,a,GA_Disabled,0)
			end
		end
		when id=id.all then call bguiset(obj.listv,a,LISTV_SelectMulti,-8)
		when id=id.none then call bguiset(obj.listv,a,LISTV_Deselect,-1)
		when id=id.quit then leave
		when id=id.winclose then nop
		otherwise nop
	end
end
rc=0

syntax:
if rc~=0 then say '+++ ['rc']' errortext(rc) 'at line' sigl
call bguiclose()
exit 0

break_c:
halt:
rc=0
say '+++ Break at line' sigl
signal syntax

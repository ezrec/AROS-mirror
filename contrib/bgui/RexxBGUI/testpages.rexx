/* REXX script for rexxbgui.library: switch pages with tabs */

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

lst.count=20
do i=0 for lst.count
	lst.i='Entry' i+1
end

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbARexx-b conversion of bList-b example',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguimx('switcher',,bguilist('pnames','Single-Select','Multi-Select'),'T')||bguilayout(LGO_FixMinHeight,1)||,
	bguipages('pages',,
		bguivgroup(,
			bguiinfo('infss',,'cSingle-Select Drag-n-Drop'lf'Listview object.')||bguilayout(LGO_FixMinHeight,1)||,
			bguilistview('sslistv',,'LST','D'),
		)||,
		bguivgroup(,
			bguiinfo('infss',,'cMulti-Select Drag-n-Drop'lf'Listview object.')||bguilayout(LGO_FixMinHeight,1)||,
			bguilistview('mslistv',,'LST','D'),
		),
	)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

drop pnames.

call bguiset(obj.mslistv,,LISTV_MultiSelect,1)
call bguiaddmap(obj.switcher,obj.pages,MX_Active,PAGE_Active)

a=bguiwindow('Listview DragNDrop',g,30,30,,arg(1))

call bguiset(obj.switcher,,BT_Key,'09'x)

if bguiwinopen(a)=0 then bguierror(12)

id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
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

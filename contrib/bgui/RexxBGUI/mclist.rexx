/* REXX script for rexxbgui.library: List example */

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
tab='09'x
grspace.narrow=-1
grspace.normal=-2
grspace.wide=-3

lst.count=27
lst.0='80%'tab'20%'
do i=1 for lst.count
	lst.i='Entry' i||tab||d2c(i+64)
end

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbRexxBGUI-b: multi-column listviews',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivgroup(,
			bguilistview('sslistv',,'LST','D',2 80 20)||setlsttoms(),
		,,'F','Single-select')||,
		bguivgroup(,
			bguilistview('mslistv',,'LST','D',2 90 10),
		,,'F','Multi-select'),
	)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

call bguiset(obj.mslistv,,LISTV_MultiSelect,1)

a=bguiwindow('Listview DragNDrop',g,50,30,,arg(1))

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

setlsttoms:
lst.0='90%'tab'10%'
return ''

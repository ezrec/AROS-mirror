/* REXX */
/* REXX script for rexxbgui.library: show Activity data files */

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

esc='1B'x
lf='0A'x
cr='0D'x
tab='09'x
grspace.narrow=-1
grspace.normal=-2
grspace.wide=-3

fname=arg(1)
if fname='' then do
	options results
	address ACTIVITY
	'GET FILE'
	if rc>0 then bguierror(12); else nop
	fname=result
end; else nop

if bguireadentries(fname,'LST')~=1 then bguierror(12); else nop

flag=0
do i=0 for lst.count
	if flag then do
		lst.i=translate(lst.i,tab,';')
	end; else do
		if substr(lst.i,1,1)='-' then flag=1
		drop lst.i
	end
end

signal on syntax /* important: bguiclose() MUST be called */
signal on halt
signal on break_c

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguilistview('listv',,'LST',ReadOnly,7)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

a=bguiwindow('User activity:' fname,g,50,90)

title='Date'tab'Time'tab'Left'tab'Right'tab'Move'tab'Keys'tab'Middle'
mem=getspace(length(title)+1)
if mem='00000000'x then bguierror(12)
call export(mem,title'00'x)
call bguiset(obj.listv,,LISTV_Titles!,mem)

if bguiwinopen(a)=0 then bguierror(12) else nop
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
call bguiclose()
exit 0

syntax:
if rc~=0 then say '+++ ['rc']' errortext(rc) 'at line' sigl; else nop
call bguiclose()
exit 10

break_c:
halt:
rc=0
say '+++ Break at line' sigl
call bguiclose()
exit 10

/* REXX */
/* REXX script for rexxbgui.library */

if ~show('l','rexxbgui.library') then do
	if ~addlib('rexxbgui.library',0,-30) then do
		exit(20)
	end
	else nop
end
else nop

fname=arg(1)
if fname='' | (fname='?') then do
	say 'Usage: RX selectline <filename>'
	exit 20
end; else nop
if exists(fname)=0 then do
	say 'File does not exist:' fname
	exit 20
end; else nop

call bguiopen() /* causes error 12 if it did not work */
/* supply a second argument for a '0' return code instead of an ARexx
   error */

signal on syntax /* important: bguiclose() MUST be called */
signal on halt
signal on break_c

if bguireadentries(fname,'LST')~=1 then bguierror(12)
do i=0 to lst.count-1
	lst.i=translate(lst.i,' ','09'x)
end

esc='1B'x
lf='0A'x
cr='0D'x
grspace.narrow=-1
grspace.normal=-2
grspace.wide=-3

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguivgroup(,
		bguilistview('sslistv',,'LST','S')||,
		bguistring('str',,,256)||bguilayout(LGO_FixMinHeight,1)||,
	'',0)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

call bguiaddmap(obj.sslistv,obj.str,LISTV_Entry,STRINGA_TextVal)

a=bguiwindow(fname,g,50,30)

call bguiset(obj.sslistv,,BT_RawKey,80) /* F1 */
call bguiset(obj.str,,BT_RawKey,81) /* F2 */

if bguiwinopen(a)=0 then bguierror(12) else nop

call bguiactivategadget(obj.str,a)

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
res=bguiget(obj.str,STRINGA_TextVal)
call bguiclose()
parse source bla1 reswanted .
if reswanted then return res; else exit 0

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

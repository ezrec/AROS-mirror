/* REXX script for rexxbgui.library: progress bar */

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
grspace.narrow=-1
grspace.normal=-2
grspace.wide=-3
FixMinWidth=bguilayout(LGO_FixMinWidth,1)

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous gadgets inaccessible */
/* do not redefine winclose etc. */

text=esc'cNew features demonstrated'lf'in this example:'lf||,
	'· Progress bar'lf'· Indicator'lf'· Opening a window'lf||,
	'inactive'

g=bguivgroup(,
	bguiinfo('inf',,text)||,
	bguiprogress('prog1',,0,100,'%ld%%')||,
	bguihgroup(,
		bguiprogress('prog2','Progress',0,100)||,
		bguiindicator('indic2',,0,100,0,'C','%ld%%')||FixMinWidth,
	)||,
	bguihgroup(,
		bguibutton('do','_Do something')||,
		bguivarspace(50)||,
		bguibutton('abort','_Abort'),
	,/*spc*/,/*offs*/,/*title*/,'W'),
,,grspace.normal)

call bguiaddmap(obj.prog2,obj.indic2,PROGRESS_Done,INDIC_Level)

a=bguiwindow('RexxBGUI new 2.0 features',g,50,'-0',,arg(1),'*')
if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
		when id=id.do then do
			p=bguiget(obj.prog1,PROGRESS_Done)+10
			call bguiset(obj.prog1,a,PROGRESS_Done,p)
			call bguiset(obj.prog2,a,PROGRESS_Done,p)
			if p=100 then do; call bguiwinbusy(a); address command wait 1; leave; end; else nop
		end
		when id=id.abort then leave
		when id=id.winclose then nop
		otherwise
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

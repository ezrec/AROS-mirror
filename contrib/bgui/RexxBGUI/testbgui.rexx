/* REXX test script for rexxbgui.library */

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

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */
/* note the bold button: <return> is the shortcut key */

bla=bguihgroup(,
	bguibutton('but1',esc's'esc'd3'||'_Change text')||,
	bguibutton('but2',esc'i'||'Disable')||,
	bguivgroup(,
		bguibutton('but3','AAA')||,
		bguibutton('but4',esc'u'||'_Enable'),
	,grspace.narrow)||,
	bguibutton('but5','A_AA')||,
	bguibutton('but6',esc'b'||esc'k'||'<RETURN> as'lf'shortcut_'||'0d'x),
,grspace.narrow)
inf=bguiinfo('info','Info','Using BGUI from ARexx'lf'via',
'rexxbgui.library')
b=bguibutton('butrq','Show _BGUI_RequestA')||bguibutton('quit','_Quit')
g=bguivgroup(inf||bla||b,grspace.normal,grspace.normal)
a=bguiwindow('BGUI from ARexx!',g,50,'-0',,arg(1))
if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
		when id=id.butrq then call bguireq('Button pressed!','*_OK',,a)
		when id=id.but1 then call bguiset(obj.info,a,INFO_TextFormat,esc||'c%%% percent sign%s!')
		when id=id.but2 then call bguiset(obj.but3,a,GA_Disabled,1)
		when id=id.but4 then call bguiset(obj.but3,a,GA_Disabled,0)
		when id=id.quit then leave
		when id=id.winclose then nop
		when id=id.winnomore then call bguiwait() /* to test bguiwinevent() */
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

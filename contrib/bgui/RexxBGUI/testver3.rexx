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
cr='0D'x
grspace.narrow=-1
grspace.normal=-2
grspace.wide=-3

call bguilist('cycent','Entry #1','Entry #2','Entry #3','Entry #4',,
	'Entry #5')

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo('inf',,esc'cRexxBGUI 3.0 new features')||,
	bguihseparator('Versions')||bguilayout(LGO_FixMinHeight,1)||,
	bguiinfo('ver','BGUI',bguiversion())||,
	bguiinfo('rver','RexxBGUI',bguirexxversion())||,
	bguiinteger('vertest','_Test BGUI version 41.')||bguilayout(LGO_FixMinHeight,1)||,
	bguihseparator('Vertical separator')||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguibutton('noop1','_Some button')||,
		bguivseparator()||bguilayout(LGO_FixMinWidth,1)||,
		bguibutton('noop2','_Another button'),
	)||,
	bguihseparator('Cycle gadgets')||bguilayout(LGO_FixMinHeight,1)||,
	bguicycle('cyc1','_Normal','cycent','N')||bguilayout(LGO_FixMinHeight,1)||,
	bguicycle('cyc2','_Forced popup','cycent','P')||bguilayout(LGO_FixMinHeight,1)||,
	bguihseparator()||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

a=bguiwindow('RexxBGUI 3.0 new features',g,50,'-0',,arg(1))

if bguiwinopen(a)=0 then bguierror(12)

id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.quit then leave
		when id=id.winclose then nop
		when id=id.vertest then do
			rev=bguiget(obj.vertest,STRINGA_LongVal)
			if bguiversion(41,rev) then,
				call bguireq('At least version 41.'rev 'of BGUI.','*_OK',,a)
			else,
				call bguireq('Version 41.'rev 'of BGUI not available.','*_OK',,a)
		end
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

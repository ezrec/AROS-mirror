/* REXX script for rexxbgui.library: conditional notification */

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
FixMinSize=bguilayout(LGO_FixMinWidth,1,LGO_FixMinHeight,1)

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbRexxBGUI-b: conditional notification',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguivgroup(,
			bguicheckbox('dismx','_Disable MX object',0)FixMinSize||,
			bguicheckbox('disi2','D_isable "iItem 2-i"',0)FixMinSize,
		)||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguimx('mx','_MX object',bguilist('it','Item 1','Item 2'),'R')FixMinSize||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

drop it.

call bguiaddmap(obj.dismx,obj.mx,GA_Selected,GA_Disabled)
call bguiaddmap(obj.dismx,obj.disi2,GA_Selected,GA_Disabled)
call bguiaddcondit(obj.dismx,obj.disi2,GA_Selected,0,GA_Selected,0,TAG_IGNORE,0)
call bguiaddcondit(obj.disi2,obj.mx,GA_Selected,0,MX_EnableButton,1,MX_DisableButton,1)
call bguiaddcondit(obj.disi2,obj.mx,GA_Selected,0,TAG_IGNORE,0,MX_Active,0)

a=bguiwindow('Conditional',g,'-','-',,arg(1),'mouse')

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

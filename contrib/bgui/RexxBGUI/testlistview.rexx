/* REXX script for rexxbgui.library: test new listview functions */

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
	bguivgroup(,
		bguilistview('listv',,bguilist('it','Item 1','Item 2'),'D')||,
		bguihgroup(,
			bguibutton('add','_Add')||,
			bguibutton('rem','_Remove')||,
			bguibutton('sort','_Sort'),
		,0,,,'W')||bguilayout(LGO_FixMinHeight,1)||,
		bguistring('str',,'New Item',80)||bguilayout(LGO_FixMinHeight,1),
	,0)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

drop it.

call bguiset(obj.rem,,GA_Disabled,1)
call bguiaddmap(obj.listv,obj.str,LISTV_Entry,STRINGA_TextVal)

a=bguiwindow('Listview',g,40,70,,arg(1))

if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.str then do
			call bguilistvreplacesel(obj.listv,a,,
				bguiget(obj.str,STRINGA_TextVal))
			call bguiset(obj.rem,a,GA_Disabled,0)
		end
		when id=id.add then do
			call bguilistvaddentry(obj.listv,a,,
				'<new>',,'S')
			call bguiset(obj.rem,a,GA_Disabled,0)
			call bguiset(obj.str,a,STRINGA_TextVal,'')
			call bguiactivategadget(obj.str,a)
		end
		when id=id.listv then do
			call bguiset(obj.rem,a,GA_Disabled,0)
		end
		when id=id.rem then do
			call bguilistvcommand(obj.listv,a,'remselected')
			if bguiget(obj.listv,'LISTV_NumEntries')=0 then,
			call bguiset(obj.rem,a,GA_Disabled,1)
		end
		when id=id.sort then call bguilistvcommand(obj.listv,a,'sort')
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

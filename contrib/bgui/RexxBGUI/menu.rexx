/* REXX script for rexxbgui.library: menus */

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
		'info',,'cbRexxBGUI-b: menus',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguiinfo('lastsel','Last selection','--- None yet ---')||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguibutton('sub1','Same as subitem #_1')||,
		bguibutton('sub2','Same as subitem #_2'),
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

menu=bguimenu(1,'Project')||,
      bguimenu(2,'About...','?',,,id.about)||,
      bguimenu(2)||,
      bguimenu(2,'Quit','Q',,,id.quit)||,
     bguimenu(1,'Testing')||,
      bguimenu(2,'Have your cake',,'CHECKIT CHECKED','10'b,id.hyc)||,
      bguimenu(2,'Eat it too',,'CHECKIT','01'b,id.eit)||,
      bguimenu(2,'Disable all subitems','D','CHECKIT TOGGLE',,id.dissub)||,
      bguimenu(2)||,
      bguimenu(2,'Subitems',,,,id.sub)||,
       bguimenu(3,'Subitem #1','1',,,id.sub1)||,
       bguimenu(3,'Subitem #2','2',,,id.sub2)||,
       bguimenu(3,'Subitem #3','3','DISABLED',,id.sub3)

a=bguiwindow('Menu',g,'-25','-',,arg(1),'mouse',menu)

if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
		when id=id.about then do
			call bguiset(obj.lastsel,a,INFO_TextFormat,'About')
			call bguireq(esc'cSimple menu example'||'0a'x'for RexxBGUI by Jilles Tjoelker','*_OK',,a)
		end
		when id=id.hyc then call bguiset(obj.lastsel,a,INFO_TextFormat,'Have your cake')
		when id=id.eit then call bguiset(obj.lastsel,a,INFO_TextFormat,'Eat it too')
		when id=id.dissub then do
			state=bguiwinitemchecked(a,id)
			call bguiwindisablemenu(a,id.sub,state)
			call bguiset(obj.sub1,a,GA_Disabled,state)
			call bguiset(obj.sub2,a,GA_Disabled,state)
			if state=1 then state='ON'; else state='OFF'
			call bguiset(obj.lastsel,a,INFO_TextFormat,'Disable all subitems' state)
		end
		when id=id.sub1 then call bguiset(obj.lastsel,a,INFO_TextFormat,'Subitem #1')
		when id=id.sub2 then call bguiset(obj.lastsel,a,INFO_TextFormat,'Subitem #2')
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

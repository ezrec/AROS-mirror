/* REXX script for rexxbgui.library: listview control with arrow keys */

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

if bguirexxversion(4)=0 then do
	call bguireq(,
		'This script requires at'lf||,
		'least RexxBGUI version 4.','*_Abort','Sorry!')
	rc=0
	signal syntax
end

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbRexxBGUI-b new 4.0 feature: listview control with arrow keys',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguivgroup(,
			bguicheckbox('actrl','Enable _arrow control',1)FixMinSize||,
			bguicheckbox('genmsg','_Generate messages for arrow keys',0)FixMinSize||,
			bguicheckbox('doenter','Generate messages for _return key',0)FixMinSize,
		)||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguilistview(,
		'listv','_Listview',,
		bguilist('entries','Entry 1','Entry 2','Entry 3','Entry 4','Entry 5',,
		'Entry 6','Entry 7','Entry 8','Entry 9','Entry 10','Entry 11',,
		'Entry 12','Entry 13','Entry 14'),
	)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

drop entries.

call bguiaddcondit(obj.actrl,obj.genmsg,GA_Selected,0,GA_Disabled,1,GA_Disabled,0)
call bguiaddcondit(obj.actrl,obj.doenter,GA_Selected,0,GA_Disabled,1,GA_Disabled,0)
/* This statement disables the two other checkboxes. For this to work,
   obj.actrl must be initialized to on. */
call bguiset(obj.actrl,,GA_Selected,0)

call bguilistvcontrol(,,0,0) /* set up defaults */

a=bguiwindow('Arrow control',g,0,0,,arg(1),'mouse')

if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.actrl then do
			if 0~=bguiget(obj.actrl,GA_Selected) then do
				call bguilistvcontrol(obj.listv,a)
			end; else do
				call bguilistvcontrol()
			end
		end
		when id=id.genmsg then do
			if 0~=bguiget(obj.genmsg,GA_Selected) then do
				call bguilistvcontrol(,,id.listv)
			end; else do
				call bguilistvcontrol(,,0)
			end
		end
		when id=id.doenter then do
			if 0~=bguiget(obj.doenter,GA_Selected) then do
				call bguilistvcontrol(,,,id.enter)
			end; else do
				call bguilistvcontrol(,,,0)
			end
		end
		when id=id.listv then say 'Received id.listv.'
		when id=id.enter then say 'Received id.enter.'
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

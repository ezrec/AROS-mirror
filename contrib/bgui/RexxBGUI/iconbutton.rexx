/* REXX script for rexxbgui.library: icon buttons */

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
FixMinWidth=bguilayout(LGO_FixMinWidth,1)
FixMinHeight=bguilayout(LGO_FixMinHeight,1)
FixMinSize=bguilayout(LGO_FixMinWidth,1,LGO_FixMinHeight,1)

call bguilist('pref','Palette','ScreenMode','Printer','PrinterGfx','PrinterPS')
max=pref.count-1

fsbuttons=bguivarspace(50)
do i=0 to max
	fsbuttons=fsbuttons||bguiibutton('fs.'||i,'I','SYS:Prefs/'pref.i,pref.i,'B')||FixMinSize
end
fsbuttons=fsbuttons||bguivarspace(50)

fbuttons=bguivarspace(50)
do i=0 to max
	fbuttons=fbuttons||bguiibutton('f.'||i,'I*','SYS:Prefs/'pref.i,pref.i,'B')||FixMinSize
end
fbuttons=fbuttons||bguivarspace(50)

nfbuttons=bguivarspace(50)
do i=0 to max
	nfbuttons=nfbuttons||bguiibutton('nf.'||i,'I-','SYS:Prefs/'pref.i,pref.i,'B')||FixMinSize
end
nfbuttons=nfbuttons||bguivarspace(50)

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbRexxBGUI-b new 4.0 feature: image buttons',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(fsbuttons,,'F','With frame & spacing')||,
	bguihgroup(fbuttons,,'F','With frame, no spacing')||,
	bguihgroup(nfbuttons,,'F','No frame, no spacing')||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

drop it.

popwin=''

nm=,
bguimenu(1,'Project')||,
 bguimenu(2,'Quit','Q',,,id.quit)||,
bguimenu(1,'Prefs')

do i=0 to max
	nm=nm||bguimenu(2,pref.i,i+1,,,id.fs.i)
end

a=bguiwindow('Icon buttons',g,'-','-',,arg(1),'mouse',nm)

if bguiwinopen(a)=0 then bguierror(12); else nop
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.quit then leave
		otherwise do
			do i=0 to max
				if id=id.fs.i | id=id.f.i | id=id.nf.i then do
					address command 'run >nil: SYS:Prefs/'pref.i
				end; else nop
			end
		end
	end
end
rc=0

syntax:
if rc~=0 then say '+++ ['rc']' errortext(rc) 'at line' sigl; else nop
call bguiclose()
exit 0

break_c:
halt:
rc=0
say '+++ Break at line' sigl
signal syntax

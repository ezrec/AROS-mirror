/* REXX script for rexxbgui.library: directory listing in listview */

if ~show('l','rexxbgui.library') then do
	if ~addlib('rexxbgui.library',0,-30) then do
		exit(20)
	end
	else nop
end
else nop

if ~show('l','rexxsupport.library') then do
	if ~addlib('rexxsupport.library',0,-30) then do
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

olddir=pragma('d',arg(1))
dir=pragma('d',olddir)

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguivgroup(,
		bguistring('str',,dir,256)||bguilayout(LGO_FixMinHeight,1)||,
		bguilistview('listv')||,
		bguihgroup(,
			bguibutton('parent','_Parent')||,
			bguibutton('root','_Root'),
		,0,,,'W')||bguilayout(LGO_FixMinHeight,1),
	,0)||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

a=bguiwindow('Directory listing in listview',g,40,70)

if bguiwinopen(a)=0 then bguierror(12)

text='cbDirectory listing in listview-b'lf||lf||,
	'This ARexx script shows a directory listing in a'lf||,
	'listview. Because of the bad design of'lf||,
	'rexxsyslib.library/SHOWDIR() the directory has to be'lf||,
	'read twice to distinguish directories and files.'lf||,
	'For the same reason only the filename is shown'lf||,
	'and not the size and similar information'lf||lf||,
	'Be warned that reading large directories is slow.'lf||lf||,
	'The first directory to be listed is:'lf||,
	'b'||dir
if bguireq(text,'*_Continue|_Quit',,a)=0 then do
	rc=0; signal syntax
end; else nop
drop text

call readdir(dir)

id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.str then do
			dir=bguiget(obj.str,STRINGA_TextVal)
			call readdir(dir)
		end
		when id=id.listv then do
			e=bguiget(obj.listv,LISTV_LastClicked)
			if left(e,3)='d8' then do
				if right(dir,1)~=':' & right(dir,1)~='/' then do
					dir=dir'/'
				end; else nop
				dir=dir||substr(e,6)
				bguiset(obj.str,a,STRINGA_TextVal,dir)
				call readdir(dir)
			end; else nop
		end
		when id=id.parent then do
			prevdir=dir
			olddir=pragma('d',dir)
			call pragma('d','/')
			dir=pragma('d',olddir)
			if prevdir~=dir then do
				bguiset(obj.str,a,STRINGA_TextVal,dir)
				call readdir(dir)
			end; else call bguireq('Can''t get parent!','*_OK',,a)
		end
		when id=id.root then do
			prevdir=dir
			olddir=pragma('d',dir)
			call pragma('d',':')
			dir=pragma('d',olddir)
			if prevdir~=dir then do
				bguiset(obj.str,a,STRINGA_TextVal,dir)
				call readdir(dir)
			end; else nop
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

readdir:
parse arg dir
call bguiwinbusy(a)
call bguilistvcommand(obj.listv,a,'clear')
lst=showdir(dir,'D','/')
do while lst~=''
	parse var lst entry '/' lst
	call bguilistvaddentry(obj.listv,,'d8'entry,'S')
end
lst=showdir(dir,'F','/')
do while lst~=''
	parse var lst entry '/' lst
	call bguilistvaddentry(obj.listv,,entry,'S')
end
call bguilistvcommand(obj.listv,a,'refresh')
call bguiwinready(a)
return 1

/* REXX script for rexxbgui.library: image buttons */

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

if bguirexxversion(4)=0 then do
	call bguireq(,
		'This script requires at'lf||,
		'least RexxBGUI version 4.','*_Abort','Sorry!')
	rc=0
	signal syntax
end

dgroupmembers=bguivarspace(50)
do i=1 to 11
	dgroupmembers=dgroupmembers||bguiibutton('but.'||i,'B',i)||FixMinSize
end
dgroupmembers=dgroupmembers||bguivarspace(50)

call makeirisimage()

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguiinfo(,
		'info',,'cbRexxBGUI-b new 4.0 feature: image buttons',
	)||bguilayout(LGO_FixMinHeight,1)||,
	bguihgroup(dgroupmembers,,'F','All built-in vector images')||,
	bguihgroup(,
		bguistring('str','_String','example for popup',80)||,
		bguiibutton('popup','B','P')||FixMinWidth,
	,grspace.narrow)||,
	bguihgroup(,
		bguistring('file','_File','ENV:Sys/palette.prefs',80)||,
		bguiibutton('getfile','B','F')||FixMinWidth,
	,grspace.narrow)||,
	bguihgroup(,
		bguistring('drawer','_Drawer','RAM:ENV',80)||,
		bguiibutton('getpath','B','D')||FixMinWidth,
	,grspace.narrow)||,
	bguiibutton('iris','M',irisimage,'_Image in'||'0a'x'script','Left')FixMinWidth||,
	bguihgroup(,
		bguivarspace(50)||,
		bguibutton('quit','_Quit')||,
		bguivarspace(50),
	)||bguilayout(LGO_FixMinHeight,1),
,grspace.normal,grspace.normal)

drop it.

popwin=''

a=bguiwindow('Image buttons',g,'5','-',,arg(1),'mouse')

call bguiset(obj.popup,,BT_Key,'?')
call bguiset(obj.getfile,,BT_Key,'!')

if bguiwinopen(a)=0 then bguierror(12); else nop
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
		when id=id.quit then leave
		when id=id.winclose then nop
		when id=id.popup then do
			call bguiwinbusy(a)
			call dopopup()
			call bguiwinready(a)
		end
		when id=id.getfile then do
			s=bguifilereq(bguiget(obj.file,STRINGA_TEXTVAL),,a)
			if s~='' then call bguiset(obj.file,a,STRINGA_TEXTVAL,s); else nop
		end
		when id=id.getpath then do
			s=bguifilereq(bguiget(obj.drawer,STRINGA_TEXTVAL),,a,'drawersonly')
			if s~='' then call bguiset(obj.drawer,a,STRINGA_TEXTVAL,s); else nop
		end
		when id=id.iris then do
			call bguireq(esc'c'||,
				'This image was converted with'||'0a'x||,
				'MakeImage and included in the'||'0a'x||,
				'script. No external file is'||'0a'x||,
				'required.','*_OK',,a)
		end
		otherwise nop
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

dopopup:
if popwin='' then do
	call bguilist('entries','example for popup','qwerty','some line',,
		'another text','RexxBGUI V'bguirexxversion(),'BGUI V'bguiversion())
	popwin=bguiwindow('Select',,
		bguivgroup(,
			bguilistview('p_listv','_List','entries')||,
			bguihgroup(,
				bguibutton('p_ok','bOK_'||'0d'x)||,
				bguivarspace(50)||,
				bguibutton('p_cancel','_Cancel'),
			)FixMinHeight,
		,,grspace.normal),
	,30,50,a,,'mouse')
	call bguiset(obj.p_listv,,LISTV_Select,0)
	drop entries.
end; else nop

if bguiwinopen(popwin)=0 then return

call bguilistvcontrol(obj.p_listv,popwin,0,0)

do while bguiwinwaitevent(popwin,'ID')~=id.winclose & (id~=id.p_cancel)
	select
		when id=id.p_ok then do
			line=bguiget(obj.p_listv,LISTV_LastClicked)
			call bguiset(obj.str,a,STRINGA_TextVal,line)
			leave
		end
		otherwise nop
	end
end

call bguiwinclose(popwin)
return

makeirisimage:
irisimage.normal.data=,
	'00000000000400000000000C00000000002C00000000006C00000000006C000000'x||,
	'00006C00000000006C00000000006C0000E01C006C00002010006C00001020006C'x||,
	'00000FC0006C00000000006C00000000006C00000000006C00000000006C0001C0'x||,
	'80006C00008000006C00008000006C0000978E006C00008C92006C00008898006C'x||,
	'00008886006C00008892006C0001DDDC006C00000000006C00000000006C000000'x||,
	'00006C0FFFFFFFFFEC00000000000C7FFFFFFFFFFCFFFFFFFFFFF8C00000000000'x||,
	'DFFFFFFFFFC0D80000000000D80000000000D80000000000D80000000000D80000'x||,
	'000000D80000000000D80000000000D80000000000D80000000000D80000000000'x||,
	'D80000000000D80000000000D80000000000D80000000000D80000000000D80000'x||,
	'000000D80000000000D80000000000D80000000000D80000000000D80000000000'x||,
	'D80000000000D80000000000D80000000000D80000000000D00000000000C00000'x||,
	'000000800000000000'x
irisimage.normal=bguiimage(0,0,46,31,2,irisimage.normal.data,3,0)
drop irisimage.normal.data
irisimage.selected.data=,
	'00000000000400000000000C00000000002C00000000006C0000003FFFEC000000'x||,
	'20006C0000FFA003EC0000002003EC0001FFA007EC00000020026C0001FFA01EEC'x||,
	'00000020006C0000FFA01FEC00000020006C0000003FFFEC00000000006C007FFF'x||,
	'C0006C0072F9C0006C0048025FF06C0046EC40006C0041B05FF86C00404040006C'x||,
	'0041B05FF86C00420840006C004C065FF06C007001C0006C007FFFC0006C000000'x||,
	'00006C0FFFFFFFFFEC00000000000C7FFFFFFFFFFCFFFFFFFFFFF8C00000000000'x||,
	'DFFFFFFFFFC0D80000000000D80000000000D800001FFF80D800001FFF80D80000'x||,
	'1FFD80D800001FFA80D800001FFD80D800001FFF80D800001FFF80D800001FFF80'x||,
	'D800001FFF80D80000000000D80000000000D80000000000D80FFE000000D837FD'x||,
	'800000D839F3800000D83E4F800000D83FBF800000D83E4F800000D83DF7800000'x||,
	'D833F9800000D80FFE000000D80000000000D80000000000D00000000000C00000'x||,
	'000000800000000000'x
irisimage.selected=bguiimage(0,0,46,31,2,irisimage.selected.data,3,0)
drop irisimage.selected.data
irisimage=irisimage.normal||irisimage.selected
return

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

/* using ARexx feature: ,<newline> is translated to nothing */
/* the || are essential - any added space will cause trouble */
/* redefining the same ID makes previous buttons unaccessible */
/* do not redefine winclose etc. */

g=bguivgroup(,
	bguistring('str1','String #_1','max 10',10)||,
	bguistring('str2','String #_2','Not in tabcycle chain.',30)||,
	bguistring('str3','String #_3','text',30)||,
	bguiinteger('int1','_Integer')||,
	bguiinfo('inf1','Info',esc's'esc'd3'||'Enter something in string #2!')||,
	bguihgroup(,
		bguitoggle('toggle','_Disable checkbox')||,
		bguivarspace(1000)||,
		bguicheckbox('check','Dis_able OK button'),
	,grspace.normal,'FRAME','Two notifying gadgets')||,
	bguihgroup(,
		bguibutton('show','_Show values')||,
		bguivarspace(50)||,
		bguibutton('ok',esc'b'||'OK_'cr),
	,grspace.normal),
,grspace.normal,grspace.normal)

a=bguiwindow('BGUI from ARexx!',g,50,'-0',,arg(1))

call bguiwintabcycleorder(a,obj.str1||obj.str3||obj.int1)
call bguiaddmap(obj.str2,obj.inf1,STRINGA_TextVal,INFO_TextFormat)
call bguiaddmap(obj.toggle,obj.check,GA_Selected,GA_Disabled)
call bguiaddmap(obj.check,obj.ok,GA_Selected,GA_Disabled)

if bguiwinopen(a)=0 then bguierror(12)
id=0
do while bguiwinwaitevent(a,'ID')~=id.winclose
	select
		when id=id.winactive then nop
		when id=id.wininactive then nop
		when id=id.show then do
			text=bguiget(obj.str1,STRINGA_TextVal)lf||,
			bguiget(obj.str2,STRINGA_TextVal)lf||,
			bguiget(obj.str3,STRINGA_TextVal)lf||,
			bguiget(obj.int1,STRINGA_LongVal)
			call bguireq('Results are:'lf||text,'*_OK',,a)
		end
		when id=id.ok then leave
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

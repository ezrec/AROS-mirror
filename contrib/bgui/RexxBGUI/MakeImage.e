
OPT OSVERSION=36

MODULE 'icon','workbench/workbench','intuition/intuition'

ENUM ERR_NONE,ERR_LIB,ERR_ARGS,ERR_DOBJ

RAISE ERR_LIB  IF OpenLibrary()=NIL,
			ERR_ARGS IF ReadArgs()=NIL,
			ERR_DOBJ IF GetDiskObject()=NIL

PROC main() HANDLE
	DEF rda=NIL,do=NIL:PTR TO diskobject,a[2]:ARRAY OF LONG,name[40]:STRING
	iconbase:=OpenLibrary('icon.library',0)
	a[0]:=NIL
	a[1]:=NIL
	rda:=ReadArgs('ICON,NAME',a,NIL)
	IF a[0]=0
		PrintF('\e[1mMakeImage\e[22m by Jilles Tjoelker\n\n'+
			'This program creates image definitions for RexxBGUI from icons to standard\n'+
			'output.\n\n'+
			'\e[4mTemplate:\e[24m ICON,NAME\n\n'+
			'ICON: file name of icon without .info; displays this text if omitted\n'+
			'NAME: variable name; default: file part of ICON argument\n')
		RETURN 20
	ENDIF
	IF a[1]=NIL THEN a[1]:=FilePart(a[0])
	do:=GetDiskObject(a[0])
	IF do.gadget.selectrender
		writeimage(StringF(name,'\s.normal',a[1]),do.gadget.gadgetrender)
		writeimage(StringF(name,'\s.selected',a[1]),do.gadget.selectrender)
		PrintF('\s=\s.normal||\s.selected\n',a[1],a[1],a[1])
	ELSE
		writeimage(name,do.gadget.gadgetrender)
	ENDIF
EXCEPT DO
	IF do THEN FreeDiskObject(do)
	IF rda THEN FreeArgs(rda)
	IF iconbase THEN CloseLibrary(iconbase)
	SELECT exception
		CASE ERR_LIB ; PrintF('MakeImage: Can''t open icon.library.\n')
		CASE ERR_ARGS; PrintFault(IoErr(),'MakeImage')
		CASE ERR_DOBJ; PrintFault(IoErr(),'MakeImage')
	ENDSELECT
ENDPROC IF exception THEN 20 ELSE 0

PROC writeimage(name,image:PTR TO image)
	DEF sz
	PrintF('\s.data=,\n',name)
	sz:=image.width+15/16*2*image.height*image.depth
	putrexxquotedhex(image.imagedata,sz)
	PrintF('\s=bguiimage(\d,\d,\d,\d,\d,\s.data,\d,\d)\ndrop \s.data\n',
		name,image.leftedge,image.topedge,image.width,image.height,image.depth,
		name,image.planepick,image.planeonoff,name)
ENDPROC

PROC putrexxquotedhex(addr,len)
	DEF n=32,i
	PrintF('\t\a')
	FOR i:=1 TO len
		PrintF('\z\h[2]',addr[]++)
		IF n--<=0
			PrintF('\ax||,\n\t\a')
			n:=32
		ENDIF
	ENDFOR
	PrintF('\ax\n')
ENDPROC

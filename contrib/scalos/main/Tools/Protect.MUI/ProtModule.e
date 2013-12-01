OPT PREPROCESS
OPT MODULE
OPT EXPORT

MODULE 'dos/dosextens', 'dos/dos', 'libraries/mui', 'muimaster',
		 'libraries/asl', 'mui/icon_mcc', 'utility/tagitem'

ENUM ERR_NONE, ERR_ASL, ERR_KICK
RAISE ERR_ASL  IF Mui_AllocAslRequest()=NIL,ERR_KICK IF KickVersion()=FALSE

#define FIBF_HIDDEN Shl(1,7)

PROC iconbutton(name,sel,frame) IS
          IconObject,
          MUIA_Frame,frame,
          MUIA_Background, MUII_BACKGROUND,
          MUIA_InputMode, MUIV_InputMode_Toggle,
          MUIA_Selected,sel,
          MUIA_Icon_Name,name,
          End

PROC instr(s1,s2); DEF u1,u2,r; u1:=news(s1); UpperStr(u1); u2:=news(s2); UpperStr(u2); r:=InStr(u1,u2); ENDPROC r

PROC news(s); DEF ns; ns:=String(StrLen(s)); StrCopy(ns,s); ENDPROC ns

PROC muiasl(bar,t,deffle,patt=0) HANDLE;
DEF fr:PTR TO filerequester,ht=265,ret=0,str[256]:STRING
IF t<>0 THEN ht:=ht-t;IF bar=0 THEN bar:='MUI Request'
StrCopy(str,deffle,StrLen(deffle)-StrLen(FilePart(deffle)))
IF patt<>0 THEN
fr:=Mui_AllocAslRequest(ASL_FILEREQUEST,
[ASL_HAIL,bar,  ASL_DIR,str,   ASL_FILE,FilePart(deffle),  ASL_TOPEDGE,t, ASL_LEFTEDGE,20,ASL_HEIGHT,ht, ASL_WIDTH,150,ASL_PATTERN,patt, ASL_FUNCFLAGS,FILF_PATGAD,FILF_DOWILDFUNC,NIL]
) ELSE fr:=Mui_AllocAslRequest(ASL_FILEREQUEST,
[ASL_HAIL,bar,  ASL_DIR,str,   ASL_FILE,FilePart(deffle),  ASL_TOPEDGE,t, ASL_LEFTEDGE,20,ASL_HEIGHT,ht, ASL_WIDTH,150, ASL_FUNCFLAGS,FILF_DOWILDFUNC,NIL])
 IF Mui_AslRequest(fr, NIL);
 StrCopy(str,fr.drawer);
 AddPart(str,fr.file,256)
 IF str[StrLen(str)-1]="/" THEN str[StrLen(str)-1]:=0
  IF ht:=Lock(str,-2)
  NameFromLock(ht,str,256)
  UnLock(ht)
  ENDIF
  ret:=news(str)
 ELSE; ret:='';
 ENDIF;
EXCEPT DO;
IF fr THEN Mui_FreeAslRequest(fr) ;
SELECT exception ;
CASE ERR_ASL;  WriteF('Error: Could not allocate ASL request\n')
CASE ERR_KICK; WriteF('Error: Requires V37\n') ;
ENDSELECT
ENDPROC ret

PROC imageobject(type) IS ImageObject,
          ImageButtonFrame,
          MUIA_InputMode        , MUIV_InputMode_RelVerify,
          MUIA_Image_Spec       , type,
          MUIA_Background       , MUII_ButtonBack,
          MUIA_Weight,0,
          End

PROC volExists(vol,type="V") HANDLE;
DEF dlock=0,dl:PTR TO doslist,v,s[256]:STRING,pars,t1;
v:=news(vol); UpperStr(v);
IF v[StrLen(v)-1]=":" THEN v[StrLen(v)-1]:=0
SELECT type
CASE "V"; t1:=LDF_VOLUMES
CASE "A"; t1:=LDF_ASSIGNS
CASE "D"; t1:=LDF_DEVICES
ENDSELECT
pars:=t1 OR LDF_READ;
dl:=(dlock:=LockDosList(pars));
 WHILE dl:=NextDosEntry(dl,pars);
 StrCopy(s,BADDR(dl.name)+1);
 UpperStr(s)
 IF StrCmp(s,v) THEN Raise(-1)
 ENDWHILE;
EXCEPT DO
IF dlock THEN UnLockDosList(pars)
ENDPROC exception

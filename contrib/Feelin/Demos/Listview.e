OPT PREPROCESS

MODULE 'feelin','libraries/feelin',
      'dos/dos','dos/datetime',
      'graphics/rastport',
      'utility','utility/hooks','a4'

DEF feelinbase:PTR TO feelinbase,
   str_size:PTR TO CHAR,
   str_day:PTR TO CHAR,
   str_date:PTR TO CHAR,
   str_time:PTR TO CHAR,
   time:PTR TO datetime

->PROC
#define ListObject   F_NewObjA('List',[TAG_IGNORE,NIL

ENUM  FM_List_Remove,
     FM_List_InsertSingle,
     FA_List_Active

OBJECT fs_List_Construct         ; entry; pool; ENDOBJECT
OBJECT fs_List_Destruct          ; entry; pool; ENDOBJECT
OBJECT fs_List_Display           ; entry; strings:PTR TO LONG; preparses:PTR TO LONG; ENDOBJECT
OBJECT fs_List_Compare           ; entry; other; type1; type2; ENDOBJECT
OBJECT fs_List_Clear             ; what; ENDOBJECT

ENUM  FV_List_Active_PageDown = -7,
     FV_List_Active_PageUp,
     FV_List_Active_Down,
     FV_List_Active_Up,
     FV_List_Active_Bottom,
     FV_List_Active_Top,
     FV_List_Active_None

ENUM  FV_List_Insert_Bottom = -3,
     FV_List_Insert_Sorted,
     FV_List_Insert_Active,
     FV_List_Insert_Top
->ENDPROC

PROC main()
   DEF app,win,l=NIL,
      h_Const:hook,h_Destr:hook,h_Displ:hook,h_Compr:hook,
      lock,fib,h_Draw:hook,back[16]:STRING

   sys_SGlob()

   IF (str_size := New(128)) = NIL THEN RETURN
   IF (str_day  := New(32))  = NIL THEN RETURN
   IF (str_date := New(128)) = NIL THEN RETURN
   IF (str_time := New(16))  = NIL THEN RETURN
   IF time := New(SIZEOF datetime)
     time.format    := FORMAT_DOS
     time.flags     := DTF_SUBST
     time.strdate   := str_date
     time.strtime   := str_time
   ELSE
     RETURN
   ENDIF

   h_Draw.entry   := {hook_draw}
   h_Draw.data    := NIL

   StringF(back,'H:%08lx',h_Draw)

   IF lock := Lock('PROGDIR:',ACCESS_READ)
     IF fib := New(SIZEOF fileinfoblock)
       IF Examine(lock,fib) THEN JUMP __ok
       Dispose(fib)
     ENDIF
     UnLock(lock)
   ENDIF

   RETURN

__ok:

   h_Const.entry  := {hook_constructor}
   h_Const.data   := lock
   h_Destr.entry  := {hook_destructor}
   h_Displ.entry  := {hook_displayer}
   h_Compr.entry  := {hook_comparer}

   IF feelinbase := OpenLibrary('feelin.library',FV_VERSION)
     utilitybase := feelinbase.utility

     SetStdOut(feelinbase.console)

     app := AppObject,
       FA_Application_Title,        'demo_List',
       FA_Application_Version,      '$VER: demo_List 1.0 (2003/02/24)',
       FA_Application_Copyright,    '©2003 Olivier LAVIALE',
       FA_Application_Author,       'Olivier LAVIALE (gofromiel@gofromiel.com)',
       FA_Application_Description,  'FC_List demo',
       FA_Application_Base,         'DEMOLIST',

       Child, win := WindowObject,
         FA_ID,            "MAIN",
         FA_Window_Title,  'Feelin : Listview',
         FA_Window_Open,   TRUE,

         Child, VGroup,
            Child, F_NewObjA('Listview',['FA_Listview_List',
              l := ListObject,
                 FA_Font, 'XHelvetica/10',
                 FA_SetMin, FV_SetWidth,
                'FA_List_ConstructHook',   h_Const,
                'FA_List_DestructHook',    h_Destr,
                'FA_List_DisplayHook',     h_Displ,
                'FA_List_CompareHook',     h_Compr,
                'FA_List_Format',          '<col weight="50%" bar="true"/><col bar="true" fixed="true" /><col span="2" fixed="true" weight="25%" bar="true"/>',
              End,
            End,
         End,
       End,
     End

     IF app
       F_DoA(win,FM_Notify,[FA_Window_CloseRequest,FTRUE,app,FM_Application_Shutdown,0])
  
       WHILE F_DoA(l,'InsertSingle',[fib,FV_List_Insert_Sorted]) DO NOP

       F_DoA(app,FM_Application_Run,NIL)

       F_DisposeObj(app)
     ENDIF

     CloseLibrary(feelinbase)
   ENDIF

   UnLock(lock)
ENDPROC

PROC hook_constructor(hook=A0:PTR TO hook,msg=A1:PTR TO fs_List_Construct)
   DEF fib:PTR TO fileinfoblock

   sys_RGlob()

->   WriteF('Constructor - Lock \h - Pool \h - Entry \h\n',hook.data,msg.pool,msg.entry)

   IF ExNext(hook.data,msg.entry)
     IF fib := F_NewP(msg.pool,SIZEOF fileinfoblock)
       CopyMem(msg.entry,fib,SIZEOF fileinfoblock)
->         WriteF('ENTRY 0x%lx - %16.s - ',fib,fib.filename)
       RETURN fib
     ENDIF
   ENDIF
ENDPROC
PROC hook_destructor(hook=A0:PTR TO hook,obj=A2,msg=A1:PTR TO fs_List_Destruct)
   sys_RGlob()

   F_DisposeP(msg.pool,msg.entry)
ENDPROC
PROC hook_displayer(hook=A0:PTR TO hook,obj=A2,msg=A1:PTR TO fs_List_Display)
   DEF fib:PTR TO fileinfoblock,i

   sys_RGlob()

->   WriteF('Displ: FIB \h - Strings \h\n',msg.entry,msg.strings)

   IF (fib := msg.entry)
     msg.strings[0] := fib.filename

     IF fib.size
       msg.preparses[1]  := '<align=right>'
       msg.strings[1]    := StringF(str_size,'%ld',fib.size)
     ELSE
       msg.preparses[1]  := '<align=right><pens text=shine>'
       msg.strings[1]    := '(dir)'
     ENDIF

     CopyMem(fib.datestamp,time.stamp,SIZEOF datestamp)
     DateToStr(time)

     FOR i := 0 TO 32
       IF time.strdate[i] = " " THEN time.strdate[i] := "\0"; EXIT time.strdate[i] = "\0"
     ENDFOR

     msg.preparses[2]  := '<align=center>'
     msg.strings[2]    := time.strdate

     msg.preparses[3]  := '<align=right>'
     msg.strings[3]    := time.strtime
   ELSE
     msg.preparses[0] := '<pens style=shadow up=shine>'
     msg.preparses[1] := '<pens style=shadow up=shine><align=right>'
     msg.preparses[2] := '<pens style=shadow up=shine><align=center>'
     msg.preparses[3] := '<pens style=shadow up=shine><align=center>'

     msg.strings[0] := 'Name'
     msg.strings[1] := 'Size'
     msg.strings[2] := 'Date'
     msg.strings[3] := 'Time'
   ENDIF
ENDPROC
PROC hook_comparer(msg=A1:PTR TO fs_List_Compare)
   DEF fib1:PTR TO fileinfoblock,fib2:PTR TO fileinfoblock

   sys_RGlob()

   fib1 := msg.entry
   fib2 := msg.other

   RETURN Stricmp(fib1.filename,fib2.filename)
ENDPROC
PROC hook_draw(msg=A1:PTR TO FS_ImageDisplay_HookDraw)
   DEF x1,y1,x2,y2,x3,
      rp:PTR TO rastport,pt,op,ap,bp

   sys_RGlob()

   pt := [%1111111100000000,
        %0111111110000000,
        %0011111111000000,
        %0001111111100000,
        %0000111111110000,
        %0000011111111000,
        %0000001111111100,
        %0000000111111110,
        %0000000011111111,
        %1000000001111111,
        %1100000000111111,
        %1110000000011111,
        %1111000000001111,
        %1111100000000111,
        %1111110000000011,
        %1111111000000001]:INT

   x1 := msg.region.x1 ; x3 := msg.region.x2 ; x2 := x3 - x1 / 3 + x1
   y1 := msg.region.y1 ; y2 := msg.region.y2 ; rp := msg.render.rport

   ap := msg.render.palette.pens[FV_Pen_Fill]
   bp := msg.render.palette.pens[FV_Pen_HalfShadow]

   _APen(bp)
   _Boxf(x1,y1,x2,y2)

   rp.areaptrn := pt
   rp.areaptsz := 4

   _APen(ap)
   _BPen(bp)
   _Boxf(x2+1,y1,x3,y2)

   rp.areaptsz := 0
   rp.areaptrn := NIL
ENDPROC

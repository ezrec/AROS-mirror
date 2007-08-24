OPT PREPROCESS

MODULE 'feelin','libraries/feelin',
       'dos/dos'

CONST NUM = 1000

PROC main()
   DEF g:FObject,i,
       ds1:datestamp,ds2:datestamp,o

   IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      IF g := F_NewObjA(FC_Group,NIL)
         WriteF('This test will first create an instance of [1mFC_Group[0m.\nThen \d instances of [1mFC_Area[0m will be created and added to the FC_Group object as children (with an error).\n\n',NUM)

         WriteF('[33mF_NewObjA()[0m    ')

         Forbid() ; DateStamp(ds1)

         FOR i := 0 TO NUM - 1
            o := F_NewObjA(FC_Area,NIL); F_DoA(g,FM_AddMember,[o,FV_AddMember_Tail])
         ENDFOR

         DateStamp(ds2) ; Permit() ; saytime(ds1,ds2)

         F_DoA(g,FM_AddMember,[o,FV_AddMember_Tail])

         WriteF('[33mF_DisposeObj()[0m ')

         Forbid() ; DateStamp(ds1)

         F_DisposeObj(g)

         DateStamp(ds2) ; Permit() ; saytime(ds1,ds2)
      ENDIF

      CloseLibrary(feelinbase)
   ENDIF
ENDPROC

PROC saytime(ds1:PTR TO datestamp,ds2:PTR TO datestamp)
   DEF ticks

   ticks := ((((ds2.minute - ds1.minute) * 60)) * TICKS_PER_SECOND) + (ds2.tick - ds1.tick)

   WriteF('%12.ld per second (%4.ld ticks)\n',NUM * TICKS_PER_SECOND / ticks,ticks)
ENDPROC


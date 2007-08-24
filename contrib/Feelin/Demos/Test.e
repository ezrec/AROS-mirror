OPT PREPROCESS

MODULE 'feelin','libraries/feelin',
        'dos/dos'

CONST NUM = 4000

PROC main()
    DEF list:PTR TO LONG,i,
        ds1:datestamp,ds2:datestamp

    IF feelinbase := OpenLibrary('feelin.library',FV_FEELIN_VERSION)
      IF list := F_New(NUM + 1 * 4)
         WriteF('This test will create \d instances of [1mFC_Object[0m.\n\n',NUM)

         WriteF('[33mF_NewObjA()[0m    ')

         Forbid() ; DateStamp(ds1)

         FOR i := 0 TO NUM - 1 DO list[i] := F_NewObjA(FC_Object,NIL)

         DateStamp(ds2) ; Permit() ; saytime(ds1,ds2)

         WriteF('[33mF_DisposeObj()[0m ')

         Forbid() ; DateStamp(ds1)

         FOR i := 0 TO NUM - 1 DO F_DisposeObj(list[i])

         DateStamp(ds2) ; Permit() ; saytime(ds1,ds2)

         F_Dispose(list)
      ENDIF

      CloseLibrary(feelinbase)
    ENDIF
ENDPROC

PROC saytime(ds1:PTR TO datestamp,ds2:PTR TO datestamp)
    DEF ticks

    ticks := ((((ds2.minute - ds1.minute) * 60)) * TICKS_PER_SECOND) + (ds2.tick - ds1.tick)

    WriteF('%12.ld per second (%4.ld ticks)\n',NUM * TICKS_PER_SECOND / ticks,ticks)
ENDPROC

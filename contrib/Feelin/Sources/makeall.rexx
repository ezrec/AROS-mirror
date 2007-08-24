/* */

SIGNAL ON FAILURE

IF ~Show('L','rexxsupport.library') THEN DO
   IF ~AddLib('rexxsupport.library',0,-30) THEN DO
     Raise('Unable to open rexxsupport.library')
   END
END

/* Classes list by type */

list_system    = 'AppServer Application Dataspace Decorator Display DOSNotify Family ModulesList Preference Thread Window'
list_support   = 'BitMap Histogram ImageDisplay Palette Picture Render TextDisplay'
list_document  = 'Document HTMLDocument XMLDocument XMLObject XMLApplication'
list_gui       = 'Frame Area Balance Bar Crawler Cycle Decorator-Flatty Decorator-Shade DOSList Gauge Group Image List Listview Numeric PreferenceEditor PreferenceGroup Prop Radio Scale Scrollbar Slider String Text'
list_adjust    = 'Adjust AdjustBrush AdjustColor AdjustFrame AdjustGradient AdjustImage AdjustPen AdjustPreParse AdjustPicture AdjustRaster AdjustRGB AdjustScheme AdjustSchemeEntry AdjustSpacing'
list_pop       = 'PopColor PopFile PopFont PopFrame PopHelp PopImage PopPreParse PopScheme'
list_preview   = 'Preview PreviewColor PreviewFrame PreviewImage PreviewScheme'
list_dialog    = 'Dialog FileChooser FontChooser FontDialog'

list = list_system list_support list_document list_gui list_adjust list_pop list_preview list_dialog

/* Let's go ! */

PARSE ARG action

IF action = '' THEN action = 'MAKE'

IF action = 'DEL' THEN DO
   ADDRESS COMMAND 'Resident QUIET ADD C:Delete PURE'

   DO i = 1 TO Words(list)
     ADDRESS COMMAND 'Delete >NIL: Feelin:Sources/' || Word(list,i) || '/#?.o'
   END

   ADDRESS COMMAND 'Resident QUIET REMOVE Delete'
END
ELSE IF action = 'CHG' THEN DO
   DO i = 1 TO Words(list)
     Pragma(D,'Feelin:Sources/' || Word(list,i))

     ADDRESS COMMAND 'F_Changes'
   END
END
ELSE IF action = 'MKMK' THEN DO
   DO i = 1 TO Words(list)
     Pragma(D,'Feelin:Sources/' || Word(list,i))

     ADDRESS COMMAND 'F_Make'
   END
END
ELSE DO
   Say('Building Classes...')

   DO i = 1 TO Words(list)
     IF EXISTS('Feelin:Sources/' || Word(list,i) || '/_locale') THEN DO
       
       Say('Feelin:Sources/' || Word(list,i) || '/_locale')
     
       ADDRESS COMMAND 'make --no-print-directory --directory=Feelin:Sources/' || Word(list,i) || '/_locale'
     END

 
     ADDRESS COMMAND 'make --no-print-directory --directory=Feelin:Sources/' || Word(list,i)
   END
END

EXIT

/* Personal Paint Amiga Rexx script - Copyright 1996 Cloanto Italia srl */

/* $VER: REKO2Soliton.pprx 1.0 */

/** ENG
 This script converts a REKO cardset into a Soliton cardset. 

 Caveats: For HAM cardsets the REKO cardset has to be saved as a picture 
 (e.g. IFF ILBM) before using this script. The number of colours is reduced to
 a maximum of 256. The PPaint screen height should not exceed 520 pixel.
*/

IF ARG(1, EXISTS) THEN
        PARSE ARG PPPORT
ELSE
        PPPORT = 'PPAINT'

IF ~SHOW('P', PPPORT) THEN DO
        IF EXISTS('PPaint:PPaint') THEN DO
                ADDRESS COMMAND 'Run >NIL: PPaint:PPaint'
                DO 30 WHILE ~SHOW('P',PPPORT)
                         ADDRESS COMMAND 'Wait >NIL: 1 SEC'
                END
        END
        ELSE DO
                SAY "Personal Paint could not be loaded."
                EXIT 10
        END
END

IF ~SHOW('P', PPPORT) THEN DO
        SAY 'Personal Paint Rexx port could not be opened'
        EXIT 10
END

ADDRESS VALUE PPPORT
OPTIONS RESULTS
OPTIONS FAILAT 10000

Get 'LANG'
IF RESULT = 1 THEN DO         /* Deutsch */
        txt_err_oldclient = 'Fuer dieses Skript_ist eine neuere Version_von Personal Paint erforderlich'
END
ELSE DO                         /* English */
        txt_err_oldclient = 'This script requires a newer_version of Personal Paint'
END

Version 'REXX'
IF RESULT < 7 THEN DO
        RequestNotify 'PROMPT "'txt_err_oldclient'"'
        EXIT 10
END

LoadImage
IF RC = 0 THEN DO
        Set '"Transp=0"'
        DO i = 0 to 12
                DO j = 0 to 3
                        DefineBrush (90*i+2) (131*j+1) (90*i+89) (131*j+130) erase
                        SetBrushHandle upperleft
                        PutBrush (88 * i) (130 * j)
                END j
        END i
        Get 'ImageW'
        Breite = RESULT
        IF Breite = 1532 THEN DO
                DrawRectangle 1144 0 1232 520 fill erase
                DefineBrush 1352 132 1439 261 erase

                SetBrushHandle upperleft
                PutBrush 1144 0
                DefineBrush 1262 132 1349 261 erase
                SetBrushHandle upperleft
                PutBrush 1144 130
        END
        IF Breite = 1262 THEN DO
                DrawRectangle 1144 0 1232 129 fill erase
                DefineBrush 1172 132 1259 261 erase
                SetBrushHandle upperleft
                PutBrush 1144 0
                DefineBrush 1172 263 1259 392 erase
                SetBrushHandle upperleft
                PutBrush 1144 130
                PutBrush 1144 260 erase
        END
        Set 'Force "IMAGEW=1232" "IMAGEH=520"'
        SaveImage 'force'
END


/*****************************************************************************
 * Modul  : mpsx.c                                                           *
 * Zweck  : Einlesen eines files im MPSX-Standardeingabeformat               *
 * Format : load <file> [-cGOAL] [-bRHS] [-rRANGES] [-uBOUNDS] [-m] [-fFILE] *
 * Autor  : Stefan Förster                                                   *
 *                                                                           *
 * Datum      | Version | Bemerkung                                          *
 * -----------|---------|--------------------------------------------------- *
 * 05.03.1989 | 0.0     |                                                    *
 * 07.03.1989 | 0.1     | PrintError()                                       *
 * 08.03.1989 | 0.2     | Pass1()                                            *
 * 09.03.1989 |         | Pass1() fertig                                     *
 * 10.03.1989 | 0.3     | Zusätzliche Fehlermeldungen                        *
 *            |         | SearchEl() liefert jetzt den Typ ITEMPTR           *
 *            |         | ChooseSymbols(), Select()                          *
 * 11.03.1989 | 0.4     | Pass2()                                            *
 *            |         | Felder GLOBAL definiert                            *
 * 13.03.1989 | 0.5     | Pass2() und CorrectBounds() fertig                 *
 *            |         | BUG in ParseLine(): &stop[1] statt &stop[2]        *
 *            |         | BUG in TakeMem(): AvailMem(NULL) statt AvailMem()  *
 *            |         | BUG in ParseLine(): list[GOALS_LIST] durchsuchen   *
 *            |         | BUG in MPSX(): ChooseSymbols() statt ChooseSymbols *
 * 14.03.1989 | 1.0     | BUG in Pass2(): Wenn symbflag & BIT_RHS, ist       *
 *            |         |                 list[RHS_LIST] leer (DeleteList()) *
 *            |         | BUG in Pass2(): Fehler bei 2 Zielfkt. behoben      *
 *            |         | BUG in Pass2(): RANGES: if(++i>num_lines) ...      *
 *            |         | BUG in Pass2(): RANGES: num_rows+i statt m+i       *
 *            |         | BUG in Pass2(): Schl.var.: Zeile iptr->nr statt i  *
 *            |         | BUG in CorrectBounds(): upper[i] -= lower[i];      *
 *            |         | 1. Testlauf erfolgreich                            *
 * 17.03.1989 | 1.1     | zusätzlich min cTx möglich                         *
 * 18.03.1989 | 1.2     | Optional Ausgabe an ein File möglich (-f)          *
 *****************************************************************************/

#include <aros/oldprograms.h>
#include <string.h>
#include <stdio.h>
#include "simplex.h"


//IMPORT STRPTR ctime();
IMPORT VOID   SetM();
IMPORT FILE   *fopen();
IMPORT INT    fseek();
IMPORT LONG   ftell()/*, time()*/;

GLOBAL DOUBLE  INFINITE;
GLOBAL BOOL    minimize;

GLOBAL DOUBLE  c0start, c0;
GLOBAL SHORT   m, n, *B, *Nq;
GLOBAL DOUBLE  *A, *AB1, *b, *b2q, *c, *c2, *upper, *lower;
GLOBAL DOUBLE  *x, *cq, *pi, *dq;
GLOBAL SHORT   *Nminus;
GLOBAL DOUBLE  *help;

GLOBAL STRPTR  errors[];


USHORT      symbflag;
GLOBAL TEXT symbols[NUM_SYMBOLS][MAX_STRLEN+1];
TEXT        filename[MAX_FILELEN+1];
GLOBAL FILE *file[2];
//struct stat file_stat;

GLOBAL ITEMPTR list[NUM_LISTS];
LONG        position[NUM_SYMBOLS];

SHORT       num_var, num_slack, num_rows, num_lines, num_goals, num_rhs;
SHORT       num_ranges, num_bounds;

LONG        mm, nn;

TEXT        line_nr[9];         /* Zählvariable, max 99999999 (8 Zeichen) */
TEXT        line[BUFFER];       /* eingelesene Zeile */
TEXT        buf[4][BUFFER2+1];  /* Zwischenspeicher */

VOID    Cap(STRPTR  str, SHORT   start, SHORT stop);
BOOL    SearchExpr(STRPTR  str, SHORT   *start, SHORT  *stop);
ITEMPTR SearchEl(STRPTR  str, ITEMPTR list, BOOL    spezial,STRPTR  chstr);    /* Zeiger auf Character */
VOID    PrintError(INT     nr,STRPTR  str);
SHORT   GetExpr(STRPTR  dest, STRPTR ptr, SHORT   start, SHORT stop);

SHORT sys_nerr;


/*****************************************************************************
 * BOOL MPSX() -> _TRUE/_FALSE                                               *
 * Hauptprogramm von MPSX.c                                                  *
 *                                                                           *
 * Input:  args   : Zeiger auf Argument-String (hinter load)                 *
 *                                                                           *
 * Output: _FALSE : Fehler beim Einlesen                                     *
 *         _TRUE  : c0start: Korrekturwert für den Zielfunktionswert         *
 *                  m,n: m und n                                             *
 *                  Zeiger auf A,AB1,etc.                                    *
 *****************************************************************************/

BOOL    MPSX(args)

STRPTR  args;

{
  SHORT i;
  BOOL  GetArgs(), Pass1(), Pass2(), ChooseSymbols(), CorrectBounds();
  VOID  GetRidOfLists(), GiveMemBack();

  num_var = num_goals = num_rhs = num_ranges = num_bounds = 0;
  num_slack = num_rows = num_lines = 0;

  GetRidOfLists();

  for(i=0; i<NUM_SYMBOLS; ++i) position[i] = -1L;

  if(!GetArgs(args)) return(_FALSE);

  if(!Pass1() || !ChooseSymbols() || !Pass2() || !CorrectBounds()) {
    GetRidOfLists();
    GiveMemBack();
    return(_FALSE);
  }

  return(_TRUE);
}



/*****************************************************************************
 * BOOL GetArgs() -> _TRUE/_FALSE                                            *
 * wertet den Argument-String hinter load aus und setzt symbflag und         *
 * symbols[][] entsprechend                                                  *
 * _TRUE  : O.K. (file wurde geöffnet)                                       *
 * _FALSE : Fehler aufgetreten                                               *
 *****************************************************************************/

BOOL    GetArgs(str)

STRPTR  str;

{
  SHORT   start, stop, length;
  LONG    t;
  TEXT    ch;
  VOID    PrintError();
  /* SHORT   GetExpr(); */
  STRPTR  ptr;

  symbflag = 0;

  if(!SearchExpr(str,&start,&stop)) {
    PrintError(ERR_INVALID_ARGS,NULL);
    return(_FALSE);
  }
  else {   /* file-Name gefunden */
    length = GetExpr(filename,str,start,stop);
    if(length > MAX_FILELEN) {
      PrintError(ERR_FILE_TOO_LONG,filename);
      return(_FALSE);
    }
    else { /* und hat korrekte Länge */
      if(!(file[0] = fopen(filename,"r"))) {
        PrintError(errno,filename);
        PrintError(ERR_NOT_READ,filename);
        return(_FALSE);
      }
    }
  }

  /* Jetzt die Argumente */

  ptr = str+stop+1;

  while(SearchExpr(ptr,&start,&stop)) {

    Cap(ptr,start,stop);

    if((length = stop-start-1)==0 && strncmp(ptr+start,"-M",2)==0)
      minimize = _TRUE;

    else if(length > 0 && strncmp(ptr+start,"-F",2) == 0) {
      length = GetExpr(buf[0],ptr,start+2,stop);        
      if(length > MAX_FILELEN) {
        PrintError(ERR_FILE_TOO_LONG,buf[0]);
        return(_FALSE);
      }
      if(!(file[1] = fopen(buf[0],"w"))) {
        PrintError(errno,buf[0]);
        PrintError(ERR_NOT_WRITE,buf[0]);
        return(_FALSE);
      }
      fputs("ASimplex Version 1.2\n",file[1]);
      fputs("THE Amiga Simplex Program\n",file[1]);
      fputs("(c) 18.03.1989 Stefan Förster\n\n",file[1]);
      t = time(NULL);
      fprintf(file[1],"Hardcopy on %s\n",ctime(&t));
    }

    else {

      if(length <= 0) goto invalid; /* (stop-start+1)-2 */

      if(length > MAX_STRLEN) {
        *(ptr+stop+1) = '\0';
        PrintError(ERR_NAME_TOO_LONG,ptr+start+2);
        return(_FALSE);
      }

      if(strncmp(ptr+start,"-C",2) == 0) {
        if(symbflag & BIT_GOAL) goto invalid; /* zwei Zielfkt. angegeben */
        else {
          symbflag |= BIT_GOAL;
          strncpy(symbols[GOAL],ptr+start+2,length);
          symbols[GOAL][length] = '\0';
        }
      }
      else if(strncmp(ptr+start,"-B",2) == 0) {
        if(symbflag & BIT_RHS) goto invalid;
        else {
          symbflag |= BIT_RHS;
          strncpy(symbols[RHS],ptr+start+2,length);
          symbols[RHS][length] = '\0';
        }
      }
      else if(strncmp(ptr+start,"-R",2) == 0) {
        if(symbflag & BIT_RANGES) goto invalid;
        else {
          symbflag |= BIT_RANGES;
          strncpy(symbols[RANGES],ptr+start+2,length);
          symbols[RANGES][length] = '\0';
        }
      }
      else if(strncmp(ptr+start,"-U",2) == 0) {
        if(symbflag & BIT_BOUNDS) goto invalid;
        else {
          symbflag |= BIT_BOUNDS;
          strncpy(symbols[BOUNDS],ptr+start+2,length);
          symbols[BOUNDS][length] = '\0';
        }
      }
      else goto invalid;
    }

    ptr += stop+1;

  } /* while() */

/* AROS
  if(stat(filename,&file_stat) == -1) {
    PrintError(errno,filename);
    return(_FALSE);
  }
  else {
    printf("== loading %s %ld bytes\n",filename,file_stat.st_size);
    printf("   %s",ctime(&file_stat.st_mtime));
  }
*/
  return(_TRUE);

invalid: /* Sprungmarke (na ja) für "invalid arguments" */
  PrintError(ERR_INVALID_ARGS,NULL);
  return(_FALSE);
}



/*****************************************************************************
 * BOOL Pass1() -> _TRUE/_FALSE                                              *
 * Pass1() durchläuft <file> zum ersten Mal. Es werden Listen der Variablen, *
 * der rechten Seiten (falls noch nicht festgelegt) usw. angelegt. Außerdem  *
 * erfolgt eine Syntaxüberprüfung.                                           *
 * _TRUE  : OK                                                               *
 * _FALSE : Fehler aufgetreten                                               *
 *****************************************************************************/

BOOL Pass1()

{
  LONG    count = 0, pos;
  SHORT   start[5], stop[5], length;
  USHORT  found = 0, actual = 0;
  TEXT    ch;
  STRPTR  ptr[5];
  ITEMPTR iptr;
  BOOL    ParseLine();
  VOID    PrintError(), UpdateLine();
  /* SHORT   GetExpr(); */
  /* ITEMPTR NewListEl(), SearchEl(); */
  ITEMPTR NewListEl();

  printf("P1 0");
  sprintf(line_nr,"%ld",count);

  ptr[0] = line;

  /* 1. Sektion muß "NAME" sein */

  do {
    if(!fgets(ptr[0],BUFFER-1,file[0])) {
      if(feof(file[0])) goto finish;
      if(ferror(file[0])) {
        puts("");
        PrintError(errno,NULL);
        return(_FALSE);
      }
    }
    UpdateLine(&count);
    pos = ftell(file[0]);
    Cap(ptr[0],0,BUFFER);
  } while(!SearchExpr(ptr[0],&start[0],&stop[0]));
  length = GetExpr(buf[0],ptr[0],start[0],stop[0]);

  if(start[0] != 0) {
    puts("");
    PrintError(ERR_SECTIONS,buf[0]);
    PrintError(ERR_NO_NAME,buf[0]);
    return(_FALSE);
  }

  if(length != 4 || strncmp(buf[0],"NAME",4) != 0) {
    puts("");
    PrintError(ERR_NO_NAME,buf[0]);
    return(_FALSE);
  }

  if(!SearchExpr((ptr[1]=ptr[0]+stop[0]+1),&start[1],&stop[1])) {
    puts("");
    PrintError(ERR_NO_NAME,NULL);
    return(_FALSE);
  }
  length = GetExpr(buf[1],ptr[1],start[1],stop[1]);

  if(length > MAX_STRLEN) {
    puts("");
    PrintError(ERR_NAME_TOO_LONG,buf[1]);
    return(_FALSE);
  }

  symbflag |= BIT_NAME;
  found |= BIT_NAME;
  strcpy(symbols[NAME],buf[1]);
  position[NAME] = pos;


  /* Jetzt folgen die anderen Sektionen */

  FOREVER {

    if(feof(file[0])) goto finish;

    if(!fgets(ptr[0],BUFFER-1,file[0])) {
      if(ferror(file[0])) {
        puts("");
        PrintError(errno,NULL);
        return(_FALSE);
      }
    }
    UpdateLine(&count);
    pos = ftell(file[0]);

    Cap(ptr[0],0,BUFFER);

    if(!SearchExpr(ptr[0],&start[0],&stop[0])) continue; /* FOREVER */
    length = GetExpr(buf[0],ptr[0],start[0],stop[0]);
    if(length > MAX_STRLEN) {
      puts("");
      PrintError(ERR_NAME_TOO_LONG,buf[0]);
      return(_FALSE);
    }

    if(start[0] == 0) {  /* Start einer neuen Sektion */

      if(length==6 && strncmp(buf[0],"ENDATA",6) == 0) {
        found |= BIT_ENDATA;
        goto finish;
      }
      else if(length==4 && strncmp(buf[0],"ROWS",4) == 0) {
        if(found & BIT_ROWS) {
          puts("");
          PrintError(ERR_2SECTIONS,buf[0]);
          return(_FALSE);
        }
        else {
          found |= BIT_ROWS;
          actual = BIT_ROWS;
          position[ROWS] = pos;
          continue;
        }
      }
      else if(length==7 && strncmp(buf[0],"COLUMNS",7) == 0) {
        if(found & BIT_COLUMNS) {
          puts("");
          PrintError(ERR_2SECTIONS,buf[0]);
          return(_FALSE);
        }
        else {
          found |= BIT_COLUMNS;
          actual = BIT_COLUMNS;
          position[COLUMNS] = pos;
          continue;
        }
      }
      else if(length==3 && strncmp(buf[0],"RHS",3) == 0) {
        if(found & BIT_RHS) {
          puts("");
          PrintError(ERR_2SECTIONS,buf[0]);
          return(_FALSE);
        }
        else {
          found |= BIT_RHS;
          actual = BIT_RHS;
          position[RHS] = pos;
          continue;
        }
      }
      else if(length==6 && strncmp(buf[0],"RANGES",6) == 0) {
        if(found & BIT_RANGES) {
          puts("");
          PrintError(ERR_2SECTIONS,buf[0]);
          return(_FALSE);
        }
        else {
          found |= BIT_RANGES;
          actual = BIT_RANGES;
          position[RANGES] = pos;
          continue;
        }
      }
      else if(length==6 && strncmp(buf[0],"BOUNDS",6) == 0) {
        if(found & BIT_BOUNDS) {
          puts("");
          PrintError(ERR_2SECTIONS,buf[0]);
          return(_FALSE);
        }
        else {
          found |= BIT_BOUNDS;
          actual = BIT_BOUNDS;
          position[BOUNDS] = pos;
          continue;
        }
      }
      else {
        puts("");
        PrintError(ERR_UNKNOWN_SEC,buf[0]);
        return(_FALSE);
      }

    }  /* if(start[0]==0) */

    else {  /* if(start[0]!=0) */

      if(actual == 0) {
        puts("");
        PrintError(ERR_SECTIONS,buf[0]);
        return(_FALSE);
      }

      switch(actual) {

        case BIT_ROWS:
          if(length!=1 || (ch = buf[0][0])!='N' && ch!='E' && ch!='L' && 
             ch!='G') {
            puts("");
            PrintError(ERR_INV_ROWS_TYPE,buf[0]);
            return(_FALSE);
          }
          ptr[1] = ptr[0]+start[0]+1;
          if(!SearchExpr(ptr[1],&start[1],&stop[1])) {
            puts("");
            PrintError(ERR_MISSING,NULL);
            return(_FALSE);
          }
          length = GetExpr(buf[0]+1,ptr[1],start[1],stop[1]);
          if(length > MAX_STRLEN) {
            puts("");
            PrintError(ERR_NAME_TOO_LONG,buf[0]+1);
            return(_FALSE);
          }
          if(ch == 'N') {  /* Zielfunktion */
            if(SearchEl(buf[0]+1,list[GOALS_LIST],_FALSE,NULL)) {
              puts("");
              PrintError(ERR_DOUBLE,buf[0]+1);
              return(_FALSE);
            }
            if(!(iptr = NewListEl(list[GOALS_LIST],buf[0]+1))) return(_FALSE);
            else {
              if(list[GOALS_LIST]) iptr->nr = list[GOALS_LIST]->nr+1;
              else                 iptr->nr = 1;
              list[GOALS_LIST] = iptr;
              ++num_goals;
            }
          }
          else {   /* Typ L, G oder E */
            if(SearchEl(buf[0]+1,list[ROWS_LIST],_TRUE,NULL)) {
              puts("");
              PrintError(ERR_DOUBLE,buf[0]+1);
              return(_FALSE);
            }
            if(!(iptr = NewListEl(list[ROWS_LIST],buf[0]))) return(_FALSE);
            else {
              if(list[ROWS_LIST]) iptr->nr = list[ROWS_LIST]->nr+1;
              else                iptr->nr = 1;
              list[ROWS_LIST] = iptr;
              ++num_rows;
              if(ch == 'L' || ch == 'G') ++num_slack;
            }
          }
          break;

        case BIT_COLUMNS:
          if(!ParseLine(VAR_LIST,buf[0],stop[0])) return(_FALSE);
          break;

        case BIT_RHS:
          if(!ParseLine(RHS_LIST,buf[0],stop[0])) return(_FALSE);
          break;

        case BIT_RANGES:
          if(!ParseLine(RANGES_LIST,buf[0],stop[0])) return(_FALSE);
          break;

        case BIT_BOUNDS:
          if(length!=2 || (strncmp(buf[0],"UP",2)!=0 &&
             strncmp(buf[0],"LO",2)!=0)) {
            puts("");
            PrintError(ERR_INV_BOUNDS_TYPE,buf[0]);
            return(_FALSE);
          }
          /* Jetzt müssen drei Ausdrücke folgen */
          if(!SearchExpr((ptr[1]=ptr[0]+stop[0]+1),&start[1],&stop[1]) ||
             !SearchExpr((ptr[2]=ptr[1]+stop[1]+1),&start[2],&stop[2]) ||
             !SearchExpr((ptr[3]=ptr[2]+stop[2]+1),&start[3],&stop[3])   ) {
            puts("");
            PrintError(ERR_MISSING,NULL);
            return(_FALSE);
          }
          length = GetExpr(buf[1],ptr[1],start[1],stop[1]);
          if(length > MAX_STRLEN) {
            puts("");
            PrintError(ERR_NAME_TOO_LONG,buf[1]);
            return(_FALSE);
          }
          if(!SearchEl(buf[1],list[BOUNDS_LIST],_FALSE,NULL)) {
            if(!(iptr = NewListEl(list[BOUNDS_LIST],buf[1]))) return(_FALSE);
            else {
              if(list[BOUNDS_LIST]) iptr->nr = list[BOUNDS_LIST]->nr+1;
              else                  iptr->nr = 1;
              list[BOUNDS_LIST] = iptr;
              ++num_bounds;
            }
          }
          length = GetExpr(buf[2],ptr[2],start[2],stop[2]);
          if(length > MAX_STRLEN) {
            puts("");
            PrintError(ERR_NAME_TOO_LONG,buf[2]);
            return(_FALSE);
          }
          if(!SearchEl(buf[2],list[VAR_LIST],_FALSE,NULL)) {
            puts("");
            PrintError(ERR_UNKNOWN_ID,buf[2]);
            return(_FALSE);
          }
          break;

      } /* switch(actual) */
    } /* else if(start[0]!=0) */
  } /* FOREVER() */


finish:  /* Sprungmarke, falls feof(file[0]) oder ENDATA erreicht */
  if(!(found & BIT_ROWS) || num_rows == 0) {
    puts("");
    PrintError(ERR_NO_ROWS,NULL);
    return(_FALSE);
  }
  if(!(found & BIT_COLUMNS) || num_var == 0) {
    puts("");
    PrintError(ERR_NO_COLUMNS,NULL);
    return(_FALSE);
  }
  if(!(found & BIT_RHS)) {
    puts("");
    PrintError(ERR_NO_RHS,NULL);
    return(_FALSE);
  }
  if(!(found & BIT_ENDATA)) {
    puts("");
    PrintError(ERR_NO_ENDATA,NULL);
    return(_FALSE);
  }
  if(position[NAME] >= position[ROWS] ||
     position[ROWS] >= position[COLUMNS] ||
     position[COLUMNS] >= position[RHS] ||
     position[RANGES] != -1 && position[BOUNDS] != -1 &&
     position[RANGES] >= position[BOUNDS]                ) {
    puts("");
    PrintError(ERR_ORDER,NULL);
    return(_FALSE);
  }

  puts("");
  return(_TRUE);  /* alle Fehlerfälle überstanden */
}



/*****************************************************************************
 * VOID UpdateLine()                                                         *
 * Aktualisiert den Zeilenzähler von Pass1()                                 *
 *****************************************************************************/

VOID UpdateLine(c)

LONG *c;

{
  SHORT i, len = strlen(line_nr);

  for(i=0; i<len; ++i) printf("%c",8);
  sprintf(line_nr,"%ld",++(*c));
  printf("%s",line_nr);
}



/*****************************************************************************
 * BOOL ParseLine(list_type,bufptr,end0) -> _TRUE/_FALSE                     *
 * Überprüft eine Zeile (aus der Sektion COLUMNS, RHS oder RANGES) auf       *
 * korrekte Syntax. bufptr ist Zeiger auf den Puffer, der den ersten Aus-    *
 * druck der Zeile enthält. Falls dieser noch nicht in der jeweiligen Liste  *
 * enthalten ist, wird er angefügt. list_type kann die Werte VAR_LIST,       *
 * RHS_LIST oder RANGES_LIST annehmen. end0 ist stop[0] von Pass1()          *
 * _TRUE  : O.K.                                                             *
 * _FALSE : Fehler aufgetreten                                               *
 *****************************************************************************/

BOOL    ParseLine(list_type,bufptr,end0)

SHORT   list_type;
STRPTR  bufptr;
SHORT   end0;

{
  SHORT   start[5], stop[5], length;
  TEXT    ch;
  STRPTR  ptr[5];
  ITEMPTR iptr, dptr;
  ITEMPTR NewListEl();


  ptr[0] = line; /* wie auch in Pass1() */

  if(!(dptr = SearchEl(bufptr,list[list_type],_FALSE,NULL))) { /* neue Var. */
    if(!(iptr = NewListEl(list[list_type],bufptr))) return(_FALSE);
    else {
      if(list[list_type]) iptr->nr = list[list_type]->nr+1;
      else                iptr->nr = 1;
      if(list_type == RANGES_LIST) iptr->anz = 0;
      list[list_type] = iptr;
      switch(list_type) {
        case VAR_LIST:    ++num_var;
                          break;
        case RHS_LIST:    ++num_rhs;
                          break;
        case RANGES_LIST: ++num_ranges;
                          break;
      }
      dptr = iptr;
    }
  }
  /* dptr zeigt jetzt auf den 1.Eintrag der Zeile */
  ptr[1] = ptr[0]+end0+1; /* mind. 2 Ausdrücke in dieser Zeile */
  if(!SearchExpr(ptr[1],&start[1],&stop[1]) ||
     !SearchExpr((ptr[2] = ptr[1]+stop[1]+1),&start[2],&stop[2])) {
    puts("");
    PrintError(ERR_MISSING,NULL);
    return(_FALSE);
  }
  length = GetExpr(buf[1],ptr[1],start[1],stop[1]);
  if(length > MAX_STRLEN) {
    puts("");
    PrintError(ERR_NAME_TOO_LONG,buf[1]);
    return(_FALSE);
  }
  ch = '\0';
  if(!SearchEl(buf[1],list[ROWS_LIST],_TRUE,&ch) &&
     !SearchEl(buf[1],list[GOALS_LIST],_FALSE,NULL) ) {
    puts("");
    PrintError(ERR_UNKNOWN_ID,buf[1]);
    return(_FALSE);
  }
  if(list_type == RANGES_LIST && ch == 'E') {
    puts("");
    PrintError(ERR_INV_RANGES,buf[1]);
    return(_FALSE);
  }
  if(list_type == RANGES_LIST) ++dptr->anz;
  ptr[3] = ptr[2]+stop[2]+1; /* noch mehr (genau 2) ? */
  if(SearchExpr(ptr[3],&start[3],&stop[3])) {
    ptr[4] = ptr[3]+stop[3]+1;
    if(!SearchExpr(ptr[4],&start[4],&stop[4])) {
      puts("");
      PrintError(ERR_MISSING,NULL);
      return(_FALSE);
    }
    else {
      length = GetExpr(buf[3],ptr[3],start[3],stop[3]);
      if(length > MAX_STRLEN) {
        puts("");
        PrintError(ERR_NAME_TOO_LONG,buf[3]);
        return(_FALSE);
      }
      ch = '\0';
      if(!SearchEl(buf[3],list[ROWS_LIST],_TRUE,&ch) &&
         !SearchEl(buf[3],list[GOALS_LIST],_FALSE,NULL) ) {
        puts("");
        PrintError(ERR_UNKNOWN_ID,buf[3]);
        return(_FALSE);
      }
      if(list_type == RANGES_LIST && ch == 'E') {
        puts("");
        PrintError(ERR_INV_RANGES,buf[1]);
        return(_FALSE);
      }
      if(list_type == RANGES_LIST) ++dptr->anz;
    }
  }

  return(_TRUE);
}



/*****************************************************************************
 * BOOL ChooseSymbols() -> _TRUE/_FALSE                                      *
 * Legt endgültig die Bezeichner der Zielfunktion, der rechten Seite, des    *
 * Bereichs und der Grenzen fest. Außerdem wird num_lines ermittelt.         *
 * _TRUE  : O.K.                                                             *
 * _FALSE : Fehler aufgetreten                                               *
 *****************************************************************************/

BOOL ChooseSymbols()

{
  ITEMPTR dptr;
  /* ITEMPTR Select(), SearchEl(); */
  ITEMPTR Select();
  VOID    DeleteList();


  if(!(symbflag & BIT_GOAL)) {
    if(num_goals == 0) {
      PrintError(ERR_NO_GOAL,NULL);
      return(_FALSE);
    }
    else if(num_goals == 1) {
      strcpy(symbols[GOAL],list[GOALS_LIST]->string);
      symbflag |= BIT_GOAL;
    }
    else {
      if(!(dptr = Select(list[GOALS_LIST],"Possible goals"))) return(_FALSE);
      else {
        strcpy(symbols[GOAL],dptr->string);
        symbflag |= BIT_GOAL;
      }
    }
  }
  else if(!SearchEl(symbols[GOAL],list[GOALS_LIST],_FALSE,NULL)) {
    PrintError(ERR_NO_GOAL,NULL);
    return(_FALSE);
  }
  DeleteList(&list[GOALS_LIST]);

  /* Ist Zielfunktion schon als normale Zeile deklariert ? */
  if(SearchEl(symbols[GOAL],list[ROWS_LIST],_TRUE,NULL)) {
    PrintError(ERR_DOUBLE,symbols[GOAL]);
    return(_FALSE);
  }

  if(!(symbflag & BIT_RHS) && num_rhs>0) {
    if(num_rhs == 1) {
      strcpy(symbols[RHS],list[RHS_LIST]->string);
      symbflag |= BIT_RHS;
    }
    else {
      if(!(dptr = Select(list[RHS_LIST],"Possible right hand sides")))
        return(_FALSE);
      else {
        strcpy(symbols[RHS],dptr->string);
        symbflag |= BIT_RHS;
      }
    }
  }
  DeleteList(&list[RHS_LIST]);

  if(symbflag & BIT_RANGES) {
    if(dptr = SearchEl(symbols[RANGES],list[RANGES_LIST],_FALSE,NULL))
         num_lines = dptr->anz;
    else num_lines = 0;
  }
  else {
    if(num_ranges == 0) num_lines = 0;
    else if(num_ranges == 1) {
      strcpy(symbols[RANGES],list[RANGES_LIST]->string);
      symbflag |= BIT_RANGES;
      num_lines = list[RANGES_LIST]->anz;
    }
    else {
      if(!(dptr = Select(list[RANGES_LIST],"Possible ranges"))) return(_FALSE);
      else {
        strcpy(symbols[RANGES],dptr->string);
        symbflag |= BIT_RANGES;
        num_lines = dptr->anz;
      }
    }
  }
  DeleteList(&list[RANGES_LIST]);

  if(!(symbflag & BIT_BOUNDS) && num_bounds>0) {
    if(num_bounds == 1) {
      strcpy(symbols[BOUNDS],list[BOUNDS_LIST]->string);
      symbflag |= BIT_BOUNDS;
    }
    else {
      if(!(dptr = Select(list[BOUNDS_LIST],"Possible bounds")))
        return(_FALSE);
      else {
        strcpy(symbols[BOUNDS],dptr->string);
        symbflag |= BIT_BOUNDS;
      }
    }
  }
  DeleteList(&list[BOUNDS_LIST]);

  return(_TRUE);

}



/*****************************************************************************
 * ITEMPTR Select(lptr,str)                                                  *
 * Auswahl eines Eintrags der Liste lptr durch den Benutzer.                 *
 * Ergebnis ist der Zeiger auf den gewählten Eintrag, NULL, falls ein Fehler *
 * aufgetreten ist.  lptr != NULL wird angenommen. Zu Beginn wird der String *
 * str ausgegeben.                                                           *
 *****************************************************************************/

ITEMPTR Select(lptr,str)

ITEMPTR lptr;
STRPTR  str;

{
  ITEMPTR ptr = lptr;
  SHORT   count = 0, /*atoi(),*/ choice, i;
  INT     GetInput(), error;

  printf("?? %s:\n",str);

  while(ptr) {
    printf("   %-8s  %3d\n",ptr->string,++count);
    ptr = ptr->next;
  }

  do {
    printf("?? Your choice : ");
    if(error = GetInput(line)) {
      PrintError(error,NULL);
      return(NULL);
    }
  } while((choice = atoi(line)) <= 0 || choice > count);

  ptr = lptr;
  for(i=1; i<choice; ++i) ptr = ptr->next;

  return(ptr);
}



/*****************************************************************************
 * BOOL Pass2() -> _TRUE/_FALSE                                              *
 * Pass2() stellt den Speicherplatz bereit und liest die Daten in die er-    *
 * zeugten Felder ein.                                                       *
 * _TRUE  : O.K.                                                             *
 * _FALSE : Fehler aufgetreten                                               *
 *****************************************************************************/

BOOL Pass2()

{
  TEXT    ch;
  SHORT   i, j, count, start[5], stop[5], length;
  STRPTR  ptr[5];
  ITEMPTR iptr;
  DOUBLE  *ptr1, *ptr2;
  /* ITEMPTR SearchEl(); */
  /* SHORT   GetExpr(); */
  BOOL    TakeMem(), SearchExpr();
  /* VOID    Cap(); */
  DOUBLE  atof();


  ptr[0] = line;

  m = num_rows+num_lines;
  n = num_var+num_slack+num_lines;

  mm = (LONG)m;
  nn = (LONG)n;

  if(!TakeMem()) return(_FALSE);

  printf("P2 NAME    %s\n",symbols[NAME]);
  if(file[1]) fprintf(file[1],"P2 NAME    %s\n",symbols[NAME]);

  /* A auf Schlupfvariablen vorbereiten */

  iptr = list[ROWS_LIST];
  for(count=0; iptr; iptr=iptr->next) {
    if((ch = *iptr->string) == 'L') SetM(A,n,iptr->nr,num_var+(++count),1.0);
    else if(ch == 'G')              SetM(A,n,iptr->nr,num_var+(++count),-1.0);
  }

  /* COLUMNS-Sektion einlesen */

  printf("P2 COLUMNS\n");
  if(file[1]) fprintf(file[1],"P2 COLUMNS\n");
  if(fseek(file[0],position[COLUMNS],0) == -1) {
    PrintError(errno,NULL);
    return(_FALSE);
  }

  do {
    if(!fgets(ptr[0],BUFFER-1,file[0])) {
      if(ferror(file[0])) {
        PrintError(errno,NULL);
        return(_FALSE);
      }
    }
    Cap(ptr[0],0,BUFFER);

    if(SearchExpr(ptr[0],&start[0],&stop[0])) {

      if(start[0] == 0) break; /* Abbruch von do{}while, Beginn neuer Sektion */
      length = GetExpr(buf[0],ptr[0],start[0],stop[0]);
      if(!(iptr = SearchEl(buf[0],list[VAR_LIST],_FALSE,NULL))) {
        PrintError(ERR_FATAL,NULL);
        return(_FALSE);
      }
      j = iptr->nr;  /* Spalte */

      if(SearchExpr((ptr[1] = ptr[0]+stop[0]+1),&start[1],&stop[1]) &&
         SearchExpr((ptr[2] = ptr[1]+stop[1]+1),&start[2],&stop[2])    ) {
        length = GetExpr(buf[1],ptr[1],start[1],stop[1]);
        length = GetExpr(buf[2],ptr[2],start[2],stop[2]);
        if(strcmp(symbols[GOAL],buf[1]) == 0)
          c[j-1] = minimize ? -atof(buf[2]) : atof(buf[2]);
        else {
          if(iptr = SearchEl(buf[1],list[ROWS_LIST],_TRUE,NULL))
            SetM(A,n,iptr->nr,j,atof(buf[2]));
        }

        if(SearchExpr((ptr[3] = ptr[2]+stop[2]+1),&start[3],&stop[3]) &&
          SearchExpr((ptr[4] = ptr[3]+stop[3]+1),&start[4],&stop[4])    ) {
          length = GetExpr(buf[1],ptr[3],start[3],stop[3]);
          length = GetExpr(buf[2],ptr[4],start[4],stop[4]);
          if(strcmp(symbols[GOAL],buf[1]) == 0)
            c[j-1] = minimize ? -atof(buf[2]) : atof(buf[2]);
          else {
            if(iptr = SearchEl(buf[1],list[ROWS_LIST],_TRUE,NULL))
              SetM(A,n,iptr->nr,j,atof(buf[2]));
          }
        }
      }
    }

  } while(!feof(file[0]));


  /* RHS-Sektion einlesen */

  if(symbflag & BIT_RHS && position[RHS]!=-1) {

    printf("P2 RHS     %s\n",symbols[RHS]);
    if(file[1]) fprintf(file[1],"P2 RHS     %s\n",symbols[RHS]);

    if(fseek(file[0],position[RHS],0) == -1) {
      PrintError(errno,NULL);
      return(_FALSE);
    }

    do {
      if(!fgets(ptr[0],BUFFER-1,file[0])) {
        if(ferror(file[0])) {
          PrintError(errno,NULL);
          return(_FALSE);
        }
      }
      Cap(ptr[0],0,BUFFER);

      if(SearchExpr(ptr[0],&start[0],&stop[0])) {

        if(start[0] == 0) break; /* Abbruch von do{}while, neue Sektion */
        length = GetExpr(buf[0],ptr[0],start[0],stop[0]);
        if(strcmp(symbols[RHS],buf[0]) == 0) {
          if(SearchExpr((ptr[1] = ptr[0]+stop[0]+1),&start[1],&stop[1]) &&
             SearchExpr((ptr[2] = ptr[1]+stop[1]+1),&start[2],&stop[2])    ) {
            length = GetExpr(buf[1],ptr[1],start[1],stop[1]);
            if(!(iptr = SearchEl(buf[1],list[ROWS_LIST],_TRUE,NULL))) {
              PrintError(ERR_FATAL,NULL);
              return(_FALSE);
            }
            i = iptr->nr;
            length = GetExpr(buf[2],ptr[2],start[2],stop[2]);
            b[i-1] = atof(buf[2]);

            if(SearchExpr((ptr[3] = ptr[2]+stop[2]+1),&start[3],&stop[3]) &&
               SearchExpr((ptr[4] = ptr[3]+stop[3]+1),&start[4],&stop[4])    ) {
              length = GetExpr(buf[1],ptr[3],start[3],stop[3]);
              if(!(iptr = SearchEl(buf[1],list[ROWS_LIST],_TRUE,NULL))) {
                PrintError(ERR_FATAL,NULL);
                return(_FALSE);
              }
              i = iptr->nr;
              length = GetExpr(buf[2],ptr[4],start[4],stop[4]);
              b[i-1] = atof(buf[2]);
            }
          }
        }
      }
    } while(!feof(file[0]));
  }


  /* RANGES-Sektion einlesen */

  if(symbflag & BIT_RANGES && num_lines!=0 && position[RANGES]!=-1) {

    printf("P2 RANGES  %s\n",symbols[RANGES]);
    if(file[1]) fprintf(file[1],"P2 RANGES  %s\n",symbols[RANGES]);

    if(fseek(file[0],position[RANGES],0) == -1) {
      PrintError(errno,NULL);
      return(_FALSE);
    }

    i = 0;  /* Zähler für zusätzliche Zeilen */
    do {
      if(!fgets(ptr[0],BUFFER-1,file[0])) {
        if(ferror(file[0])) {
          PrintError(errno,NULL);
          return(_FALSE);
        }
      }
      Cap(ptr[0],0,BUFFER);

      if(SearchExpr(ptr[0],&start[0],&stop[0])) {

        if(start[0] == 0) break; /* Abbruch von do{}while, neue Sektion */
        length = GetExpr(buf[0],ptr[0],start[0],stop[0]);

        if(strcmp(buf[0],symbols[RANGES]) == 0) {

          if(SearchExpr((ptr[1] = ptr[0]+stop[0]+1),&start[1],&stop[1]) &&
             SearchExpr((ptr[2] = ptr[1]+stop[1]+1),&start[2],&stop[2])   ) {
            length = GetExpr(buf[1],ptr[1],start[1],stop[1]);
            if(!(iptr = SearchEl(buf[1],list[ROWS_LIST],_TRUE,NULL))) {
              PrintError(ERR_FATAL,NULL);
              return(_FALSE);
            }
            if(++i > num_lines) {
              PrintError(ERR_FATAL,NULL);
              return(_FALSE);
            }
            ptr1 = A+(LONG)(iptr->nr-1)*nn;
            ptr2 = A+(LONG)(num_rows+i-1)*nn;
            CopyMemQuick(ptr1,ptr2,(LONG)((LONG)num_var*S_DOUBLE));
            length = GetExpr(buf[2],ptr[2],start[2],stop[2]);
            if(*iptr->string == 'L') {
              SetM(A,n,num_rows+i,num_var+num_slack+i,-1.0);
              b[num_rows+i-1] = b[iptr->nr-1]-atof(buf[2]);
            }
            else {
              SetM(A,n,num_rows+i,num_var+num_slack+i,1.0);
              b[num_rows+i-1] = b[iptr->nr-1]+atof(buf[2]);
            }

            if(SearchExpr((ptr[3] = ptr[2]+stop[2]+1),&start[3],&stop[3]) &&
              SearchExpr((ptr[4] = ptr[3]+stop[3]+1),&start[4],&stop[4])   ) {
              length = GetExpr(buf[1],ptr[3],start[3],stop[3]);
              if(!(iptr = SearchEl(buf[1],list[ROWS_LIST],_TRUE,NULL))) {
                PrintError(ERR_FATAL,NULL);
                return(_FALSE);
              }
              if(++i > num_lines) {
                PrintError(ERR_FATAL,NULL);
                return(_FALSE);
              }
              ptr1 = A+(LONG)(iptr->nr-1)*nn;
              ptr2 = A+(LONG)(num_rows+i-1)*nn;
              CopyMemQuick(ptr1,ptr2,(LONG)((LONG)num_var*S_DOUBLE));
              length = GetExpr(buf[2],ptr[4],start[4],stop[4]);
              if(*iptr->string == 'L') {
                SetM(A,n,num_rows+i,num_var+num_slack+i,-1.0);
                b[num_rows+i-1] = b[iptr->nr-1]-atof(buf[2]);
              }
              else {
                SetM(A,n,num_rows+i,num_var+num_slack+i,1.0);
                b[num_rows+i-1] = b[iptr->nr-1]+atof(buf[2]);
              }
            }
          }
        }
      }
    } while(!feof(file[0]));
  }


  /* BOUNDS-Sektion einlesen */

  if(symbflag & BIT_BOUNDS && position[BOUNDS]!=-1) {

    printf("P2 BOUNDS  %s\n",symbols[BOUNDS]);
    if(file[1]) fprintf(file[1],"P2 BOUNDS  %s\n",symbols[BOUNDS]);

    if(fseek(file[0],position[BOUNDS],0) == -1) {
      PrintError(errno,NULL);
      return(_FALSE);
    }

    do {
      if(!fgets(ptr[0],BUFFER-1,file[0])) {
        if(ferror(file[0])) {
          PrintError(errno,NULL);
          return(_FALSE);
        }
      }
      Cap(ptr[0],0,BUFFER);

      if(SearchExpr(ptr[0],&start[0],&stop[0])) {

        if(start[0] == 0) break; /* Abbruch von do{}while, neue Sektion */
        length = GetExpr(buf[0],ptr[0],start[0],stop[0]);

        if(SearchExpr((ptr[1] = ptr[0]+stop[0]+1),&start[1],&stop[1]) &&
           SearchExpr((ptr[2] = ptr[1]+stop[1]+1),&start[2],&stop[2]) &&
           SearchExpr((ptr[3] = ptr[2]+stop[2]+1),&start[3],&stop[3])   ) {
          length = GetExpr(buf[1],ptr[1],start[1],stop[1]);
          if(strcmp(buf[1],symbols[BOUNDS]) == 0) {
            length = GetExpr(buf[2],ptr[2],start[2],stop[2]);
            if(!(iptr = SearchEl(buf[2],list[VAR_LIST],_FALSE,NULL))) {
              PrintError(ERR_FATAL,NULL);
              return(_FALSE);
            }
            length = GetExpr(buf[3],ptr[3],start[3],stop[3]);
            if(buf[0][0] == 'U') upper[iptr->nr-1] = atof(buf[3]);
            else                 lower[iptr->nr-1] = atof(buf[3]);
          }
        }
      }
    } while(!feof(file[0]));
  }

  return(_TRUE);
}



/*****************************************************************************
 * BOOL CorrectBounds() -> _TRUE/_FALSE                                      *
 * Anpassung des Gleichungssystems an lower == 0, Berechnung von c0start     *
 * und rechte Seite b >= 0 machen.                                           *
 * _TRUE  : O.K.                                                             *
 * _FALSE : ERR_UP_LO                                                        *
 *****************************************************************************/

BOOL CorrectBounds()

{
  REGISTER DOUBLE *ptr1, dummy;
  REGISTER SHORT  j, i;
  BOOL            found = _FALSE;
  VOID            PrintError();

  for(i=0; i<num_var; ++i) {
    if(upper[i] != INFINITE && lower[i] > upper[i]) {
      PrintError(ERR_UP_LO,NULL);
      return(_FALSE);
    }
    if(lower[i] != 0.0) found = _TRUE;
    if(upper[i] != INFINITE)  upper[i] -= lower[i];
  }

  if(found) {
    for(i=0; i<m; ++i) {
      ptr1 = A+(LONG)i*nn;
      for(dummy=0.0,j=0; j<num_var; ++j) dummy += (*ptr1++)*lower[j];
      b[i] -= dummy;
    }
    for(c0start=0.0,j=0; j<num_var; ++j) c0start += c[j]*lower[j];
  }
  else c0start = 0.0;

  for(i=0; i<m; ++i) {
    if(b[i] < 0.0) {
      ptr1 = A+(LONG)i*nn;
      for(j=0; j<n; ++j) *ptr1++ *= -1.0;
      b[i] *= -1.0;
    }
  }


  return(_TRUE);
}



/*****************************************************************************
 * BOOL SearchExpr(str,start,stop) -> _TRUE/_FALSE                           *
 * Sucht den ersten Ausdruck im String str und bestimmt die Position des     *
 * ersten (start) und letzten (stop) Zeichens in str.                        *
 * _TRUE  : O.K.                                                             *
 * _FALSE : kein Ausdruck gefunden                                           *
 *****************************************************************************/

BOOL    SearchExpr(str,start,stop)

STRPTR  str;
SHORT   *start, *stop;

{
  TEXT            ch;
  REGISTER SHORT  pos1 = 0, pos2;

  /* Trennzeichen überspringen */
  while((ch = str[pos1]) != '\0' && isspace(ch)) ++pos1;

  if(ch == '\0') return(_FALSE); /* Stringende erreicht, kein Ausdruck vorh. */

  pos2 = pos1+1;
  while((ch = str[pos2]) != '\0' && !isspace(ch)) ++pos2;

  *start = pos1;
  *stop = pos2-1;
  return(_TRUE);
}



/*****************************************************************************
 * SHORT GetExpr(dest,ptr,start,stop)                                        *
 * Kopiert den Ausdruck, der bei ptr+start beginnt und bei ptr+stop endet,   *
 * nach dest. Danach werden alle Kleinbuchstaben zu Großbuchstaben. Das Er-  *
 * gebnis ist die Länge des Ausdrucks                                        *
 *****************************************************************************/

SHORT   GetExpr(dest,ptr,start,stop)

STRPTR  dest, ptr;
SHORT   start, stop;

{
  SHORT length, reallength;

  length = stop-start+1;
  reallength = length>BUFFER2-1 ? BUFFER2-1 : length; /* verhindert Überlauf */
  strncpy(dest,ptr+start,reallength);
  dest[reallength] = '\0';
  return(length);
}



/*****************************************************************************
 * VOID Cap(str,start,stop)                                                  *
 * String str in Großbuchstaben umwandeln (von start bis einschl. stop)      *
 * Cap() bricht ab, falls stop oder das Stringende erreicht sind             *
 *****************************************************************************/

VOID    Cap(str,start,stop)

STRPTR  str;
SHORT   start, stop;

{
  REGISTER SHORT pos;
  TEXT           ch;

  for(pos = start; pos<=stop && (ch=str[pos])!='\0'; ++pos) {
    if(isalpha(ch) && islower(ch)) str[pos] = toupper(ch);
  }
}



/*****************************************************************************
 * ITEMPTR NewListEl(list,str)                                               *
 * Fügt an die Liste list ein neues Element an (an den Anfang) und übergibt  *
 * den neuen Listenanker. Falls dieser NULL ist, ERR_MEM. Außerdem wird der  *
 * String str in das neue Listenelement kopiert                              *
 *****************************************************************************/

ITEMPTR NewListEl(list,str)

ITEMPTR list;
STRPTR  str;

{
  VOID    PrintError();
  ITEMPTR ptr;

  if(ptr = AllocMem(SIZE_ITEM,MEMF_CLEAR)) {
    ptr->next = list;
    strcpy(ptr->string,str);
  }
  else PrintError(ERR_MEM,NULL);

  return(ptr);
}



/*****************************************************************************
 * ITEMPTR SearchEl(str,list,spezial,chstr)                                  *
 * Sucht den String str in der Liste list. Ist spezial==_TRUE, so wird erst  *
 * ab dem zweiten Zeichen verglichen (für spezielles Format von ROWS_LIST).  *
 * Wird bufstr != NULL angegeben, so wird das erste Zeichen des gefundenen   *
 * Strings dort hineingeschrieben (Format für ROWS_LIST).                    *
 * Ergebnis ist entweder der Zeiger auf den gefundenen Eintrag, oder NULL,   *
 * falls str nicht in der Liste enthalten ist                                *
 *****************************************************************************/

ITEMPTR SearchEl(str,list,spezial,chstr)

STRPTR  str;
ITEMPTR list;
BOOL    spezial;
STRPTR  chstr;    /* Zeiger auf Character */

{
  ITEMPTR ptr = list;
  STRPTR  sptr;

  while(ptr) {
    sptr = spezial ? ptr->string+1 : ptr->string;
    if(strcmp(str,sptr)==0) {
      if(chstr) *chstr = *ptr->string;
      return(ptr);
    }
    ptr = ptr->next;
  }

  return(NULL);
}



/*****************************************************************************
 * VOID DeleteList(list)                                                     *
 * Löscht die Liste list. Vorsicht: list ist Zeiger auf Zeiger !!            *
 *****************************************************************************/

VOID    DeleteList(list)

ITEMPTR *list;

{
  if(*list) {
    DeleteList(&(*list)->next);
    FreeMem(*list,SIZE_ITEM);
    *list = NULL;
  }
}



/*****************************************************************************
 * VOID GetRidOfLists()                                                      *
 * Löscht alle Bezeichnerlisten                                              *
 *****************************************************************************/

VOID GetRidOfLists()

{
  SHORT i;
  VOID  DeleteList();

  for(i=0; i<NUM_LISTS; ++i) DeleteList(&list[i]);
}



/*****************************************************************************
 * VOID PrintError(nr,str)                                                   *
 * Gibt die Fehlermeldung zur Fehlernummer nr aus, optional davor einen      *
 * String str.
 * 0 <= nr <= sys_nerr : vorgegebene Systemfehlermeldungen                   *
 * nr > sys_nerr       : eigene Fehlermeldungen                              *
 *****************************************************************************/

VOID    PrintError(nr,str)
INT     nr;
STRPTR  str;

{
  printf(":: Error %2d : ",nr);
  if(str) printf("(%s) ",str);
/* AROS
  if(nr<=sys_nerr) printf("%s\n",sys_errlist[nr]);
  else             printf("%s\n",errors[nr-sys_nerr-1]);
*/
  if(file[1]) {
    fprintf(file[1],":: Error %2d : ",nr);
    if(str) fprintf(file[1],"(%s) ",str);
/* AROS
    if(nr<=sys_nerr) fprintf(file[1],"%s\n",sys_errlist[nr]);
    else             fprintf(file[1],"%s\n",errors[nr-sys_nerr-1]);
*/
  }
}



/*****************************************************************************
 * INT GetInput(str)                                                         *
 * Liest eine Zeile von der Console (=stdin) in den String "str" ein         *
 * Ergebnis: Fehlernummer bzw. 0, falls kein Fehler auftrat                  *
 *****************************************************************************/

INT     GetInput(str)
STRPTR  str;
{
  if(!gets(str)) {
    if(feof(stdin)) return(ERR_EOF); /* nur, wenn stdin != Console ist */
    else if(ferror(stdin)) return(errno);
  }

  return(0);
}



/*****************************************************************************
 * BOOL TakeMem() -> _TRUE/_FALSE                                            *
 * TakeMem() alloziert den Speicher für die Felder.                          *
 * _TRUE  : O.K.                                                             *
 * _FALSE : ERR_MEM                                                          *
 *****************************************************************************/

BOOL TakeMem()

{
  LONG  mem_needed, mem_avail, i;
  VOID  GiveMemBack(), PrintError();

  mem_needed = S_SHORT*(mm+2L*nn)+S_DOUBLE*(mm*(mm+nn+8L)+6L*nn);

  if(mem_needed > (mem_avail = AvailMem(NULL))) {
    sprintf(line,"%ld needed,%ld available",mem_needed,mem_avail);
    PrintError(ERR_MEM,line);
    return(_FALSE);
  }

  A       = AllocMem(mm*nn*S_DOUBLE,MEMF_CLEAR);
  AB1     = AllocMem(mm*mm*S_DOUBLE,MEMF_CLEAR);
  b       = AllocMem(mm*S_DOUBLE,MEMF_CLEAR);
  b2q     = AllocMem(mm*S_DOUBLE,MEMF_CLEAR);
  c       = AllocMem(nn*S_DOUBLE,MEMF_CLEAR);
  c2      = AllocMem((nn+mm)*S_DOUBLE,MEMF_CLEAR);
  upper   = AllocMem((nn+mm)*S_DOUBLE,NULL);
  lower   = AllocMem(nn*S_DOUBLE,MEMF_CLEAR);
  x       = AllocMem((nn+mm)*S_DOUBLE,MEMF_CLEAR);
  cq      = AllocMem(nn*S_DOUBLE,MEMF_CLEAR);
  pi      = AllocMem(mm*S_DOUBLE,MEMF_CLEAR);
  dq      = AllocMem(mm*S_DOUBLE,MEMF_CLEAR);
  help    = AllocMem(mm*S_DOUBLE,MEMF_CLEAR);

  B       = AllocMem(mm*S_SHORT,MEMF_CLEAR);
  Nq      = AllocMem(nn*S_SHORT,MEMF_CLEAR);
  Nminus  = AllocMem(nn*S_SHORT,MEMF_CLEAR);

  if(!(A && AB1 && b && b2q && c && c2 && upper && lower && x && cq && pi &&
       dq && help && B && Nq && Nminus)) {
    GiveMemBack();
    PrintError(ERR_MEM,NULL);
    return(_FALSE);
  }

  for(i=0; i<(nn+mm); ++i) upper[i] = INFINITE;

  return(_TRUE);
}



/*****************************************************************************
 * VOID GiveMemBack()                                                        *
 * Allozierten Speicher an das System zurückgeben.                           *
 *****************************************************************************/

VOID GiveMemBack()

{
  if(A)       FreeMem(A,mm*nn*S_DOUBLE);
  if(AB1)     FreeMem(AB1,mm*mm*S_DOUBLE);
  if(b)       FreeMem(b,mm*S_DOUBLE);
  if(b2q)     FreeMem(b2q,mm*S_DOUBLE);
  if(c)       FreeMem(c,nn*S_DOUBLE);
  if(c2)      FreeMem(c2,(nn+mm)*S_DOUBLE);
  if(upper)   FreeMem(upper,(nn+mm)*S_DOUBLE);
  if(lower)   FreeMem(lower,nn*S_DOUBLE);
  if(x)       FreeMem(x,(nn+mm)*S_DOUBLE);
  if(cq)      FreeMem(cq,nn*S_DOUBLE);
  if(pi)      FreeMem(pi,mm*S_DOUBLE);
  if(dq)      FreeMem(dq,mm*S_DOUBLE);
  if(help)    FreeMem(help,mm*S_DOUBLE);

  if(B)       FreeMem(B,mm*S_SHORT);
  if(Nq)      FreeMem(Nq,nn*S_SHORT);
  if(Nminus)  FreeMem(Nminus,nn*S_SHORT);

  A = AB1 = b = b2q = c = c2 = upper = lower = x = cq = pi = dq = help = NULL;

  B = Nq = Nminus = NULL;
}

#include <dos/dos.h>
#include <exec/types.h>
#ifndef Prototype
#define Prototype extern
#endif

/*********************************************************************/
/*      Die Funktion OpenRexx öffnet, wenn möglich einen RexxPort.   */
/*********************************************************************/

/*
 * OpenRexx     initialisiert die ARexxumgebung. Dafür wird, wenn benötigt
 *      Arexxlibrary geöffnet, ein Port initialisiert, die Defaultextension
 *      festgelegt und die std Ein- und Ausgabekanäle bestimmt.
 *
 * --> name     Name des ARexxPorts den unser Programm bekommt.
 *              Existiert ein solcher Port bereits wird der FehlerCode
 *              ERROR_PORT_ALLREADY_EXISTS zurückgegeben
 * --> ext      Zeiger auf die extension, wenn NULL dan .REXX
 * --> do_res   Zeiger auf die Funktion zum Message handling.
 *              Wird aufgerufen wenn während der Arbeit ein RexxMsg anfällt
 * --> in,out   std input/output handle
 * <-- RESLUT   SigBit   Success FALSE wenn nicht geöffnet
 */


/******************************************************************/
/*      beantwortet alle anstehenden Messages, mit RXRERRORIMGONE  */
/*      wartet auf alle noch zu beantwortenden Messages           */
/*      schließt den Port                                         */
/*      schließt die ArexxSysBase                                 */
/******************************************************************/


/********************************************************************/
/*      Mit dieser Funktion kann man den letzten aufegetreten       */
/*      FehlerCode festellen                                        */
/********************************************************************/


/****************************************************************/
/*      BuildRexxMsg erstellt eine ARexx Message und füllt      */
/*      die Argument slots mit den Parametern                   */
/****************************************************************/
/*
 *      Aufruf erfolgt: BuildRexxMsg(com,flags,arg0,arg1,arg2,...,argn)
 *      Es dürfen maximal 15 Argumente übergeben werden. Weitere Argumente
 *      werden ignoriert. Will man nur 3 Argumente übergeben (arg1-arg3)
 *      übergibt man als letztes Argument (arg4) NULL
 *      Die Funktion gibt einen Zeiger auf die erstellte Message zurück,
 *      im Fehlerfalle NULL.
 */
   /*
    *      com ist der Name des aufzurufenden Kommandos.
    *      Alles Strings (com & args) sind normale mit NULL abgeschloßene
    *      C-Strings.
    */
   /*
    *      action dieses Flag wir direkt im Command (action) Field
    *      der RexxMessage struktur eingefügt
    */
   /*
    *      first_arg is ein Zeiger auf das erste Argument (aufruf der
    *      Funktion s.o.
    *      Aufgrund der internen Handhabung wird nur der erste Parameter im
    *      Funktionskopf angenommen.
    *      Die Argumente sind normale 0-terminierte C-Strings
    */
   /*
    *      Weitere Argumente:
    */


/*****************************************************************/
/*      Dies Funktion gibt den Speicher einer RexxMessage frei.  */
/*      Also die Argumente, einen ggf vorhandenen ReturnWert     */
/*      und die Message selbst. Aus diesem Grund müßen Felder    */
/*      (Argumente und Return Code, wenn sie außerhalb der       */
/*      Funktion freigegeben worden sind auf NULL gesetzt werden */
/*****************************************************************/
   /* Zeiger auf die zu löschende RexxMessage */
   /*
    *      gibt TRUE für Erfolg zurück
    */


/****************************************************************/
/*      PutRexxMsg schickt eine Message an den genannten Port   */
/*      un übernimmt die Buchführung über fehlende Antworten    */
/****************************************************************/
   /*
    *      Port ist der Name des Ziel Ports. Der Name ist NULL beendeter
    *      String. Achtung !! kein PortPointer.
    *      Ist port NULL so wird die Message an den RexxHostPort geschickt.
    *      Da man einen eigenen Namen angeben kann, ist möglich, Messages
    *      direkt an andere Rexxfähige Programme zu senden.
    */
   /*
    *      rmsg    ist ein Zeiger auf gültige, vollstänig initialisierte
    *      RexxMessage-Struktur.
    */


/****************************************************************/
/*      GetRexxMsg funktioniert GetMsg des Betriebsystems,      */
/*      nur das Über die Messages Buchgeführt wird              */
/****************************************************************/
/*
 *      Die Funktion gibt einen Zeiger auf die Message oder NULL
 *      zurück.
 *      Ist NULL ein Fehler, so kann das mit LastRexxError festgestellt
 *      werden. Sonst bedeutet es einfach, das keine Msg am Port
 *      wartet.
 */


/****************************************************************/
/*      FetchRexxMsg wartet bis eine Message an unserm          */
/*      Port ankommt und gibt diese zurück.                     */
/****************************************************************/
/*
 *      Die Funktion kehrt mit einem Zeiger auf die vom
 *      Port geholte Message zurück.
 */


/****************************************************************/
/*      ReplyRexxMsg    setzt den ArexxErrorCode, füllt den     */
/*      Returnwert_slot und beantwortet die Message.            */
/*      sollte es eine NonReply Message sein, so wird sie       */
/*      einfach gelöscht                                        */
/****************************************************************/
/*
 *      Im falle eines Fehlers kehrt die Funktion mit FALSE zurück.
 *      Nähere Information kann man LastRexxError holen
 */
   /*
    *      rmsg    ist ein Zeiger auf die zu beantwortende Message
    */
   /*
    *      rexx_error ist ein Langwort, das den ArexxErrorCode enthält.
    *      Der Code wird in das Result1 Fe ld der Message eingetragen.
    */
   /*
    *      return_string ist ein String, der als Return wert and den
    *      Caller zurückgegeben wird. ReplyRexxMsg generiert aus diesem
    *      C-String ein ArexxArgument un füllt dies in das Result2
    *      Feld ein.
    *      Ist rexx_error != 0, also ein Fehler aufgetreten, so wird der
    *      String ignoriert und das Feld auf NULL gesetz. Soll kein
    *      Returnwert zurückgegeben werden ist ein NULL-Pointer zu übergeben.
    */


/**********************************************************************/
/*      SyncRexxCommand sendet ein Kommando, und wartet bis das       */
/*      Kommando abgearbeitet ist. Sollten in der Zwischenzeit        */
/*      Kommandos an unser Port gehen, wird die Funktion              */
/*      process_message mit dem Zeiger auf die Message aufgerufen.    */
/*      So ist es möglich, währen eines SyncAufrufes Kommandos zu     */
/*      Empfangen. SyncRexxCommand unterstützt einen returncode.      */
/*      Dazu muß eine StringBuffer und die Größe des Buffer über-     */
/*      geben werden .                                                */
/*      Der aufruf erfolgt:                                           */
/*      SyncRexxCommand(port,com,result,result_len,arg1,arg2,...,argn)*/
/*      Werden nicht alle Argumente genutzt, muß das letzte Argument  */
/*      NULL sein. Mehr als 15 Argumente werden ignoriert.            */
/**********************************************************************/
/*
 *      Die Funktion kehrt mit dem ArexxErrorCode oder dem Internen
 *      errorcode zurück.
 *      0 für eine erfolgreiche aktion.
 *      Kommt es zu einem RexxFehler, so wird der ResultString gelöscht.
 */
   /*
    *      port ist der Name des Ports an den das Kommando gesendet werden soll.
    *      wird als PortName 0 angegeben, so wird das Kommando an den Rexx-
    *      Host geschickt.
    */
   /*
    *      Com ist der String der den KommandoNamen enthält
    */
   /*
    *      result ist ein Pointer auf den StringBuffer in den der
    *      ResultString des Kommandos kopiert werden soll. Ist result == NULL
    *      oder result_len == NULL so wird die Fukntion als Kommando
    *      ohne ReturnWert betrachtet, und die Flags in der RexxMsg
    *      entsprechend gesetzt.
    */
   /*
    *      first_arg Zeiger auf den ersten Argument String. Der aufruf Syntax
    *      ist oben beschrieben, und Funktioniert wie bei BuildRexxMessage.
    */
   /*
    *   weitere args
    */


/****************************************************************/
/*      RexxMessageHandler sucht die FunktionsListe durch, und  */
/*      ruft, falls es das Kommando gibt, die dazugehörige      */
/*      Funktion auf. Sollte das Kommando nicht existieren      */
/*      wird der ARexxFehlerCode Label NotFound zurückgegeben.  */
/****************************************************************/
/*
 * RexxMessageHandler ruft bei ankommenden Messages die in der
 * Funktionliste übergebenen Kommandos auf.
 *
 * --> rmsg     Zeiger auf die RexxMessage, die bearbeitet werden soll
 */


/****************************************************************/
/*      ChangeProcessMessage    ändert den Zeiger der process_  */
/*      message funktion.                                       */
/****************************************************************/
/*
 * ChangeProcessMessage ändert die Funktion für das InterneMessagehandling
 *
 * --> new_function     Zeiger auf die neue Funktion, bei -1 wird
 *              der HRexx interne Handler verwendet
 * <-- RESULT   Pointer auf alten hndler
 */

/****************************************************************/
/*      SetRexxCommandTable  ändert die interne Kommandotabelle */
/****************************************************************/
   /*
    *      Zeiger auf eine vom Benutzer definierte Kommando Tabelle
    */
   /*
    *      gibt den Zeiger auf die alte Funktiontabelle zurück.
    */


/* Internal ErrorCodes */
#define  RERROR_AREXX_ALLREADY_OPEN      -1
#define  RERROR_OUT_OF_MEMORY            -2
#define  RERROR_NO_AREXX                 -3
#define  RERROR_PORT_ALLREADY_EXISTS     -4
#define  RERROR_INVALID_PORT_NAME        -5
#define  RERROR_NO_PORT                  -6
#define  RERROR_AREXX_NOT_OPEN           -7
#define  RERROR_INVALID_COMMAND_NAME     -8
#define  RERROR_UNKNOWN_DESTINATION_PORT -9
#define  RERROR_NO_MSG                   -10
#define  RERROR_STRING_TOO_LONG          -11
#define  RERROR_NO_COMMAND_TABLE         -12

struct  RexxCommand
{
   char *Name;
   long  (*Function)(char * _s,struct RexxMsg *rmsg);
};

/*************************************************************/
/*  ModifyRepCount  ändert Anzahl fehlender Messages         */
/*************************************************************/
/*
 ModifyRepCount
        ändert den Counter mit den fehlenden Messages um den in
        delta angegebenen Betrag.
        Der alte wert von RepCount wird zurückgegeben
*/

/*
 AttempCloseRexx
        probiert den ARexxPort zu schliessen (funktioniert nur, wenn
        OutstandingReplies==0 ist), kann das RexxSystem geschlossen
        werden kommt TRUE zurück, sonst False
*/



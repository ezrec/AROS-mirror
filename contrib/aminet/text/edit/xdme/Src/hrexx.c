/*
 *	$Id$
 *
 *	einfaches ARexx-Interface für Amiga
 *	Achtung der Code ist NICHT reentrant
 *
 */

/*
 *	Danke an die Authoren von MinRexx.
 *	Das Listing von MinRexx war beim erstellen dieses ARexxInterfaces
 *	sehr hilfreich. (Der Ausdruck lag immer griffbereit neben mir.)
 *	MinRexx befindet sich (gezoot) auf der ARexx Disk, und normal
 *	auf der FishDisk 188.
 */

#define  COMPILING_HREXX
#define  HREXX

#define  RXSDIR "REXX"
#define  ERROR_CODE	long
#define  SUCCES 	long

#ifndef _DCC
#define __stkargs
#endif

#include "hrexx.h"
#include <string.h>
#include <ctype.h>
#include <exec/ports.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/dos.h>
#ifndef __AROS__
#warning Need ARexx !
#include <proto/rexxsyslib.h>

#include <rexx/rexxio.h>
#endif
#include <rexx/errors.h>
#include <rexx/storage.h>
#include "rexx.h"

#include <stdio.h>

Prototype ULONG 	       OpenRexx 	    (char *, const char *, __stkargs void (*do_res)(struct RexxMsg *), BPTR, BPTR);
Prototype BOOL		       CloseRexx	    (void);
Prototype long		       LastRexxError	    (void);
Prototype struct RexxMsg     * BuildRexxMsg	    (const char *, const long, char *, ...);
Prototype BOOL		       RemoveRexxMsg	    (struct RexxMsg *);
Prototype BOOL		       PutRexxMsg	    (char *, struct RexxMsg *);
Prototype struct RexxMsg     * GetRexxMsg	    (void);
Prototype struct RexxMsg     * FetchRexxMsg	    (void);
Prototype BOOL		       ReplyRexxMsg	    (struct RexxMsg *, long, char *);
Prototype long		       SyncRexxCommand	    (const char *, const char *, char *, long, char *, ...);
Prototype __stkargs void       RexxMessageHandler   (struct RexxMsg *);
Prototype void		     * ChangeProcessMessage (__stkargs void (*new_func)(struct RexxMsg *));
Prototype struct RexxCommand * SetRexxCommandTable  (struct RexxCommand *);
Prototype long		       ModifyRepCount	    (long);
Prototype BOOL		       AttempCloseRexx	    (void);

extern struct DosLibrary * DOSBase;

/*
 *	Diesen ErrorCode "I'm Gone" habe ich direkt von MinRexx übernommen.
 *	Er ist genausogut wie jeder andere, und auf diese Art wird das
 *	Ganze etwas einheitlicher.
 */
#define  RERROR_IMGONE	(100)

/*
 *	Library Pointer. Enthält den Zeiger auf die LibraryBase.
 *	Ist er NULL, so sind alle lokalen globalen Variablen diese Moduls
 *	ungültig. Der Zeiger wird mit OpenRexx geholt, und sollte nie
 *	vom Klienten geändert werden. Beim Ordnungsgemäßen verlassen
 *	(mit CloseRexx) wird er wieder auf NULL gesetzt.
 *	So ist das Module zwar nicht PURE, kann aber, wenn es nur einmal
 *	zur selben Zeit läuft Resident gemacht werden.
 *	Eine komplett lokale Version ist geplant.
 */
struct	RxsLib *RexxSysBase=0;

/*
 *	Dies ist ein Zeiger auf den eigenen RexxPort, er wird von
 *	der OpenRexx Routine gesetzt.
 */
static	struct	MsgPort *rexx_port;

/*
 *	Dieser Zähler enthält die Menge der abgesandten Messages, die
 *	noch beantwortet werden müßen.
 */

static	long	outstanding_replies;

/*
 *	Dieser Pointer enthält einen Zeiger auf die Default Extension
 *	die bei einem Rexx aufruf übergeben werden soll.
 */
static	const  char    *default_extension;	/*+++*/

/*
 *	error	enthält den letzen FehlerCode
 *	negative Werte sind HRexxinterne FehlerCodes.
 */
static	long	error;

/*
 *	process_message  ist die Funktion die von myrexx zur internen
 *	Verarbeitung von ankommenden Messages verwendet wird während es mit
 *	SyncRexxCommand auf eine AntwortMessage wartet.
 *	Sollte dies Funktion eingesetzt werden, so muß sie die Messages
 *	die sie übergeben bekommt selbst beantworten.
 */
static	__stkargs void	(*process_message)(struct RexxMsg * rmsg);

/*
 *	Zeiger auf die Standard Handle (in && Out) die bei einem
 *	ArexxAufruf übergeben werden sollen.
 *	z.B. Say und Arg greifen auf diese StandardAmigaFileHandles
 *	zurück.
 */
static	BPTR	std_input,std_output;

/*
 *	Command List ist ein Zeiger auf Feld von RexxCommandStrukturen.
 *	In dieser Struktur steht der FunktionsName und ein Zeiger auf die
 *	Funktion. Kommt einen Message zum internen RexxMessageHandler,
 *	so wird die Liste nach einer Funktion des gegebenen Namens und
 *	springt die Funktion an.
 *	Der letzte Eintrag der Tabelle muß als Namen einen NullZeiger
 *	beinhalten.
 *	Die Funktion wird mit folgenden Parametern auf gerufen:
 *	ret=func(arg0,rexx_mess);
 *	arg0 ist ein Zeiger auf den StringRest der nach dem Kommando folgt.
 *	rexx_mess ist ein Zeiger auf die RexxStruktur.
 *	Gibt die Funktion TRUE als RET zurück, so übernimmt der Handler
 *	das beantworten der Message.
 */
static	struct RexxCommand	*command_table;




/*********************************************************************/
/*	Die Funktion OpenRexx öffnet, wenn möglich einen RexxPort.   */
/*********************************************************************/

ULONG OpenRexx(char * name,const char *ext, __stkargs void (*do_res)(struct RexxMsg *rmsg),BPTR in, BPTR out)
/*
 * OpenRexx	initialisiert die ARexxumgebung. Dafür wird, wenn benötigt
 *	Arexxlibrary geöffnet, ein Port initialisiert, die Defaultextension
 *	festgelegt und die std Ein- und Ausgabekanäle bestimmt.
 *
 * --> name	Name des ARexxPorts den unser Programm bekommt.
 *		Existiert ein solcher Port bereits wird der FehlerCode
 *		ERROR_PORT_ALLREADY_EXISTS zurückgegeben
 * --> ext	Zeiger auf die extension, wenn NULL dan .REXX
 * --> do_res	Zeiger auf die Funktion zum Message handling.
 *		Wird aufgerufen wenn während der Arbeit ein RexxMsg anfällt
 * --> in,out	std input/output handle
 * <-- RESLUT	SigBit	 Success FALSE wenn nicht geöffnet
 */
{
   /*	Portname vorhanden ?	*/
   if (name==0 || strlen(name)==0)
   {
      error=RERROR_INVALID_PORT_NAME;
      return(0);
   }

   /*	Prüfen ob ARexx schon geöffnet ist, wenn ja, */
   /*	dann keinen Port öffnen */
   if (RexxSysBase)
   {
      error=RERROR_AREXX_ALLREADY_OPEN;
      return(0);
   }

   /*	ARexxLib öffnen */
   if ((RexxSysBase=(struct RxsLib *)OpenLibrary("rexxsyslib.library",0))==0)
   {
      error=RERROR_NO_AREXX;
      return(0);
   }

   /*	Prüft ob ein Port mit dem selben Namen existiert,  */
   /*	wenn ja bricht das Programm mit einem Fehler ab.   */
   Forbid();
   if (FindPort(name))
   {
      Permit();
      CloseLibrary((struct Library *)RexxSysBase);
      RexxSysBase=0;
      error=RERROR_PORT_ALLREADY_EXISTS;
      return(0);
   }

   /*  Port kreieren, wenn möglich  */
   if ((rexx_port=CreatePort(name,0))==0)
   {
      /* konnte keinen Port aufmachen */
      Permit();
      error=RERROR_NO_PORT;
      CloseLibrary((struct Library *)RexxSysBase);
      RexxSysBase=0;
      return(0);
   }
   Permit();

   /*	E/A-Kanäle festlegen	*/
   if (in<0)    std_input=Input();
   else 	std_input=in;
   if (out<0)   std_output=Output();
   else 	std_output=out;

   /*	extension zuweisen	*/
   default_extension=ext;

   /*	ResultFunktions Pointer setzen	*/
   process_message=do_res;

   /*	keine Messages */
   outstanding_replies=0;

   /*	FunktionTable löschen */
   command_table=0;

   /* Zeiger auf den MessagePort zurückgeben   */
   return ((ULONG)1L<<rexx_port->mp_SigBit);
}

BOOL AttempCloseRexx(void)
/*
 AttempCloseRexx
	probiert den ARexxPort zu schliessen (funktioniert nur, wenn
	OutstandingReplies==0 ist), kann das RexxSystem geschlossen
	werden kommt TRUE zurück, sonst False
*/
{
   if (outstanding_replies>0)
      return(FALSE);
   return(CloseRexx());
}

/******************************************************************/
/*	beantwortet alle anstehenden Messages, mit RXRERRORIMGONE  */
/*	wartet auf alle noch zu beantwortenden Messages 	  */
/*	schließt den Port					  */
/*	schließt die ArexxSysBase				  */
/******************************************************************/

BOOL CloseRexx(void)
{
   struct	RexxMsg *rmsg;

   /*	prüft ob es übehaupt etwas zu schließen gibt	*/
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(FALSE);
   }

   /*
    *	Achtung diese Funktion prüft nur, ob alle abgeschickten
    *	Messages eine Antwort bekommen haben. Nicht aber ob alle
    *	angekommenen Messages beantwortet wurden.
    *	Antworten wandern ins NIL
    */
   while (outstanding_replies)
   {
      /* Message vom Port holen */
      while ((rmsg = (struct RexxMsg *)GetMsg (rexx_port)) == 0)
	 WaitPort ((struct MsgPort *)rexx_port);

      /* gesuchte antwort ? */
      if (rmsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
      {
	 /* erwartete antwort, message freigeben */
	 RemoveRexxMsg (rmsg);

	 outstanding_replies --;
      } else
      {
	 /* alle jetzt ankommenden Kommandos dankent ablehnen
	  * sollte es eine no-return message sein, den Speicher
	  * freigeben und weitermachen, sonst mit RXRERRORIMGONE
	  * beantworten
	  */
	 if (rmsg->rm_Action & RXFF_NONRET)
	 {
	    RemoveRexxMsg (rmsg);
	 } else
	 {
	    rmsg->rm_Result2 = 0;
	    rmsg->rm_Result1 = RERROR_IMGONE;

	    ReplyMsg((struct Message *) rmsg);
	 }
      }
   } /* while outstanding_replies */

   /* Port vom System löschen	*/
   DeletePort (rexx_port);

   /* ArexxLib schließen	*/
   CloseLibrary ((struct Library *)RexxSysBase);
   RexxSysBase = 0;

   return (TRUE);
}


/********************************************************************/
/*	Mit dieser Funktion kann man den letzten aufegetreten	    */
/*	FehlerCode festellen					    */
/********************************************************************/
long LastRexxError(void)
{
   return(error);
}


/****************************************************************/
/*	BuildRexxMsg erstellt eine ARexx Message und füllt	*/
/*	die Argument slots mit den Parametern			*/
/****************************************************************/
/*
 *	Aufruf erfolgt: BuildRexxMsg(com,flags,arg0,arg1,arg2,...,argn)
 *	Es dürfen maximal 15 Argumente übergeben werden. Weitere Argumente
 *	werden ignoriert. Will man nur 3 Argumente übergeben (arg1-arg3)
 *	übergibt man als letztes Argument (arg4) NULL
 *	Die Funktion gibt einen Zeiger auf die erstellte Message zurück,
 *	im Fehlerfalle NULL.
 */
struct RexxMsg *BuildRexxMsg
(
   /*
    *	   com ist der Name des aufzurufenden Kommandos.
    *	   Alles Strings (com & args) sind normale mit NULL abgeschloßene
    *	   C-Strings.
    */
   const char	 *com,
   /*
    *	   action dieses Flag wir direkt im Command (action) Field
    *	   der RexxMessage struktur eingefügt
    */
   const long	 action,
   /*
    *	   first_arg is ein Zeiger auf das erste Argument (aufruf der
    *	   Funktion s.o.
    *	   Aufgrund der internen Handhabung wird nur der erste Parameter im
    *	   Funktionskopf angenommen.
    *	   Die Argumente sind normale 0-terminierte C-Strings
    */
   char    *first_arg,
   /*
    *	   Weitere Argumente:
    */
   ...
)
{
#ifndef __AROS__
   struct	RexxMsg *rmsg;
   int		i;
   char 	**args;

   /*	Rexx geöffnet ? */
   if	(RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(0);
   }

   /*	gültiges Kommando ? */
   if	(com==0 || *com==0)
   {
      error=RERROR_INVALID_COMMAND_NAME;
      return(0);
   }

   /* RexxMessage-Body erstellen */
   if ((rmsg=CreateRexxMsg(rexx_port,default_extension, rexx_port->mp_Node.ln_Name))==0)
   /*	konnte keine RexxMessage erstellen */
   {
      error=RERROR_OUT_OF_MEMORY;
      return(0);
   }


   /* Die Kommandos und Argumente einfügen */
   /* erst das Kommando */
   rmsg->rm_Args[0]=(STRPTR) com;

   /*
    * Die Argumente einlesen. Zur einfacheren Verarbeitung generieren
    * wir ein Feld in dem die Argumente stehen, und das man mit einer
    * Schleife abarbeiten kann
    */
   args=&first_arg;

   /* Jetzt alle Argumente bis zum ersten NULL argument in die
      Message eintragen.*/
   for (i=1;i<15 && args[i-1];i++)
      rmsg->rm_Args[0]=(STRPTR) args[i-1];

   /*
    * Mit der Funktion FillRexxMsg kann man nun die Strings in der
    * Message durch Rexx Argsstrings ersetzen lassen.
    */
   if (!FillRexxMsg(rmsg,i,0))
   {
      /*
       * wahrscheinlich zuwenig mem für die Umwandlung, also
       * ErrorCode setzen, die nicht gebrauchte Message zurückgeben
       * und Abgang
       */
      error=RERROR_OUT_OF_MEMORY;
      DeleteRexxMsg(rmsg);
      return(0);
   }

   /* Command feld initialisieren */
   rmsg->rm_Action=action;

   return(rmsg);
#else
   return NULL;
#endif
}


/*****************************************************************/
/*	Dies Funktion gibt den Speicher einer RexxMessage frei.  */
/*	Also die Argumente, einen ggf vorhandenen ReturnWert	 */
/*	und die Message selbst. Aus diesem Grund müßen Felder	 */
/*	(Argumente und Return Code, wenn sie außerhalb der       */
/*	Funktion freigegeben worden sind auf NULL gesetzt werden */
/*****************************************************************/
BOOL RemoveRexxMsg
(
   /* Zeiger auf die zu löschende RexxMessage */
   /*
    *	   gibt TRUE für Erfolg zurück
    */
   struct	RexxMsg *rmsg
)
{
#ifndef __AROS__
   /* Rexx geöffnet ? */
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(FALSE);
   }

   /*
    * Als erstes prüfen, ob ein Return String vorhanden ist,
    * wenn ja, dann löschen. Diese Funktion löscht den ReturnString
    * nur dann, wenn in Action ein Result gefordert ist. Und kein
    * Error im Result1 Feld eigetragen ist.
    */
   if ((rmsg->rm_Action&RXFF_RESULT) && rmsg->rm_Result1==0 && rmsg->rm_Result2!=0)
   {
      DeleteArgstring((char *)rmsg->rm_Result2);
      rmsg->rm_Result2=0;
   }

   /*
    * Alle übergebenen Argumente mit ClearRexxMsg löschen.
    * Bei dieser Funktion werden alle Argumente die nicht NULL sind
    * als RexxStringPointer interpretiert und diese Strings freigegeben
    */
   ClearRexxMsg(rmsg,16);

   /* zum Abschluß noch die eigentliche Message freigeben */
   DeleteRexxMsg(rmsg);
   return(TRUE);
#else
   return FALSE;
#endif
}


/****************************************************************/
/*	PutRexxMsg schickt eine Message an den genannten Port	*/
/*	un übernimmt die Buchführung über fehlende Antworten	*/
/****************************************************************/
BOOL PutRexxMsg
(
   /*
    *	   Port ist der Name des Ziel Ports. Der Name ist NULL beendeter
    *	   String. Achtung !! kein PortPointer.
    *	   Ist port NULL so wird die Message an den RexxHostPort geschickt.
    *	   Da man einen eigenen Namen angeben kann, ist möglich, Messages
    *	   direkt an andere Rexxfähige Programme zu senden.
    */
   char    *port,
   /*
    *	   rmsg    ist ein Zeiger auf gültige, vollstänig initialisierte
    *	   RexxMessage-Struktur.
    */
   struct	RexxMsg *rmsg
)
{
#ifndef __AROS__
   struct	MsgPort 	*msg_port;

   /*	Rexx opened ? */
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(FALSE);
   }

   /*	Prüfen ob der standard RexxPort gefragt ist, wenn ja */
   /*	dann RXSDIR ("REXX") einsetzen                       */
   if (port==0)
      port=RXSDIR;

   /*	Prüfen ob es überhaupt einen PortNamen gibt, wenn nicht
    *	Fehler */
   if (*port==0)
   {
      error=RERROR_NO_PORT;
      return(FALSE);
   }

   /*	Message Port suchen */
   Forbid();
   if ((msg_port=FindPort(port))==0)
   {
      /* gewünschter Port existiert nicht	*/
      Permit();
      error=RERROR_UNKNOWN_DESTINATION_PORT;
      return(FALSE);
   }
   PutMsg((struct MsgPort *)msg_port,(struct Message *) rmsg);
   Permit();

   /* abgeschickte Message merken */
   if ((rmsg->rm_Action & RXFF_NONRET) == 0)
   {
      outstanding_replies ++;
   }

   return (TRUE);
#else
   return FALSE;
#endif
}


/****************************************************************/
/*	GetRexxMsg funktioniert GetMsg des Betriebsystems,	*/
/*	nur das Über die Messages Buchgeführt wird		*/
/****************************************************************/
struct RexxMsg *GetRexxMsg(void)
/*
 *	Die Funktion gibt einen Zeiger auf die Message oder NULL
 *	zurück.
 *	Ist NULL ein Fehler, so kann das mit LastRexxError festgestellt
 *	werden. Sonst bedeutet es einfach, das keine Msg am Port
 *	wartet.
 */
{
#ifndef __AROS__
   struct	RexxMsg *rmsg;

   /* Rexx geöffnet ? */
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(0);
   }
   error=0;

   /* Message holen */
   if ((rmsg=(struct RexxMsg *)GetMsg(rexx_port))==0)
      /* keine Message am Port also return(0) */
      return(0);

   /* Überprüfen ob es eine Antwort auf eine von uns abgeschickte
    * Message ist. Wenn ja die Antwort veruchen.
    * Achtung !! Auch die Rückantwort wird nicht freigegeben
    */
   if (rmsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
      outstanding_replies--;

   return(rmsg);
#else
   return NULL;
#endif
}


/****************************************************************/
/*	FetchRexxMsg wartet bis eine Message an unserm		*/
/*	Port ankommt und gibt diese zurück.			*/
/****************************************************************/
struct RexxMsg *FetchRexxMsg(void)
/*
 *	Die Funktion kehrt mit einem Zeiger auf die vom
 *	Port geholte Message zurück.
 */
{
#ifndef __AROS__
   struct	RexxMsg *rmsg;

   /* Rexx geöffnet ? */
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(0);
   }

   while ((rmsg=GetRexxMsg())==0)
      WaitPort(rexx_port);

   return(rmsg);
#else
   return NULL;
#endif
}


/****************************************************************/
/*	ReplyRexxMsg	setzt den ArexxErrorCode, füllt den	*/
/*	Returnwert_slot und beantwortet die Message.		*/
/*	sollte es eine NonReply Message sein, so wird sie	*/
/*	einfach gelöscht					*/
/****************************************************************/
BOOL ReplyRexxMsg
/*
 *	Im falle eines Fehlers kehrt die Funktion mit FALSE zurück.
 *	Nähere Information kann man LastRexxError holen
 */
(
   /*
    *	   rmsg    ist ein Zeiger auf die zu beantwortende Message
    */
   struct  RexxMsg *rmsg,
   /*
    *	   rexx_error ist ein Langwort, das den ArexxErrorCode enthält.
    *	   Der Code wird in das Result1 Feld der Message eingetragen.
    */
   long    rexx_error,
   /*
    *	   return_string ist ein String, der als Return wert and den
    *	   Caller zurückgegeben wird. ReplyRexxMsg generiert aus diesem
    *	   C-String ein ArexxArgument und füllt dieses in das Result2
    *	   Feld ein.
    *	   Ist rexx_error != 0, also ein Fehler aufgetreten, so wird der
    *	   String ignoriert und das Feld auf NULL gesetz. Soll kein
    *	   Returnwert zurückgegeben werden ist ein NULL-Pointer zu übergeben.
    */
   char    *return_string
)
{
#ifndef __AROS__
   STRPTR      rx_string;

   /*	Rexx geöffnet ? */
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(FALSE);
   }

   /*	Message übergeben worden ? */
   if (rmsg==0)
   {
      error=RERROR_NO_MSG;
      return(FALSE);
   }

   /* Ist es eine NonRet, wenn ja, so wird die Message
    * nicht beantwortet, sondern gelöscht.
    */
   if (rmsg->rm_Action & RXFF_NONRET)
   {
      RemoveRexxMsg(rmsg);
      return(TRUE);
   }

   /*	ErrorCode eintragen	*/
   rmsg->rm_Result1=rexx_error;

   /*
    *	im FehlerFalle Result2 auf NULL setzen, das ist zwar nach
    *	der ARexx Dokumentation nicht nötig, aber man weis ja
    *	nie mit wem es HRexx zu tun hat.
    */
   if (rexx_error)
      rmsg->rm_Result2=NULL;
   else
      /*
       * Wenn kein Fehler aufgetreten ist und eine ReturnString übergeben
       * wurde und die Message eine Antowrt erwartet, so wird aus dem
       * return_string ein Argument String erstellt und an den Caller
       * zurückgegeben
       */
      if (return_string && (rmsg->rm_Action & RXFF_RESULT))
      {
	 rx_string=(STRPTR) CreateArgstring(return_string,strlen(return_string));
	 if (rx_string==0)
	 {
	    /*
	     * Falls kein String erstellt werden kann, wird die
	     * Funktion mit eine Fehler abgebrochen. In diesem Fall
	     * kann man es z.B. damit versuchen, das man keinen
	     * return_string übergibt
	     */
	    error=RERROR_OUT_OF_MEMORY;
	    return(FALSE);
	 }
	 rmsg->rm_Result2=(LONG)rx_string;
      }

   /* unsere hübsche Message zurück an den Chef */
   ReplyMsg((struct Message *) rmsg);
   return(TRUE);
#else
   return FALSE;
#endif

}


/**********************************************************************/
/*	SyncRexxCommand sendet ein Kommando, und wartet bis das       */
/*	Kommando abgearbeitet ist. Sollten in der Zwischenzeit	      */
/*	Kommandos an unser Port gehen, wird die Funktion	      */
/*	process_message mit dem Zeiger auf die Message aufgerufen.    */
/*	So ist es möglich, währen eines SyncAufrufes Kommandos zu     */
/*	Empfangen. SyncRexxCommand unterstützt einen returncode.      */
/*	Dazu muß eine StringBuffer und die Größe des Buffer über-     */
/*	geben werden .						      */
/*	Der aufruf erfolgt:					      */
/*	SyncRexxCommand(port,com,result,result_len,arg1,arg2,...,argn)*/
/*	Werden nicht alle Argumente genutzt, muß das letzte Argument  */
/*	NULL sein. Mehr als 15 Argumente werden ignoriert.	      */
/**********************************************************************/
long SyncRexxCommand
/*
 *	Die Funktion kehrt mit dem ArexxErrorCode oder dem Internen
 *	errorcode zurück.
 *	0 für eine erfolgreiche aktion.
 *	Kommt es zu einem RexxFehler, so wird der ResultString gelöscht.
 */
(
   /*
    *	   port ist der Name des Ports an den das Kommando gesendet werden soll.
    *	   wird als PortName 0 angegeben, so wird das Kommando an den Rexx-
    *	   Host geschickt.
    */
   const char	 *port,
   /*
    *	   Com ist der String der den KommandoNamen enthält
    */
   const char	 *com,
   /*
    *	   result ist ein Pointer auf den StringBuffer in den der
    *	   ResultString des Kommandos kopiert werden soll. Ist result == NULL
    *	   oder result_len == NULL so wird die Fukntion als Kommando
    *	   ohne ReturnWert betrachtet, und die Flags in der RexxMsg
    *	   entsprechend gesetzt.
    */
   char    *result,
   long    result_len,
   /*
    *	   first_arg Zeiger auf den ersten Argument String. Der aufruf Syntax
    *	   ist oben beschrieben, und Funktioniert wie bei BuildRexxMessage.
    */
   char    *first_arg,
   /*
    *	weitere args
    */
    ...
)
{
#ifndef __AROS__
   char **args;
   struct	RexxMsg *rmsg;
   struct	RexxMsg *our_msg;
   long 	flag;
   register	i;
   register	char	*s;

   /*	Rexx geöffnet ? */
   if (RexxSysBase==0)
   {
      error=RERROR_AREXX_NOT_OPEN;
      return(-1);
   }

   /* wurde ein Kommando übergeben ? */
   if (com==0 || *com==0)
   {
      error=RERROR_INVALID_COMMAND_NAME;
      return(-1);
   }

   /* prüfen ob ein Result erwartet wird und die Flags für die
    * Message entprechend vorbereiten
    */
   flag=RXCOMM;
   if (result && result_len) flag|=RXFF_RESULT;

   /* Argumente Feld übernehmen */
   args=&first_arg;
   /* Die Message erstellen */
   our_msg=BuildRexxMsg(com,flag,args[0],args[1],args[2],args[3],
	args[4],args[5], args[6],args[7],args[8],args[9],args[10],
	args[10],args[11], args[12],args[13],args[14]);
   /* geklappt ?*/
   if (our_msg==0) return(-1);

   /* let's fets und schon schicken wir die Msg ab */
   if (!PutRexxMsg(port,our_msg))
   {
      /* speicher der Message freigeben */
      RemoveRexxMsg(our_msg);
      return(-1);
   }

   /* jetzt warten wir bis unsere Message beantwortet zurück kommt */
   do
   {
      rmsg=FetchRexxMsg();

      /* unsere Message ? */
      if (rmsg==our_msg) break;

      /* eine Antwort auf eine alte Message ? */
      if (rmsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
	 RemoveRexxMsg(rmsg);
      else
	 /*
	  * haben wir eine Funktion die für die Bearbeitung
	  * ankommender Messages zuständig ist ?
	  * wenn ja, dann Funktion anspringen, sonst Message mit
	  * Unknown Command zurückgeben
	  */
	 if (process_message!=0)
	 {
	    (*process_message)(rmsg);
	 }
	 else
	    ReplyRexxMsg(rmsg,ERR10_030,0);
   } /* auf Antwort warten */
   while (TRUE);

   /* error code retten */
   flag=our_msg->rm_Result1;

   /* wenn kein error aufgetreten ist und ein Result erwartet wird
    * wird der Result String in den Buffer kopiert
    */
   if (flag==0 && result && result_len)
   {
      /* Zeiger Resultstring holen	*/
      if (s=(char *)rmsg->rm_Result2)
      {
	 for (i=0;i<(result_len-1) && s[i];i++)
	    result[i]=s[i];
	 result[i]=0;
	 if (s[i]!=0)
	 {
	    /* wenn der Ergebniss string zu lang ist error code setzen
	     * zurückgeben und den string mittendrin abbrechen.
	     */
	    flag=RERROR_STRING_TOO_LONG;
	 }
      } else
	 *result = 0;
   }
   RemoveRexxMsg(our_msg);
   return(flag);
#else
   return -1;
#endif
}

/****************************************************************/
/*	RexxMessageHandler sucht die FunktionsListe durch, und	*/
/*	ruft, falls es das Kommando gibt, die dazugehörige	*/
/*	Funktion auf. Sollte das Kommando nicht existieren	*/
/*	wird der ARexxFehlerCode Label NotFound zurückgegeben.	*/
/****************************************************************/
__stkargs void RexxMessageHandler (struct RexxMsg *rmsg)
/*
 * RexxMessageHandler ruft bei ankommenden Messages die in der
 * Funktionliste übergebenen Kommandos auf.
 *
 * --> rmsg	Zeiger auf die RexxMessage, die bearbeitet werden soll
 */
{
#ifndef __AROS__
   register	i;
   register	act_com;
   char 	*s;

   /*	existier ein CommandTable ? */
   if (command_table==0)
   {
      /* Message mit Unknown Label beantworten */
      ReplyRexxMsg(rmsg,ERR10_030,0);
      return ;
   }


   /*	Zeiger auf den Commando String holen */
   s=(char *)rmsg->rm_Args[0];

   /*
    * Kommando liste nach dem Kommando druchsuchen, wenn die Funktion
    * gefunden wird, aufrufen sonst mit UNKNOWN_LABEL beantworten.
    * Aufgrund der Vergleichs Funktion müßen Kommandos, die als Anfang
    * in einem anderen Kommando der Tabelle enthalten sind hinter
    * diesen stehen, das sonst die längeren Kommandos nicht gefunden
    * würden.
    * So  muß  die Funktion "test" nach der Funktion "testme" stehen.
    * Beim vergleich wird die groß klein Schreibung ignoriert.
    * Strings mit der Länge 0 verwirren die Funktion ebenfalls.
    */
   for (act_com=0;command_table[act_com].Name;act_com++)
   {
      /* String vergleichen */
      for (i=0;(toupper(command_table[act_com].Name[i])==toupper(s[i])) && (s[i]!=0);i++);
      if (command_table[act_com].Name[i]==0)
      {
	 /* Skip Spaces in Kommando bis zu argumenten */
	 while (s[i]==32) i++;
	 /* Funktion aufrufen */
	 if (command_table[act_com].Function(s+i,rmsg))
	    /* Message beantworten */
	    ReplyRexxMsg(rmsg,0,0);
	 goto done;
      }
   }

   /* Kommando unbekannt */
   ReplyRexxMsg(rmsg,ERR10_030,0);

   done:;
#endif
}


/****************************************************************/
/*	ChangeProcessMessage	ändert den Zeiger der process_	*/
/*	message funktion.					*/
/****************************************************************/
void * ChangeProcessMessage(__stkargs void (*new_function)(struct RexxMsg * rmsg))
/*
 * ChangeProcessMessage ändert die Funktion für das InterneMessagehandling
 *
 * --> new_function	Zeiger auf die neue Funktion, bei -1 wird
 *		der HRexx interne Handler verwendet
 * <-- RESULT	Pointer auf alten hndler
 */
{
#ifndef __AROS__
   void   (*old_function)(struct RexxMsg * rmsg);

   /* alte Funktion zurückgeben */
   old_function=process_message;

   /*	Internen handler ? */
   if ((long)new_function==-1)
      process_message = RexxMessageHandler;
   else
      process_message = new_function;

   if ((long)old_function==(long)RexxMessageHandler)
      old_function=(void (*)(struct RexxMsg *))-1;

   return (old_function);
#else
   return NULL;
#endif
}

/****************************************************************/
/*	SetRexxCommandTable  ändert die interne Kommandotabelle */
/****************************************************************/
struct RexxCommand * SetRexxCommandTable
(
   /*
    *	   Zeiger auf eine vom Benutzer definierte Kommando Tabelle
    */
   struct  RexxCommand	   *new_com_tab
   /*
    *	   gibt den Zeiger auf die alte Funktiontabelle zurück.
    */
)
{
   struct RexxCommand	   *old_com_tab;

   old_com_tab=command_table;
   command_table=new_com_tab;

   /* Ist der FunktionTable != NULL so wird die Handler Funktion
    * auf den internen MessageHandler umgelenkt. Sonst wird der Message
    * Handler Pointer auf NULL gesetzt, wenn er vorher auf den
    * RexxMessageHandler gezeigt hat
    */
   if (command_table!=0)
   {
      ChangeProcessMessage((void *)-1);
   }
   else
      if ((long)process_message==(long)RexxMessageHandler)
	 process_message=0;

   return(old_com_tab);
}

/*************************************************************/
/*  ModifyRepCount  ändert Anzahl fehlender Messages	     */
/*************************************************************/
long ModifyRepCount(long delta)
/*
 ModifyRepCount
	ändert den Counter mit den fehlenden Messages um den in
	delta angegebenen Betrag.
	Der alte wert von RepCount wird zurückgegeben
*/
{
   long old;			/* alter wert von outstandig_replies */

   old=outstanding_replies;	/* alten wert merken */
   outstanding_replies+=delta;	/* ändern */
   return(old);
}


#ifdef DEBUG
/* testing enviroment */

#include <stdio.h>
hallo(arg)
char	*arg;
{
   printf("Rexx called hallo with:\"%s\"\n",arg);
   return (TRUE);
}

struct	RexxCommand	rcs[]=
{
   "hallo",(void *)hallo,
   0,0
};


main(argc,argv)
long   argc;
char **argv;
{
   struct	RexxMsg *rmsg;
   char 	result[100];
   long error;

   if (OpenRexx("RexxTest","test",0,-1,-1)==0)
   {
      printf("OpenError %ld\n",LastRexxError());
      goto err;
   }

   SetRexxCommandTable(rcs);

   result[0]=0;

   error=SyncRexxCommand(0,"ootest tatara",result,100,0);
   printf("%ld %s\n",error,result);

   err:;
   CloseRexx();
} /* main */
#endif

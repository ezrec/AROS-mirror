From opty@club-internet.fr  Thu Feb 27 22:37:05 1997
Return-Path: opty@club-internet.fr
Received: from mailhost.grolier.fr (mailhost.grolier.fr [194.158.97.88]) by iutsoph.unice.fr (8.7.6/8.7.3) with ESMTP id WAA00369 for <masson@iut-soph.unice.fr>; Thu, 27 Feb 1997 22:37:04 +0100
Received: from club-internet.fr (Opty@ppp-196-244.neuilly.club-internet.fr [194.117.196.244]) by mailhost.grolier.fr (8.8.5/MGC-960516) with SMTP id WAA26334 for <masson@iut-soph.unice.fr>; Thu, 27 Feb 1997 22:33:50 +0100 (MET)
From: =?iso-8859-1?Q?St=E9phane?= Barbaray <opty@club-internet.fr>
To: Gilles Masson <masson@iutsoph.unice.fr>
Date: Thu, 27 Feb 1997 21:42:36 MET-1
Message-ID: <yam6997.1809.33511160@mail.club-internet.fr>
In-Reply-To: <199702270832.JAA03607@ogpsrv.unice.fr>
X-Mailer: YAM 1.3.4 [020] - Amiga Mailer by Marcel Beck
Organization: Opty inc
Subject: Re: Incroyable +(new bug report)! (was: Enforcer hit avec NList 0.47  )
MIME-Version: 1.0
Content-type: multipart/mixed; boundary="BOUNDARY.6997.1809.33511160.3"

Warning: This is a message in MIME format. Your mail reader does not
support MIME. Some parts of this message will be readable as plain text.
To see the rest, you will need to upgrade your mail reader.

This message was composed on an Amiga using the YAM mailer.
YAM is available at http://bitcom.ch/~mbeck/

--BOUNDARY.6997.1809.33511160.3
Content-Type: text/plain; charset=iso-8859-1
Content-Transfer-Encoding: quoted-printable

Hi Gilles!

On 27-F=E9v-97, you wrote:


>> >> _BIG BUG NList/NListview_:
>> >> Tes classes ne fonctionnent pas (ou fonctionnent mal) avec les
>> >BalanceObjects.
>> >> Ils limitent ou interdisent le redimensionnement. Pour v=E9rifier? =
C'est
>tr=E9s
>> >> simple:
>> >> =

>> >> - Essayer avec une fen=EAtre contenant horizontalement 1 ListObject=
 et 1
>> >> NListObject (ou/avec ListviewObject/NListviewObject) avec entre deu=
x un
>> >> BalanceObject: =E7a se resize difficilement
>> >> - Essayer maintenant avec une fen=EAtre contenant horizontalement 2=

>> >NListObject
>> >> et 1 BalanceObject entre deux: =E7a se resize plus du tout!

>T'as un exemple ?  par ce que l'exemple que je me suis test avec 2 NList=

>object reagissait tres bien...

vivi, mais je dois te pr=E9venir que =E7=E0 ne se resize qu'uniquement en=
 mode "show
objects"! En mode "show frames", =E7=E0 se resize mais pas autant qu'avec=
 un
Listview/List Object (voir cet example)
Je te souhaite un bon WE de debug! ;-P

>> Pour un IUT?

>ben oui. La DECstation 5000/240 sous Ultrix commence a etre vielle (et
>pas tres rapide pour l'heure actuelle). Et un PPro 200/Linux coute pas
>mal moins cher qu'un AlphaServeur a 233 MHz, et encore, un alpha a 233
>c'est pas specialement mieux qu'un PPro a 200. Ou bien c'est le choix
>de linux qui te surprend ?  ben c'est pas cher et ya tous les sources :)=

>Et puis ca tourne plutot tres bien dans l'ensemble...
>Idealement, un Bi-PPro aurait ete mieux, mais bon...

  Ben habituellement, c'est plut=F4t des stations SUN (tout les mod=E8les=
), des
Risk 6000 (pas au point), et des AS/400 (pour le Cobol), plus les parcs d=
e PC
386 ou 486 (Compaq) sous r=E9seau Novell. Evidemment moi j'=E9tais tout l=
e temps
fourr=E9 dans la salle des stations SUN... Les enseignants avaient 1 ou 2=

station SUN =E0 leur disposition (minimum Sparc10, pas de PC) =

  En tout cas, c'=E9tait comme =E7=E0 =E0 mon IUT de Lille 1 et ils avaie=
nt l'air de
dire que c'=E9tait pareil ailleurs...


-- =


Greetings, St=E9phane Barbaray.

=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=
=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=
=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=
=3D=3D=3D
St=E9phane Barbaray                   E-Mail: opty@club-internet.fr
186, rue Pierre Joigneaux           IRC: Opty on EfNet servers after 21h3=
0 GMT
92250 La Garenne Colombes           Tel: +33-1-42420796
France                              WEB: http://perso.club-internet.fr/op=
ty/


--BOUNDARY.6997.1809.33511160.3
Content-Type: text/plain; charset=iso-8859-1; name="test.c"
Content-Transfer-Encoding: quoted-printable
Content-Description: L'exemple en question...

#include <proto/muimaster.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <libraries/mui.h>
#include <mui/nlist_mcc.h>
#include <mui/nlistview_mcc.h>

struct Library *MUIMasterBase =3D NULL;

void main(void)
{
   void *window, *app;

   if (!(MUIMasterBase =3D OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
	return;

   if(app =3D ApplicationObject,
	MUIA_Application_Title      , "OptyTest",
	MUIA_Application_Version    , "$VER: OptyTest 1.0 (27.02.97)",
	MUIA_Application_Copyright  , "=A91997, Opty inc.",
	MUIA_Application_Author     , "Opty",
	MUIA_Application_Description, "just a test",
	MUIA_Application_Base       , "OPTYTEST",
	SubWindow,window =3D WindowObject,
	    MUIA_Window_Title, "Opty Test",
	    MUIA_Window_ID   , 1,
	    MUIA_Window_Width , MUIV_Window_Width_Screen(50),
	    MUIA_Window_Height, MUIV_Window_Height_Screen(50),
	    WindowContents, VGroup,
		GroupFrame,
		Child, HGroup,
		    Child, NListviewObject,
			MUIA_CycleChain, 1,
			MUIA_NListview_NList, NListObject, End,
		    End,
		    Child, BalanceObject, MUIA_CycleChain, 1, End,
		    Child, NListviewObject,
			MUIA_CycleChain, 1,
			MUIA_NListview_NList, NListObject, End,
		    End,
		End,
		Child, HGroup,
		    Child, ListviewObject,
			MUIA_CycleChain, 1,
			MUIA_Listview_List, ListObject, InputListFrame, End,
		    End,
		    Child, BalanceObject, MUIA_CycleChain, 1, End,
		    Child, ListviewObject,
			MUIA_CycleChain, 1,
			MUIA_Listview_List, ListObject, InputListFrame, End,
		    End,
		End,
	    End,
	End,
   End)
   {
	ULONG sigs =3D 0;

	DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MU=
IM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	set(window, MUIA_Window_Open, TRUE);

	while (DoMethod(app, MUIM_Application_NewInput, &sigs) !=3D MUIV_Applica=
tion_ReturnID_Quit)
	{
		if (sigs)
		{
			sigs =3D Wait(sigs | SIGBREAKF_CTRL_C);
			if (sigs & SIGBREAKF_CTRL_C) break;
		}
	}

	set(window, MUIA_Window_Open, FALSE);

        MUI_DisposeObject(app);
   }

   CloseLibrary(MUIMasterBase);
}

--BOUNDARY.6997.1809.33511160.3--



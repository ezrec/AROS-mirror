===========
Nerladdning
===========

.. Note::

   Om du vill **testa AROS** så rekommenderar vi att du laddar ner
   **distributioner**, vilka är fullt utrustade och testade AROS-paket.
   "Nightly builds" (nattkompilationer), som du hittar längst ner, är enbart
   till för utvecklare och testare. Nightly builds är inte enkla att använda,
   är inte alltid stabila och de saknar de flesta program som en vanlig
   användare troligen skulle efterfråga.


.. Contents::

.. Include:: download-abstract.sv


Icaros Desktop LIVE!
--------------------

.. image:: /images/icaroslive_logo.png
   :align: left

`Icaros`__ är en välfylld distribution riktad till kraftfullare
skrivbordsdatorer. Den kan antingen köras via en Live DVD (eller CD för den
nedbantade versionen) utan att installeras på din dator (den måste dock
fortfarande ha AROS-stödd hårdvara), via en datorsimulator som VMWare eller
`VirtualBox`__ eller så kan man installera den på en hårddisk. Den kan även
installeras parallellt med andra OS som Microsoft Windows på en egen partition
(kräver kunskap i GRUB). Inkluderas gör även en användarhandbok i PDF-format.

__ http://live.icarosdesktop.org/
__ https://www.virtualbox.org/


AspireOS
--------

.. image:: /images/aspireos_logo.png
   :align: left

`AspireOS`__ är en resurseffektiv distribution, ursprungligen gjord med
netbooks såsom Acer Aspire One i åtanke. Dess filosofi är att vara så simpel
och nerbantad som möjligt. Alla inkluderade program är väl testade.

__ https://www.aspireos.com/


AROS Broadway
-------------

`AROS Broadway`__ är den distribution som medföljer ARES-datorerna som
standard, men det fungerar lika på på andra AROS-kompatibla datorer.

__ http://www.aros-broadway.de/index.html


AROS Vision
-----------

`AROS Vision`__ är distributionen för M68K-Amigor. För att fungera
tillfredsställande krävs antingen en kraftig Amiga eller en emulator som
Win UAE.

__ http://www.aros-platform.de/download.htm



Snapshots
=========

`Snapshots`__ är icke regelbundet utgivna och icke automatiserade versioner
av AROS. De är skapade av utvecklare som av någon anledning inte kan använda
sig av vanliga nightly builds.

Dessa versioner är inte övergivna, så använd den vanliga kanalen för att
rapportera buggar (`bug tracker`__).

__ snapshots
__ http://sourceforge.net/p/aros/bugs/



Nightly Builds
==============

`Nightly builds`__ betyder "nattkompilationer" och, precis som namnet antyder,
skapas varje kväll direkt från "Subversion tree" och innehåller den senaste
koden. Men de är inte testade och kan innehålla oförutsedda buggar. Oftast
fungerar de utmärkt dock.

Var snäll och rapportera alla buggar du hittar via buggrapportkanalen
(`bug tracker`__). För andra viktiga ärenden kan du kontakta oss via
`AROS-Exec`__-forumet.

__ nightly
__ http://sourceforge.net/p/aros/bugs/
__ http://aros-exec.org/



Nightly Builds (ABIv1)
======================

AROS håller på att övergå till en ny `ABI`__. En egen uppsättning nightly
builds har skapats för denna experimentella källkod, men `dessa`__ är bara
användbara för utvecklare som vill hålla sig uppdaterade om ABI-övergången.
**AVIv1 är inte kompatibel med någon distribution eller AROS-mjukvara från
AROS Archives eller Aminet.** Användare som vill testa nyheter och buggfixade
versioner av AROS som ännu inte implementerats i distributionerna bör endast
använda de vanliga nightly builds.

__ http://en.wikipedia.org/wiki/Application_binary_interface
__ nightly1


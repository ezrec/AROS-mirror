=================
Krátký úvod AROSu
=================

:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Témìø hotovo, myslím...


.. raw:: html

   <h1>Úvod<br><img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"></h1>

AROS Research Operating System je velmi jednoduchý, výkonný a flexibilní
desktopový operaèní systém urèený k tomu, abych ti pomohl vytì¾it maximum
z tvého poèítaèe. Je to nezávislý, pøenosný a svobodný projekt, který se
sna¾í být kompatibilní s AmigaOS 3.1 na úrovni API (jako Wine, ne v¹ak jako UAE),
pøi souèasném zlep¹ování v mnoha oblastech. Zdrojové kódy jsou dostupné
pod open source licencí, která je dovoluje komukoli zdokonalovat.


Cíle
====

Cílem AROS projektu je vytvoøit OS, který:

1. Je co mo¾ná nejvíce kompatibilní se operaèním systémem AmigaOS 3.1.

2. Mù¾e být portován na rùzné druhy hardwarových architektur a
   procesorù, jako jsou x86, PowerPC, Alpha, Sparc, HPPA a dal¹í.

3. By mìl být kompatibilní binárnì na Amize a zdrojovì na jakémkoli jiném
   hardwaru.

4. Mù¾e bì¾et jako samostatná verze, která bootuje pøímo z pevného disku, i
   jako emulace, která otevøe okno ve stávajícím OS pro vývoj softwaru a
   bìh Amigy a nativních aplikací zároveò.

5. Vylep¹í funkènost systému AmigaOS.

Pro dosa¾ení tohoto cíle pou¾íváme øadu technik. V prvé øadì ve velké míøe
vyu¾íváme internet. Na na¹em projektu se mù¾e¹ podílet, i kdy¾ umí¹ napsat
pouze jednu jedinou funkci 0S. Poslední verze zdrojových kódù
je dostupná 24 hodin dennì a opravy v nich mohou být kdykoli mergnuty.
Malá databáze s otevøenými úkoly (open tasks) zaji¹»uje, ¾e práce není duplikována.


Historie
========

Nìjaký èas zpátky (v roce 1993) vypadala situace pro Amigu ponìkud hùø
ne¾ obvykle a nìkteøí fanou¹ci Amigy se spojili a zaèali diskutovat, co by
se mìlo udìlat, aby se zvý¹ilo pøijetí na¹eho milovaného stroje. Hlavní dùvod
pro chybìjící úspìch Amigy byl ihned jasný: byla to propagace,
tedy spí¹e její nedostatek. Amiga by mìla získat ¹ir¹í základnu, aby
se stala atraktivnìj¹í pro v¹echny - u¾ivatele i vývojáøe. Proto byly
vytvoøeny plány k dosa¾ení tohoto cíle. Jedním z plánù bylo opravit chyby AmigaOS,
dal¹ím bylo uèinit z nìj moderní operaèní systém. Zrodil se projekt AOS.

Ale co pøesnì jsou chyby? A jak by tyto chyby mìly být opraveny? Jaké funkce
musí takzvaný *moderní* OS mít? A jak by mìly být implementovány
do AmigaOS?

O dva roky pozdìji se o tom stále dohadovali a nebyl napsán
jediný øádek kódu (nebo alespoò nikdo nikdy ¾ádný nevidìl). Diskuze
byly stále o tom, ¾e nìkdo øekl "musíme mít..." a
jiný odpovìdìl "pøeèti si staré maily" nebo "nejde to udìlat, proto¾e...",
co¾ bylo krátce poté následováno "plete¹ se, proto¾e...", a tak dále.

V zimì roku 1995 byl u¾ Aaron Digulla z této situace unaven a poslal
RFC (¾ádost o komentáøe) do AOS mailing listu, ve které se ptal, jaké
by mìlo být základní spoleèné východisko. K dispozici bylo nìkolik variant a závìr
byl, ¾e témìø v¹ichni by chtìli vidìt otevøený OS, který je kompatibilní s
AmigaOS 3.1 (Kickstart 40.68). Na tomto základì se mohly stavìt dal¹í diskuze,
aby se zjistilo, co je mo¾né a co ne.

A tak zaèala práce a AROS se narodil.

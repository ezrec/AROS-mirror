==========
Contribuir
==========

:Authors:   Adam Chodorowski 
:Copyright: Copyright © 1995-2007, The AROS Development Team
:Version:   $Revision: 26356 $
:Date:      $Date: 2007-07-21 19:17:15 +1000 (Sat, 21 Jul 2007) $
:Status:    Done. 

.. Contents::

.. Include:: contribute-abstract.en


Tareas disponibles
==================

Esta es una lista de algunas tareas con las que necesitamos ayuda y en las
que nadie está trabajando ahora. No es una lista completa, simplemente contiene
las cosas más destacadas con las que necesitamos ayuda en AROS.


Programación
------------

+ Implementar las bibliotecas, recursos, dispositivos faltantes o partes de éstas.
  Vea el informe de estado detallado para tener más información sobre lo que falta.

+ Implementar o mejorar los controladores de los dispositivos de hardware:
  
  - AROS/m68k-pp:
    
    + Graphics
    + Input (touchscreen, buttons)
    + Sound
    + Gráficos
    + Entrada (pantalla tactil, botones)
    + Sonido

     
  - AROS/i386-pc:
    
    + Specific graphic card drivers (we only have general, not very well
      accelerated ones). A short wishlist:
    + Los controladores de tarjetas gráficas específicas (solamente tenemos unas
      generales, no muy bien aceleradas. Una breve lista de deseos:
      
      - nVidia TNT/TNT2/GeForce (started, but incomplete) 
      - S3 Virge
      - Matrox Millenium
    
    + missing USB classes
    + SCSI
    + Specific IDE chipsets
    + Sound
    + ...anything else you can think of.
    + las clases USB faltantes
    + SCSI
    + los chipsets IDE específicos
    + Sonido

  - Un soporte genérico de la impresora.
 
+ Adaptar a otras arquitecturas. Algunos ejemplos de hardware para el cual no
  hay puertos de AROS mantenidos o iniciados:

  - Amiga, m68k y PPC.
  - Atari.
  - HP 300 series.
  - SUN Sparc.
  - iPaq.
  - Macintosh, m68k y PPC.

+ Implementar los editores de Preferencias faltantes:

  - Overscan
  - Palette
  - Pointer
  - Printer
 
+ Mejorar la biblioteca de enlace de C

  Esto significa implementar las funciones ANSI (y algunas POSIX) en la clib,
  para hacer más fácil llevar software UNIX (eg. GCC, make y binutils). La cosa
  más grande que falta es el soporte para el señalamiento de estilo POSIX, pero
  también otras funciones.

+ Implementar más datatypes y mejorar los existentes

  El número de datatypes disponibles en AROS es bastante pequeño. Algunos
  ejemplos de datatypes que necesitar mejorar para convertirse en usables
  o necesitan ser implementados desde cero son:

  - amigaguide.datatype
  - sound.datatype
    
    + 8svx.datatype

  - animation.datatype
    
    + anim.datatype
    + cdxl.datatype
    
  
+ Adaptar programas de terceros:

  - Los editores de texto como ViM y Emacs.
  - La cadena de herramientas de desarrollo, que incluye GCC, make, binutils y
    las otras herramientas GNU.
  - El software de fuente abierta para el AmigaOS como SimpleMail, YAM, Jabbwerwocky.


Documentación
-------------

+ Escribir la documentación del usuario. Esto consiste en escribir una
  Guía de Usuario general para los novicios y los expertos, y también la 
  documentación de referencia para todos los programas standard de AROS.

+ Escribir la documentación del desarrollador. Aunque esto está un poco 
  mejor que la documentación del usuario, todavía hay bastante trabajo
  para hacer. Por ejemplo, todavía no hay un tutorial para los programadores
  novicios realmente bueno. Sería agradable tener el equivalente de los Manuales 
  del Núcleo de la ROM.

Traducción
----------

+ Traducir AROS a más idiomas. Actualmente, sólo los siguientes
  lenguajes tienen un soporte más o menos completo:

  - English
  - Deutsch
  - Svenska
  - Norsk
  - Italiano
  - French
  - Russian

+ Traducir la documentación y el sitio web a más idiomas. Actualmente, está
  disponible por completo en inglés. Se han traducido partes a otros lenguajes,
  pero todavía queda mucho por hacer.


Otras
-----

+ Coordinar el diseño de la GUI para los programas AROS, como el programa Prefs,
  las herramientas y las utilidades.


Unirse al equipo
================

¿Quieres unirte al esfuerzo de desarrollo? ¡Fantástico! Entonces únete a las
`development mailing lists`__ en las que estés interesado (al menos se 
recomienda *mucho* unirse a la lista de desarrollo principal) y pide acceso
al depósito de Subversion. Eso es todo. :)

Escribe un breve mensaje a la lista de desarrollo que contenga una presentación
de tí mismo y con qué estás animado para ayudar. Si tienes problemas, por favor
no dudes en enviar un mensaje a la lista o pregunta en los `IRC channels`__.
También, antes de empezar en algo específico, por favor escribe un mensaje a la
lista afirmando lo que estás por hacer para actualizar la base de datos de tareas.
De esta manera podemos asegurar a la gente que no trabaje en la misma cosa por
error...

__ ../../contact#mailing-lists
__ ../../contact#irc-channels


El depósito de Subversión
-------------------------

El depósito de AROS está funcionando en un servidor Subversion protegido con contraseña,
lo que significa que necesitas pedir acceso a él para poder colaborar en el desarrollo.
Las contraseñas están encriptadas; la puedes generar con nuestra `online password
encryption tool`__.

Por favor envía la contraseña encriptada junto tu nombre de usuario preferido y
tu nombre real a `Aaron Digulla`__ y espera una respuesta. Para facilitar una
rápida respuesta, por favor pon en el campo asunto "Acces to the AROS SVN server"
y en el cuerpo "Please add <nombre de usuario> <contraseña>", por ej.::

    Please add digulla xx1LtbDbOY4/E

Podría tardar un par de días porque Aaron está bastante ocupado, así que sé paciente.

Para información sobre cómo usar el servidor SVN de AROS, por favor lee "`Working with
SVN`__". Incluso si ya sabes cómo usar SVN es útil que lo mires pues contiene 
información y consejos específicos para el depósito de AROS (por ej. sobre cómo ingresar.

__ http://aros.sourceforge.net/tools/password.html 
__ mailto:digulla@aros.org?subject=[Access%20to%20the%20AROS%20SVN%20server]
__ svn
 

=========================
Trabajando con Subversion
=========================

:Authors:   Aaron Digulla, Adam Chodorowski 
:Copyright: Copyright (C) 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.
:Abstract: 
    Subversion (o SVN para abreviar) es una herramienta de control de versión que
    mantiene una base de datos de los archivos de un proyecto. Con SVN, es posible
    inspeccionar y controlar los cambios aplicados a cualquier archivo específico:
    qué cambios fueron hechos y cuándo, quién hizo aquellos cambios, cuál fue el
    propósito del cambio (como una entrada de registro), la posibilidad
    de revertir los cambios mal hechos, mezclar los cambios de varias personas 
    distintas y más.
    
    En esencia, hace *mucho* más fácil colaborar entre un grupo de gente en un
    proyecto común permitiendo que todos sepan qué está pasando con los archivos,
    asegurándose que la gente no eche a la basura los cambios de los otros por error y
    proporcionando los medios para hacer todo esto en Internet. Naturalmente, la
    usamos para colaborar en AROS.


.. Contents::


Introducción
============

El servidor mantiene un "depósito" central, que es la base de datos principalç
que contiene el código base común publicado del proyecto. Los desarrolladores
individuales tienen sus propias "copias de trabajo", que son copias locales de
la base de datos de un momento específico junto con los cambios locales que el
desarrollador no cargó al servidor. Cuando un desarrollador quiere compartir 
sus cambios con el resto del equipo simplemente "envía" sus cambios al
servidor usando el programa cliente, que se encarga de cargarlos y mezclarlos
con los cambios hechos por los otros desarrolladores.


El software
===========

UNIX
----

Si estás ejecutando Linux, FreeBSD o cualquier otro clon moderno de UNIX entonces
sólo necesitas instalar el software oficial SVN, versión 1.0 o superior,
para tu OS. Todas las distribuciones comunes de Linux vienen ya con el paquete
SVN.

También hay una interfaz GUI interplataforma para el sistema de revisión
Subversion llamado `RapidSVN <http://rapidsvn.org/>`__, disponible para los
Unixes(las distribuciones GNU/Linux, FreeBSD, Sun Solaris y otras), Mac OS X
y Windows.

.. Note:: El servidor ejecuta Subversion 1.1 que se puede acceder con un cliente
          versión 1.0, 1.1 o 1.2.

SVN no funciona con la localización UTF-8. Tienes que cambiar el locale a
ISO8859 antes de cualquier acción con SVN.

AmigaOS
-------

Si estás ejecutando AmigaOS, necesitarás una pila TCP/IP y algún puerto
de SVN instalados. Una opción es el puerto para Amiga de Olaf Barthel que se
puede hallar en AminNET__ (busca "subversion").

__ http://main.aminet.net/

Windows
-------

Si ejecutars Microsoft Windows (TM) puedes usar el cliente SVN TortoisseSVN__, especialmente si
te gusta usar Windows Explorer. Este programa es de fuente abierta y gratuito, con muchas características
y un buen soporte. Por favor asegúrate también que presentas los archivos con los fin de renglón estilo UNIX,
de otro modo puedes *romper* la generación de código.
Si los archivos que estás cambiando tienen puesta la propiedad de SVN eol-style:nativa 
(ahora los nuevos archivos de cualquier modo) entonces puedes olvidarte, y los fin de renglón 
(EOL) se convertirán automáticamente.
Ejemplos de editores que se sepa que pueden guardar tal EOL incluyen 
`Notepad++ <http://notepad-plus.sourceforge.net>`__, TigerPad (free 
enhanced notepad), `DOS Navigator OSP <http://dnosp.com/>`__, y muchos más.
También está disponible el cliente RapidSVN, como se dijo antes.

__ http://www.tortoisesvn.net/

MacOS X
-------

Si ejecutas MacOS X puedes usar uno de los varios puertos de SVN disponibles
como `el puerto de Subversion`__ de Martin Ott. También puedes querer instalar un cliente
adicional con GUI como `svnX`__. svnX es el cliente SVN más puesto al día para OS X y
también soporta la integración con, entre otros, el editor de texto gratuito
`Textwrangler`__ (antes BBedit Lite). Fíjate que cuando uses Textwrangler u otro editor
de texto Mac, necesitarás cambiar la codificación de los caracteres del 'Macos roman' a
'ISO latin 1' (ISO8859) para tus documentos (haz el cambio en las preferencias). Para Textwrangler
todas las restantes configuraciones son correctas por omisión. 
Todas las aplicaciones nombradas son freeware y están disponibles como Binario Universal
para las Mac PPC e Intel. Como ya se dijo, también está disponible RapidSVN.

__ http://www.codingmonkeys.de/mbo/
__ http://www.lachoseinteractive.net/en/community/subversion/svnx/
__ http://www.barebones.com/products/textwrangler/


Ingresar al servidor
====================


A diferencia de CVS, no necesitas ingresar (login) en el servidor. En su
lugar, SVN preguntará cuando necesite conocer tu cuenta de usuario y 
contraseña.

.. Note:: 

    El depósito de AROS está funcionando sobre un srvidor SVN protegido por
    contraseña, lo que significa que necesitas `solicitar acceso`__ para
    poder colaborar en el desarrollo. A pedido de Amiga Inc., el acceso
    anónimo de solo-lectura al depósito está deshabilitado.
          
__ contribute#joining-the-team


Obtener los fuentes de AROS
===========================

Para obtener una copia de los fuentes de AROS usa el comando "checkout", así::

    > svn checkout https://svn.aros.org/svn/aros/trunk/AROS

Esto creará un directorio llamado AROS y lo poblará con todos los fuentes,
lo que podría tomar bastante tiempo si tienes una conexión lenta a la red.
El módulo "contrib" contiene los programas de terceros que han sido transferidos
a AROS. Tienes que descarga para compilar todos los sabores de AROS::

    > cd AROS
    > svn checkout https://svn.aros.org/svn/aros/trunk/contrib

.. Consejo::
    Después de la descarga, SNV recordará de dónde vino el fuente.


Obtener los fuentes extras
==========================

Además de los fuentes principales de AROS que descargaste en la sección
previa, hay también otras cosas en el servidor SVN no relacionadas directamente
al núcleo del sistema operativo. Por ejemplo, el módulo "binaries" que 
contiene imágenes como capturas de pantalla, fondos de escritorio y cosas similares,
y el módulo "documentation" que contiene los fuentes del sitio web.

Puedes tener una lista de los módulos disponibles con el comando::

    > svn ls https://svn.aros.org/svn/aros/trunk/

Actualizar los fuentes
======================

Después de haber descargado los fuentes, podrías querer actualizarlos
periódicamente para tener los últimos cambios que los otros desarrolladores
han enviado. Para esto usa el comando "update"::

    > cd AROS
    > svn update
    
Esto mezclará cualquier cambio que los otros desarrolladores hicieron en tus
fuentes y también buscará los directorios y archivos nuevos que se han
agregados. Si alguien envió cambios a un archivo que tu has cambiado
localmente, SVN intentará mezclar los cambios automáticamente. Si ambos cambiaron
los mismos renglones SVN podría fallar en mezclar los fuentes. Cuando esto
sucede, SVN se quejará y pondrá **ambas** versionea en el archivo separadas
por un ``<<<<``. Necesitarás editar el archivo y resolver el conflicto a mano.

.. Warning:: 

    Porque SVN haya mezclado con éxito los cambios de los otros desarrolladores
    con los tuyos no significa que todo esté bien. SVN solamente se encarga del
    contenido *textual*; todavía podría haber conflictos lógicos después de la 
    mezcla (por ej. el otro desarrollador podría haber cambiado la semántica de
    la misma función que usaste en tus cambios). Deberías siempre inspeccionar
    los archivos que fueron mezclados y ver si todavía tienen sentido.


Enviar los cambios
==================

Si has hecho algunos cambios y sientes que quieres compartir tu trabajo con 
los otros desarrolladores, deberías usar el comando "commit"::

    > svn commit

Puedes especificar una lista de los archivos para enviar; de otra manera
SVN revisará recursivamente el directorio actual y hallará todos los archivos
que has cambiado y los enviará.
Antes de enviar tus cambios al servidor para su incorporación, SVN te pedirá
que ingreses un mensaje de registro. Este mensaje de registro debería 
contener una breve descripción de lo que has cambiado y en ciertos casos un
comentario para ellos. Los mensajes de registro bien escritos son muy importantes
porque pueden hacer mucho más fácil para los otros desarrolladores ver 
rápidamente lo que hiciste y quizás el por qué. Los mensajes de registro son
reunidos y después enviados en un mensaje de correo diario a la lista de 
correo de desarrollo para que todos puedan seguir a la par de los desarrollos
con el código base.

Antes de enviar tus cambios en un directorio, deberías hacer primero
una puesta al día ahí para ver si alguien más ha cambiado los archivos mientras
tú has estado trabajando en ellos. En caso de que eso suceda, necesitarás resolver
cualquier problema antes de hacer el envío. También asegúrate que has
probado tus cambios antes de enviarlos; al menos para que no rompan la compilación.


Agregar nuevos archivos y directorios
=====================================

Para agregar archivos y directorios nuevos al depósito, usa el comando "add"::

    > svn add file.c
    > svn add dir

SVN no revisará recursivamente de modo automático en los directorios agregados
recientemente y agregará los contenidos; tienes que hacerlo por tí mismo. 
Después de haber agregado el archivo, necesitas usar el comando "commit"
para de verdad agregarlos al depósito.

.. Note::

    No agregues los archivos generados (usualmente *mmakefile*, *strings.h*) al
    depósito. De otro modo estos archivos no serán actualizados cuando el 
    archivo fuente se haya cambiado.

    
Ajustar las propiedades
=======================

Los sistemas operativos difieren en los códigos que se usan para marcar el fin de
renglón. Para asegurarte que los archivos de texto que no son generados/modificados
en Linux tienen el correcto fin de renglón es necesario hacer::

    svn propset svn:eol-style native <source.c>

Subversion puede substituir las palabras clave especiales en los archivos fuente.
Para habilitar esto necesitas hacer::

    svn propset svn:keywords Author Date Id Revision <main.c>

.. Note::

    Puedes configurar Subversion para que ponga automáticamente las propiedades
    para ciertos tipos de archivo. Por favor consulta tu documentación de Subversion.


Importar
========

Cuando quieras agregar una gran colección de archivos, por ej. el código fuente
de algún software existente, "svn add" se vuelve rápidamente fastidioso. Para
esto deberías usar el comando "svn import". Desafortunadamente, la sección sobre
el comando import en el manual de SVN es muy pobremente escrito, dado el ejemplo:

1. Pon los archivos y los directorios que quieres importar en donde te guste,
   mientras **no** sea en tu directorio de trabajo. Ejecuta el comando
   "import" en el directorio situado dentro de una copia existente de SVN
   que funcione puede llevar a resultados muy extraños, así que es mejor
   evitarlo.
   
2. Cambia el directorio que contiene los archivos que deseas importar::

       > cd name-1.2.3

3. Importa los archivos con el comando "svn import"::

       > svn import -m <logMessage> <destinationPath>

   Esto importará recursivamente todos los archivos del directorio actual
   y de los internos en el depósito, en la ruta de destino y con el mensaje de 
   registro que has especificado. En realidad, no *todos* los archivos serán
   agregados: SVN ignorará los nombres de archivo que son comunes para respaldo
   y los archivos ocultos, como ``#?.bak``, ``.#?`` y ``#?~``.

   No obstante, deberías quitar todos los archivos que no quieres que 
   terminen en el depósito antes de que empiece la importación. No intentes
   interrumpir SVN durante el import cuando veas que un archivo está siendo
   agregado que no quiera. Sólo haz una nota y después borra el archivo.
   
   Por ejemplo, digamos que quieres importar los fuentes de SVN 1.1.3
   en el directorio "contrib/development/versioning/svn"::

      > cd subversion-1.1.3
      > svn import -m "Initial import of SVN 1.11.12" 
      \ https://svn.aros.org/svn/aros/trunk/contrib/development/versioning/svn

Lecturas adicionales
====================

Información más detallada sobre SVN se puede encontrar por supuesto en las páginas
del manual (man pages) y en los archivos distribuídos con el mismo SVN, y también
hay numerosos sitio que contienen tutoriales y guías útiles que podrían ser
más fáciles de leer.
Las siguientes páginas se recomiendan bastante:

+ `Version Control with Subversion`_
+ `Subversion Home`_

.. _`Version Control with Subversion`: http://svnbook.red-bean.com/
.. _`Subversion Home`:               http://subversion.tigris.org/


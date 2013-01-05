=============
PORTING GUIDE
=============

:Authors:   Johann Samuellson, Matthias Rustler
:Copyright: Copyright © 2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Beta
:ToDo:      Some real world examples


.. Contents::


Introductión
============

Esta guía es sobre la transferencia del software que viene con guionees 
configure o make a AROS.
Esto es verdad para la mayoría del software de fuente abierta del mundo Linux.
Se basa en un `documento`__ escrito por Johan Samuellson para el AmigaOS4.

__ http://www.os4depot.net/index.php?function=showfile&file=document/manual/spots-pfd.lha

¿Qué transferir?
----------------

* El lenguaje con mejor soporte es C. A C++ le faltan algunas características.

* Mira qué paquetes adicionales se necesitan para el software que quieres
  transferir. No puedes simplemente transferir cuando se necesitan bibliotecas
  de GUI como GTK o QT. Únicamente se puede transferir de modo directo el
  software que se ejecuta en modo consola.

* Cuando quieras trasferir juegos, mira si usan la `libSDL`__. Evita el
  software que requiera OpenGL.  
  
__ http://www.libsdl.org

Conseguir el SDK reciente
-------------------------

La manera más fácil de transferir para AROS es desde una caja Linux,
porque en general tiene todas las herramientas necesarias. Mira en la
`Guía de Desarrollo`__ para la información sobre la instalación del SDK.

__ app-dev/introduction.php#compiling-on-linux-with-gcc


Los fundamentos
---------------

Lo primero que debes hacer es revisar cómo compilar tu proyecto.
Si hay un archivo llamado *configure* en el directorio raíz del archive,
eso significa que tendrás que configurarlo para tu plataforma. Mira en la
descripción del siguiente capítulo sobre cómo hacer eso.

Luego de que el proyecto esté apropiadamente configurado es el momento
de hacer rodar al compilador.
Empieza a compilar tu proyecto tipeando ``make``.

Empero, si no hay un archivo llamado *configure*, busca uno llamado
*Makefile*. Edítalo para que se ajuste con AROS y compila el proyecto
con ``make``.

Si todo está compilado y enlazado ya estás listo para probar tu puerto.
Un problema muy común es que no encuentre sus archivos de datos, y se
cuelgue. Corrige las rutas como se describe en el capítulo corregir-las-rutas,
y ejecuta de nuevo. 

Luego quita la información de depuración de los binarios para hacerlos 
más pequeños.

¡Listo! Ya tienes el puerto. ¡Ahora cárgalo en http://archives.aros-exec.org
y en http://www.aminet.net!


Cómo configurar
===============

En Linux, los paquetes que tienen un guión configure por lo general se
instalan con tres pasos::

    ./configure
    make
    sudo make install

Lo que hace que la configuración de AROS sea más difícil que, por ej. para 
AmigaOS4 es el hecho que estamos usando un compilador cruzado.

Tipea ``./configure --help`` para ver las opciones que están disponibles.
Si falla la configuración podemos intentar deshabilitar algunas características.

Un ejemplo simple
-----------------

::

    ./configure CC=i386-aros-gcc \
    --build=local --host=i686-aros
    --disable-nls --without-x --without-pic --disable-shared

.. Hint::

    Tiene sentido escribir un comando de configure en un archivo de texto. De
    esta manera podemos fácilmente ejecutarlo de nuevo escribiendo ``sh build.sh``.
    Pero antes que ejecutemos de nuevo al guión tenemos que hacer ``rm -f config.cache``
    para volver al estado inicial el proceso de configuración.


Aquí está la explicación de las opciones que usamos arriba:

``CC=i386-aros-gcc``
    Queremos usar el compilador para AROS. De otro modo obtendremos un binario para Linux.

``--build=local --host=i686-aros``
    Indica al guión configure que haga una compilación cruzada para i686-aros.

``--disable-nls --without-x --without-pic --disable-shared``
    Deshabilita algunas características que no están soportadas por AROS.


.. Warning::

    No hagas ``sudo make install`` porque esto instalará tu aplicación en alguna
    ruta de Linux como */usr/local*.


Ejemplo de SDL (instalación manual)
-----------------------------------

El siguiente ejemplo es para `Ltris`__, pero debería ser similar para las otras
aplicaciones SDL::

    ./configure CC=i386-aros-gcc  LDFLAGS="-nix" \
    --prefix=/PROGDIR \
    --build=local --host=i686-aros \
    --disable-nls --without-x --without-pic --disable-shared \
    --with-sdl-prefix=/usr/local/aros-sdk/i386-aros

``LDLAGS="-nix"``
    This enables Linux semantics for paths. Linux applications often use absolute paths to
    their data files. But an absolute Linux path like */usr/local/app* means for AROS: go one
    level up, then go into the *usr* directory.

``--prefix=/PROGDIR``
    Prefixes all paths with */PROGDIR*. Together with the *-nix* option this allows to use
    Linux paths during compiling and AROS paths during running.

``--with-sdl-prefix=/usr/local/aros-sdk/i386-aros``
    The configure script calls *sdl_config* to find out the required CFLAGS and LDFLAGS for SDL
    applications. Without this option it would call the Linux version of *sdl_config*, which
    would give wrong results.

Después de una ejecución exitosa del guión configure y de *make* tenemos que
copiar el binario de ltris y los archivos de datos de modo que el binario
encuentre a éstos::

    ltris
        ltris (binary)
        share
	    ltris
	        gfx
		sounds


__ http://lgames.sourceforge.net/index.php?project=LTris


Ejemplo de SDL (instalación semiautomática)
-------------------------------------------

Podemos usar casi las mismas opciones de arriba::

    ./configure CC=i386-aros-gcc  LDFLAGS="-nix" \
    --prefix=/PROGDIR --bindir=/PROGDIR \
    --build=local --host=i686-aros \
    --disable-nls --without-x --without-pic --disable-shared \
    --with-sdl-prefix=/usr/local/aros-sdk/i386-aros

``--bindir=/PROGDIR`` 
    Evita se cree un subdiretorio *bin*.

No olvides la opción *--prefix* o sino instalará los archivos de AROS en alguna 
de las rutas de Linux.

Ahora puedes recompilar tu proyecto (``make distclean``, ejecuta el guión
configure, ``make``). Ahora podremos invocar ``sudo make install``. Pero es
mejor dar primero los siguientes pasos:

+ sudo mkdir /PROGDIR
+ sudo chmod a+rwx /PROGDIR

Esto tiene la ventaja que podemos acceder a */PROGDIR* sin los derechos de root.
Ahora finalmente podemos hacer ``make install``, que instalará el juego en 
*/PROGDIR*. Como éste no es un buen lugar tenemos que copiarlo a un lugar en
donde AROS pueda alcanzarlo (por ej. cp -r /PROGDIR ~/AROS/games/ltris).

Recuerda hacer ``rm -rf /PROGDIR/*`` antes de compilar otro proyecto.


Errores comunes que pueden ocurrir cuando configures
----------------------------------------------------

Obtengo errores como target or host i686-aros isn't available.

Solución:
  El guión *config.sub* probablemente sea viejo o no esté. Ejecuta ``autoconf``
  en el directorio raíz del paquete. Si eso no ayuda, copia una versión reciente
  del archivo *config.sub* de por ej. */usr/share/automake*.

.. _autoconf: http://ftp.gnu.org/gnu/autoconf/

Obtengo errores como éste:
  checking for IMG_Load in -lSDL_image... no

Solución:
  Estamos enlazando con bibliotecas estáticas, y necesitamos decir con
  exactitud con qué enlazar. Ubica este renglón en el guión configure::

    LIBS="-lSDL_image $LIBS"

  SDL_image depende de algunas bibliotecas para que funcione correctamente,
  agrégalas de este modo::

    LIBS="-lSDL_image -lpng -ljpeg -lz $LIBS"


Obtengo errores como éste:
  checking for Mix_OpenAudio in -lSDL_mixer... no

Solution:
  Estamos enlazando con bibliotecas estáticas, y necesitamos decir con
  exactitud con qué enlazar. Ubica este renglón en el guión configure::

    LIBS="-lSDL_mixer $LIBS"

  SDL_mixer depende de algunas bibliotecas para que funcione correctamente,
  agrégalas de este modo::

    LIBS="-lSDL_mixer -lvorbisfile -lvorbis -logg $LIBS"


Lo mismo sucede cuando configure está buscando SDL_ttf, y ahora sabes
por qué. Necesitamos especificar algunas bibliotecas más en las que
depende SDL_ttf. Necsita -lfreetype y -lz. Dejaré el resto para tí 
como un ejercicio.

Si has agregado las dependencias necesarias al guión configure, y 
todavía no funciona puede deberse a archivos faltantes en el SDK.
Por ej. no están incluidas las bibliotecas SDL.

This is not recommended, but if there is no other way...
Si todavía no funciona, y está seguro que tienes la biblioteca
instalada, prueba con quitar toda la sección donde revisa la
biblioteca que falla en el archivo configure.

Ahora deberías estar listo para build tu proyecto. Cuando transfieras
apps de unix siempre tipea ``make``.


Crear un makefile a mano
========================

Este makefile se podría usar is el build system es un mess y quieres 
simplificarlo un poco, alterarlo para que se ajuste a tus necesidades.
Lo usual es que solamente necesites modificar un makefile existente, cambia
el nombre del compilador de C (de otro modo crearía binarios para Linux) y
agrega algunas bibliotecas de enlazado.

Aquí está la explicación de lo que significan las banderas.
 
CC
    El nombre del ejecutable del compilador de C.

RM
    El nombre del comando delete.

STRIP
    The name of the strip command (used to remove debug data from exe files).

CFLAGS
    Tells the compiler where to find the includes (-I) etc.
    Indica al compilador dónde hallar los includes (-I) etc.

LDFLAGS
    Indica al enlazador qué bibliotecas incluir (-l) y dónde
    hallarlas (-L).

OBJS
    El compilador (GCC/G++) compila a archivos objeto (#?.o) de tus
    archivos .c que después son enlazados juntos para convertirse en
    un archivo ejecutable. Especifica aquí los nombres de los archivos
    objeto.

OUTPUT
    El nombre del archivo ejecutable final.

::

  CC      = i386-aros-gcc
  RM      = rm
  STRIP   = i386-aros-strip --strip-unneeded --remove-section .comment
  CFLAGS  = -Wall -O2 
  LDFLAGS = -nix -lsmpeg -lSDL_gfx -lSDL_net -lSDL_image -lpng -ljpeg -lz -lSDL_mixer \
            -lvorbisfile -lvorbis -logg -lSDL_ttf -lfreetype -lz -lsdl -lauto -lpthread -lm
  OBJS    = a.o b.o c.o
  OUTPUT  = test.exe

  all: $(OBJS)
          $(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(OUTPUT) 

  main.o: main.cpp main.h 
          $(CC) $(CFLAGS) -c main.cpp

  strip:
          $(STRIP) $(OUTPUT)
        
  clean:
          $(RM) -f $(OBJS) $(OUTPUT)

Recuerda que tienes que usar caracteres tabulador antes del comando.


Usar el build system
====================

El build system contiene algunos guiones para configurar los paquetes. La gran 
ventaja de usar el build system es que puedes transferir fácilmente a diferentes 
sabores de AROS.

+ %build_with_configure
+ %fetch_and_build
+ %fetch_and_build_gnu_development

Mira en el archivo $(TOP/config/make.tmpl para una explicación de los argumentos. En
*$(TOP)/contrib/gnu* ahora puedes hallar muchos paquetes GNU.


Misc
====

Convertir las rutas unix a rutas AROS
-------------------------------------

¿Cómo convierto las rutas Unix en rutas AROS? Intercambia *getenv("HOME")* por *"/PROGDIR/"*

Ejemplos::

    old: strcpy(path, getenv("HOME"));
    new: strcpy(path, "/PROGDIR/");

    old: strcpy(home,getenv("HOME"));
    new: strcpy(home,"/PROGDIR/");

    old: sprintf(rc_dir, "%s/.gngeo/romrc.d", getenv("HOME"));
    new: sprintf(rc_dir, "%sgngeo/romrc.d", "/PROGDIR/");

Fíjate que quité "/." en el último ejemplo.

Las rutas a los directorios de los datos a menudo se establecen durante 
el proceso de configure haciendo *-DDATADIR=*. Si éste es el caso establece a
*-DDATADIR=/PROGDIR/*. También es común que el directorio de los datos 
se establezca en los makefiles. Ubica *DATADIR=* y cámbialo a *DDATADIR=/PROGDIR/*


Defines
-------

Los defines se establecen en general en *config.h*, si algo está mal configurado,
puedes cambiarlo ahí usando *#define* y *#undef*.

Un ejemplo de define que considera todos los sabores de AmigaOS::

  #ifdef __AMIGA__
	  blah blah blah
  #else 
          blah blah blah
  #endif

Un ejemplo de define que sólo considera a AROS::

  #ifdef __AROS__
	  blah blah blah
  #else 
          blah blah blah
  #endif

Un ejemplo de define que considera a BeOS y a AROS::

  #if !defined(__BEOS__) && !defined(__AROS__)

Un ejemplo más complejo::

  #ifdef GP2X
	char *gngeo_dir="save/";
  #elif defined __AROS__
	char *gngeo_dir="/PROGDIR/save/";
  #else
	char *gngeo_dir=get_gngeo_dir();
  #endif

Algunos paquetes de fuente abierta ya han adoptado a los sistemas operativos
de la familia Amiga. Si encuentras algo como *#ifdef __AMIGA* en el fuente
puedes probar agregar el define a las opciones de configuración (por ej.
CFLAGS="-nix -D__AMIGA__").


Entender los mensajes de error
------------------------------

Error: No return statement in function returning non-void 
  No hay un *return* en una función que necesita uno.

Error: Control reaches end of non-void function
  Se llegó al final de una función que necesita devolver un valor, pero no hay ningun *return*.

Error: May be used uninitialized in this function
  La variable no está inicializada.

Warning: implicit declaration of function 'blah blah'
  Necesitas incluir una cabecera.


Errores comunes
---------------

warning: incompatible implicit declaration of built-in function 'exit';
warning: incompatible implicit declaration of built-in function 'abort'::
  
    solución: #include <stdlib.h>

warning: implicit declaration of function 'strlen';
warning: incompatible implicit declaration of built-in function 'strlen'::

    solución: #include <string.h>

warning: implicit declaration of function 'memcpy';
warning: incompatible implicit declaration of built-in function 'memcpy'::

    solución: #include <string.h>

error: memory.h: No such file or directory::

    solución: #include <string.h>

error: malloc.h: No such file or directory::

    solución: #include <stdlib.h>

warning: incompatible implicit declaration of built-in function 'printf'::

    solución: #include <stdio.h>

warning: implicit declaration of function 'MyRemove'::

    solución: #define MyRemove Remove


Consejos y trucos
-----------------

¿Cómo hago una búsqueda de cadenas de texto usando GREP?

::

  grep -R "I am looking for this" *

¿Cómo hago un archivo DIFF con mis cambios?

::

  diff originalfile.c mychangedfile.c >./originalfile.patch

Mi ejecutable se cuelga, ¿cómo lo depuro?
  Mira en el `Manual para la Depuración <debugging>`_. Puedes usar sys:utilites/snoopy
  para descubrir qué intenta hacer tu aplicación.

¿Cómo dirigo las advertencias y errores de GCC a un archivo de texto?

::

  make 2>warnings.txt


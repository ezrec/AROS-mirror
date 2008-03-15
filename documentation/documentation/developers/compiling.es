==================
Cómo compilar AROS
==================

:Authors:   + Flavio Stanchina
            + Henning Kiel
            + Bernardo Innocenti
            + Lennard voor den Dag
            + Aaron Digulla
            + Adam Chodorowski
:Copyright: Copyright ² 2001-2003, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.
:Abstract:  
    Este documento explicará cómo compilar AROS. En la actualidad el
    desarrollo de AROS no se aloja en sí mismo, o sea que
    no puedes compilar AROS desde dentro de AROS. Para compilar y 
    desarrollar para AROS necesitarás un systema Linux o FreeBSD.


.. Contents::


Requerimientos
==============

Se necesita el siguiente software para compilar AROS:

+ GCC 3.2.2+
+ GNU Binutils
+ GNU Make
+ GNU AWK (GAWK) - other awks may also be suitable
+ Python 2.2.1+
+ Bison
+ Flex
+ pngtopnm and ppmtoilbm (part of the netpbm package)
+ Autoconf
+ Automake
+ Los utilidades comunes como cp, mv, sort, uniq, head...

Si quieres compilar los puertos hosted i386-linux o i386-freebsd,
necesitarás también:

+ las cabeceras y las bibliotecas de desarrollo de X11.

El código fuente
================

Puedes descargar el código fuente de AROS de la `download page`__
o usando SVN (aunque requiere que tu `apply for acces`__). En el primer
caso, es suficiente tener el paquete ``source`` (a menos que también
quieras compilar los programas contribuídos). En el último caso, mira
en la `subversion documentation`__.

__ ../../download
__ ../../documentation/developers/contribute#the-svn-repository
__ ../../documentation/developers/svn


Building
========

Configuración
-------------

Antes que nada, necesitas ejecutar configure en la raíz del código fuente
de AROS::

    > cd AROS
    > ./configure

Puedes especificar varias opciones a configure. Están disponibles las
siguientes opciones para todos los targets:

``--enable-debug=LIST [default: none]`` 
    Habilita los diferentes tipos de depuración. Se pueden usar comas o
    espacios para separar los elementos de la lista. Si no se proporciona
    una lista entonces se supone ``all``. Si no se especifica ``--enable-debug``
    por omisión es ``none``. Los tipos disponibles son:
    
    ``none``
        Deshabilita todos los tipos de depuración y la depuración misma.
    
    ``all``
        Habilita todos los tipos de depuración siguientes.
        
    ``stack``
        Habilita la depuración de la pila.
        
    ``mungwall``
        Habilita la depuración de mungwall.
        
    ``modules``
        Habilita la depuración de los módulos.
    

Hosted AROS/i386-linux or AROS/i386-freebsd
"""""""""""""""""""""""""""""""""""""""""""

No tienes que especificar la opción ``--target`` para compilar estos
targets. Están disponibles las siguientes opciones para los
compilados hosted:

``--with-resolution=WIDTHxHEIGHTxDEPTH [default: 800x600x8]``
    Set the default resolution and depth AROS will use. 
    Establece la resolución y la profundidad que AROS usará por omisión 
    
``--enable-xshm-extension [default: enabled]``
    Habilita el uso de la extensión MIT-SHM de X11. Da una ventaja
    de rendimiento significativa, pero podría no funcionar bien si estás
    usando una pantalla remota.
    
No puedes hacer la compilación cruzada de estos puertos.


Native AROS/i386-pc
"""""""""""""""""""

Para compilar el puerto i386-pc, necesitas pasar ``--target=pc-i386``
a configure. Además, están disponibles las siguientes opciones específicas
para i386-pc:

``--with-serial-debug=N [default: disabled]``
    Habilita la depuración en serie, enviando la salida al puerto ``N``.
    
No puedes hacer la compilación cruzada de este puerto.


La compilación
--------------

Para empezar la compilación, simplemente ejecuta::

    > make

Si esto no funciona después de una actualización con SVN,
prueba::

    > make clean
    > rm -rf bin/
    > ./configure {options}
    > make

Si usas FreeBSD o algún otro sistema que no use GNU Make como el make del
sistema, entonces deberías substituírlo con el comando GNU Make.
Por ejemplo, bajo FreeBSD deberías instalar el puerto de GNU Make, 
después ejecuta::

    > gmake


Hosted AROS/i386-linux or AROS/i386-freebsd
"""""""""""""""""""""""""""""""""""""""""""

Si estás compilando un hosted i386-linux o i386-freebsd, deberías
ejecutar también lo siguiente para configurar apropiadamente el soporte
del teclado::

    > make default-x11keymaptable


Native AROS/i386-pc
"""""""""""""""""""

Si estás compilando el puerto nativo i386-pc, hallarás una imagen
de un disquete de arranque en ``bin/pc-i386/gen/rom/boot/aros.bin`` 
después de que la compilación haya terminado. Adicionalmente puedes
crear una imagen ISO de arranque ejecutando::

    > make bootiso-pc-i386

La imagen ISO se encuentrará en ``bin/pc-i386/gen/rom/boot/aros.iso``.

Apéndice
========

Compilar varios targets de los mismos fuentes
----------------------------------------------
   
Si pretendes compilar varios targets diferentes de un mismo árbol
de fuentes, primero tienes que ir por el paso de configuración para 
cada uno de tus targets.
Puedes agregar targets en cualquier momento que quieras. El último
target especificado para configure es el target por omisión.

Para seleccionar un target específico cuando compiles,
simplemente ejecuta make así::

    > AROS_TARGET_ARCH=$ARCH AROS_TARGET_CPU=$CPU make
    
Donde ``$ARCH`` es la arquitectura del compilado buscado, y ``$CPU`` es
la CPU. Por ej., para compilar AROS/i386-pc ejecutarías::

    > AROS_TARGET_ARCH=pc AROS_TARGET_CPU=i386 make

Si estás compilando varios puertos que usan la misma CPU,
solamente tienes que especificar ``AROS_TARGET_ARCH`` ya que la CPU
quedará igual.

Después que hayas hecho "make" una vez, y algunos cambios sean hechos
a los archivos, puedes usar la compilación rápida de los cambios usando
el comando make <target-name>-quick.

Compilar los Cómo... (HowTo)
----------------------------

Esta guía paso a paso describirá cómo preparar el ambiente de desarrollo
y compilar AROS en Ubuntu Linux 6.10 "Edgy Eft". Supongamos que tengas una
imagen de CD (iso) de los sitios de Ubuntu y hayas instalado el sistema.
También deberías ajustarlo para hacer que acceda a Internet.
      
Conseguir los paquetes necesarios
"""""""""""""""""""""""""""""""""

Puesto que al Live CD le faltan los paquetes necesarios tenemos que
conseguirlos de Internet.

    > sudo apt-get install subversion gcc-3.4 gawk bison flex netpbm autoconf automake1.4 libx11-dev

Necesitarás ingresar tu contraseña de usuario en el prompt.

Configurar el locale a ISO8859
""""""""""""""""""""""""""""""

Necesitaremos establecer el locale para usar los fuentes de AROS
y compilarlos.
Sólo encuentra la cadena en_US iso 8859-1 en la lista proporcionada
por las siguientes aplicaciones y escógela)::

     > sudo apt-get install localeconf
     > sudo dpkg-reconfigure localeconf

Luego estableceremos el locale de la consola:

     > sudo locale-gen "en_US"
     > sudo dpkg-reconfigure locales
     > export LANG="en_US.ISO-8859-1"

Instalar make v. 3.80
"""""""""""""""""""""

Para instalar la versión de make que necesitaremos, se deben
agregar los depósitos adicionales de Ubuntu. Lancemos la 
consola y ejecutemos::

     > sudo nano /etc/apt/sources.list

Agrega estos dos renglones ahí::

    deb http://us.archive.ubuntu.com/ubuntu breezy main restricted
    deb http://us.archive.ubuntu.com/ubuntu dapper main restricted
    (save and exit nano via "ctrl-x")
    (guarda y sal de nano con "ctrl-x")

Ahora obtén la lista de los programas disponibles actualizados::

     > sudo apt-get update

Ahora usaremos el administrador de paquetes Synaptic. Vé al menú::

    System > Administration > Synaptic package manager

Después busca el paquete "make", escoge "make" en la ventana
derecha y establece la versión con ''package>force version..'' "3.80 (breezy)".



Obtener los fuentes
"""""""""""""""""""

Para encontrar más instrucciones sobre Cómo usar nuestro Depósito SVN
por favor refiérete a `Working with Subversion <svn.php>`__

Para ser breve, los comandos que deberás usar son los siguientes::

   > svn checkout https://svn.aros.org:8080/svn/aros/trunk/AROS
   > cd AROS
   > svn checkout https://svn.aros.org:8080/svn/aros/trunk/contrib


Configurar y compilar los fuentes de AROS
"""""""""""""""""""""""""""""""""""""""""

Primero estableceremos los parámetros y configuraremos::

      > export CC="gcc-3.4"
      > ./configure

Podrías necesitar abrir de nuevo la consola cuando ./configure
encuentre problemas con el compilador.

Por último, tipea::

      > make

Esto tomará un tiempo (hasta algunas horas en las
máquinas lentas) :)
Después de eso tendrás compilado AROS hosted.
Por favor fíjate que podrías necesitar un acceso a Internet durante el 
progreso, para descargar los más recientes fuentes de EUAE.


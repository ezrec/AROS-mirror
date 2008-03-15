=======
Puertos
=======

:Authors:   Adam Chodorowski, Matthias Rustler 
:Copyright: Copyright © 1995-2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Done.

.. Contents::


Introducción
============

Puesto que AROS es un sistema operativo portátil, está disponible para
varias diferentes plataformas. Un "puerto" de AROS es exactamente lo que
la palabra implica, una versión de AROS adaptada a alguna plataforma 
específica.


Sabores
-------

Los puertos están divididos en dos grupos mayores, o "sabores" en la 
terminología de AROS, denominados "nativo" y "alojado".

Los puertos nativos se ejecutan directamente sobre el hardware y tienen
un control total de la computadora. En el futuro éstos se convertirán en
la manera recomendada para ejecutar AROS ya que tiene un rendimiento y 
eficiencia superior, pero hoy están demasiado incompletos para ser 
útiles (al menos para desarrollar).

Los puertos alojados funcionan encima de otro sistema operativo y no 
tienen acceso directo al hardware, pero usan las capacidades 
proporcionadas por el OS anfitrión. Las ventajas de los puertos alojados 
es que son más fáciles de escribir porque no es necesario escribir los 
controladores de bajo nivel. También, ya que el desarrollo de AROS 
todavía no es efectuado en sí mismo (no podemos compilar AROS desde 
dentro de AROS) eso acelera mucho la programación ya que podemos 
ejecutar el ambiente de desarrollo y AROS uno al lado del otro sin
perder el tiempo en los arranques seguidos para probar el código nuevo.


Nomenclatura
------------

Los diferentes puertos AROS se nombran de la forma <cpu>-<plataforma>, 
donde <cpu> es la arquitectura de CPU y <plataforma> es un nombre 
simbólico para la plataforma. La plataforma de un puerto puede ser el 
hardware para un puerto nativo, por ejemplo, "pc" o "amiga", o un 
sistema operativo para los puertos alojados, como "linux" o "freebsd". 
En los casos que no es obvio que el tema es AROS es común poner el 
prefijo "AROS/" al nombre del puerto, resultado en por ej."AROS/i386-pc".


Portabilidad
------------

Los ejecutables de AROS para una CPU específica son portátiles a través
de todos los puertos que usen la misma CPU, es decir, que los ejecutables 
compilados para "i386-pc" funcionarán bien sobre "i386-linux" y 
"i386-freebsd".


Puertos existentes
==================

Abajo está la lista de todos los puertos de AROS que están funcionando 
en orden y/o desarrollados activamente. No todos están disponibles para 
descargar, ya que podrían no estar suficientemente completos o tener 
requerimientos de compilación que no podemos siempre cumplir debido a 
los limitados recursos.


AROS/i386-pc
------------

:Flavour:    Nativo
:Status:     Funcionando, soporte incompleto de los controladores
:Manteined:  Sí

AROS/i386-pc es el puerto nativo de AROS para las computadoras comunes 
IBM PC AT y compatibles que usen los procesadores de la familia x86. 
El nombre en verdad es un poco errado porque AROS/i386-pc necesita al 
menos una CPU de clase 486 debido al uso de algunas instrucciones que 
no están en el 386.

Este puerto funciona bastante bien, pero nada más tenemos soportados a 
los controladores más básicos. Una de las limitaciones más grandes es 
que ahora sólo tenemos soporte para la aceleración por hardware de las 
tarjetas de nVidia y de ATI. Los demás adaptadores gráficos deben ser 
usados con los controladores VGA (no acelerado) y VBE. Hay más 
controladores en camino, pero el desarrollo es bastante lento porque 
solamente tenemos 2,5 hackers de hardware. Este puerto está disponible 
para bajar.



AROS/m68k-pp
------------

:Flavour:    Nativo 
:Status:     Funciona en parte (en un emulador), soporte incompleto de 
             los controladores
:Maintained: Sí

AROS/m68k-pp es el puerto nativo de AROS para la línea Palm de 
computadoras de mano y compatibles ("pp" significa "palm pilot", que fue 
el nombre del primer dispositivo de esta línea). Esto significa que 
en el futuro podrás ser capaz de llevar a AROS contigo en tu bolsillo...

Este puerto es actualmente muy crudo. En su mayoría funciona (ejecutándose 
en un emulador todavía ya que nadie quiere arriesgarse a inutilizar su caro 
hardware) pero aún queda bastante por hacer. Hay un controlador 
gráfico, pero ninguna entrada de datos. Este puerto no está disponible 
para descargar en este momento.


AROS/i386-linux
---------------

:Flavour:    Alojado
:Status:     Funcionando
:Maintained: Sí

AROS/i386-linux es el puerto alojado de AROS en el sistema opertivo Linux [#]_
ejecutándose en los procesadores de la familia x86.

Éste es el más completo de los puertos de AROS en características, 
puesto que la mayoría de los desarrolladores usan Linux cuando 
desarrolla AROS, y hay bastante menos controladores que escribir. 
Este puerto está disponible para descargar.


AROS/i386-freebsd
-----------------

:Flavour:    Alojado
:Status:     Funcionando
:Maintained: Sí (5.x)

AROS/i386-freebsd es el puerto alojado de AROS en el sistema operativo 
FreeBSD ejecutándose en los procesadroes de la familia x86.

Este puerto está relativamente completo porque comparte la mayoría de su 
código con el de AROS/i386-linux, pero ya que no hay tantos desarrolladores 
que usen FreeBSD es ###lagging un poco atrás. Nosotros intentamos 
compilar AROS/i386-freebsd cuando hacemos las instantáneas, pero no 
siempre es posible, así que podría ser que no siempre esté disponible 
para bajar.


AROS/ppc-linux
---------------

:Flavour:    Alojado
:Status:     Funcionando
:Maintained: Sí

AROS/ppc-linux es el puerto alojado de AROS para el sistema operativo 
Linux ejecutándose en la familia de procesadores PPC.

Una versión precompilada se puede descargar de `Sourceforge`__.
El rebuilding requiere un gcc3.4.3 parchado. El archivo diff se puede 
hallar en contrib/gnu/gcc.

__ http://sourceforge.net/project/showfiles.php?group_id=43586&package_id=194077


Notas al pie
============

.. [#] Sí, sabemos que Linux es realmente sólo un núcleo y un OS entero,
       pero es mucho más breve de escribir que "los sistemas operativos 
       basados en el núcleo Linux, algunas de las herramientas comunes 
       GNU y el sistema de ventanas X". Esta optimización de tamaño por 
       supuesto que es negada al tener que escribir esta explicación 
       para los lectores pedantes, ...


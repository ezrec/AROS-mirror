====================================
Manual para la Documentación de AROS
====================================

:Authors:   Adam Chodorowski, Matthias Rustler 
:Copyright: Copyright Š 1995-2006, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Work in progress.

.. Warning::

   ¡Este documento no está terminado! Es muy probable que muchas partes
   estén anticuadas, contengan información incorrecta o simplemente
   falten. Si quieres ayudar a rectificar esto, por favor contáctanos.

Este manual describe el sistema de documentación de AROS. Está dirigido
a los escritores de la documentación como también a los
desarrolladores que deseen extender o enredarse con el sistema en sí.
Con este fin, el manual está separado en tres partes: una 
introducción común, el modo de escribir la documentación y 
por último las notas técnicas y las especificaciones. Además de
informar sobre el uso del sistema, el segundo capítulo también contiene
algunas directrices generales a seguir cuando se escriba la 
documentación de AROS.


.. Contents::


Introducción
============

La documentación es vital para cualquier proyecto, al hacerlo *útil*. Lo
usual, en un proyecto con tan limitados esfuerzos de desarrollador como
el nuestro, es que la documentación esté considerablemente demorada o ausente.
Además, la mayoría de los programadores no son tan buenos escribiendo 
documentos, así que lo que ya se escribió en general no es legible. Se 
necesitan combinar los esfuerzos de los desarrolladores y de los usuarios
para que la documentación llegue a la meta de un "AROS Ultimate
Documentary". Así que si sabes de algunos usuarios que puedan ayudarnos, 
¡sólo permíteles! =)


*FIXME: ¿Escribir algo más?*


Escribir la documentación
=========================

El formato de la documentación que usamos es ReStructuredText (o para
abreviar ReST) como está especificado en el `Docutils Project`__.
ReST es una sintaxis de marcas de texto plano, extensible, fácil
de leer y de escribir y del tipo "obtienes lo que ves".

Puede describirse como un híbrido entre una sintaxis de marcas 
implícita y una de marcas explícita, que lo hace fácil de aprender 
y muy legible, y a la vez poderoso y extensible. La `Introduction to
reStructuredText`__ contiene algunos buenos puntos sobre las metas
que tiene la sintaxis.

Dado que el formato es básicamente texto plano es muy fácil de aprender
simplemente inspeccionando la documentación existente y usando el sentido
común, no obstante se recomienda al menos una ligera lectura del
`ReStructuredText Primer`__ antes de empezar a trabajar en la documentación
de AROS.

Para más información sobre el formato, se recomienda leer los siguientes
documentos:

+ `Quick reStructuredText Reference`__

__ http://docutils.sourceforge.net/
__ http://docutils.sourceforge.net/docs/ref/rst/introduction.html
__ http://docutils.sourceforge.net/docs/user/rst/quickstart.html
__ http://docutils.sourceforge.net/docs/user/rst/quickref.html

El archivo de la documentación
------------------------------

Para cambiar o escribir la documentación necesitas descargar del
`subversion archive <svn>`_ 'documentation'. El archivo contiene:

+ algunos documentos comunes como contact.en, links.en, etc.
+ un directorio 'documentation' con la documentación del usuario y del
  desarrollador
+ un directorio 'scripts' con los guiones usados para compilar los
  targets HTML y WWW
+ ...

.. Note::

    El archivo de subversion y el directorio con los manuales del usuario
    y del desarrollador tienen el nombre 'documentation'. No los mezcles.

Los subdirectorios
------------------

Puedes crear directorios adicionales en 'documentation/users' y en
'documentation/developers'. El build system explora recursivamente
los subdirectorios. Se recomienda que crees un archivo 'index.en' en
los nuevos directorios.

La internacionalización
-----------------------

El build system soporta I18n para el target 'www'. Tienes que
agregar un sufijo de idioma al nombre del archivo del documento (por ej.
commands.en).
Actualmente hay soporte para el inglés (.en), alemán (.de), finés (.fi),
italiano (.it), ruso (.ru), sueco (.sv) y holandés (.nl). Si deseas 
traducir la documentación a más idiomas entonces por favor ponte en
contacto con nosotros a través de la `website mailing list`__.

Cuando creas un enlace a un documento tienes que omitir el sufijo
(por ej. \`Commands <user/commands>\`_). Pero si usas el directorio
'include' para los documentos traducidos debes dejar el sufijo.

El build system usa la versión en inglés cuando no está disponible
la traducción de una página.

__ http://aros.sourceforge.net/contact.php#mailing-lists

El código de muestra
--------------------

El directorio 'documentation/developers/samplecode' está para los ejemplos
en código fuente. El contenido se copia sin modificar a los targets.
           
Las imágenes
------------

Los nombres y las rutas de las imágenes están hard-coded en el guión
de python 'buildit.py'. Si quieres agregar imágenes tienes que cambiar
el guión. Siéntete en libertad de hacer un pedido
de característica en la lista de correo del desarrollador si quieres que
esto se cambie.

Envío
-----

Antes que envíes los documentos nuevos o modificados deberías 
`build`__ ambos targets HTML y WWW localmente. Busca los mensajes de
error y repáralos. Comprueba el resultado en un navegador web. Lo normal
es que los cambios sean aplicados a http://aros.sourceforge.net/ algunas horas
después de su envío.

__ building_

 
Notas técnicas
==============

Las bases de datos
------------------

Hay varias pequeñas bases de datos, que puedes encontrar en el directorio
``db``, que están en texto plano pero usan un par de formatos ligeramente
diferentes. Principalmente, esto es un artefacto histórico y quizás debería
ser rectificado en el futuro, pero por ahora es más fácil mantenerlos
tal como están.


credits
~~~~~~~

Esto es una lista de las personas que han contribuído con AROS. El
build system crea el archivo 'credits.en'. Este archivo se usa en
ambos targets WWW y HTML. Por favor fíjate que también deberías
cambiar el archivo workbench/system/AboutAROS/db/credits en el depósito
de AROS.


quotes
~~~~~~

Las citas notables de las celebridades de AROS. El formato es
dicho;autor. El build system copia este archivo al target HTML.


mirrors
~~~~~~~

Puesto que AROS está alojado en Sourceforge no hay servidores 
espejo. Este archivo es ignorado por el build system.


tasks
~~~~~

Es la base de datos TODO (Para Hacer). Este archivo no ha sido mantenido
por un largo tiempo y no es usado para los targets.


Sizes
~~~~~
Los archivos ``aros.size`` y ``contrib.size`` no se usan más y es 
probable que serán borrados.


Building
--------

Requerimientos
~~~~~~~~~~~~~~

Necesitarás lo siguiente para compilar la documentación de AROS 
desde cero:

+ Python__ 2.2.1

Para construir el sitio web necesitarás adicionalmente:

+ The `Python Imaging Library`__ 1.1.3

MacOS X viene con Python pre-instalado, pero para compilar el
sitio web necesitarás:

+ The `MacPorts`__ package 

+ The `MacPorts Python Imaging Library`__ 1.1.3

__ http://www.python.org/
__ http://www.pythonware.com/products/pil/
__ https://www.macports.org/
__ https://trac.macports.org/browser/trunk/dports/python/py-pil/

Setting up
~~~~~~~~~~

Antes de que empieces a compilar la documentación, podría ser necesario
algún ajuste si la versión necesaria del interpretador Python
no se llama "python" en tu sistema. Si no es así, simplemente puedes 
saltar esta sección.

Primero, copia los ajustes por omisión::

    % cd AROS/documentation/scripts/config
    % cp defaults user

Después, edita el archivo ``user`` y asegúrate que las variables son
correctas para tu sistema. Un archivo de configuración de ejemplo 
podría verse algo así::

    PYTHON=python

``PYTHON`` indica el nombre del interpretador de Python; podría también
ser una ruta absoluta si no lo tienes en la ruta de búsqueda. Podrías 
necesitar establecerlo a ``python2`` o ``python2.2`` en algunos sistemas.


Targets
~~~~~~~

En este momento hay dos targets soportados:

+ ``www`` 
  
  Este target genera el sitio web de AROS que incluye toda la 
  documentación, las novedades-noticias, las galerías de fotografías
  con miniaturas y la información del estado. La documentación
  se integrará satisfactoriamente con el resto del sitio.

+ ``html``
  
  Este target genera el HTML separado de la documentación, adecuado
  para la visión offline y la inclusión en los paquetes de documentación.
  
  
Procedimiento
~~~~~~~~~~~~~

Para compilar un target específico, simplemente invoca el guión de compilación
con el nombre del target como primer argumento. El directorio actual
necesita ser la raíz del árbol de la documentación. Por ejemplo, para
compilar el sitio web haz::

    > cd AROS/documentation
    > ./build www

Si quieres compilar la documentación HTML separada::

    > cd AROS/documentation
    > ./build html

Consejo: agregar un sufijo de idioma (por ej. en, du o it) después del nombre
del target dará solamente las páginas para ese lenguaje. Todas las
páginas faltantes o no traducidas serán substituídas por sus contrapartes
en inglés. Esto resulta en tiempos de compilación enormemente reducidos. ::

    > cd AROS/documentation
    > ./build www du


Los archivos generados serán puesto en ``../bin/documentation/<target-name>``,
por ej. ``../bin/documentation/www`` para el target ``www``. Los
archivos específicos de un lenguajes son puesto en ``../bin/documentation/<target-name>/<language-name>``.

Adicionalmente, hay un target ``clean``, que borrará entero el 
directorio ``../bin/documentation``.

.. Note::

    Actualmente hay muchos archivos intermedios generados en
    los directorios fuente, así que no puedes ponerlos en un sistema de
    archivo de solo-lectura. Si haz descargado los fuentes desde SVN
    no necesitas preocuparte: los archivos generados son ignorados según
    sea apropiado.

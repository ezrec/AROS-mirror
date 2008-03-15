=============
Status Update
=============

:Author:   Paolo Besser
:Date:     2007-11-15

Últimas noticias
----------------

AROS tuvo bastantes correcciones y mejoras en las recientes semanas.
Por ejemplo, Neil Cafferkey corrigió algunos importantes fallos en su
amado `AROS Installer`__; Nic Andrews trabajó en su controlador de 
red RTL8139; y Robert Norris arregló las notificaciones de archivo,
que antes rompían las preferencias; sólo para nombrar tres. 

Robert Norris añadió un controlador SDL para el AROS alojado en Linux.
Esto te permite armar un AROS alojado que no necesite de X (incluso ya 
no tienes que instalarlo para armar AROS). En teoría esto podría ayudar
para tener la versión alojada funcionando en otras plataforma (donde 
exista SDL), aunque es un poco más lento que el controlador de X.

Matthias Rustler pasó la ptplay.library a AROS. Esta biblioteca reproduce
los módulos de Protracker como muestras de sonido. También pasó a
ShellPlayer, que es un reproductor de ejemplo. Ambos traspasos estarán
en las nightly builds, en el cajón Extras/Multimedia/Audio.

Matthias tambíen acaba de hacer un puerto preliminar del `Wazp3D`__
de Alain Thellier para AROS. Se trata de una biblioteca pensada 
para ser compatible con la famosa Warp3D.library de AmigaOS 68040,
y hace más fácil pasar algunos juegos tridimensionales de Amiga a
AROS. Wazp3D también puede funcionar como un renderer por software,
engañando a las aplicaciones que buscan un controlador de hardware 3D.

Michal Schulz dió otros grandes pasos con su puerto `x86-64`__ de AROS.
Se acerca el día de AROS 64 bits. Mientras tanto, Michal agregó
el soporte a las instrucciones SSE.

Petr Novak tradujo `aros.org al checo`__.


__ http://aros-exec.org/modules/newbb/viewtopic.php?topic_id=2319
__ http://ftp.ticklers.org/pub/aminet/driver/video/Wazp3D.readme
__ http://msaros.blogspot.com/2007/10/very-close.html
__ http://www.aros.org/cs



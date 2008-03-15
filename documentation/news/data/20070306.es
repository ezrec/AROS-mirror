=============
Status Update
=============

:Author:   Paolo Besser
:Date:     2007-03-06

¡Habemus DOSpackets!
--------------------

Robert Norris hizo el envío preliminar del sistema de paquetes de
AROS a nuestro depósito, además de un manejador del sistema de archivos
FAT. Aquí está lo que dijo:

"packet.handler es un sistema de archivos "proxy" de estilo AROS que tiene
la responsabilidad principal de tomar FSA/IOFileSys, convertirlos a DosPackets
y enviarlos al manejador basado en paquetes.

Lo importante a resaltar del packet.handler es que está diseñado para imitar
el ambiente de manejadores del AmigaOS tanto como es posible, a fin de 
disminuir la cantidad del trabajo necesario para transferir un sistema de archivos.

fat.handler es un manejador FAT12/16/32. Es de solo-lectura, y aún tiene algunos
errores, pero demuestra el concepto. Queda bastante trabajo por hacer antes de 
que considere terminada la recompensa. Los comentarios son bienvenidos."

El soporte de DOSPackets y el solo-lectura de FAT debería estar disponible en las
venideras nightly builds.

Otras noticias
--------------

Pavel Fedin añadió el soporte de VESA v1.2, permitiendo que más (y viejas)
placas de video sean compatibles con AROS. También agregó el cambio del
modo VESA al bootstrap, haciendo al hidd menos dependiente del GRUB arreglado.

Neil Cafferkey reparó el soporte de 64 bits del disco. Ahora la partición
de arranque puede tener más de 4GB de espacio.

Hogne Titlestad presentó la versión 0.3.2 de su asombroso programa de pintura
`Lunapaint`__, que tiene una herramienta de texto. `Aquí`__ puedes verlo en una nueva
captura de pantalla.

El `KGB Archiver`__ de Tomasz Pawlak ha sido pasado a AROS por FOrest. Lo hallarás
en los `Archives`__. Advierte que todavía no ha tenido mucha prueba.

__ http://www.sub-ether.org/lunapaint/
__ http://www.sub-ether.org/lunapaint/upload/032.jpg
__ http://www.aros.ppa.pl/images/photoalbum/48.jpg
__ http://archives.aros-exec.org

===================
Cuadro de Situación
===================

:Autor:   Paolo Besser
:Fecha:   2008-04-12

Distribuciones
--------------

Recientemente han sido lanzadas dos nuevas distribuciones de AROS:
VmwAROS LIVE! y una nueva versión de WinAROS. La primera es un entorno
preconfigurado (de arranque o instalable) basado en una máquina virtual
VmwAROS; la otra es una máquina virtual completa adaptada a QEMU,
con IDE y entorno de desarrollo actualizado. Más detalles en nuestra
`página de descargas`__. Se invita a usuarios y desarrolladores de aplicaciones
a descargarlos.

Últimas noticias
----------------

Krysztof Smiechowicz y Alain Greppin han provisto un paquete gcc/g++ 3.3.1
de código ejecutable específico para i386 - basado en los parches de Fabio,
que puede descargarse desde nuestros Archivos. Obviamente, son buenas
noticias para aquellos interesados en desarrollar o adaptar software a AROS,
y no son las únicas: en los Archivos también se encuentra la nueva versión
del Murks!IDE con respaldo para C++, el mejor Entorno de Desarrollo Integrado
de AROS, provisto por Krysztof Smiechowicz y Heinz-Raphael Reinke.

También es hora de hacer grandes correcciones. Krysztof Smiechowicz ha
comenzado a revisar la completitud de las API, mientras Barry Nelson
revisaba, filtraba y comenzaba a administrar nuestro rastreador de errores.
Muchos de los errores ya corregidos han sido removidos de la lista.

Nic Andrews y Alain Greppin finalmente han implementado grub2 en AROS. Nic
también expuso una agradable `captura de pantalla`__ en Aros-Exec. Lo bueno
de esto, es que los usuarios podrán desahacerse finalmente de las lentas
particiones FFS y cargar los archivos de sistema desde otras en SFS.
Sin embargo, no recomendamos aún esta opción, debido a incompatibiliades
todavía existentes con algunas aplicaciones AROS.

Alain Greppin ha obtenido la recompensa del AROS DHCP con su comando dhclient.
Ahora, AROS puede obtener una dirección IP automáticamente. También adaptó
`TeXlive`__.

Tomasz Wiszkowski y Michal Schulz están trabajando para mejorar el ata.device.
Se ha agregado cierto respaldo inicial para algunos chipsets Serial ATA:
"Los controladores SATA que respaldan el modo de operación heredado (legacy)
deberían estar operativos (es decir que no tenemos AHCI por el momento)".

__ http://aros.sourceforge.net/download.php
__ http://i175.photobucket.com/albums/w131/Kalamatee/AROS/grub2gfx-1.jpg
__ http://www.chilibi.org/aros/texlive


=============
Status Update
=============

:Author:   Paolo Besser
:Date:     2007-03-21

Soporte USB preliminar
----------------------

El Dr. Michal Schulz acaba de hacer un envío preliminar de la 
pila USB de AROS, que permite el uso de un ratón USB y la 
creación de los fundamentos para agregar el soporte, subsecuentemente,
para otros dispositivos. Por favor advierte que este software
está en etapa pre-alpha, lo que significa que solamente tienen
soporte los controladores UHCI de USB 1.1 por ahora, y quizás
no funcionen correctamente con tu placa madre. Lo siguiente a
desarrollar son las extensiones a la clase USBHID, que permitan
el uso de tabletas gráficas y teclados USB con AROS, y el
soporte de los controladores OHCI. Debemos avisar que el
soporte para los controladores EHCI de USB 2.0 no están
definidos en la recompensa en que trabaja Michal.
Para mejorar la funcionalidad y la compatibilidad, se necesita
una enorme prueba beta. Así que animamos a todos los 
suficientemente valientes para que nos ayuden, descargando
las nightly builds de los días que vienen e informen de
cualquier problema que encuentren. Para activar la pila entera
en las máquinas UHCI, el comando es::

  C:Loadresource DRIVERS:uhci.hidd


Otras noticias
--------------

Nic Andrews ha mejorado ligeramente la AROSTCP, dándonos una
mejor (y más compatible) pila de red. También se hizo el trabajo
de reparación en el controlador del VIA Rhine y se agregó una prueba 
RTL8139.

Pavel Fedin está todavía trabajando en los modos VESA mejorados:
"Arreglado el código de conmutación del modo VESA. Se quitó la
paleta estática, ya no es necesaria. Los colores del puntero del
ratón en las pantallas de 16 y 256 colores están fijos". También
reparó la inicialización de algunas viejas tarjetas con unas
BIOS defectuosas. Además incorporó el dispositivo (device)
Trackdisk de MorphOS. Un nuevo programa pref para Trackdisk
permite el modo 'sin-apretar' (no-click) de la unidad de disquete
sin agregar argumentos a la línea de comandos del núcleo en GRUB.
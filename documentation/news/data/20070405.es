=============
Status Update
=============

:Author:   Paolo Besser
:Date:     2007-04-05

Skinnable Wanderer
------------------

Ha sido dado otro importante paso en la personalización de AROS. 
Darius Brewka y Georg Steger han añadido un nuevo sistema de decoración
al Wanderer, que le permite al usuario editar y cambiar los temas.
Como un ejemplo se usó el tema ICE de Damir Sijakovic que ahora es el
tema predeterminado de las nightly builds de AROS. Mira en esta
captura de pantalla (`a tamaño completo`__): 

.. image:: /images/20070405.jpeg
   :alt: AROS decorated desktop
   :align: center

¿No es bonita? Como sea, intentamos hacer que AROS tenga más estilo
y sea agradable de ver. Así que cualquier contribución artística
(temas, íconos, wallpapers, gadgets y cosas como ésas) es muy
bien recibida. Para información adicional, por favor mira en 
`AROS-Exec`__.


Increíbles noticias
-------------------

Michal Schulz está trabajando en el controlador de teclado para
su pila USB. Ha enviado una versión inicial que debería funcionar
casi bien (o sea, debería manejar las teclas correctamente aunque
los LEDs no andan). Recuerda que AROS todavía es compatible
solamente con los controladores USB UHCI en esta etapa.

El sistema de archivos SFS ahora se compila automáticamente. Esto
permite que los usuarios monten y usen las particiones SFS de 
una manera fácil y segura. Pavel Fedin ha actualizado el comando
format para manejar los diferentes sistemas de archivos. Para
formatear una partición SFS, deberías crear con HDToolBox, 
cambia su tipo a SFS y ejecuta este comando del shell:

  FORMAT DRIVE=DH1: NAME=MyVolume

Advierte que GRUB todavía no puede arrancar AROS de SFS. Por esto
tienes que crear una pequeña partición FFS (o EXT3, o FAT) para 
poner el núcleo y los archivos de arranque y deja que GRUB haga
su trabajo.

Pavel también está trabajando en un nuevo sistema de archivo
de CD llamado CDVDFS, que en este momento soporta los CD y DVD
con las extensiones Joliet. Será puesto en las nocturnas tan
pronto que sean corregidos algunos errores.

Matthias Rustler ha actualizado AROS LUA a la versión 5.1.2.
`Aquí`__ está una lista de los errores reparados. Matthias
también ha creado un nuevo commodity DepthMenu. Cuando el usuario
aprieta el botón derecho del ratón en el gadget profundidad
(es el gadget en el extremo derecho de una ventana), abre
un menú emergente con todas las ventanas abiertas, permitiendo
una selección más rápida.

Staf Verhaegen ha mejorado el soporte para los guiones ARexx/Regina.
Hay un nuevo comando rx en AROS, que espera en el CLI para ejecutar
todos tus guiones ARexx.

Gracias a Bernd Roesch, Zune ahora puede usar las imágenes MUI4
(los archivos .mim). Pavel Fedin ha arreglado un cuelgue en la
clase pendisplay que ocurría cuando no había pinceles libres
en la pantalla. Paolo Besser ha agregado otros Íconos Gorilla.


__ http://aros-exec.org/modules/xcgal/displayimage.php?pid=239
__ http://aros-exec.org/modules/news/article.php?storyid=227
__ http://www.lua.org/bugs.html#5.1.1

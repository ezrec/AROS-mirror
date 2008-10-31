===================
Cuadro de Situación
===================

:Autor:   Paolo Besser
:Fecha:   2008-02-18

Últimas noticias
----------------

Michal Schulz está trabajando a pleno en la adaptación de AROS a la placa
SAM440 de Acube Systems, y ha obtenido `resultados`__ interesantes. He aquí
algunas palabras publicadas en `su blog`__: "He decidido separar el núcleo
(y las bibliotecas que se cargan con él) del espacio del usuario. El núcleo
se carga en algún lugar de los primeros 16MB de RAM y luego es reubicado
a la dirección virtual en lo más alto del espacio de memoria de 32 bits.
El cargador funciona de manera similar a como lo hacía el de x86_64. Pone
todas las secciones exclusivamente de lectura (read-only) por encima de la
base del núcleo, y todas las secciones de escritura por debajo de la misma.
Dado que soy malvado por naturaleza, mi núcleo de AROS para SAM440 acapara
toda la memoria *debajo* de su ubicación física para sí mismo. Esta memoria
(unos pocos megas) es utilizada como un depósito local para el núcleo y queda
excluído de cualquier tipo de acceso desde modo de usuario".

Nic Andrews está trabajando en Wanderer, para mejorarlo y arreglar algunos
errores molestos. Actualmente está "trabajando a los ponchazos en reescribir
el código de presentación de la clase iconlist de Wanderer. El objetivo a
corto plazo es permitir que las ventanas conserven la presentación de íconos y
fondo para que, por ejemplo, el uso de presentación en mosaico para fondos de
iconlist no produzca un notorio parpadeo como lo hace ahora". En `su blog`__
puede encontrarse información más detallada sobre su trabajo.

Internet fácil
--------------

Michael Grunditz ha lanzado oficialmente sus primeras betas de `SimpleMail`__ 0.32
para AROS. SimpleMail cuenta con la mayoría de las características necesarias
para un cliente de correo electrónico moderno, y sigue creciendo. La adaptación
a AROS se encuentra en la sección `Archives`__.

Robert Norris hizo montones de avances con su Traveller, su navegador basado
en Webkit. Para hacerlo funcionar, aún le quedan por escribir algunas
opciones y bibliotecas. Sin embargo, su adaptación de Cairo.library está bien
encaminada y ha logrado presentar mayormente de manera correcta algunas
páginas web. Una `captura de pantalla`__ prometedora se publicó fue publicada
en `su blog`__.

Otras novedades
---------------

Recientemente, João "Hardwired" Ralha ha escrito varios buenos manuales para AROS.
Sin embargo, aún no están terminados y actualmente anda buscando ayuda.
Los documentos disponibles son el `AROS User Manual`__ (50% completo), el
`AROS Shell Manual`__ (70%) y el `AROS Install Manual`__ (25%). Se puede contactar
al autor en `su página`__.

Alain Greppin ha obtenido una recompensa, adaptando TeXlive a AROS. Más información
sobre el tema en `su página`__.

Tomek 'Error' Wiszkowski ha estado trabajando en Frying Pan, una aplicación para
quemar CD/DVD. Publicó `algunas capturas de pantalla`__ en AROS-Exec.org.
La version 1.3 para AROS (shareware) se puede descargar desde la `página de aplicaciones`__.
Para hacerlo funcionar, también corrigió algunos errores en ATA device de AROS.

...y, para aquellos que se lo hayan perdido: el sistema de archivos AROS FFS
recientemente incorporó una rutina de validación adecuada. ¡Se acabaron las
particiones de sistema inutilizables!

__ http://msaros.blogspot.com/2008/01/ive-promised-to-show-you-some.html
__ http://msaros.blogspot.com
__ http://kalamatee.blogspot.com/
__ http://simplemail.sourceforge.net/index.php?body=screenshots
__ http://archives.aros-exec.org/index.php?function=showfile&file=network/email/simplemail_beta_aros-i386.tgz
__ http://cataclysm.cx/2008/02/18/cow-launched
__ http://cataclysm.cx
__ http://archives.aros-exec.org/share/document/manual/aros_user_manual_version_0.56a.pdf
__ http://archives.aros-exec.org/share/document/manual/aros_shell_manual_version_0.7a.pdf
__ http://archives.aros-exec.org/share/document/manual/aros_install_manual_version_0.25a.pdf
__ http://aros-wandering.blogspot.com
__ http://www.chilibi.org/aros/texlive
__ http://aros-exec.org/modules/newbb/viewtopic.php?viewmode=flat&topic_id=2569&forum=2
__ http://www.tbs-software.com/fp/welcome.phtml

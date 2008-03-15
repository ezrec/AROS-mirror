
:Authors:   Aaron Digulla, Stefan Rieken, Matt Parsons, Adam Chodorowski 
:Copyright: Copyright © 1995-2002, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Casi terminado, creo...


.. raw:: html

   <h1>Introducción<br><img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"></h1>

El Sistema Operativo de Investigación AROS es un sistema operativo de escritorio
liviano, eficiente y flexible, diseñado para 
ayudarte a hacer tus tareas de computadora. Es un proyecto independiente, 
portátil y libre, que pretende ser compatible con AmigaOS 3.1 a nivel 
de API (como Wine, en vez de UAE), a la par que lo mejora en muchas 
áreas. El código fuente está disponible bajo una licencia de fuente 
abierta, que permite a cualquiera mejorarlo sin restricciones.



Metas
=====

La meta del proyecto AROS es crear un OS que:

1. Sea tan compatible como sea posible con AmigaOS 3.1.

2. Pueda ser llevado a diferentes tipos de arquitecturas de harware
   y procesadores, como x86, PowerPC, Alpha, Sparc, HPPA y demás.

3. Debería ser compatible a nivel de binarios en una Amiga y a nivel
   de fuentes en otro hardware.

4. Pueda ejecutarse como una versión autónoma que arranque directamente 
   del disco duro y como una emulación que abra una ventana en un OS 
   existente para desarrollar software y ejecutar aplicaciones del Amiga y 
   nativas al mismo tiempo.

5. Mejore la funcionalidad del AmigaOS.

Para lograr esta meta, usamos un número de técnicas. Antes que nada, 
hacemos un uso intensivo de Internet. Tú puedes participar en nuestro
proyecto incluso si sólo puedes escribir una sola función del OS. La 
versión más actual del código fuente es accesible las 24 horas del día 
y los parches pueden mezclarse en él en cualquier momento. Una pequeña 
base de datos con las tareas encargadas asegura que no haya duplicación 
de trabajo.


Historia
========

Tiempo atrás, en el año 1993, la situación para la Amiga se veía algo
peor que lo usual y algunos fans de la Amiga se reunieron y discutieron qué
se debería hacer para aumentar la aceptación de nuestra amada máquina. 
De inmediato se hizo evidente la principal razón de la pérdida de éxito: 
su propagación, o la falta de ella. La Amiga debería conseguir una base 
más amplia para hacerse más atractiva para que todos la usen y desarrollen 
para ella. Entonces se hicieron planes para alcanzar esta meta. Uno de 
los planes era reparar los errores del AmigaOS, otro fue hacerlo un 
moderno sistema operativo. Así nació el proyecto AOS.

Pero, ¿qué es exactamente un error? Y, ¿cómo deberían ser reparados? 
¿Cuáles eran las características que un OS llamado *moderno* debería 
tener? ¿Y cómo deberían ser implementadas en el AmigaOS?

Dos años después, la gente todavía estaba argumentando sobre esto y ni
una línea de código se había escrito (al menos nadie había visto tal 
código). Las discusiones todavía eran del tono en que alguien afirmaba 
que "deberíamos tener..." y otro que respondía "lee los viejos mensajes" 
o "esto es imposible de hacer, porque..." a lo que le seguía un "tú estás
equivocado porque..." y así en adelante.


En el invierno (boreal) de 1995, Aaron Digulla se hartó de esta 
situación y puso un RFC (pedido de comentarios) en la lista de correo de 
AOS en el que preguntaba cuál podría ser un terreno mínimo común. Se 
dieron varias opciones y la conclusión fue que a casi todos les gustaría 
ver un OS abierto que sea compatible con el AmigaOS 3.1 (Kickstart 40.68) 
sobre el que se pudieran basar las discusiones posteriores para ver qué 
es posible y qué no.

Así comenzó el trabajo y nació AROS.


========================
Itinerario para AROS 1.0
========================


:Authors:   Adam Chodorowski, Sergey Mineychev
:Copyright: Copyright ¦ 2007, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$

:Abstract:  
    Este es el itinerario previsto para esbozar los requerimientos
    que necesitan ser satisfechos antes de que se pueda presentar
    AROS 1.0. Esto *no* significa forzar a los desarrolladores para
    que trabajen en ciertas cosas (dado que eso no es posible de ninguna
    manera en un proyecto de fuente abierta) sino que debería ser visto
    como una recomendación de en dónde nos gustaría que la comunidad enfoque
    sus esfuerzos. Esperamos, que esto ayudará a organizar el esfuerzo
    de desarrollo y hará que todos trabajemos más en la misma dirección.


Puertos
-------

Para que sea presentado AROS 1.0, estos requerimientos deben ser
cumplidos por los puertos i386-pc y i386-linux (abajo referidos como
"los puertos obligatorios"). Los otros puertos no son vistos como 
requeridos para la 1.0, para mantener reducida la cantidad de trabajo.


Requerimientos
--------------

Como el estado actual no se puede estimar de un modo objetivo en
porcentajes, sólo podemos dar una estimación cualitativa para estos puntos.
También se pueden resaltar recomendaciones adicionales.

1.  Compatibilidad con la API de AmigaOS 3.1, excluyendo las partes que
    están juzgadas ser no portátiles u obsoletas y que no merecen
    el esfuerzo.
    
    Para marcar algo como obsoleto incluso aunque sea posible implementarlo
    de una manera portátil deberían haber buenas razones; por ejemplo
    que es usado por las aplicaciones muy de vez en cuando y el esfuerzo
    necesario para implementarlo es substancial.
    
    Estado actual: La mayoría de la API de AmigaOS 3.1 está implementada
    e incluso supercedeed algunas partes. (Note? que las partes del
    hardware Amiga que faltan como lowlevel.lib, card.device, gameport,
    audio.device no han sido implementadas ni reemplazadas). Aunque necesita
    que se le haga un buen trabajo de corrección de errores.


2.  Compatibilidad parcial con las API de AmigaOS 3.5 y 3.9. Escogimos 
    las partes que sentimos que son útiles y valiosas de tener,
    y deshacerse del resto.
    
    Por ejemplo, parece *muy* improbable que queramos la compatibilidad 
    con ReAction, dado que ya decidimos normalizar a Zune para la
    biblioteca de GUI (implementar la API ReAction no es un proyecto
    trivial). Por supuesto, que esto tendrá que ser discutido apropiadamente
    antes de que se llegue a un acuerdo sobre una lista detallada.
    
    Estado actual: <?>
    

3.  La biblioteca de GUI completada. Esto significa que Zune debe tener
    una compatibilidad completa con la API de MUI y estar terminado el 
    programa de preferencias.
    
    Estado actual: Casi listo. Al editor de preferencias le faltan
    algunas funciones.


4.  Aplicaciones standard comparables a las que vienen con AmigaOS 3.1.
    
    Esto *no* significa que debemos tener las mismas aplicaciones
    funcionando exactamente igual que en AmigaOS, sino que la funcionalidad
    disponible para el usuario debería ser aproximadamente equivalente.

    Estado actual: La mayoría de las aplicaciones básicas están desarrolladas.

+ Están faltando:

  - Overscan (FIXME:not really needed)
  - Palette (FIXME:not really needed)
  - Pointer 
  - Printer (absent)
  - Sound (it`s different - we have AHI prefs)
  - WBPattern (we have Wanderer prefs, which is in works)
  - Workbench (we have Wanderer prefs, which is in works)
                  
5.  El soporte de sonido, lo que significa compatibilidad con la API y
    con las aplicaciones básicas. Debería haber al menos un controlador
    para cada puerto obligatorio.
    
    Estado actual: En la actualidad está adaptado AHI, y hay algunos
    controladores (unos pocos) para el puerto i386. Las aplicaciones
    están en desarrollo; están disponibles Madahi y un reproductor de MP3.
    (FIXME: para despejar - ¿el código de estas aplicaciones debe
    ser incluído en el sistema o nada más hacerlo disponible, o eso es
    innecesario?).
        
6.  El soporte de red. Esto incluye una pila TCP/IP y algunas aplicaciones
    básicas como los clientes de correo y de SSH, y también un *simple*
    navegador web. Debería haber al menos un controlador de NIC para
    cada puerto obligatorio.
    
    Los requerimientos en el navegador web no deberían ser altos, pero
    al menos debería ser posible navegar la web de algún modo (incluso
    si es en modo texto).
    
    Estado actual: El AROSTCP que tenemos es una antigua aunque funcional
    implementación de la pila AmiTCP. Están desarrolladas algunas 
    aplicaciones (es decir, los clientes de FTP, de telnet y de IRC) pero
    no son parte del sistema en sí. Otras aplicaciones, incluídas
    algunas posibles implementaciones de un navegador, están en camino.
    
    
7.  El ambiente de desarrollo auto-hosted y un SDK para los desarrolladores.
    Específicamente, esto incluye todo el software necesario para 
    compilar AROS como GCC, GNU binutils, GNU make y el resto. Tiene
    que ser posible compilar AROS dentro de AROS.
    
    La ABI para la arquitecturas soportadas (en este punto solamente i386(
    debe estar finalizada antes de la 1.0. Una vez que la 1.0 sea presentada
    la ABI deberá ser estable durante un tiempo considerable.
   
    Estado actual: AROS tiene un completo puerto GCC, lo que permite 
    compilar las aplicaciones, pero todavía faltan alguna herramientas GNU,
    así que en este momento no es posible la auto-compilación.

+ Faltan: 
 
  - GNU AWK (GAWK) or other awks
  - Python 2.2.1+ (an old port of python is available though)
  - Bison
  - Flex
  - pngtopnm and ppmtoilbm (part of the netpbm package)
  - Autoconf
  - Automake
  - Las utilidades comunes de Unix como cp, mv, sort, uniq, head, ...
    
8.  La documentación completa para los desarrolladores. Esto incluye 
    los manuales de referencia completos de todas las bibliotecas, 
    dispositivos, clases y herramientas de desarrollo y también las guías
    y los tutoriales para presentar subsistemas enteros y dar una
    introducción general. También, debería haber disponible una guía de 
    migración/porting.
    
    Estado actual: <?>

    
9.  La documentación completa para los usuarios. Esto incluye una
    referencia completa de los comandos, y tutoriales y guías para la
    instalación, configuración, y demás.
    
    Current status: Documentation exists and extensively translated to
    different languages. Still needs the tutorials and guides, and the help
    system to be complete.
    Estado actual: La documentación existe y está traducida
    a diferentes idiomas. Todavía se necesitan los tutoriales y las 
    guías, y que el sistema de ayuda esté completo.


10. Una prueba substancial y una completa caza de errores. La entrega
    1.0 debería estar virtualmente libre de errores, y ser *muy* estable.
    No deberíamos tener los fiascos que algunos proyectos de fuente 
    abierta han tenido con sus entregas ".0".
        
    Es probable que esto requiera un congelamiento extenso de 
    características, seguido por un congelamiento del código y por varios
    hitos intermedios para hacer pruebas y recibir la opinión
    de los usuarios. Los pedidos de características no se consideran errores,
    a menos que sea algo pedido (y falte) en los hitos
    precedentes. Por ejemplo, "necesitamos un reproductor de películas"
    no califica, en cambio "el editor de texto debería tener una opción
    de menú 'guardar'" sí.

    Estado actual: En la actualidad no se puede hacer ningún congelamiento
    mientras aún las características no estén completas. Todavía quedan
    bastantes errores sin reparar, pero la creciente actividad de los
    usuarios se ve prometedora. Se buscan los procedimientos y servicios
    para la caza de errores y de accounting.
    

Estado global
<para hacer>

Recomendaciones
<para hacer>

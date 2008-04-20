==========
Traducción
==========

:Authors:   William Ouwehand, Keen
:Copyright: Copyright © 1995-2007, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$
:Status:    Work in progress.

.. Warning:: 
    
    Casi terminado


.. Contents::


Introducción
------------

Traducir el OS y el sitio web ayuda a que AROS llegue a más personas internacionalmente,
y a hacer el OS más fácil de usar. Hacer estimaciones es imposible, pero nadie
puede negar que hay una base potencialmente grande de usuarios que son incapaces de leer,
hablar o escribir en inglés. Alcanzar a estas personas pueda *posiblemente* ayudar a 
tener más usuarios y desarrolladores.
Para este propósito aún estamos buscando nuevos traductores o personas a las que les 
gustaría ayudar con los esfuerzos actuales en esto.

Hasta el momento, AROS solamente ha sido traducido a unos pocas lenguas, y a menudo
parcialmente. Los pocos traductores disponibles y la cantidad de trabajo son una de las 
causas de esto. También, sucede que la mayoría de los traductores y escritores son los 
desarrolladores (principales); absorber sus tareas aquí (incluso parcialmente) puede
ayudarles a maximizar sus esfuerzos en la codificación o escritura de nueva documentación.
Entonces si estás interesado en ayudarnos, por favor `contáctanos`_. Incluso el mínimo esfuerzo
puede ayudarnos enormemente.


Dónde empezar
-------------
Primero, por favor `contáctanos`_ para verificar si puedes contribuir (sólo para estar en el
lado seguro).

Traducir es tan básico como implica. Todo lo que necesitas para empezar la traducción
de AROS es un editor de texto adecuado, acceder a las `SVN sources` de AROS y un
conocimiento de tu lengua y gramática. Información sobre las dos primeras se pueden encontrar
aquí en la `página Trabajando con SVN`_, la última es para tí por supuesto. Advierte que no
necesitas incluso de AROS para trabajar en el sitio o el OS - también Linux, Windows o
OS X lo harán.

El trabajo de traducción mismo se puede dividir en el sitio web (noticias/documentación) y la
localización del sistema operativo AR(OS). El sitio web tiene una prioridad mayor ya que
es la primera cosa que los nuevos usuarios descubren, y porque contiene documentación útil
e importante para los usuarios. la localización del OS no es menos importante, pero siempre
ocurre en una fecha posterior. A la par, la traducción más básica del sitio involucra sólo
10 a 15 páginas no muy largas, haciendo que tal cosa no sea un gran trabajo de cualquier 
modo.


Traducir el sitio web
=====================

El sitio web es la presencia oficial online de AROS. Mantiene la información
general sobre AROS, las noticias, las descargas (del OS) y las guías del usuario
y del desarrollador. Juega un papel importante en informar a la gente sobre AROS,
y en servir como la guía de referencia para usuarios y desarrolladores. De las
dos últimas, las guías del usuario son lo más importante para ser traducido.

El sitio mismo es bastante extenso, aproximadamente más de 100 páginas largas. Aunque
es fácil, no todo esto necesita ser traducido. Algunas partes son menos importantes,
otras no están terminadas y las restantes están bastante anticuadas. Así que para
tu propia facilidad, ponte por límite las primeras páginas ('el nivel 1'). Eso
significa sobre todo los documentos del usuario y las otras páginas principales
del sitio.

De todo el contenido, la sección del desarrollador es la menos importante. Generalmente
los desarrolladores tienden a hablar bien en inglés, haciendo que no sea valioso
el esfuerzo de traducirla; los usuarios requerirán más atención en general. La
excepción a la regla son las páginas 'working with SVN' y 'contribute' que pueden
resultar ser útiles cuando tú reclutes otras personas en tu país.


Procedimiento
~~~~~~~~~~~~~
El sitio web de AROS se compila de archivos de solo-texto, almacenados en un
archivo online de SVN. Éstos son más tarde puestos en una plantilla, creando el
sitio web de AROS que conoces.

Para resumir, se recomienda mucho que tú:
+ hagas conocer tus intenciones a la LC (lista de correo) del desarrollador y veas si tus servicios son necesarios (para estar en el lado seguro)
+ leas entera esta página
+ leas la `página Trabajando con SVN`_
+ solicites acceso a SVN
+ te anotes en las LC del desarrollador y del sitio web.

*SVN*

Si no estás familiarizado con SVN, lee sobre eso en la `página Trabajando con SVN`_ 
como ya se dijo. En pocas palabras, SVN funciona como un tipo de FTP que se
enfoca solamente en los archivos. Sin embargo, en vez de mover los archivos de
a uno como lo hace FTP, mantienes una completa (y sincronizada) copia de los
fuentes en tu disco duro. Después trabajas offline desconectado en estos
archivos, y cuando lo desees, sincronizas tus cambios con el depósito principal
en el servidor.
Los archivos fuentes con los que trabajarás son archivos de texto plano 
(no HTML). 
El sofisticado aspecto del sitio web de AROS, junto con el menú, se agrega 
en un proceso separado en el servidor que transforma esto archivos de texto en 
un completo sitio HTML. Esto será tratado más tarde con alguna extensión.

Se recomienda mucho que esperes hasta que consigas acceso a SVN antes de empezar. Eso hace más fácil
enviar y cambiar tus archivos traducidos más tarde. Sin embargo, el problema puede ser que pase
largo tiempo hasta que recibas tu verdadero acceso a SVN - a veces incluso varias semanas después de
tu pedido inicial. Puede ser una prueba de paciencia, deberías estar realmente entusiasmado...
afortunadamente para ti, puedes hacer un inicio descargando una copia de los fuentes
del sitio web desde el sitio web de AROS. Esto es simplemente una copia del depósito SVN
sitio web mencionado. Al lado, también es una gran manera para familiarizarte con el modo
con que el contenido es manejado y el archive está estructurado. El único inconveniente
con este modo es que más tarde tendrás que copiar a mano tus archivos para tu verdadero
revisión de SVN. Hay un escenario posible donde traduces desde el archive y 
envías los archivos a la LC, pero el trabajo extra que esto lleva consigo no es
recomendado. No obstante, continuemos.

Cuando tengas acceso a SVN, ejecuta una revisión de los fuentes del sitio web 
con SVN. Esto descargará el depósito del momento a tu HD, conteniendo todos los
archivos del sitio web.
Las ubicaciones son::

  https://svn.aros.org/svn/aros/trunk/documentation/

Cuando recorras la copia local (o la descargada) notarás las extensiones para los 
diferentes lenguajes. Cada extensión corresponde a un lenguaje específico, y como
puedes adivinar lo mismo será necesario para tus traducciones. Ahora simplemente escoge
una página existente (en inglés de preferencia), duplica este archivo en algún lugar
mientras agregas la extensión c orrecta, por ej. contribute.en (inglés) -> contribute.nl
(holandés). Entonces puedes traducir los contenidos de la página.

*La codificación de caracteres*

La codificación de caracteres de tus archivos a traducir necesitan atención
extra. La diferencia de idiomas y los símbolos que usan requiere que
los archivos de texto usen una codificación específica capaz de mostrar
los símbolos correctos. Si debes traducir para un idioma occidental 
genérico (por ej. inglés, alemán) tienes poco de qué preocuparte - sólo
asegúrate de configurar tu editor de texto para guardar los archivos
con la codificación ISO-8859-1 o ISO Latin-1.
Cuando tu idioma use símbolos más 'exóticos' que las normas occidentales
(por ej. ruso o griego), requerirás ajustes diferentes. Especialmente 
cuando configures la traducción de un nuevo sitio web se recomienda 
que contactes a la lista Dev ML para recibir consejo.
También consulta la `página de documentación`_ para información adicional.

*Envío*

Cuando esté hecho, envía tus archivos al depósito para que sean
procesados. Por favor usa mensajes de rgistro típicos como "traducciones
al alemán" o "actualización de la página X del alemán" y prueba a juntar
más de un archivo en un envío (cuando estén relacionados). Los cambios
clave pueden enviarse mejor por separado: si se comete un error, el cambio
puede ser fácilmente deshecho por los administradores sin desperdiciar
tu otro trabajo para ese particular envío (y entérate, que puesto que
el sistema sincroniza todas las copias, ¡incluso puedes perder tu trabajo
local!).

¿Puedes ver los archivos de texto plano que en el servidor crean el sitio web?
Son guiones, que toman los textos fuente y los convierten en páginas HTML/PHP
adecuadas para navegar. Este proceso se llama 'building', y puede también
hacerse en tu máquina para propósitos de prueba - más sobre eso después. El
sitio web ejecuta las builds una vez cada unas pocas horas (o días), 
después de lo cual tus paginas estarán listas para navegarse en el sitio web.

La excepción a esta regla es cuando una nueva sección de lenguaje entera se agrega: son
necesarios más cambios en la configuración del proceso de build. 
Dependiendo de tu habilidad, puede ser mejor que contactes a la LC de Desarrollo
para instrucciones.

.. Note:: 
    
	Los archivos en inglés deberían ser considerados la base 'por omisión' para todas
	las traduccciones, ya que por lo general tiene los textos más al día. Para todas las 
	páginas que todavía no has traducido, el sitio mostrará la versión en inglés,
	incluso cuando navegues con tu lenguaje específico.

Revisa tu trabajo
~~~~~~~~~~~~~~~~~

Se aconseja que revises tus textos respecto a dos asuntos: ortografía y errores de
las marcas.

Los correctores ortográficos deberían considerarse obligatorios, la opción
que vuelvas a leer tu trabajo puede ayudar. La combinacióon debería
atrapar el 99% de los errores ortográficos y  gramaticales, e involucra
poco trabajo. No te quejes, pero un uso correcto del lenguaje hace
a la presentación del sitio.

Los errores de marcado son otra historia. Estos errores se cometen en la etiquetas
usadas en el documento fuente. Las etiquetas son símbolo/códigos especiales, como
enlaces o renglones de '===' que indican que el renglón previo es una cabecera.
Cuando se presenten, estos errores rompen el proceso de 'building' para esa
página. A pesar de estos errores, la página todavía se generará y puesta online
-pero- con mensajes de error molestos y partes rotas. No es agradable, pero los
problemas están limitados cuando cometes un error ;-)
El proceso de building informa todos los errores automáticamente a la LC del 
sitio web. También se muestra información extra haciendo que el arreglo sea más
fácil. (Por lo tanto deberías siempre estar anotado en la LC).
También puedes esperar que uno de los otros desarrolladores corrigan el error,
pero eso genera trabajo extra para los otros, y tú no aprendes de tus errores.
Sabe que es bastante usual cometer errores en el primer trabajo, mientras 
que la mayoría de los posteriores son errores casuales.

*Compilar el sitio de modo local*

Estarás de acuerdo que este proceso de solo enviar al servidor y esperar es un modo de
'volar a ciegas'. ¿Cómo puedes evitar (razonablemente) que las páginas defectuosas 
terminen en el sitio web? Compilar el sitio web por tí mismo también, igual
que se hace en el servidor. Esto requiere que instales algo de software extra
(Python), pero después puedes revisar tu trabajo con el mismo guión que
crea los archivos HTML del sitio web en el servidor; no es magia =).
Trabajas de un modo más eficiente de esta manera, mantienes el sitio ordenado
y ahorras el tiempo de los otros desarrolladores.
Para intrucciones sobre cómo compilar localmente y las herramientas necesarias, revisa
la `página de documentación`_. 

Como con los arreglos, las nuevas páginas deberían mostrarse en unas pocas horas
(a un máximo de un día) en el sitio web.

Localizar AROS o las aplicaciones
=================================
Localizar el OS y sus aplicaciones agrega mucho de la usabilidad y 
experiencia a los usuarios no angloparlantes, quizás incluso hace que AROS
pueda ser usado por ellos en primer lugar. El procedimiento para localizar el OS es básicamente
el mismo que para el sitio, pero difiere en sus detalles.
Como con el sitio web incluso una traducción básica basta, limitándose al
Workbench y sus aplicaciones/utilidades. Esto hace un buen comienzo/base para 
la adicional localización. Siempre puedes progresar a las aplicaciones
extra del OS y los archivos contribuidos.

Procedimiento
~~~~~~~~~~~~~ 

AROS, como el AmigaOS, ofrece la opción de almacenar las cadenas de texto
de una aplicación en un archivo separado, permitiendo una fácil y eficiente
localización. Los archivos de catálogo para AROS están ubicados en su mayoría
dentro del depósito de código principal, que es diferente del depósito del 
sitio web. En este caso tu puedes ya sea ejecutar una completa revisión de SVN, o
navegar el depósito SVN con tu navegador y descargar los archivos especificados.
Necesitarás acceso a SVN para esto, a diferencia de las fuentes del sitio web
los fuentes completos del OS no están disponibles por separado.

El depósito del OS se puede acceder por un navegador usando este enlace
https://svn.arog.org:8080/svn/aros/trunk/AROS lo que te permite que
tengas uno o dos archivos de manera eficiente.

  https://svn.aros.org:8080/svn/aros/trunk/AROS

Nota que la revisión de SVN es bastante grande (200MB) y por esto toma algún tiempo 
completarlo. No te intimides por el tamaño del depósito: solamente necesitas
enfocarte en una fraccción del estos archivos, por suerte. Ahora puedes empezar
el trabajo. Si estás confundido, no dudes en pedir ayuda en AROS-exec o en la LC.

*Definiciones del idioma y de las fuentes*

Primero, debes buscar tu país en Locale/Countries y el archivo de
definición del idioma en Locale/Languages. La mayoría de los idiomas
ya fueron agregados por si acaso. Si no puedes encontrar algo como <tu_idioma>.c
ahí, entonces debes crearlo. Intenta evitar los caracteres especiales en el
nombre del archivo, esto puede ayudar a evitar problemas con algunos
programas no-utf (recuerda, AROS pretende ser portátil). Puedes buscar el 
más completo lang.c y copiarlo a tu_idioma.c. Entonces, abre el archivo
en tu editor de texto y traduce todas las cadenas de texto que necesites.
Esto incluye las unidades usuales como días, meses, moneda, etc. Puedes
buscar tablas con descripciones de los caracteres usados en estos archivos
(tablas de collation), o puedes saltarlas al empezar. Después de compilar,
tu locale debe aparecer en la lista de locales del Pref Locale.

¿Qué sigue? Si tu lenguaje usa fuentes no-ISO, las necesitas.
Busca en Internet las fuentes bitmap de AmigaOS o True Type de
fuente abierta, que pueden ser incluídas al sistema. Pónlas en /Fonts
o /Fonst/TrueType y arregla el mmakefile (¡se **cuidadoso** con los
mmakefiles!)

*Crear los archivos CT*

Ahora viene el verdadero trabajo de traducción. Entérate que la localización en 
AROS es similar al del AmigaOS. Esto significa que puedes encontrar algunos
archivos .cd y .ct junto con el código (por lo general en un cajón Catalog).
Los archivos <nombre>.cd contienen las definiciones de catálogo y no deberían
ser tocados a menos que sepas que cambiaste el programa mismo.
Los archivos .CT contienen todas las cadenas traducidas, y son el objeto de 
tu trabajo. Navega el árbol SVN por estos archivos y tradúcelos: encuentra
el archivo <nombre>.ct más reciente (mira laversión y la fecha en la 
cabecera), copia tu <tu_idioma>.ct (es mucho más fácil que traducirlos del
archivo <lang>.cd) y edítalo. Intenta poner las palabras en los lugares correctos,
para lo que necesitarás revisar dónde se usan.

Para ayudarte con este trabajo de copia y cambio de nombre, hay una aplicación
CLI/Terminal llamada Flexcat. Puede generar los archivos .CT para tu idioma 
de modo automático de los archivos .CD. Es útil que también comente el texto
original (para inspección) y que cree renglones vacíos para que llenes con tu traducción. 
También ofrece las opciones de compilación necesarias para probar los
catálogos, que se trata más adelante. Flexcat se puede encontrar en AROSTrunk/Tools
pero necesita que se lo compile para AmigaOS o Linux. Puedes también revisar
Aminet o preguntar en la LC del Desarrollador.

*Usar Flexcat*

Antes de generar los archivos CT, pon a Flexcat en la misma ubicación
que tus archivos CD (o en AROS/AmigaOS, pónlo en el cajón "C"). Ahora navega
la ubicación con el Shell/Terminal y haz que Flexcat cree un archivo .CT
de su original archivo .CD usando el siguiente comando::

	FlexCat <application>.cd NEWCTFILE=deutsch.ct

Esto te dará un archivo deustch.ct listo para traducir. A menudo no se agrega
la información sobre la versión; cópiala a mano del archivo original.
Traduce sus contenidos y cuando esté listo, envía el archivo al depósito.

Un nota especial sobre actualizar los archivos de catálogo más viejos - se
puede hacer eficiéntemente con Flexcat usando el siguiente comando::

	FlexCat <application>.cd deutsch.ct NEWCTFILE=deutsch.ct
	
Esto conservará las cadenas viejas e insertará nuevas donde las encuentre. La
documentación de Flexcat tiene información adicional.

Revisa tu trabajo
~~~~~~~~~~~~~~~~~
De nuevo, ejecuta las correcciones ortográficas y relecturas de los textos como
ya se mencionó. La prueba es posible de varias maneras: (1) envíalos al 
servidor SVN y espera la siguiente nightly build. O (2), que es la más inmediata,
compila los locales por tí mismo usando Flexcat, luego cópialos al cajón
Locale de AROS y revísalos con las aplicaciones interesadas.
La compilacióon de los archivos catálogo 'finales' con Flexcat se hace
con la siguiente sentencia::

	Flexcat <application>.cd <yourlanguage>.ct CATALOG <File>.catalog

Advierte que <tu_idioma> no siempre tiene su equivalente en inglés; verifícalo 
con los locales existentes. Además de verificar el correcto uso textual también
se recomienda probar la legibilidad de las traducciones en tamaños de pantalla
más pequeños.

Mantenimiento
-------------
Como con la mayoría de las cosas, la información tiende a perder actualidad o se 
actualiza con frecuencia, lo que obliga a las subsecuentes sincronizaciones de 
los documentos traducidos. No es un trabajo recompensador (excepto del punto de 
vista de los usuarios finales), pero necesario. Así que es tuya la responsabilidad
por mantenerlos actualizados.

El seguimiento de los cambios se puede hacer con los registros de cambios de SVN,
disponibles del cliente de SVN. Esta lista muestra *todos* los cambios hechos
al depósito. Juzga de los mensajes de registro y de los archivos modificados 
si el cambio es relevante.
Hacer después la comparación y sincronización de los documentos puede ser 
tediosa. Algunos editores de texto ofrecen la opción de desplazamiento sincronizado
en la ventana, que puede ayudar algo cuando se comparan dos páginas. 
FIXME: ¿hay una manera más fácil con SVN?

Cuando actualices los catálogos del OS, se sigue lo mismo respecto a los
registros de cambios de SVN, excepto que ahora tienes a Flexcat, que probará
ser una herramienta indispensable.

.. _`contáctanos`: http://aros.sourceforge.net/contact.php#mailing-lists
.. _`página Trabajando con SVN`: svn
.. _`página de documentación`: documenting

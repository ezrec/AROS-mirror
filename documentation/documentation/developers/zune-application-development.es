==================================================
Manual para el Desarrollo de Aplicaciones con Zune
==================================================

:Authors:   David Le Corfec
:Copyright: Copyright © 2004, The AROS Development Team
:Version:   $Revision: 24429 $
:Date:      $Date: 2006-05-08 03:24:40 +1000 (Mon, 08 May 2006) $
:Status:    Unfinished;
:ToDo:      All


.. Contents::


------------
Introducción
------------

¿Qué es Zune?
=============

Zune es una biblioteca de elementos para la GUI (Interfaz
Gráfica de Usuario) orientada a objetos. Es un clon cercano 
(a nivel de API y de Aspecto&Sensación) de MUI, un producto shareware bien 
conocido del Amiga de Stefan Stuntz. Por eso los desarrolladores de 
MUI aquí se sentirán en casa; los otros descubrirán los conceptos 
y las cualidades que Zune comparte con MUI.

+ El programador tiene a much easier time para diseñar su
  GUI: no necesita de los valores hardcoded, Zune es 
  sensible a los cambios de las fuentes, y se adapta a cualquier tamaño de
  ventana gracias a su sistema de disposición de los elementos.
  En su mayoría tiene que darle a Zune la semántica de su GUI,
  y Zune le acomodará los detalles de bajo nivel de manera 
  automática.

+ Como un efecto lateral, el usuario tiene más control del Aspecto&Sensación
  de la GUI: es él quien decide los ajustes particulares que Zune
  usará para presentar la GUI diseñada por el programador.

Zune se basa en el sistema BOOPSI,  el entorno de trabajo heredado del AmigaOS
para la programación orientada a objetos en C. Las clases de Zune no derivan
de las existentes clases de gadget BOOPSI; en su lugar, la clase
Notify (Notificar) (la clase base de la jerarquía de Zune) deriva de la
clase raíz de BOOPSI.


Requisitos previos
==================

Es bienvenido algo de conocimiento en la programación OO (orientada a 
objetos). Si no lo tienes, Google puede ayudarte a encontrar buenos 
documentos introductorios sobre este tema.

Es esencial conocer las APIs y conceptos de AROS (o de AmigaOS) como 
las taglists (listas de etiquetas) y BOOPSI. Tener los Amiga Reference
Manuals (Manuales de Referencia del Amiga) (también conocidos como RKM)
es muy práctico.

Como Zune es un clon de MUI, toda la documentación pertinente a MUI se
aplica a Zune. En particular, los más recientes kits del desarrollador
de MUI que están disponibles se pueden encontrar aquí__. En este archivo LHA,
son cálidamente recomendados dos documentos:

+ `MUIdev.guide`, la documentación del programador de MUI.
+ `PSI.c`, el código fuente de una aplicación que demuestra todas las modernas
  prácticas de MUI como el diseño orientado a objeto y la creación 
  dinámica de objetos.

__ http://main.aminet.net/dev/mui/mui38dev.lha

Adicionalmente, este archivo contiene los autodocs de MUI, que son
los documentos de referencia para todas las clases de Zune.


-------------
BOOPSI Primer
-------------

Conceptos
=========

Clase
-----

Una clase se define por su nombre, su clase antecesora y un dispatcher.

+ el nombre: una cadena para las clases públicas, así puedan ser usadas por
  cualquier programa en el sistema, o nada si es una clase privada usada
  únicamente por una aplicación.

+ la clase antecesora: todas las clases BOOPSI forman una jerarquía con raíz
  en la clase con el apropiado nombre de rootclass (claseraíz). Esto 
  permite que cada subclase implemente su propia versión de una operación
  específica del padre, o fall back vuelvan a una proporcionada por su padre.
  También se la conoce como clase base o súper clase.

+ el dispatcher: da acceso a todas las operaciones (llamadas métodos)
  proporcionadas por esta clase, asegurando que cada operación sea
  manejada por el código apropiado o sea pasada a su súper clase.

El tipo BOOPSI para una clase es ``Class *`` también conocida como
``IClass``.

Objeto
------

Un objeto es una instancia de una clase: cada objeto tiene sus
datos específicos pero entre objetos de una misma clase se comparte el
comportamiento.
Un objeto tiene varias clases si contamos los antecesores de su clase 
verdadera (la más derivada, o sea la clase del objeto) hasta la clase raíz.

El tipo BOOPSI para un objeto es ``Object *``. No tiene un campo al que
puedas acceder directamente.

Atributo
--------

Un atributo está relacionado a los datos de la instancia de cada objeto:
no puedes acceder a los datos directamente, solamente puedes establecer (set)
u obtener (get) los atributos proporcionados por un objeto para
modificar su estado interno. Un atributo está implementado como un
Tag (etiqueta) (un valor ``ULONG`` o con ``TAG_USER``).

``GetAttr()`` y ``SetAttrs()`` se usan para modificar los atributos de
un objeto.

Los atributos puede ser uno o más de los siguientes:

+ Initialization-settable (``I``) :
  el atributo puede ser pasado como un parámetro en la creación del objeto.
+ Settable (``S``) :
  Puedes establecer este atributo en cualquier momento (o al menos, también
  después de la creación del objeto).
+ Gettable (``G``) :
  Puedes obtener el valor de este atributo.

Método
------

Un método BOOPSI es una función que recibe como parámetros un objeto,
una clase y un mensaje:

+ el objecto: es el objeto sobre el que actúas.
+ la clase: es la clase considerada para este objeto.
+ el mensaje: contiene un ID (identificador) de método que determina
  la función a invocar dentro de un dispatcher, seguido por sus 
  parámetros.

Para enviar un mensaje a un objeto, usa ``DoMethod()``. Usará primero la
clase verdadera. Si la clase implementa este método, lo manejará.
Sino probará con su clase antecesora, hasta que el mensaje sea manejado o
se alcance la clase raíz (en este caso, el mensaje desconocido es
silenciosamente descartado).

Ejemplos
========

Veamos unos ejemplos básicos de este entorno de trabajo OOP:

Obtener un atributo
-------------------

Consultaremos el contenido de un objeto Cadena MUI::

    void f(Object *string)
    {
        IPTR result;
        
        GetAttr(string, MUIA_String_Contents, &result);
        printf("String content is: %s\n", (STRPTR)result);
    }

+ ``Object *`` es el tipo de los objetos BOOPSI.
+ ``IPTR`` se debe usar para el tipo de resultado, que puede ser un entero
  o un puntero. Un IPTR siempre se escribe en la memoria, ¡por eso usar
  un tipo más pequeño llevaría a una corrupción de la memoria!
+ Aquí consultamos a un objeto MUI String sobre su contenido: 
  ``MUIA_String_Contents``, como cualquier atributo, es un ``ULONG``
  (o sea, es un Tag).

Las aplicaciones Zune usan más a menudo las macros ``get()`` y ``XGET`` en vez de::

    get(string, MUIA_String_Contents, &result);
    
    result = XGET(string, MUIA_String_Contents);


Estableciendo un atributo
-------------------------

Cambiemos es contenido de nuestra cadena::

    SetAttrs(string, MUIA_String_Contents, (IPTR)"hello", TAG_DONE);

+ Los parámetros puntero deben ser convertidos a `IPTR` para evitar
  las advertencias.
+ Después del parámetro de objeto, se pasa una taglist a `SetAttrs`
  y así debe terminar con `TAG_DONE`.

Encontrarás útil la macro ``set()``::

    set(string, MUIA_String_Contents, (IPTR)"hello");

Pero solamente con SetAttrs() puedes poner varios atributos a la vez::

    SetAttrs(string,
             MUIA_Disabled, TRUE,
             MUIA_String_Contents, (IPTR)"hmmm...",
             TAG_DONE);


Invocando un método
-------------------

Veamos al método más llamado en un programa Zune, el método de 
procesamiento de eventos invocado en el bucle principal::

    result = DoMethod(obj, MUIM_Application_NewInput, (IPTR)&sigs);

+ Los parámetros no tienen una taglist, y así no terminan con ``TAG_DONE``.
+ Tienes que hacer la conversión de los punteros a ``IPTR`` para evitar las
  advertencias.

-----------
Hola, mundo
-----------

.. Figure:: zune/images/hello.png

    First things first! I knew you would be all excited.
    ¡Primero lo primero! Sé que estarás exitado.


Fuente
======

Estudiemos nuestro primero ejemplo de la vida real::

    // gcc hello.c -lmui
    #include <exec/types.h>
    #include <libraries/mui.h>
    
    #include <proto/exec.h>
    #include <proto/intuition.h>
    #include <proto/muimaster.h>
    #include <clib/alib_protos.h>
    
    int main(void)
    {
        Object *wnd, *app, *but;
    
        // GUI creation
    	app = ApplicationObject,
    	    SubWindow, wnd = WindowObject,
    		MUIA_Window_Title, "Hello world!",
    		WindowContents, VGroup,
    		    Child, TextObject,
    			MUIA_Text_Contents, "\33cHello world!\nHow are you?",
    			End,
    		    Child, but = SimpleButton("_Ok"),
    		    End,
    		End,
    	    End;
    
    	if (app != NULL)
    	{
    	    ULONG sigs = 0;
    
            // Click Close gadget or hit Escape to quit
    	    DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
                     (IPTR)app, 2,
                     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    
            // Click the button to quit
    	    DoMethod(but, MUIM_Notify, MUIA_Pressed, FALSE,
                     (IPTR)app, 2,
                     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    
            // Open the window
    	    set(wnd, MUIA_Window_Open, TRUE);

            // Check that the window opened
    	    if (XGET(wnd, MUIA_Window_Open))
    	    {
                // Main loop
    		while((LONG)DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)
    		      != MUIV_Application_ReturnID_Quit)
    		{
    		    if (sigs)
    		    {
    			sigs = Wait(sigs | SIGBREAKF_CTRL_C);
    			if (sigs & SIGBREAKF_CTRL_C)
    			    break;
    		    }
    		}
    	    }
	    // Destroy our application and all its objects
    	    MUI_DisposeObject(app);
    	}
    	
    	return 0;
    }


Observaciones
=============

General
-------

No abrimos a mano las bibliotecas, se hace automáticamente para nosotros.

Creación de la GUI
------------------

Usamos un lenguaje basado en macros para facilitar la construcción
de nuestra GUI.
Una aplicación Zune tiene siempre 1 y solamente 1 objeto Application::

    :	app = ApplicationObject,

Una aplicación puede tener 0, 1 o más objetos Window. Lo más común es 
uno solo::

    :	    SubWindow, wnd = WindowObject,

Sé bueno, dale un título a la ventana::

    :		MUIA_Window_Title, "Hello world!",

Una ventana debe tener 1 y sólo 1 hijo, lo usual es un grupo. Éste es vertical,
lo que significa que sus hijos estarán acomodados verticalmente::

    :		WindowContents, VGroup,

Un grupo al menos debe tener 1 hijo, aquí es sólo un texto::

    :		    Child, TextObject,

Zune acepta varios códigos de escape (aquí, para centrar el texto)
y los saltos de renglón::

    :			MUIA_Text_Contents, "\33cHello world!\nHow are you?",

Y una macro ``End`` debe corresponder a cada macro ``xxxObject``
(aquí, TextObject)::

    :			End,

Agreguemos un segundo hijo a nuestro grupo,¡un botón! Con un atajo de 
teclado ``o`` indicado por un subrayado::

    :		    Child, but = SimpleButton("_Ok"),

Terminar el grupo::

    :		    End,

Terminar la ventana::

    :		End,

Terminar la aplicación::

    :	    End;

Entonces, ¿quién necesita todavía un constructor de GUI? :-)


Manejo de errores
-----------------

Si alguno de los objetos en el árbol de la aplicación no se puede crear,
Zune destruye todos los objetos ya creados y la creación de la aplicación
falla. Si no, tienes una aplicación totalmente en funcionamiento::

    :	if (app != NULL)
    :	{
    :	    ...

Cuando estés listo, sólo invoca ``MUI_DisposeObject()`` sobre tu objeto
aplicación para destruir todos los objetos actuales en la aplicación,
y libera todos los recursos::

    :       ...
    :	    MUI_DisposeObject(app);
    :	}


Notificaciones
--------------

Las notificaciones son la manera más simple para reaccionar a los
eventos. ¿El principio? Queremos ser notificados cuando cierto atributo
de cierto objeto sea puesto a un cierto valor::

    :        DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,

Aquí escucharemos al ``MUIA_Window_CloseRequest`` de nuestro objeto
Window y seremos notificados cuando este atributo sea puesto a ``TRUE``.
¿Qué pasa cuando se dispara una notificación? Se envía un mensaje a un
objeto, aquí le diremos a nuestra Application que salga
``MUIV_Application_ReturnID_Quit`` en la siguiente vuelta del bucle
de eventos::

    :                 (IPTR)app, 2,
    :                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

Puesto que podamos especificar cualquier cosa que queramos aquí, tenemos
que decir el número de parámetros extra que estamos suministrando a
MUIM_Notify: aquí, 2 parámetros.

Para el botón, escuchamos a su atributo ``MUIA_Pressed``: es puesto a 
``FALSE`` cuando el botón es *soltado* (reaccionar cuando es apretado es
una mala práctica, podrías querer liberar el ratón afuera del botón para
cancelar tu acción - además queremos ver cómo se ve cuando es apretado).
La acción es la misma que la anterior, envía un mensaje a la aplicación::

    :        DoMethod(but, MUIM_Notify, MUIA_Pressed, FALSE,
    :                 (IPTR)app, 2,
    :                 MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


Abrir la ventana
----------------

Las ventanas no se abren hasta que tú se lo pides::

    :        set(wnd, MUIA_Window_Open, TRUE);

Si todo va bien, tu ventana debería mostrarse en este punto. ¡Pero puede
fallar! Así que no olvides revisar eso consultando el atributo, que 
debería ser TRUE::

    :        if (XGET(wnd, MUIA_Window_Open))


El bucle principal
------------------

Déjame presentarte a mi leal amigo, el bucle de eventos ideal de Zune::

    :        ULONG sigs = 0;

No olvides inicializar las señales a 0 ...
La prueba del bucle es el método MUIM_Application_NewInput::

    :        ...
    :        while((LONG) DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)
    :              != MUIV_Application_ReturnID_Quit)

Toma como entradas las señales de los eventos que tiene que procesar
(el resultado de ``Wait()`` o 0), modificará este valor para ubicar las
señales que Zune está esperando (para el siguiente ``Wait()``) y 
devolverá un valor. Este mecanismo de valor devuelto históricamente
era la única manera para reaccionar a los eventos, pero era feo 
y ha quedado obsoleto favor de las clases custom y el diseño OO.

El cuerpo del bucle está casi vacío, solamente esperamos señales y 
manejamos Ctrl-C para salir del bucle::

    :        {
    :            if (sigs)
    :            {
    :                sigs = Wait(sigs | SIGBREAKF_CTRL_C);
    :                if (sigs & SIGBREAKF_CTRL_C)
    :                    break;
    :            }
    :        }


Conclusión
----------

Este programa te inicia con Zune, y te permite jugar con el diseño
de la GUI, pero nada más.


------------------------
Acciones de notificación
------------------------

Como vimos en hello.c, usas MUIM_Notify para invocar un método si ocurre
cierta condición.
Si quieres que tu aplicación reaccione de una manera específica a los
eventos, puedes usar uno de estos planes:

+ MUIM_Application_ReturnID: puedes pedirle a tu aplicación que devuelva
  un ID arbitrario en la siguiente iteración del bucle, y revisar el
  valor en el bucle. Es la vieja y sucia manera de hacer las cosas.
+ MUIM_CallHook, para llamar al standard callback hook del Amiga: esta
  es la elección promedio, no OO pero tampoco fea.
+ un método custom: el método pertenece a una de tus clases custom.
  Ésta es la mejor solución que soporta el diseño OO en la aplicaciones.
  Requiere que crees las clases custom, puede que no sea lo más fácil
  para los principiantes o para la gente apurada.

==========================================
Manual para el Desarrollo del Sistema AROS
==========================================

:Authors:   Aaron Digulla, Bernardo Innocenti
:Copyright: Copyright © 2001, The AROS Development Team
:Version:   $Revision: 23374 $
:Date:      $Date: 2005-06-17 18:41:00 +1000 (Fri, 17 Jun 2005) $

.. Warning::

   ¡Este documento no está terminado! Es muy probable que muchas
   partes estén desactualizadas, contengan información incorrecta o
   simplemente falten. Si quieres ayudar a rectificar esto,
   por favor, contáctanos.

.. Contents::

--------------------------
Política de licenciamiento
--------------------------

Casi todo el código escrito por el AROS Development Team está licenciado con
la AROS Public License (APL), y es la elección recomendada para todo el
nuevo código escrito para el proyecto.

Sin embargo, nos damos cuenta que esto no siempre es posible; por ejemplo,
a menudo queremos usar las buenas bibliotecas y aplicaciones de terceros en vez 
de inventar la rueda otra vez por nosotros mismos. Por lo tanto se permite importar
código ajeno en el depósito SVN que no esté licenciado con la APL, mientras
la licencia de ese código satisfaga los siguientes requerimientos.


Requerimientos sobre el código fuente del árbol contrib
=======================================================

Para incluir código fuente en el árbol contrib, se deben cumplir los
siguientes requerimientos:

1. La licencia nos debiera permitir:

   a. Redistribuir los fuentes.
   b. Redistribuir los binarios.

   En el caso que sean necesarios cambios al código fuente para poderlo
   compilar y hacerlo funcionar en AROS, entonces la licencia además nos debiera
   permitir hacer las modificaciones y redistribuir los fuentes y binarios que
   contengan aquellas modificaciones.

2. La licencia debe estar expresada por escrito en un archivo llamado LEGAL
   ubicado en el directorio raíz de los fuentes a los que se aplica.

   
Requerimientos en el código fuente en el árbol principal de AROS
================================================================

Para incluir código fuente ajeno, que no esté licenciado con la APL en el
árbol principal de AROS se deben cumplir los siguientes requerimientos:

1. Que el código fuente se necesite para build algún componente o sea necesario
   para algún otro componente (que podría o no ser APL) que querramos que esté
   incluído en la distribución base binaria de AROS.

2. Que la licencia deba ser de fuente abierta como se define en la
   Open Source Initiative (OSI), lo que significa que debe permitirnos:

   a. Hacer modificaciones.
   b. Redistribuir el fuente (posiblemente con modificaciones).
   c. Redistribuir los binarios (posiblemente basados en los fuentes modificados).

3. Que la licencia no tenga conflictos con la APL:
   
   a. Si se trata de un programa separado, casi toda licencia que cumple con
      (2) está permitida.

   b. Si se trata de una biblioteca, la licencia debe permitir enlazar con
      programas y bibliotecas que usen una licencia diferente sin ningún 
      problema. Esto significa que las bibliotecas que estén cubiertas por
      la GPL (no la LGPL) no se permiten.

4. La licencia debe estar expresada por escrito en un archivo llamado LEGAL
   ubicado en el directorio raíz de los fuentes a los que se aplica.
   

----------------------------
Convenciones de codificación
----------------------------

Estilo general
==============

Este código es usado por mucha gente y por lo tanto debieras tener algunas
cosas en mente cuando envíes código fuente:

+ Conservar simple las cosas
+ Conservar limpio el código
+ Siempre debes saber qué haces
+ Dí lo que estás haciendo
+ Recuerda que el código se escribe una vez pero se lee muchas otras y por bastantes personas


Los comentarios
===============

AROS usa parte de los comentarios en los fuentes para generar la documentación.
Por lo tanto, es necesario mantener cierto formato para que las herramientas
puedan encontrar su información. Los otros comentarios son ignorados aunque
deberían explicar lo que pensabas cuando escribías el código. Si de verdad
no puedes pensar en una explicación, entonces no escribas the code a 
second time like this::


    /* Esto suma 1 a t */
    t++;
    
Lo que pensamos de esto es::

    /* Ir al siguiente elemento */
    t++;


Los prototipos y las cabeceras de función
=========================================

Toda función en AROS debe tener un prototipo ANSI C completo. Los prototipos
deberían estar reunidos en una cabecera por archivo si son necesarios para
unos pocos archivos (no se necesita recompilar el proyecto entero si cambias
una función que se usa una sola vez), en una cabecera por directorio si es
muy usada en ese directorio, o en una cabecera por grupo lógico (por ej.
una cabecera para todas las funciones en una biblioteca).

La cabecera de función (es decir, el comentario que precede a la función) debe
tener un formato especial porque los AutoDocs se generan de allí. Aquí está
un ejemplo (de <filename>AROS/exec/addhead.c</filename>)::

    /*****************************************************************************

        NAME */
    #include <exec/lists.h>
    #include <clib/exec_protos.h>

        AROS_LH2I(void, AddHead,

    /*  SYNOPSIS */
            AROS_LHA(struct List *, list, A0),
            AROS_LHA(struct Node *, node, A1),

    /*  LOCATION */
            struct ExecBase *, SysBase, 40, Exec)

    /*  FUNCTION
            Insert Node node as the first node of the list.

        INPUTS
            list - The list to insert the node into
            node - This node is to be inserted

        RESULT
            None.

        NOTES

        EXAMPLE
            struct List * list;
            struct Node * pred;

            // Insert Node at top
            AddHead (list, node);

        BUGS

        SEE ALSO
            NewList(), AddTail(), Insert(), Remove(), RemHead(), RemTail(),
            Enqueue()

        INTERNALS

    ******************************************************************************/
    {

Como ves, los comentarios se usan para mezclar el prototipo de función y la
cabecera en uno.

NAME 
    Este campo contiene todos los prototipos necesarios para usar la función
    desde el punto de vista del usuario, y el nombre de la función en una
    macro `AROS_LH#?()` (Library Header, Cabecera de Biblioteca). Estas macros
    se usan para hacer que el mismo código funcione en diferente hardware. El
    nombre depende de la cantidad de parámetros y si la función necesita de la
    de la biblioteca base. `AddHead()` no, y por lo tanto se anexa una "I" al
    nombre de la macro. Si se necesita la biblioteca base (como lo hace `AddTask()`),
    entonces se omite la "I".

    Si la función no es parte de una biblioteca compartidad y sus argumentos 
    deben ser pasados a ciertos registros (por ej. las callback hooks), debes 
    usar las macros `AROS_UFH#?()` (User Function Header, Cabecera de Función del
    Usuario) en lugar de `AROS_LH#?()`. Añade la cantidad de argumentos a esta
    macro. Dado que nunca tiene una base, el campo LOCATION se debe omitir
    y no se necesita agregar la "I" al nombre de la macro. Un ejemplo para una
    callback hook `foo()` sería::

        AROS_UFH3(ULONG, foo,
            AROS_UFHA(struct Hook, hook,  A0),
            AROS_UFHA(APTR,        obj,   A2),
            AROS_UFHA(APTR,        param, A1)
        )

    (Nota que los registros no necesitan tener un orden en particular).

    Si la función no es parte de una biblioteca compartidad y sus argumentos no
    necesitatn estar en registros específicos, no necesitas las macros `AROS_#?H#?()`::

        /*****************************************************************************

            NAME */
        #include <header.h>

	        int foo (

        /*  SYNOPSIS */
	        int a,
	        int b)

        /*  FUNCTION
	        blahblahblah.
	        ...

        *****************************************************************************/
        
SYNOPSIS 
    Este campo contiene todos los argumentos de la función uno
    por uno en las macros `AROS_LHA()` (Library Header Argument, Argumento de
    la Cabecera de Biblioteca). Estas macros aseguran que el argumento respectivo
    es puesto en el registro correcto de la CPU cuando se invoca la función
    (si es posible y es necesario). El primer argumento para la macro es el
    tipo del parámetro seguido del nombre del parámetro y el registro en el
    que el parámetro espera estar. Los nombres válidos para los registros son
    D0, D1, D2 hasta D7 y A0 hasta A6.

    Si la función no es parte de una biblioteca pero los argumentos deben ser
    pasados a los registros, entonces usa las macros `AROS_UFHA()` (User 
    Function Header Argument, Argumento de la Cabecera de Función del Usuario)
    que toma los mismos parámetros que las macros `AROS_LHA()`. No olvides
    cerrar los paréntesis de la `AROS_UFC`.

    Si la función no es parte de una biblioteca y los argumentos no necesitan
    ser pasados a los registros, no son necesarias las macros.

LOCATION
    Este campo solamente es necesario para las bibliotecas compartidas. Contiene
    los últimos cuatro parámetros de la macro `AROS_LH#?()` que son el tipo de
    la biblioteca, el nombre de la variable, en qué biblioteca base se espera la
    función, el desplazamiento de la función en la tabla de salto (el primer
    vector tiene 1 y el primer vector que podría ser usado por una función es 5)
    y el nombre de la biblioteca.

FUNCTION
    Este campo contiene una descripción de la función.

INPUTS
    Este campo contiene una lista de todos los parámetros de la forma
    "nombre - descripción" o "nombre, nombre, nombre - descripción". La descripción
    debería decir qué es el parámetro y qué valores puede tener. There is 
    no point es explicar el parámetro dos veces, en FUNCTION y aquí. Si la función
    no tiene parámetros, pon "None." aquí.

RESULT
    Lo que devuelve la función. Esto incluye los valores devueltos y los 
    valores pasados en los argumentos de la función. Si la función puede fallar,
    deberías explicar qué regresa en caso de falla y por qué podría fallar.

NOTES
    Las cosas importantes que el usuario debiera conocer o tener en cuenta.

EXAMPLE
    Este ejemplo debería contener un ejemplo pequeño o con todas las características.
    Una buena manera de presentar un ejemplo es escribir algo de código que pruebe
    la función, ponlo en `#ifdef TEST` en alguna parte del archivo y pon 
    "See below" aquí. Si necesitas que haya comentarios en el código, tienes dos maneras.
    Si solamente necesitas comentarios breves de un renglón, usa el estilo
    de C++ (``//``). Todo desde ``//`` hasta el final del renglón es el comentario.
    Si necesitas comentar más, entonces puedes terminar el comentario después de
    `EXAMPLE` y usar `#ifdef EXAMPLE` para enmascarar el ejemplo::

            EXAMPLE */
        #ifdef EXAMPLE
	        struct List * list;
	        struct Node * pred;

	        /* Insert Node at top of the list */
	        AddHead (list, node);
        #endif

    No uses `#ifdef EXAMPLE` si tienes un ejemplo completo (es decir, uno que se
    pueda compilar sin errores).


BUGS
    Este campo contiene una lista de los errores conocidos.

SEE ALSO
    Este campo contiene una lista de las otras funciones y documentos que
    podrían ser de interés. Esto incluye la función que necesitas para inicializar,
    crear o destruir un objeto que necesite esta función, las funciones 
    que hagan cosas similares u opuestas a las del objeto principal.

    Por ejemplo, `SetAttrs()` debería contener aquí las funciones para crear, 
    destruir y manipular los objetos BOOPSI pero no las taglists.

INTERNALS
    Este campo debería contener la información para los otros desarrolladores que
    sea irrelevante para el usuario, por ejemplo una explicación del algoritmo
    de la función o sus dependencias.


Formateo
========

Aquí está un ejemplo de cómo se formatea el código de AROS::

    {
        /* a */
        struct RastPort * rp;
        int               a;

        /* b */
        rp = NULL;
        a  = 1;

        /* c */
        if (a == 1)
            printf ("Init worked\n");

        /* d */
        if
        (
            !(rp = Get_a_pointer_to_the_RastPort
                (
                    some
                    , long
                    , arguments
                )
            )
        ||
            a <= 0
        )
        {
            printf ("Something failed\n");
            return FAIL;
        }

        /* e */
        a = printf ("My RastPort is %p, a=%d\n"
            , rp
            , a
        );

        return OK;
    }


Se ve feo, ¿eh? :-) De acuerdo, aquí vienen las reglas:

+ Si varios renglones tienen código similar, pon las cosas similares
  una debajo de la otra (mira a y b);

+ Pon espacios entre los operandos y los operadores

+ Pon las aperturas y cierres de llaves ``{}``, corchetes ``[]`` y paréntesis
  ``()`` alineadas (d) si hay mucho código en el bloque que encierran (c).
  Los corchetes y los paréntesis pueden estar en el mismo renglón si el
  código entre ellos es pequeño (c)

+ La sangría debe ser de 4 espacios. Dos niveles de sangría pueden abreviarse
  con una tabulación.

  Las razones para esto son:
  
  1. Mientras que algunos editores pueden usar tamaños arbitrarios para las
     tabulaciones, es un poco complicado indicarle a otro editor qué
     tamaño de tabulación se usó en el editor con que se escribió el código.
  2. La mayoría del código en AROS fue escrito de esta manera y tu código
     debería verse como el resto.
  3. Puedes imprimir este código en cualquier impresora sin la necesidad
     de herramientas especiales para "arreglar" las tabulaciones.
  4. La mayoría de los editores tienen tabulaciones inteligentes que hacen
     exactamente esto. Si tu editor no lo hace, escribe un informe de error.

+ Si tienes una función con muchos argumentos (d, e) deberías poner los
  paréntesis en renglones aparte y cada argumento en su propio renglón (d)
  o poner el primer argumento detrás del paréntesis de apertura (e) y
  los sucesivos argumentos en un renglón distinto con una coma delante.
  El paréntesis de cierre debe estar en su propio renglón y alineado
  con el comienzo de la expresión (es decir, el a y no el paréntesis de
  apertura o el `printf()`).

+ Usa un renglón vacío para separar los bloques lógicos. Los comentarios 
  largos deberían tener un renglón vacío antes y después, los comentarios
  cortos deberían suceder al código que explican con un único renglón
  vacío de separación.


Escribir el código para ROM
===========================

El código de los módulos de AROS debería ser escrito de modo que se
ajuste para incorporarlo en una ROM, FlashRAM u otro tipo de memoria
de solo-lectura. Están previstas las siguientes reglas de estilo de 
codificación para que eso sea posible. Por supuesto, que se aplican
a todos los módulos Kickstart y al código que podría ser residente,
compartido o enlazado a otros módulos.

+ Los módulos ROM no deben tener las secciones .data y .bss.
  Básicamente, necesitamos quitar todos los datos globales no constantes.
  El Amiga Kickstart auténtico prueba que es posible y fácil lograr esto.

  Si encuentras una variable externa (estática o no) que sea 
  modificada por el código, prueba a quitarla o moverla a la base de
  biblioteca/dispositivo (o en el nodo dispositivo de tu manejador o en los
  datos de usuario de tu clase).

+ Lo de arriba también se aplica a las bases de las bibliotecas. Si estás
  escribiendo una biblioteca, pon las bases de las otras bibliotecas en
  la estructura base de tu propia bibliteca. Las clases BOOPSI pueden
  almacenar las bases de la biblioteca en sus datos privados de la clase.

+ Intenta establecer los atributos `static` y `const` para todos tu datos
  globales. También puedes usar los tipos `CONST_STRPTR` y `CONST_APTR`
  definidos en <exec/types.h>. Usar `static const` permite que el 
  compilador mueva los datos al segmento ".text" (más conocido como código).
  Si necesitas pasar estas variables globales a otra función, prueba a 
  cambiar su prototipo para usar `const` también. Fíjate que, del OS 3.5,
  Olaf Barthel ha cambiado para usar `const` en las cabeceras <clib/#?_protos.h>.

+ **NUNCA JAMÁS** toques los búferes pasados por el usuario como
  un parámetro "input". El concepto de los parámetros de entrada 
  está implícito en la descripción de la función. Por ejemplo,
  el nombre de archivo pasado a `Open()` claramente es una variable
  de entrada y `Open()` no debe mess con ella, incluso si va a 
  dejarla como estaba después. Ten en cuenta, que el búfer podría
  estar en una memoria de solo-lectura o compartida entre varias
  instancias de un programa residente o multihilo.

+ Intenta evitar las llamadas del OS anfitrión como `malloc()` y
  `free()` si puedes usar `AllocMem()` y `FreeMem()`. Porque
  las macros de depuración que revisan el puntero se basan en
  encontrar al puntero dentro de los bloques de memoria de Exec
  con `TypeOfMem()`.
  
-------
Porting
-------

Este archivo describe como pasar AROS a un nuevo tipo de hardware.

1. Selecciona un nombre identificatorio para tu CPU (por ej. i386, m68k,
   hppa, sparc) y añádele "-emul" (por ej. i386-emul) si tu puerto
   se ejecutará como un "sub-OS" o "-native" (por ej. m68k-native) si el
   puerto será un OS separado.

2. Selecciona un nombre identificatorio para tu sistema (por ej. sgi, linux, amiga, etc.).

3. Edita "configure" y make reconocerá tu tipo de hardware y acomodará las
   numerosas variables que necesite tu sistema.

   KERNEL
   	El tipo de CPU que usas (mira 1.)
   
   ARCH
   	El nombre de tu sistema (mira 2.)
    
   SYS_CC
   	El nombre de tu compilador de C
    
   COMMON_CFLAGS
   	Las opciones que deberías ser consideradas por cada invocación al
	compilador de C (por ej. -g -Wall -O0 etc.)
   
   ILDFLAGS
   	Las banderas que debes darle al compilador cuando enlaces para
	evitar usar cualquier biblioteca estándar o módulos de inicio
	(para GCC las opciones son -nostartfiles -nostdlib -Xlinker -i).
	Esto se usa para crear los ejecutables de AROS. Estos ejecutables 
	no deben tener ningún símbolo sin resolver y todas las referencias
	deben estar verificadas.
    
   RANLIB
   	contiene el nombre de tu programa ranlib. Si no tienes uno
	especifica "true" (o el nombre de cualquier otro comando del shell
	que sencillamente ignore todos los parámetros y no devuelva 
	ningún código de error).

4. Tipea "make". Abortará porque no hay $(KERNEL) aún, pero configurará
   algunos archivos importantes y árboles de directorio.

5. Haz una copia de i386-emul a $(KERNEL) y convierte todos fuentes en
   ensamblador de x86 a tu CPU.

6. Llena $(KERNEL)/. Se recomienda que hagas una copia de i386-emul, porque
   es la versión más al día del núcleo.

7. Tipea "make machine". Compilará un programa y lo ejecutará. La salida
   se puede usar para modificar $(KERNEL)/machine.h.

8. Ejecuta "make machine.i" en $(KERNEL). Generará el archivo "machine.i"
   que necesitarás para compilar los archivos en ensamblador. "machine.i"
   contiene los valores de numerosas constantes del sistema (los
   desplazamientos de los vectores de función, los desplazamientos de los
   campos de estructura y las banderas del sistema).

9. Edita todos los archivos #?.s en $(KERNEL) y genera el código máquina
   adecuado para tu CPU. Para compilar los archivos, tipea "make".

10. Ve al directorio principal y tipea "make". Si hay algún error, escríbelos,
    luego corrígelos y sigue con el paso 10.

11. Ve a bin/$(ARCH)/AROS e inicia "arosshell". Ahora puedes escribir algunos
    comandos (por ej. "dir all", "list" o "demowin"). Si todo va bien, tienes
    una lista de los directorios y archivos con "dir all" y "demowin" abre
    una ventana con algunos gadgets y renderings con los que puedes jugar.
    Tipea "Esc" o aprieta en "Exit" para salir de la demo. Para detener el
    arosshell, debes apretar ^C (Ctrl-C) puesto que como un verdadero OS no
    hay manera de pararlo limpiamente.

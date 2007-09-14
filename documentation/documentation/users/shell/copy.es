====
Copy
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <changetaskpri>`_ `Next <date>`_ 

---------------

::

 Copy 

Synopsis
~~~~~~~~
::


     FROM/M, TO, ALL/S, QUIET/S, BUF=BUFFER/K/N, CLONE/S, DATES/S, NOPRO/S,
     COM=COMMENT/S, NOREQ/S,

     PAT=PATTERN/K, DIRECT/S,SILENT/S, ERRWARN/S, MAKEDIR/S, MOVE/S,
     DELETE/S, HARD=HARDLINK/S, SOFT=SOFTLINK/S, FOLNK=FORCELINK/S,
     FODEL=FORCEDELETE/S, FOOVR=FORCEOVERWRITE/S, DONTOVR=DONTOVERWRITE/S,
     FORCE/S


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Crea copias idénticas de uno o más archivos.


Entradas
~~~~~~~~
::


     FROM      --  muchos archivos de entrada.
     TO        --  archivo o directorio de destino.
     ALL       --  incluir los subdirectorios.
     QUIET     --  suprimir todas las salidas y los requesters.
     BUFFER    --  tamaño del búfer para el búfer de copia en
                   bloques de 512 bytes (por omisión 1024 (= 512K))
     CLONE     --  copiar los comentarios, los bits de protección y las fechas también.
     DATES     --  copiar las fechas.
     NOPRO     --  no copiar los bits de protección.
     COMMENT   --  copiar los comentarios de archivo.
     NOREQ     --  suprimir los requesters.

     PATTERN   --  el patrón que los nombres de archivo deben respetar.
     DIRECT    --  modo sólo copia: copia el archivo sin hacer pruebas u opciones.
     VERBOSE   --  muestra más información de salida.
     ERRWARN   --  no continuar si falla un archivo.
     MAKEDIR   --  producir directorios.
     MOVE      --  borrar el archivo fuente después de una copia exitosa.
     DELETE    --  no copiar, sino borrar el archivo.
     HARDLINK  --  hacer un hardlink al archivo fuente en lugar de copiar.
     SOFTLINK  --  hacer un softlink al archivo fuente en vez de copiar.
     FOLNK     --  hacer también vínculos a los directorios.
     FODEL     --  borrar también los archivos protegidos.
     FOOVR     --  también sobreescribir los archivos protegidos.
     DONTOVR   --  nunca sobreescribir el archivo de destino.
     FORCE     --  NO USE ÉSTE. Solamente para compatibilidad en la llamada.


 Descripciones más detalladas:

 FROM:
 El(los) archivo(s) de origen. Para los directorios, todos los archivos contenidos
 son archivos de origen. Puede tener patrones normales.

 TO:
 El archivo de destino o para muchos orígenes, el directorio de destino. Se crea
 el directorio de destino (incluídos todos los directorios padres necesarios).
 

 ALL:
 Revisa los directorios recursivamente.

 QUIET:
 La copia es completamente silenciosa. Realmente no hay ninguna salida, ¡tampoco
 requesters para discos faltantes u otros problemas!

 BUF=BUFFER:
 Especifica el número de búferes de 512 bytes a copiar. Por omisión son 200
 búferes [100KB de memoria]. La cantidad mínima es 1 pero no debería ser usado.

 PAT=PATTERN:
 PATTERN permite especificar un patrón normal dos, todos los archivos tienen
 que cumplirlo. Esto es útil con la opción ALL.

 Ejemplo:
 Cuando quieras borrar todos los archivos .info en un árbol de directorio,
 necesitarás esta opción: Copy DELETE #? ALL PATH #?.info

 CLONE:
 El comentario de archivo, la fecha y los bits de protección de los archivos
 de origen se copian al archivo o directorio de destino.

 DATES:
 La información de fecha del origen se copia al destino.

 NOPRO:
 Los bits de protección de los orígenes NO se copian. Entonces el destino
 tiene los bits por omisión [rwed].

 COM=COMMENT:
 El comentario de archivo se copia al destino.

 NOREQ:
 No se muestran las peticiones normales dos,
 cuando ocurre un error.


 DIRECT:
 Ciertos dispositivos no permiten algunos de los tipos de peticiones de
 paquetes usados por DOS. Esta opción es realmente un comando fácil de
 copia, que solamente abre los orígenes y destino directamente sin
 ninguna prueba ni verificación.
 Las opciones ALL, PAT, CLONE, DATES, NOPRO, COM, MAKEDIR, MOVE, DELETE, HARD,
 SOFT, FOLNK, FODEL, FOOVR, DONTOVR y los archivos múltiples de entrada no
 se pueden especificar junto con DIRECT. Esta opción necesita un archivo de
 entrada y uno de salida.
 Cuando quieras borrar un softlink, que ya no apunta a un archivo válido,
 necesitarás también esta opción.
 Ejemplo de uso: 'Copy DIRECT texto PRT:' para imprimir un archivo
 llamado texto. - COPY maneja bastantes casos como éste de modo automático,
 pero quizás a veces se necesite esta opción.

 VERBOSE:
 Copy proporciona salida informativa adicional.

 ERRWARN:
 Copy conoce y devuelve tres tipos de errores de la biblioteca dos.library:
 5   WARN    El procesamiento de un archivo falló, Copy salta este
             archivo y continúa con el siguiente.
 10  ERROR   Falló la creación de un directorio u ocurrió algún otro
             error grave. Copy termina después del aviso.
 20  FAIL    Pasó un error realmente grave (no hay memoria, falló Examine...)
             Copy termina después del aviso.
 Cuando se usa la opción ERRWARN, el resultado 5 (WARN) se vuelve el resultado
 10 (ERROR). Así que Copy aborta cada vez un error sucede.

 MAKEDIR:
 Todos los nombres especificados en el campo FROM se toman como directorios, los 
 que se deben crear.

 MOVE:
 Los archivos no se copian, sino que se mueven (o renombran). Esto significa que
 después de una operación mover el origen no existe más.

 DELETE:
 ¡Esto no copia nada, sino que borra los archivos de origen!

 HARD=HARDLINK:
 En vez de copiar los archivos, se crea un hard link. Esto solamente
 funciona cuando el destino está en el mismo dispositivo que el origen.

 SOFT=SOFTLINK:
 En vez de copiar directorios, se crea un soft link. Estos vínculos se
 usan también entre diferentes dispositivos. Los soft links se crean
 solamente para los directorios. Se saltan los archivos. La opción
 FORCELINK es siempre puesta a verdadera.
 NOTA: Los softlinks no están oficialmente soportados por el OS y
 pueden ser peligrosos. ¡Yo sugiero que no uses esta opción! Mira en la
 descripción de abajo.

 FOLNK=FORCELINK:
 Cuando es posible la vinculación de directorios, se necesita esta opción.
 Mira en la sección "About links" si hay problemas.

 FODEL=FORCEDELETE:
 Cuando está habilitada esta opción, los archivos también se borrar aunque
 estén protegidos de ser borrados.

 FOOVR=FORCEOVERWRITE:
 Cuando está habilitada esta opción, los archivos también se sobreescriben
 aunque estén protegidos contra la sobreescritura.

 DONTOVR=DONTOVERWRITE:
 Esta opción evita sobreescribir los archivos de destino.



Mira también
~~~~~~~~~~~~

::


     Delete, Rename, MakeDir, MakeLink



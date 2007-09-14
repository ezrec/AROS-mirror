==
If
==

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <getenv>`_ `Next <info>`_ 

---------------

::

 If 

Synopsis
~~~~~~~~
::


     NOT/S,WARN/S,ERROR/S,FAIL/S,,EQ/K,GT/K,GE/K,VAL/S,EXISTS/K


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Ejecuta todos los comandos en un bloque si la condición dada es
     verdadera (Un bloque es una sucesión de líneas de comandos
     terminada con un comando ELSE o ENDIF). Por cada comando IF debe
     haber un ENDIF correspondiente.
     Si la condición es false, la ejecución de comandos saltará al
     correspondiente comando ELSE o ENDIF.


Entradas
~~~~~~~~
::


     NOT               --  Niega el valor de la condición.

     WARN              --  Es verdadero si el código devuelto anterior
                           fue igual o mayor que 5.
     ERROR             --  Es verdadero si el código devuelto anterior
                           fue igual o mayor que 10.
     FAIL              --  Es verdadero si el código devuelto anterior
                           fue igual o mayor que 20.

     EQ, GE, GT        --  Es verdadero si el primer valor es igual,
                           igual o mayor, o mayor que el segundo valor
                           respectivamente.

     VAL               --  Indica que la comparación debería tratar a las
                           cadenas como valores numéricos.

     EXISTS  <cadena>  --  Es verdadero si existe el archivo o directorio
                           <cadena>.



Ejemplo
~~~~~~~
::


     If 500 GT 200 VAL
         echo "500 is greater than 200"
     Else
         If EXISTS S:User-Startup
             echo "User-Startup script found in S:"
             Execute S:User-Startup
         EndIf
     EndIf


Notas
~~~~~
::


     ERROR y FAIL solamente serán apropiados si el nivel de falla del
     guión es puesto mediante FailAt (el nivel de falla normal es 10 y si
     algún código devuelto iguala o supera este valor, el guión abortará).


Vea también
~~~~~~~~~~~
::


     Else, EndIf, FailAt



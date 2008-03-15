====
Eval
====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <endskip>`_ `Next <execute>`_ 

---------------

::

 Eval 

Synopsis
~~~~~~~~
::


     VALUE1/A,OP,VALUE2/M,TO/K,LFORMAT/K


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Evalúa una expresión entera e imprime el resultado. El resultado se
     escribe a la salida estándar si no se usa el parámetro TO en cuyo caso
     el resultado se imprime a un archivo. Usando el parámetro LFORMAT,
     es posible controlar cómo se escribe el resultado. Los números que
     tienen el prefijo 0x o el #x son interpretados como hexadecimales,
     y aquéllos con el prefijo # o 0 se interpretan como octales. Los 
     caracteres alfabéticos se indican por signo apóstrofo (') previo, y
     se evalúan igual que su equivalente ASCII.


Entradas
~~~~~~~~
::


     VALUE1,
     OP,
     VALUE2      --  La expresión a evaluar. Están soportados los siguientes
                     operadores:

                     Operator              Symbols
                     ----------------------------------
                     addition              +
                     subtraction           -
                     multiplication        *
                     division              /
                     modulo                mod, M, m, %
                     bitwise and           &
                     bitwise or            |
                     bitwise not           ~
                     left shift            lsh, L, l
                     right shift           rsh, R, r
                     negation               -
                     exclusive or          xor, X, x
                     bitwise equivalence   eqv, E, e
                     
                     Operador              Símbolos
                     -------------------------------------
                     suma                  +
                     resta                 -
                     producto              *
                     cociente              /
                     resto                 mod, M, m %
                     and entre bits        &
                     or entre bits         |
                     shift a la izq.       lsh, L, l
                     shift a la der.       rsh, R, r
                     negación              -
                     o exclusivo           xor, X, x
                     equivalencia de bits  eqv, E, e

      TO         --  Archivo en donde escribir el resultado
      LFORMAT    --  Como printf especifica qué escribir.
                     Los parámetros posibles son:
                     
                     %x  --  salida hexadecimal
                     %o  --  salida octal
                     %n  --  salida decimal
                     %c  --  salida de caracter (el caracter ASCII
                             correspondiente al valor del resultado).
                             
                     Especificando *n en la cadena LFORMAT, se añade
                     un retorno de carro a la salida.



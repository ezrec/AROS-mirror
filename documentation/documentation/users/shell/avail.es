=====
Avail
=====

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <assign>`_ `Next <beep>`_ 

---------------

::

 Avail [CHIP | FAST | TOTAL | FLUSH] [H | HUMAN]

Synopsis
~~~~~~~~
::


     CHIP/S, FAST/S, TOTAL/S, FLUSH/S, H=HUMAN/S        


Ubicación
~~~~~~~~~
::


     Workbench:C


Función
~~~~~~~
::


     Hace un resumen del uso y disponibilidad de la memoria del sistema.
     Para liberar la memoria no usada que todavía puede ser asignada
     (bibliotecas, dispositivos, fuentes y demás presente en la memoria
     pero que no está en uso actual), use la opción FLUSH.


Entradas
~~~~~~~~
::


     CHIP   --  muestra solamente la memoria "chip".
     FAST   --  muestra solamente la memoria "rápida".
     TOTAL  --  muestra información sobre la memoria sin considerar el tipo.
     FLUSH  --  retira las cosas innecesarias residentes en la memoria.
     HUMAN  --  muestra valores más legibles (Gigabytes como "G",
                Megabytes como "M", Kilobytes como "K").


Notas
~~~~~
::


     Las memorias "chip" y "rápida" están asociadas con la computadora
     Amiga y no se aplican a tu plataforma de hardware.



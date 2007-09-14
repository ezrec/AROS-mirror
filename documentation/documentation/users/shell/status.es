======
Status
======

.. This document is automatically generated. Don't edit it!

`Index <index>`_ `Prev <stack>`_ `Next <type>`_ 

---------------

::

 Status 

Synopsis
~~~~~~~~
::


     PROCESS/N,FULL/S,TCB/S,CLI=ALL/S,COM=COMMAND/K


Ubicación
~~~~~~~~~
::


     Workbench:c


Función
~~~~~~~
::


     Muestra información sobre los procesos que se están ejecutando
     dentro de los Shell/CLI.


Entradas
~~~~~~~~
::


     PROCESS      --  El número de identificación del proceso.

     FULL         --  Muestra toda la información sobre los procesos.

     TCB          --  Igual que Full, excepto que esta opción omite
                      el nombre del proceso.

     CLI=ALL      --  Default. Muestra todos los procesos.

     COM=COMMAND  --  Muestra el id de un proceso dado. Especifica el
                      nombre del comando.


Resultado
~~~~~~~~~
::


     Los códigos de error DOS normales.


Ejemplo
~~~~~~~
::


     Status

         Process  2: Loaded as command: c:status
         Process  3: Loaded as command: c:NewIcons
         Process  4: Loaded as command: GG:Sys/L/fifo-handler
         Process  5: Loaded as command: Workbench
         Process  6: Loaded as command: ToolsDaemon

     Status full

         Process  2: stk 300000, pri   0 Loaded as command: c:status
         Process  3: stk  4096, pri   0 Loaded as command: c:NewIcons
         Process  4: stk  4096, pri   0 Loaded as command: GG:Sys/L/fifo-handler
         Process  5: stk  6000, pri   1 Loaded as command: Workbench
         Process  6: stk  4000, pri   2 Loaded as command: ToolsDaemon


Vea también
~~~~~~~~~~~
::


     <dos/dosextens.h>



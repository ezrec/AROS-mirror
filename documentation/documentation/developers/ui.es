========================================
Guía de Estilo de la Interfaz de Usuario
========================================

:Authors:   Adam Chodorowski
:Copyright: Copyright © 2003, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$

.. FIXME: Introduction here...

.. Warning::

   ¡Este documento no está terminado! Si quieres ayudar a rectificar esto,
   por favor ponte en contacto con nosotros.

.. Contents::


------------
Las ventanas
------------

Preferencias
============

Las ventanas de preferencias son similares en apariencia a la ventanas
de diálogo, en que tienen una fila de botones a lo ancho del fondo y 
ningún gadget de cierre en la barra de título.

.. Figure:: ui/images/windows-prefs-titlebar.png

   Ejemplo de la barra de título de una ventana de preferencias. Note la
   ausencia del gadget de cierre.

.. Topic:: Justificación

   No hay un gadget de cierre porque su significado sería ambiguo. En
   otras palabras, no estaría claro para el usuario qué efecto lateral
   es exactamente cerrar la ventana. ¿Guarda las preferencias o abandona todos los
   cambios?
 
El siguiente conjunto de botones siempre está presente, ubicados de modo
horizontal en la ventana abajo (en este orden, de izquierda a derecha):

    Test (Probar)
        Aplica las configuraciones en la ventana para que tengan 
        efecto inmediatamente. No cierra la ventana.
        
    Revert (Revertir)
        Restaura las configuraciones en la ventana al estado que tenían
        cuando se abrió la ventana, y lo hace inmediatamente. No cierra 
        la ventana.
        
    Save (Guardar)
        Aplica las configuraciones en la ventana inmediatamente y
        las guarda de modo permanente [#]_. Cierra la ventana. Si no es
        posible guardar definitivamente las configuraciones (por ej. si
        el disco donde deberían guardarse es de solo-lectura) el botón
        está fantasmal.
        
    Use (Usar)
        Aplica las configuraciones en la ventana inmediatamente y 
        las guarda de modo temporal (solamente para esta sesión) [#]_.
        Cierra la ventana.
        
    Cancel (Cancelar)
        Restaura las configuraciones en la ventana al estado que tenían
        cuando se abrió la ventana y de inmediato. Cierra la ventana.

.. Topic:: Disposición

   Los botones están divididos en dos grupos, con Test y Revert en uno y
   Save, Use y Cancel en el otro, el primer grupo alineado a la izquierda,
   y el segundo alineado a la derecha. Hay un espacio para separarlos
   visualmente [#]_. Todos los botones tienen el mismo ancho, que debería
   ser tan pequeño como sea posible (cuando la ventana se agrande, solamente
   debería ensancharse el espacio entre los dos grupos y no los botones).
        
.. Figure:: ui/images/windows-prefs-buttons.png

   Ejemplo de la fila de botones en una ventana de preferencias.

.. [#] Guarda ambos a ``ENVARC:`` y ``ENV:``.
.. [#] Guarda solamente a ``ENV:``.
.. [#] Fíjese que los botones del grupo izquierdo no cierran la ventana,
       mientras que los del grupo derecho sí lo hacen. 

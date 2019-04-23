=======
gallium
=======

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`BltPipeResourceRastPort()`_            `CreatePipe()`_                         `CreatePipeScreen()`_                   `DestroyPipe()`_                        
`DestroyPipeScreen()`_                  
======================================= ======================================= ======================================= ======================================= 

-----------

BltPipeResourceRastPort()
=========================

Synopsis
~~~~~~~~
::

 void BltPipeResourceRastPort(
          PipeHandle_t pipe,
          struct pipe_resource * srcPipeResource,
          LONG xSrc,
          LONG ySrc,
          struct RastPort * destRP,
          LONG xDest,
          LONG yDest,
          LONG xSize,
          LONG ySize );

Function
~~~~~~~~
::

     Copies part of pipe resource onto rast port. Clips output by using layers of
     rastport.


Inputs
~~~~~~
::

     srcPipeResource - Copy from this pipe resource.
     xSrc, ySrc - This is the upper left corner of the area to copy.
     destRP - Destination RastPort.
     xDest, yDest - Upper left corner where to place the copy
     xSize, ySize - The size of the area to copy



----------

CreatePipe()
============

Synopsis
~~~~~~~~
::

 PipeHandle_t CreatePipe(
          struct TagItem * tags );
 
 PipeHandle_t CreatePipeTags(
          TAG tag, ... );

Function
~~~~~~~~
::

     Instantiates a gallium pipe.


Inputs
~~~~~~
::

     tags - a pointer to tags to be used during creation.


Tags
~~~~
::

     CPS_PipeFriendBitmap - a bitmap our pipe screen will target.
     CPS_PipeScreenDriver * - where to store the driver.
     CPS_GalliumInterfaceVersion - Indicates a version of gallium interface
         that a client is expected to receive. The client expected version
         must ideally match with the version that the driver provides,
         because gallium interface is not backwards compatible. This tag is
         required. Unless otherwise needed, the value
         GALLIUM_INTERFACE_VERSION should be passed.
         See also CreatePipeV.


Result
~~~~~~
::

     A valid pipe instance or NULL if creation was not successful.



----------

CreatePipeScreen()
==================

Synopsis
~~~~~~~~
::

 struct pipe_screen * CreatePipeScreen(
          PipeHandle_t pipe );

Function
~~~~~~~~
::

     Creates a gallium pipe screen.


Inputs
~~~~~~
::

     pipe - a pipe handle created using CreatePipe().


Result
~~~~~~
::

     A valid pipe screen instance or NULL if creation was not successful.



----------

DestroyPipe()
=============

Synopsis
~~~~~~~~
::

 void DestroyPipe(
          PipeHandle_t pipe );

Function
~~~~~~~~
::

     Destroys a peviously created pipe.


Inputs
~~~~~~
::

     pipe - a pipe handle created using CreatePipe().



----------

DestroyPipeScreen()
===================

Synopsis
~~~~~~~~
::

 void DestroyPipeScreen(
          PipeHandle_t pipe,
          struct pipe_screen * pscreen );

Function
~~~~~~~~
::

     Disposes the pipe screen


Inputs
~~~~~~
::

     handle - a pointer to pipe screen structure. A NULL pointer will be
         ignored.


Result
~~~~~~
::

     The pipe screen is freed. Don't use it anymore.




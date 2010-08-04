==========
bootloader
==========

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`GetBootInfo()`_                        
======================================= ======================================= ======================================= ======================================= 

-----------

GetBootInfo()
=============

Synopsis
~~~~~~~~
::

 APTR GetBootInfo(
          ULONG infoType );

Function
~~~~~~~~
::

     Return information from the bootloader


Inputs
~~~~~~
::

     infoType - The type of information requested. Valid types are:
       BL_Video      (struct VesaInfo*) - VESA video mode data
       BL_LoaderName (STRPTR)           - Name of bootloader
       BL_Args       (struct List *)    - Kernel arguments. Data item is struct Node.
                                          ln_Name points to argument text.
       BL_Drives     (struct List *)    - Legacy disk drives present in the system.
                                          Data item is struct DriveInfoNode.


Result
~~~~~~
::

     Pointer to data or NULL if not supplied




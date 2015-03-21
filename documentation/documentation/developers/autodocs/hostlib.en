=======
hostlib
=======

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`HostLib_DropInterface()`_              `HostLib_FreeErrorStr()`_               `HostLib_GetInterface()`_               `HostLib_GetPointer()`_                 
`HostLib_Lock()`_                       `HostLib_Unlock()`_                     
======================================= ======================================= ======================================= ======================================= 

-----------

HostLib_DropInterface()
=======================

Synopsis
~~~~~~~~
::

 void HostLib_DropInterface(
          APTR * interface );

Function
~~~~~~~~
::

     Free an array of symbol values obtained by HostLib_GetInterface()


Inputs
~~~~~~
::

     interface - A pointer to values array


Result
~~~~~~
::

     None


Notes
~~~~~
::

     This function appeared in v2 of hostlib.resource.



See also
~~~~~~~~

`HostLib_GetInterface()`_ 

----------

HostLib_FreeErrorStr()
======================

Synopsis
~~~~~~~~
::

 void HostLib_FreeErrorStr(
          char * error );

Function
~~~~~~~~
::

     Release error description string.
     In some dynamic library loaders (like in Windows) error strings
     are allocated dynamically. They need to be explicitly freed
     when not used any more.


Inputs
~~~~~~
::

     error  - Pointer to a string ro free.


Result
~~~~~~
::

     None.


Notes
~~~~~
::

     For portability sake, it is recommended to call this function on any
     error string returned by hostlib.resource. However you can omit this
     if you exactly know that your code runs only on UNIX-alike operating
     system (it's UNIX-specific library or driver).



----------

HostLib_GetInterface()
======================

Synopsis
~~~~~~~~
::

 APTR * HostLib_GetInterface(
          void * handle,
          const char ** symtable,
          ULONG * unresolved );

Function
~~~~~~~~
::

     Resolve array of symbols in the host operating system library.
     The result is a pointer to a dynamically allocated array of
     symbol values.


Inputs
~~~~~~
::

     handle     - An opaque library handle provided by HostLib_Open()
     symbable   - A pointer to a NULL-terminated array of symbol names
     unresolved - An optional location where count of unresolved symbols
                  will be placed. Can be set to NULL to ignore it.


Result
~~~~~~
::

     A pointer to a dynamically allocated array of symbol values or NULL if
     empty symbol table was given.


Notes
~~~~~
::

     Note that the resulting array will always have as many entries as there
     are in symbol names array. It some symbols (or even all of them) fail
     to resolve, corresponding entries will be set to NULL. You may supply
     a valid unresolved pointer if you want to get unresolved symbols count.
     
     Even incomplete interface needs to be freed using HostLib_DropInterface().

     Resulting values are valid as long as the library is open. For portability
     sake it's advised to free interfaces before closing corresponding libraries.

     This function appeared in v2 of hostlib.resource.



See also
~~~~~~~~

`HostLib_GetPointer()`_ 

----------

HostLib_GetPointer()
====================

Synopsis
~~~~~~~~
::

 void * HostLib_GetPointer(
          void * handle,
          const char * symbol,
          char ** error );


----------

HostLib_Lock()
==============

Synopsis
~~~~~~~~
::

 void HostLib_Lock();

Function
~~~~~~~~
::

     Acquire global host OS call semaphore.


Inputs
~~~~~~
::

     None.


Result
~~~~~~
::

     None.


Notes
~~~~~
::

     Host OS calls are typically not reentrant. You have to
     call this function before you may use any host OS API.
     Use HostLib_Unlock() function when you're done with it.

     This function has negative impact on AROS multitasking, so
     use it gently.



See also
~~~~~~~~

`HostLib_Unlock()`_ 

----------

HostLib_Unlock()
================

Synopsis
~~~~~~~~
::

 void HostLib_Unlock();

Function
~~~~~~~~
::

     Release global host OS call semaphore.


Inputs
~~~~~~
::

     None.


Result
~~~~~~
::

     None.



See also
~~~~~~~~

`HostLib_Lock()`_ 


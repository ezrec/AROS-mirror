===============
mathieeedoubbas
===============

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`IEEEDPAbs()`_                          `IEEEDPAdd()`_                          `IEEEDPCeil()`_                         `IEEEDPCmp()`_                          
`IEEEDPDiv()`_                          `IEEEDPFix()`_                          `IEEEDPFloor()`_                        `IEEEDPFlt()`_                          
`IEEEDPMul()`_                          `IEEEDPNeg()`_                          `IEEEDPSub()`_                          `IEEEDPTst()`_                          

======================================= ======================================= ======================================= ======================================= 

-----------

IEEEDPAbs()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPAbs(
          double y );

Function
~~~~~~~~
::

     Calculate the absolute value of the given IEEE double precision
     floating point number.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.


Result
~~~~~~
::

     x - absolute value of y.

     Flags:
       zero     : result is zero
       negative : 0
       overflow : 0



See also
~~~~~~~~

`IEEEDPNeg()`_ 

----------

IEEEDPAdd()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPAdd(
          double y,
          double z );

Function
~~~~~~~~
::

     Calculate the sum of two IEEE double precision numbers.


Inputs
~~~~~~
::

     y - first addend.
     z - first addend.


Result
~~~~~~
::

     x - sum of y and z.

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : result is too large or too small for IEEE DP format



----------

IEEEDPCeil()
============

Synopsis
~~~~~~~~
::

 double IEEEDPCeil(
          double y );

Function
~~~~~~~~
::

     Calculates the ceiling value of an IEEE double precision number.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.


Result
~~~~~~
::

     x - ceiling of y.

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



See also
~~~~~~~~

`IEEEDPFloor()`_ `IEEEDPFix()`_ 

----------

IEEEDPCmp()
===========

Synopsis
~~~~~~~~
::

 LONG IEEEDPCmp(
          double y,
          double z );

Function
~~~~~~~~
::

     Compares two IEEE double precision numbers.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.
     z - IEEE double precision floating point number.


Result
~~~~~~
::

     c -
         +1: y > z
          0: y = z
         -1: y < z

     Flags:
       zero     : y = z
       negative : y < z
       overflow : 0



See also
~~~~~~~~

`IEEEDPTst()`_ 

----------

IEEEDPDiv()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPDiv(
          double y,
          double z );

Function
~~~~~~~~
::

     Divides two IEEE double precision numbers.


Inputs
~~~~~~
::

     y - dividend.
     z - divisor.


Result
~~~~~~
::

     x - quotient.



----------

IEEEDPFix()
===========

Synopsis
~~~~~~~~
::

 LONG IEEEDPFix(
          double y );

Function
~~~~~~~~
::

     Converts an IEEE double precision floating point number to an integer.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.


Result
~~~~~~
::

     x - the closest signed 32-bit integer to y.

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : ieeedp out of integer-range



See also
~~~~~~~~

`IEEEDPFlt()`_ `IEEEDPFloor()`_ `IEEEDPCeil()`_ 

----------

IEEEDPFloor()
=============

Synopsis
~~~~~~~~
::

 double IEEEDPFloor(
          double y );

Function
~~~~~~~~
::

     Calculates the floor value of an IEEE double precision number.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.


Result
~~~~~~
::

     x - floor of y.

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



See also
~~~~~~~~

`IEEEDPCeil()`_ `IEEEDPFix()`_ 

----------

IEEEDPFlt()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPFlt(
          LONG y );

Function
~~~~~~~~
::

     Convert an integer into an IEEE double precision floating point
     number.


Inputs
~~~~~~
::

     y - a 32-bit integer.


Result
~~~~~~
::

     x - IEEE double precision floating point number.

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



See also
~~~~~~~~

`IEEEDPFix()`_ 

----------

IEEEDPMul()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPMul(
          double y,
          double z );

Function
~~~~~~~~
::

     Multiplies two IEEE double precision numbers.


Inputs
~~~~~~
::

     y - first multiplicand.
     z - second multiplicand.


Result
~~~~~~
::

     x - product.



----------

IEEEDPNeg()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPNeg(
          double y );

Function
~~~~~~~~
::

     Switches the sign of the given IEEE double precision floating point
     number.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.


Result
~~~~~~
::

     x - the negation of y.

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



See also
~~~~~~~~

`IEEEDPAbs()`_ 

----------

IEEEDPSub()
===========

Synopsis
~~~~~~~~
::

 double IEEEDPSub(
          double y,
          double z );

Function
~~~~~~~~
::

     Subtracts two IEEE double precision floating point numbers.


Inputs
~~~~~~
::

     y - minuend.
     z - subtrahend.


Result
~~~~~~
::

     x - difference.



See also
~~~~~~~~

`IEEEDPNeg()`_ 

----------

IEEEDPTst()
===========

Synopsis
~~~~~~~~
::

 LONG IEEEDPTst(
          double y );

Function
~~~~~~~~
::

     Compares an IEEE double precision floting point number against zero.


Inputs
~~~~~~
::

     y - IEEE double precision floating point number.


Result
~~~~~~
::

     c -
         +1: y > 0.0
          0: y = 0.0
         -1: y < 0.0

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



See also
~~~~~~~~

`IEEEDPCmp()`_ 


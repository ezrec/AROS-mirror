===============
mathieeesingbas
===============

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`IEEESPAbs()`_                          `IEEESPAdd()`_                          `IEEESPCeil()`_                         `IEEESPCmp()`_                          
`IEEESPDiv()`_                          `IEEESPFix()`_                          `IEEESPFloor()`_                        `IEEESPFlt()`_                          
`IEEESPMul()`_                          `IEEESPNeg()`_                          `IEEESPSub()`_                          `IEEESPTst()`_                          

======================================= ======================================= ======================================= ======================================= 

-----------

IEEESPAbs()
===========

Synopsis
~~~~~~~~
::

 float IEEESPAbs(
          float y );

Function
~~~~~~~~
::

     Calculate the absolute value of a given floating point number


Result
~~~~~~
::

     absolute value of y

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



----------

IEEESPAdd()
===========

Synopsis
~~~~~~~~
::

 float IEEESPAdd(
          float y,
          float z );

Function
~~~~~~~~
::

     Calculate the sum of two IEEE single precision numbers


Result
~~~~~~
::

     sum of y and z

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : result is too large or too small for IEEESP format



----------

IEEESPCeil()
============

Synopsis
~~~~~~~~
::

 float IEEESPCeil(
          float y );

Function
~~~~~~~~
::

     Calculate the least integer ieeesp-number
     greater than or equal to y


Result
~~~~~~
::

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



See also
~~~~~~~~

`IEEESPFloor()`_ 

----------

IEEESPCmp()
===========

Synopsis
~~~~~~~~
::

 LONG IEEESPCmp(
          LONG y,
          LONG z );

Function
~~~~~~~~
::

     Compares two ieeesp numbers


Result
~~~~~~
::

    +1 : y > z
     0 : y = z
    -1 : y < z

     Flags:
       zero     : y = z
       negative : y < z
       overflow : 0



----------

IEEESPDiv()
===========

Synopsis
~~~~~~~~
::

 float IEEESPDiv(
          float y,
          float z );

Function
~~~~~~~~
::

     Divide two IEEE single precision floating point numbers
     x = y / z;


Result
~~~~~~
::

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : result is out of range



----------

IEEESPFix()
===========

Synopsis
~~~~~~~~
::

 LONG IEEESPFix(
          float y );

Function
~~~~~~~~
::

     Convert ieeesp-number to integer


Result
~~~~~~
::

     absolute value of y

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : ieeesp out of integer-range



----------

IEEESPFloor()
=============

Synopsis
~~~~~~~~
::

 float IEEESPFloor(
          float y );

Function
~~~~~~~~
::

     Calculate the largest integer ieeesp-number
     less than or equal to fnum


Result
~~~~~~
::

     IEEE single precision floating point

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0 (???)


Example
~~~~~~~
::

    IEEESPFloor(10.5) = 10
    IEEESPFloor(0.5)  = 0
    IEEESPFloor(-0.5) = -1
    IEEESPFloor(-10.5)= -11



----------

IEEESPFlt()
===========

Synopsis
~~~~~~~~
::

 float IEEESPFlt(
          LONG y );

Result
~~~~~~
::

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : IEEE single precision number is not exactly the integer



----------

IEEESPMul()
===========

Synopsis
~~~~~~~~
::

 float IEEESPMul(
          float y,
          float z );

Function
~~~~~~~~
::

     Multiply two IEEE single precision numbers
     res = y * z;


Result
~~~~~~
::

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : result is out of range



----------

IEEESPNeg()
===========

Synopsis
~~~~~~~~
::

 float IEEESPNeg(
          float y );

Function
~~~~~~~~
::

     Switch the sign of the given ieeesp number


Result
~~~~~~
::

     -y

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0



----------

IEEESPSub()
===========

Synopsis
~~~~~~~~
::

 float IEEESPSub(
          float y,
          float z );

Function
~~~~~~~~
::

     Subtract two ieeesp numbers
     x = y-z;


Result
~~~~~~
::

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : result is out of range



----------

IEEESPTst()
===========

Synopsis
~~~~~~~~
::

 LONG IEEESPTst(
          float y );

Function
~~~~~~~~
::

     Compare a ieeesp-number against zero.


Result
~~~~~~
::

     +1 : y > 0.0
      0 : y = 0.0
     -1 : y < 0.0

     Flags:
       zero     : result is zero
       negative : result is negative
       overflow : 0




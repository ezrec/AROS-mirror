                                                      Copyright by Marcel Lemke

    Description of public UNACE v1.2b  June 1998
   ----------------------------------------------

     1. Features
     2. Platform compability
     3. Compiling
     4. Warranty
     5. Latest public UNACE
     6. Thanks

------------------------------------------------------------------------------


  1. Features
 -------------
      The public version of UNACE is limited in its functionallity:

        * no EMS/XMS support
        * decompression dictionary limited by the target system;
            this means that the 16bit version has a maximum of 32k only
        * no decryption
        * no wildcard-handling


  2. Platform compability
 -------------------------
      There are different makefiles and OS.H files in the directories of
      public UNACE. Just copy the files of the directory for the system
      you want to compile for. It might be necessary to modify different
      things a bit to get it working with new compilers.
      If you had to do so, please tell me about these changes!

      This source has been tested as 16bit and 32bit MS-DOS, 16bit and 32bit
      Windows (character mode), Amiga, PPC-Amiga and LINUX executable.

      If you have any suggestions to make it more compatible for
      other platforms tell me please:

        email: mlemke6413@aol.com


  3. Compiling
 --------------
      To get UNACE.EXE choose the correct makefile and copy it to the source
      directory to run your MAKE-utility on it.

      Choose a 32bit target if possible. Otherwhise there might be not enough
      memory to decompress archives with dictionaries larger than 32kb.


  4. Warranty
 -------------
      Marcel Lemke, the author of ACE and the original source of this
      extraction utility, disclaims any liability for any damage caused by:

        * using or misusing this software
        * any bugs introduced by modifying source or due to the
            compile/link environment

      You use/distribute the UNACE program compiled by you at your own risk.
      The original author (Marcel Lemke) cannot be held responsible for data
      loss or other damages and any consequences from this loss or damages.


  5. Latest public UNACE
 ------------------------
      Please make sure you have the latest UNACE source.
      Take a look on my web-site to do so:

        http://members.aol.com/mlemke6413/ace.html


  6. Thanks 
 -----------
      A lot of thanks to Wilfred van Velzen <wilfred@aobh.xs4all.nl> for
      porting UNACE to the Amiga and setting up the basic elements for
      general portability this way.


                     Happy programming ... Marcel Lemke


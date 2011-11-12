========
identify
========

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`IdAlert()`_                            `IdEstimateFormatSize()`_               `IdExpansion()`_                        `IdFormatString()`_                     
`IdFunction()`_                         `IdHardware()`_                         `IdHardwareNum()`_                      `IdHardwareUpdate()`_                   

======================================= ======================================= ======================================= ======================================= 

-----------

IdAlert()
=========

Synopsis
~~~~~~~~
::

 LONG IdAlert(
          ULONG id,
          struct TagItem * taglist );
 
 LONG IdAlertTags(
          ULONG id,
          TAG tag, ... );

Function
~~~~~~~~
::

     Get a human readable description of the alert ('Guru') code.


Inputs
~~~~~~
::

     Code    -- (ULONG) alert code, as defined in exec/alerts.h

     TagList -- (struct TagItem *) tags that describe further
                options.


Tags
~~~~
::

     IDTAG_DeadStr   -- (STRPTR) Alert type string (deadend or
                        recoverable). You may skip this tag if you do not
                        want to get the string.

     IDTAG_SubsysStr -- (STRPTR) String of the subsystem that caused
                        the alert (CPU, exec.library, ...). You may skip this tag
                        if you do not want to get the string.

     IDTAG_GeneralStr-- (STRPTR) General alert cause. You  may skip
                        this tag if you do not want to get the string.

     IDTAG_SpecStr   -- (STRPTR) Specified alert cause. You may skip
                        this tag if you do not want to get the string.

     IDTAG_StrLength -- (UWORD) Maximum length of the string buffer,
                        including termination. Defaults to 50.

     IDTAG_Localize  -- [V8] (BOOL) FALSE to get English strings
                        only, TRUE for localized strings. This is useful for
                        applications with English as only language. Defaults to TRUE.


Result
~~~~~~
::

     Error   -- (LONG) error code, or 0 if everything went fine.



----------

IdEstimateFormatSize()
======================

Synopsis
~~~~~~~~
::

 ULONG IdEstimateFormatSize(
          STRPTR string,
          struct TagItem * tags );
 
 ULONG IdEstimateFormatSizeTags(
          STRPTR string,
          TAG tag, ... );

Function
~~~~~~~~
::

     Estimates the size of the buffer that will contain the output
     of the format string when used on IdFormatString().


Inputs
~~~~~~
::

     String  -- (STRPTR) Format string
     Tags    -- (struct TagItem *) Tags, currently NULL or TAG_DONE.


Tags
~~~~
::

     None yet.


Result
~~~~~~
::

     Length  -- (ULONG) Length of the buffer size that will
                be able to hold the entire result.


Notes
~~~~~
::

     The returned size will be large enough to contain the result
     of a IdFormatString(). It is not necessarily the size of the
     resulting buffer (the result length of IdFormatString()).



See also
~~~~~~~~

`IdHardware()`_ `IdFormatString()`_ 

----------

IdExpansion()
=============

Synopsis
~~~~~~~~
::

 LONG IdExpansion(
          struct TagItem * taglist );
 
 LONG IdExpansionTags(
          TAG tag, ... );

Function
~~~~~~~~
::

     Gets the name and class of the expansion and it's manufacturer.


Inputs
~~~~~~
::

     TagList -- (struct TagItem *) tags that describe further options.


Tags
~~~~
::

     IDTAG_ConfigDev -- (struct ConfigDev *) ConfigDev structure
                        containing all information. You should use this tag if ever
                        possible, since there are more possibilities to recognize and
                        distinguish between a board.

     IDTAG_ManufID   -- (UWORD) Manufacturer ID if ConfigDev is not
                        provided. You must also provide IDTAG_ProdID!

     IDTAG_ProdID    -- (UBYTE) Product ID if ConfigDev is not
                        provided. You must also provide IDTAG_ManufID!

     IDTAG_ManufStr  -- (STRPTR) Pointer to a buffer space for the
                        manufacturer name. You may skip this tag if you do not want
                        to get this string.

     IDTAG_ProdStr   -- (STRPTR) Pointer to a buffer space for the
                        product name. You may skip this tag if you do not want
                        to get this string.

     IDTAG_ClassStr  -- (STRPTR) Pointer to a buffer space for the
                        product class. You may skip this tag if you do not want to get
                        this string.

     IDTAG_StrLength -- (UWORD) Buffer length, including
                        termination. Defaults to 50.

     IDTAG_Expansion -- [V6] (struct ConfigDev **) Use this tag to
                        easily traverse through the expansion board list. Init the
                        pointed variable with NULL. After each call, you will find
                        the current ConfigDev pointer in this variable. If you are
                        done, this function returns IDERR_DONE and the variable is set
                        to NULL. See example.

     IDTAG_Secondary -- [V7] (BOOL) If set to TRUE, identify will
                        warn about secondary expansions. E.g. some graphic boards
                        create more than one entry in the expansion list. Then, one
                        entry is the primary entry, and any additional are secondary.
                        This tag does only make sense when checking all mounted
                        expansions. Defaults to FALSE. (See Bugs)

     IDTAG_ClassID   -- [V8] (ULONG *) The ULONG field will be filled
                        with a numerical class ID of the expansion (see include file:
                        IDCID_...). IMPORTANT: You MUST be prepared to get a number
                        that does not match to any IDCID value. In this case, assume
                        IDCID_MISC.

     IDTAG_Localize  -- [V8] (BOOL) FALSE to get English strings
                        only, TRUE for localized strings. This is useful for applications
                        with English as only language. Defaults to TRUE.


Result
~~~~~~
::

     Error   -- (LONG) error code, or 0 if everything went fine.

                IDERR_NOLENGTH  -- IDTAG_StrLength has been set to 0!
                IDERR_BADID     -- IDTAG_ManufID and IDTAG_ProdID were
                                   out of range or one of them was missing.
                IDERR_DONE      -- Checked all expansions using the
                                   IDTAG_Expansion tag. This is not really an error.
                IDERR_SECONDARY -- This expansion is secondary to a primary
                                   expansion entry.


Example
~~~~~~~
::

     To check all expansion boards, you may use this code:

     void PrintExpansions(void)
     {
       struct ConfigDev *expans = NULL;
       char manuf[IDENTIFYBUFLEN];
       char prod[IDENTIFYBUFLEN];
       char pclass[IDENTIFYBUFLEN];

       while(!IdExpansionTags(
               IDTAG_ManufStr ,manuf,
               IDTAG_ProdStr  ,prod,
               IDTAG_ClassStr ,pclass,
               IDTAG_Expansion,&expans,
               TAG_DONE))
       {
         Printf("Current ConfigDev = 0x%08lx\n",expans);
         Printf("  Manufacturer    = %s\n",manuf);
         Printf("  Product         = %s\n",prod);
         Printf("  Expansion class = %s\n\n",class);
       }
     }


Notes
~~~~~
::

     This function isn't implemented yet.

     If the manufacturer or the product is not known, the string will be
     filled with its number.

     This call is guaranteed to preserve all registers except D0.


Bugs
~~~~
::

     You must also provide IDTAG_ProdStr if you want to use IDTAG_Secondary.



----------

IdFormatString()
================

Synopsis
~~~~~~~~
::

 ULONG IdFormatString(
          STRPTR string,
          STRPTR buffer,
          ULONG len,
          struct TagItem * tags );
 
 ULONG IdFormatStringTags(
          STRPTR string,
          STRPTR buffer,
          ULONG len,
          TAG tag, ... );

Function
~~~~~~~~
::

     The buffer will be filled with the format string until
     the format string terminates or the buffer size is reached.

     The format string may contain format tags, which are
     surrounded by dollar signs. Doing so, the printf formattings
     are kept for a following printf.

     Format tags are case sensitive!

     If you want to write a dollar sign, then double it: '$$'.

     These format tags are known:

             $SYSTEM$
             $CPU$
             $FPU$
             $MMU$
             $OSVER$
             $EXECVER$
             $WBVER$
             $ROMSIZE$
             $CHIPSET$
             $GFXSYS$
             $CHIPRAM$
             $FASTRAM$
             $RAM$
             $SETPATCHVER$
             $AUDIOSYS$
             $OSNR$
             $VMMCHIPRAM$
             $VMMFASTRAM$
             $VMMRAM$
             $PLNCHIPRAM$
             $PLNFASTRAM$
             $PLNRAM$
             $VBR$
             $LASTALERT$
             $VBLANKFREQ$
             $POWERFREQ$
             $ECLOCK$
             $SLOWRAM$
             $GARY$
             $RAMSEY$
             $BATTCLOCK$
             $CHUNKYPLANAR$
             $POWERPC$
             $PPCCLOCK$
             $CPUREV$
             $CPUCLOCK$
             $FPUCLOCK$
             $RAMACCESS$
             $RAMWIDTH$
             $RAMCAS$
             $RAMBANDWIDTH$
             $TCPIP$
             $PPCOS$
             $AGNUS$
             $AGNUSMODE$
             $DENISE$
             $DENISEREV$
             $EMULATED$
             $XLVERSION$
             $HOSTOS$
             $HOSTVERS$
             $HOSTMACHINE$
             $HOSTCPU$
             $HOSTSPEED$

     For their meanings, see the include file.


Inputs
~~~~~~
::

     String  -- (STRPTR) Format string

     Buffer  -- (STRPTR) Buffer to be filled with the result
                until the format string terminates or the buffer
                size is reached.

     Length  -- (ULONG) Length of the buffer, including the
                null termination.

     Tags    -- (struct TagItem *) For future compatibility.
                You must provide NULL or a pointer to TAG_DONE.


Result
~~~~~~
::

     Length  -- (ULONG) Length of the buffer that really
                has been used.


Example
~~~~~~~
::

     "Your CPU is a $CPU$ with $CPUCLOCK$ MHz"


Notes
~~~~~
::

     Remember that, unlike RawDoFmt(), the format tags must be
     surrounded, i.e. started and ended, by a dollar sign '$'.



See also
~~~~~~~~

`IdHardware()`_ `IdEstimateFormatSize()`_ 

----------

IdFunction()
============

Synopsis
~~~~~~~~
::

 LONG IdFunction(
          STRPTR libname,
          LONG offset,
          struct TagItem * taglist );
 
 LONG IdFunctionTags(
          STRPTR libname,
          LONG offset,
          TAG tag, ... );

Function
~~~~~~~~
::

     Decodes the offset of the provided library name into function name.

     This function requires the .fd files in a drawer with 'FD:' assigned
     to it. All files must have the standard file name format, e.g.
     'exec_lib.fd'.

     The appropriate .fd file will be scanned. The result will be
     cached until the identify.library is removed from system.


Inputs
~~~~~~
::

     LibName -- (STRPTR) name of the function's library, device
                or resource. All letters behind the point (and
                the point itself) are optional. The name is
                case sensitive.

                Examples: 'exec.library', 'dos', 'cia.resource'.

     Offset  -- (LONG) offset of the function. It must be a
                multiple of 6. You do not need to provide the
                minus sign.

                Examples: -456, 60

     TagList -- (struct TagItem *) tags that describe further
                options.


Result
~~~~~~
::

     Error   -- (LONG) error code, or 0 if everything went fine.

TAGS
     IDTAG_FuncNameStr   -- (STRPTR) Buffer where the function name
                            will be copied into.

     IDTAG_StrLength     -- (UWORD) Maximum length of the string buffer,
                            including termination. Defaults to 50.


Bugs
~~~~
::

     Every line in the .fd file must have a maximum of 254 characters.
     Otherwise the internal offset table may be corrupted (but the
     system won't be harmed). Anyhow, this should be no problem.



----------

IdHardware()
============

Synopsis
~~~~~~~~
::

 CONST_STRPTR IdHardware(
          ULONG type,
          struct TagItem * taglist );
 
 CONST_STRPTR IdHardwareTags(
          ULONG type,
          TAG tag, ... );

Function
~~~~~~~~
::

     Gets information about the current system environment. The result
     is returned as read only string. This function is fully DraCo
     compatible!

     Once a information has been evaluated, it will be cached internally,
     so changes will be ignored. Use IdHardwareUpdate() to clear the cache
     contents.


Inputs
~~~~~~
::

    Type -- (ULONG) Information type. These types are known:

            IDHW_SYSTEM     -- What system is used?
            (e. g. "Amiga 4000")

            IDHW_CPU        -- What kind of CPU is available?
            (e. g. "68060")

            IDHW_FPU        -- What kind of FPU is available?
            (e. g. "68060")

            IDHW_MMU        -- What kind of MMU is available?
            (e. g. "68060")

            IDHW_OSVER      -- What OS version is used?
            (e.g. "V39.106")

            IDHW_EXECVER    -- What exec version is used?
            (e.g. "V39.47")

            IDHW_WBVER      -- What WorkBench version is used?
            (e.g. "V39.29")

            IDHW_ROMSIZE    -- Size of AmigaOS ROM
            (e.g. "512KB")

            IDHW_CHIPSET    -- What Chipset is available?
            (e.g. "AGA")

            IDHW_GFXSYS     -- What Graphic OS is used?
            (e.g. "CyberGraphX")

            IDHW_CHIPRAM    -- Size of complete Chip RAM
            (e.g. "~2.0MB")

            IDHW_FASTRAM    -- Size of complete Fast RAM
            (e.g. "12.0MB")

            IDHW_RAM        -- Size of complete System RAM
            (e.g. "~14.0MB")

            IDHW_SETPATCHVER -- [V4] Version of current SetPatch
            (e.g. "V40.14")

            IDHW_AUDIOSYS   -- [V5] What Audio OS is used?
            (e.g. "AHI")

            IDHW_OSNR       -- [V5] What AmigaOS is used?
            (e.g. "3.1")

            IDHW_VMMCHIPRAM -- [V5] Size of virtual Chip RAM
            (e.g. "0")

            IDHW_VMMFASTRAM -- [V5] Size of virtual Fast RAM
            (e.g. "40.0MB")

            IDHW_VMMRAM     -- [V5] Size of total virtual RAM
            (e.g. "40.0MB")

            IDHW_PLNCHIPRAM -- [V5] Size of non-virtual Chip RAM
            (e.g. "2.0MB")

            IDHW_PLNFASTRAM -- [V5] Size of non-virtual Fast RAM
            (e.g. "12.0MB")

            IDHW_PLNRAM     -- [V5] Size of total non-virtual RAM
            (e.g. "14.0MB")

            IDHW_VBR        -- [V6] Vector Base Register contents
            (e.g. "0x0806C848")

            IDHW_LASTALERT  -- [V6] Last Alert code
            (e.g. "80000003")

            IDHW_VBLANKFREQ -- [V6] VBlank frequency (see execbase.h)
            (e.g. "50 Hz")

            IDHW_POWERFREQ  -- [V6] Power supply frequency (see execbase.h)
            (e.g. "50 Hz")

            IDHW_ECLOCK     -- [V6] System E clock frequency
            (e.g. "709379 Hz")

            IDHW_SLOWRAM    -- [V6] A500/A2000 "Slow" RAM expansion
            (e.g. "512.0KB")

            IDHW_GARY       -- [V6] GARY revision
            (e.g. "Normal")

            IDHW_RAMSEY     -- [V6] RAMSEY revision
            (e.g. "F")

            IDHW_BATTCLOCK  -- [V6] Battery backed up clock present?
            (e.g. "Found")

            IDHW_CHUNKYPLANAR -- [V7] Chunky to planar hardware present?
            (e.g. "Found")

            IDHW_POWERPC    -- [V7] PowerPC CPU present?
            (e.g. "603e")

            IDHW_PPCCLOCK   -- [V7] PowerPC processor clock
            (e.g. "200 MHz")

            IDHW_CPUREV     -- [V8] Revision of the main processor, if
            available (e.g. "Rev 1")

            IDHW_CPUCLOCK   -- [V8] CPU clock
            (e.g. "50 MHz")

            IDHW_FPUCLOCK   -- [V8] FPU clock, if available
            (e.g. "50 MHz")

            IDHW_RAMACCESS  -- [V8] Access time of the main board RAM
            (e.g. "80 ns")

            IDHW_RAMWIDTH   -- [V8] Width of the main board RAM
            (e.g. "16 bit")

            IDHW_RAMCAS     -- [V8] CAS mode of the main board RAM
            (e.g. "Double")

            IDHW_RAMBANDWIDTH -- [V8] Bandwidth of the main board RAM
            (e.g. "2")

            IDHW_TCPIP      -- [V9] Used TCP/IP stack
            (e.g. "AmiTCP/IP")

            IDHW_PPCOS      -- [V9] Used PowerPC OS
            (e.g. "PowerUp")

            IDHW_AGNUS      -- [V9] Agnus chip type and revision
            (e.g. "Alice 8374 Rev. 3-4")

            IDHW_AGNUSMODE  -- [V9] Agnus chip mode
            (e.g. "PAL")

            IDHW_DENISE     -- [V10] Denise chip type
            (e.g. "Lisa 8364")

            IDHW_DENISEREV  -- [V10] Denise chip revision
            (e.g. "0")

     TagList -- (struct TagItem *) tags that describe further
                options. You may provide NULL.


Tags
~~~~
::

     IDTAG_Localize  -- [V8] (BOOL) FALSE to get English strings
                        only, TRUE for localized strings. This is useful for applications
                        with English as only language. Defaults to TRUE.

     IDTAG_NULL4NA   -- [V8] (BOOL) TRUE to get NULL pointer instead
                        of a 'not available' string. Defaults to FALSE.


Result
~~~~~~
::

     String  -- (STRPTR) String containing the desired
                information, or NULL if not available. Note that
                all strings are READ ONLY!


Notes
~~~~~
::

     Some results are nonsense on AROS.



See also
~~~~~~~~

`IdHardwareNum()`_ `IdHardwareUpdate()`_ 

----------

IdHardwareNum()
===============

Synopsis
~~~~~~~~
::

 ULONG IdHardwareNum(
          ULONG type,
          struct TagItem * taglist );
 
 ULONG IdHardwareNumTags(
          ULONG type,
          TAG tag, ... );

Function
~~~~~~~~
::

     Gets information about the current system environment. The result
     is returned numerical. This function is fully DraCo compatible!

     Once a information has been read, it will be cached internally, so
     changes will be ignored. Use IdHardwareUpdate() to clear the cache
     contents.


Inputs
~~~~~~
::

     Type    -- (ULONG) Information type. These types are known
                (see include file and NOTE for detailed description):

                IDHW_SYSTEM     -- What system is used?
                (include file: IDSYS_...)

                IDHW_CPU        -- What kind of CPU is available?
                (include file: IDCPU_...)

                IDHW_FPU        -- What kind of FPU is available?
                (include file: IDFPU_...)

                IDHW_MMU        -- What kind of MMU is available?
                (include file: IDMMU_...)

                IDHW_OSVER      -- What OS version is used?
                (version, revision)

                IDHW_EXECVER    -- What exec version is used?
                (version, revision)

                IDHW_WBVER      -- What WorkBench version is used?
                (version, revision; 0 if not available)

                IDHW_ROMSIZE    -- Size of AmigaOS ROM
                (size in bytes)

                IDHW_CHIPSET    -- What Chipset is available?
                (include file: IDCS_...)

                IDHW_GFXSYS     -- What Graphic OS is used?
                (include file: IDGOS_...)

                IDHW_CHIPRAM    -- Size of complete Chip RAM
                (size in bytes)

                IDHW_FASTRAM    -- Size of complete Fast RAM
                (size in bytes)

                IDHW_RAM        -- Size of complete System RAM
                (size in bytes)

                IDHW_SETPATCHVER -- Version of current SetPatch
                (version, revision; 0 if not available)

                IDHW_AUDIOSYS   -- What Audio OS is used?
                (include file: IDAOS_...)

                IDHW_OSNR       -- What AmigaOS is used?
                (include file: IDOS_...)

                IDHW_VMMCHIPRAM -- Size of virtual Chip RAM
                (size in bytes)

                IDHW_VMMFASTRAM -- Size of virtual Fast RAM
                (size in bytes)

                IDHW_VMMRAM     -- Size of total virtual RAM
                (size in bytes)

                IDHW_PLNCHIPRAM -- Size of non-virtual Chip RAM
                (size in bytes)

                IDHW_PLNFASTRAM -- Size of non-virtual Fast RAM
                (size in bytes)

                IDHW_PLNRAM     -- Size of total non-virtual RAM
                (size in bytes)

                IDHW_VBR        -- Vector Base Register contents
                (address)

                IDHW_LASTALERT  -- Last Alert code
                (ULONG, 0xFFFFFFFF: no last alert yet)

                IDHW_VBLANKFREQ -- VBlank frequency (see execbase.h)
                (ULONG, Unit Hertz)

                IDHW_POWERFREQ  -- Power supply frequency (see execbase.h)
                (ULONG, Unit Hertz)

                IDHW_ECLOCK     -- System E clock frequency
                (ULONG, Unit Hertz)

                IDHW_SLOWRAM    -- A500/A2000 "Slow" RAM expansion
                (size in bytes)

                IDHW_GARY       -- GARY revision
                (include file: IDGRY_...)

                IDHW_RAMSEY     -- RAMSEY revision
                (include file: IDRSY_...)

                IDHW_BATTCLOCK  -- Battery backed up clock present?
                (BOOL)

                IDHW_CHUNKYPLANAR -- [V7] Chunky to planar hardware present?
                (BOOL)

                IDHW_POWERPC    -- [V7] PowerPC CPU present?
                (include file: IDPPC_...)

                IDHW_PPCCLOCK   -- [V7] PowerPC processor clock
                (ULONG clock in MHz units, or 0: not available)

                IDHW_CPUREV     -- [V8] Revision of the main processor
                (LONG revision or -1 if not available)

                IDHW_CPUCLOCK   -- [V8] CPU clock
                (ULONG clock in MHz units)

                IDHW_FPUCLOCK   -- [V8] FPU clock, if available
                (ULONG clock in MHz units, or 0: not available)

                IDHW_RAMACCESS  -- [V8] Access time of the main board RAM
                (ULONG in ns units, or 0: not available)

                IDHW_RAMWIDTH   -- [V8] Width of the main board RAM
                (ULONG in bit, or 0: not available)

                IDHW_RAMCAS     -- [V8] CAS mode of the main board RAM
                (include file: IDCAS_...)

                IDHW_RAMBANDWIDTH -- [V8] Bandwidth of the main board RAM
                (ULONG in times, or 0: not available)

                IDHW_TCPIP      -- [V9] Used TCP/IP stack
                (include file: IDTCP_...)

                IDHW_PPCOS      -- [V9] Used PowerPC OS
                (include file: IDPOS_...)

                IDHW_AGNUS      -- [V9] Agnus chip type and revision
                (include file: IDAG_...)

                IDHW_AGNUSMODE  -- [V9] Agnus chip mode
                (include file: IDAM_...)

                IDHW_DENISE     -- [V10] Denise chip type
                (include file: IDDN_...)

                IDHW_DENISEREV  -- [V10] Denise chip revision
                (LONG, -1 means not available)

     TagList -- (struct TagItem *) tags that describe further
                options. You may provide NULL.


Tags
~~~~
::

     None yet.


Result
~~~~~~
::

     Result -- (ULONG) Numerical result containing the desired
               information.


Notes
~~~~~
::

     Some results are nonsense on AROS.

     If you queried a version, you'll find the version in the *lower*
     UWORD (because it is more important) and the revision in the
     *upper* UWORD.

     All memory sizes are always in bytes.

     Boolean results are ==0 for FALSE, !=0 for TRUE.

     If you have to look up the result in the include file, you might
     also get a numerical result that is beyond the maximum value you'll
     find there. Be prepared for it! In this case, just print "not known"
     or anything similar, or use the IdHardware() result.



See also
~~~~~~~~

`IdHardware()`_ `IdHardwareUpdate()`_ 

----------

IdHardwareUpdate()
==================

Synopsis
~~~~~~~~
::

 void IdHardwareUpdate();

Function
~~~~~~~~
::

     Once a hardware information has been evaluated, the result will be
     stored in an internal cache. All subsequent queries return the cache
     contents, irregarding of any changes.

     This function invalidates the cache and forces identify to re-check
     ALL hardware features. Useful if e.g. the amount of memory has changed
     after VMM has been started.

     Use this function wisely. DO NOT call it just to make sure to get the
     latest information, let the user decide to do so. Also, DO NOT call
     it when you will only query hardware information that will for sure
     not change while run-time, e.g. CPU.

     IDHW_VBR, IDHW_LASTALERT and IDHW_TCPIP are NOT cached, so there is
     absolutely no need to call IdHardwareUpdate() just to check them out.


Bugs
~~~~
::

     Calling this function in identify V8.0 will lead to a system crash.
     This has been fixed in V8.1.



See also
~~~~~~~~

`IdHardware()`_ `IdHardwareNum()`_ 


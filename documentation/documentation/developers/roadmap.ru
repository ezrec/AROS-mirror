==================
Маршрут к AROS 1.0
==================

:Authors:   Adam Chodorowski
:Copyright: Copyright Љ 2003, The AROS Development Team
:Version:   $Revision: 22461 $
:Date:      $Date: 2004-10-18 03:05:43 +0500 (а†аН, 18 аОаКб™ 2004) $

:Abstract:  
    
    Намеченный здесь путь введен с целью описать все требования, которые 
    необходимо выполнить, преджде чем будет выпущена AROS 1.0. Цель его *не*
    следует видеть, как нечто, что должно заставить разработчиков работать на 
    какими-то определенными вещами (поскольку это всё равно невозможно в 
    проекте open source), но как рекомендацию, где людям следует приложить
    свои усилия. Мы надеемся, что это поможет сделать работу более 
    организованной и целенаправленной.
    

Порты
-----

Перед выпуском AROS 1.0, эти требования должны быть выполнены для портов
i386-pc и i386-linux (позже упоминаемых, как "обязательные порты"). Все прочие порты не рассматриваются как требования для 1.0 для снижения объёма работ.


Требования
----------

1.  AmigaOS 3.1 API compatibility, excluding parts which are deemed to be
    non-portable or obsolete and not worth the effort. 
    
    To mark something as obsolete even though it is possible to implement in a
    portable way should require good reasons; for example if it is very rarely
    used by applications and the effort required to implement it is
    substantial.

    Details to follow...


2.  Partial AmigaOS 3.5 and 3.9 API compatibility. We choose the parts that 
    we feel are useful and worthwhile to have, and ditch the rest. 
    
    For example, it seems *very* unlikely that we want ReAction compatibility
    since we have already chosen to standardize on Zune for the GUI toolkit
    (and implementing the ReAction API is not a trivial undertaking). Of
    course, this will have to be discussed properly before a detailed list is
    agreed upon.
    
    Details to follow...    


3.  Completed GUI toolkit. This means Zune must have complete MUI API
    compatability and a finished preferences program.
    
    Details to follow...


4.  Standard applications comparable to those which come with AmigaOS 3.1.
    
    This does *not* mean that we have to have the exact same applications which
    work exactly the same as in AmigaOS, but the functionality available to the
    user should be roughly equivalent.

    Details to follow...

    
5.  Sound support, meaning API compatibility and basic applications. There 
    should be at least one driver for each mandatory port. 
    
    Details to follow...
    
    
6.  Networking support. This includes a TCP/IP-stack and some basic
    applications like email and SSH clients, and also a *simple* web browser.
    There should be at least one NIC driver for each mandatory port. 
    
    The requirements on the web browser should not be high, but it should be
    possible to at least browse the web in some way (even if it is only in text
    mode).
    
    Details to follow...
    
    
7.  Self-hosted development environment and SDK for developers. Specifically,
    this includes all software required to build AROS like GCC, GNU binutils,
    GNU make and the rest. It must be possible to compile AROS on AROS. 
    
    The ABI for the supported architectures (only i386 at this point) must be
    finalized before 1.0. Once 1.0 is released, the ABI should be stable for a
    considerable time.
   
    Details to follow...
    
    
8.  Comprehensive documentation for developers. This includes complete reference
    manuals over all libraries, devices, classes and development tools and also
    guides and tutorials to introduce whole subsystems and give an overview.
    Also, a migration/porting guide should be available.
    
    Details to follow...

    
9.  Comprehensive documentation for users. This includes a complete command 
    reference, tutorials and installation-, configuration- and other guides.
    
    Details to follow...


10. Substantial testing and bug hunting complete. The 1.0 release should be
    virtually free of bugs, and be a *very* stable release. We should not have
    the fiascos some open source projects have had with their ".0" releases. 
        
    This will probably require an extended feature freeze, followed by a code
    freeze and several intermediate milestones for user feedback and testing.
    Feature requests are not regarded as bugs, unless it is something required
    (but missed) in the preceding milestones. For example, "we need a movie
    player" does not qualify, but "the text editor should have a 'save' menu
    option" does.

    Details to follow...

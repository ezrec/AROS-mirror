================
AROS 1.0 Roadmap
================

:Authors:   Adam Chodorowski, Sergey Mineychev
:Copyright: Copyright (C) 2008, The AROS Development Team
:Version:   $Revision$
:Date:      $Date$

:Abstract:
    This is a roadmap intended to outline the requirements that need to be
    fulfilled before AROS 1.0 can be released. It is *not* meant to force
    developers to work on certain things (since it's not possible in an open
    source project anyway) but should rather be seen as a recommendation
    where we would like people to focus their efforts. Hopefully, this will
    help organize the development effort and make us all work more in the
    same direction.


Ports
-----

For AROS 1.0 to be released, these requirements must be fulfilled by the
i386-pc and i386-linux ports (referred to as "mandatory ports" below).
Other ports are not seen as required for 1.0, to keep the amount of work
down.


Overall status
--------------

As of 2009-07-04:

The main goal of AmigaOS 3.1 source compatibility is nearing completion. Several
modules are still missing, but most of them will not be required to reach the
goal. Out of 10 roadmap requirements, 2 are completed, 1 is skipped, 6 are in
progress and 1 is not started.

Looking closer at the requirements in progress, the greatest amounts of
programming work are required to implement the ABI_V1 and to complete
Zune - the GUI toolkit.

Concerning the documentation, both topics (developer and user) require more
work to reach completion. From developer standpoint, a set of documents
outlining creation of basic software components is required.
User documentation needs to focus on installation and configuration.

Next review of AROS 1.0 Roadmap: **December 2009**


Requirements
------------

1.  **AmigaOS 3.1 API compatibility**

    We exclude parts which are deemed to be non-portable or obsolete
    and not worth the effort and implement the remaining API.

    To mark something as obsolete even though it is possible to implement in a
    portable way should require good reasons; for example if it is very rarely
    used by applications and the effort required to implement it is
    substantial.

    Current status: **In Progress**, 81% Completed, see `Status page`__

    Current recommendations: see `Recommendations`__

2.  **Partial AmigaOS 3.5 and 3.9 API compatibility**

    We choose the parts that we feel are useful and worthwhile to have, and
    ditch the rest. For example, it seems *very* unlikely that we want
    ReAction compatibility since we have already chosen to standardize on Zune
    for the GUI toolkit (and implementing the ReAction API is not a
    trivial undertaking).

    Some parts of 3.5 and 3.9 are already implemented. They can be found in
    `Extensions`__ section of Status page

    Current status: **Skipped**

    *Reasoning: This goal is defined a in way that does not allow benchmarking
    its completeness.*

3.  **Completed GUI toolkit**

    This means Zune must have complete MUI 3.8 API compatibility and a
    finished preferences program.

    Current status: **In Progress**, see `Zune page`__

    Current recommendations: see `Recommendations`__

4.  **Standard applications comparable to those which come with AmigaOS 3.1**

    This does *not* mean that we have to have the exact same applications which
    work exactly the same as in AmigaOS, but the functionality available to the
    user should be roughly equivalent.

    Current status: **In Progress**, 73% Completed, see `Programs page`__

    Current recommendations: see `Recommendations`__

5.  **Sound support**

    API compatibility and basic applications. There should be at least one
    driver for each mandatory port.

    Current status: **Completed**

    *Reasoning: Currently AHI is ported, and there are some drivers (a few)
    for i386-port. AHI Preferences editor as well as MP3 player are
    available.*

6.  **Networking support**

    This includes a TCP/IP-stack and some basic applications, like email and
    SSH clients, and also a web browser. There should be at least one NIC
    driver for each mandatory port.

    The requirements on the web browser should not be high, but it should be
    possible to at least browse the web in some way.

    Current status: **Completed**

    *Reasoning: AROSTCP we got now is an ancient but working implementation of
    the AmiTCP stack. There are several network drivers available for both
    real hardware and Linux hosted TAP device. The AROSTCP preferences editor
    is available. Network applications are available: ftp client, IRC client,
    mail program, modern web browser, etc.*

7.  **Self-hosted development environment and SDK for developers**

    Specifically, this includes all software required to build AROS, like
    GCC, GNU binutils, GNU make and the rest. It must be possible to compile
    AROS on AROS.

    The ABI for the supported architectures (only i386 at this point) must be
    finalized before 1.0. Once 1.0 is released, the ABI should be stable for a
    considerable time.

    Current status: **In Progress**

    Current recommendations: see `Recommendations`__

8.  **Comprehensive documentation for developers**

    This includes reference manuals over all libraries, devices,
    classes and development tools and also guides and tutorials to
    introduce whole subsystems and give an overview. Also, a migration/porting
    guide should be available.

    Current status: **In Progress**

    Current recommendations: see `Recommendations`__

9.  **Comprehensive documentation for users**

    This includes a complete command reference, tutorials, and an installation
    guide and a configuration guide, as well as other guides.

    Current status: **In Progress**

    Current recommendations: see `Recommendations`__

    *Reasoning: Documentation exists and has extensively been translated to
    different languages. To be complete, the tutorials and guides, and the
    help system are needed.*

10. **Substantial testing and bug hunting complete**

    The 1.0 release should be virtually free of bugs, and be a *very* stable
    release. We should not have the fiascos some open source projects
    have had with their ".0" releases.

    This will probably require an extended feature freeze, followed by a code
    freeze and several intermediate milestones for user feedback and testing.

    Feature requests are not regarded as bugs, unless it is something required
    (but missed) in the preceding milestones. For example, "we need a movie
    player" does not qualify, but "the text editor should have a 'save' menu
    option" does.

    Current status: **Not Started**

    Current recommendations: see `Recommendations`__

    *Reasoning: Currently no freeze can be done as features aren't complete
    yet. There is still lot of unfixed bugs, but increasing users activity looks
    promising. Bug hunting and accounting procedures and services are wanted.*

__ ../../introduction/status/amigaos
__ recommendations#amigaos-3-1-api-compatibility
__ ../../introduction/status/extensions
__ ../../introduction/status/zune
__ recommendations#completed-gui-tool-kit
__ ../../introduction/status/programs
__ recommendations#standard-applications-comparable-to-those-which-come-with-amigaos-3-1
__ recommendations#self-hosted-development-environment-and-sdk-for-developers
__ recommendations#comprehensive-documentation-for-developers
__ recommendations#comprehensive-documentation-for-users
__ recommendations#substantial-testing-and-bug-hunting-complete


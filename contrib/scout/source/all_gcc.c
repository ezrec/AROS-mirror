#if defined(__amigaos4__)
    #include "extrasrc/stccpy.c"
    #include "extrasrc/stcd_i.c"
    #include "extrasrc/stcd_l.c"
    #include "extrasrc/init_inet_daemon.c"
#endif
#if defined(__MORPHOS__)
    #include "extrasrc/vasprintf.c"
    #include "extrasrc/init_inet_daemon.c"
#endif
#if defined(__AROS__)
    #include "extrasrc/vasprintf.c"
    #include "extrasrc/init_inet_daemon.c"
#endif
#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
    #include "extrasrc/stccpy.c"
    #include "extrasrc/stcd_i.c"
    #include "extrasrc/stcd_l.c"
    #include "extrasrc/vasprintf.c"
    #include "extrasrc/init_inet_daemon.c"
#endif

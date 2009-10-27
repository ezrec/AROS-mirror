#if defined(__amigaos4__)
    #include "extrasrc/stccpy.c"
    #include "extrasrc/stcd_i.c"
    #include "extrasrc/stcd_l.c"
    #include "extrasrc/init_inet_daemon.c"
    #include "extrasrc/set_socket_stdio.c"
#endif
#if defined(__MORPHOS__)
    #include "extrasrc/vasprintf.c"
    #include "extrasrc/init_inet_daemon.c"
    #include "extrasrc/set_socket_stdio.c"
#endif


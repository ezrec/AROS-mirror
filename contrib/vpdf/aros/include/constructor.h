#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include <aros/symbolsets.h>

#define CONSTRUCTOR_P(x, pri) \
    int __CSTP_##x(void); \
    ADD2INIT(__CSTP_##x, pri); \
    int __CSTP_##x(void)

#define DESTRUCTOR_P(x, pri) \
    void __DSTP_##x(void); \
    ADD2EXIT(__DSTP_##x, pri); \
    void __DSTP_##x(void)

#endif

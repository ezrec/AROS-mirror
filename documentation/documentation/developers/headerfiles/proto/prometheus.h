#ifndef PROTO_PROMETHEUS_H
#define PROTO_PROMETHEUS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/prometheus/prometheus.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/prometheus_protos.h>

#if !defined(PrometheusBase) && !defined(__NOLIBBASE__) && !defined(__PROMETHEUS_NOLIBBASE__)
 #ifdef __PROMETHEUS_STDLIBBASE__
  extern struct Library *PrometheusBase;
 #else
  extern struct Library *PrometheusBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(PROMETHEUS_NOLIBDEFINES)
#   include <defines/prometheus.h>
#endif

#endif /* PROTO_PROMETHEUS_H */

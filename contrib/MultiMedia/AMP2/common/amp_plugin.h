/*
 *
 * amp_plugin.h
 *
 */

#ifndef AMP_PLUGIN_H
#define AMP_PLUGIN_H

typedef struct {
  plugin_ident *ident;
  plugin_data *data;
} amp_plugin_type;

extern int amp_plugin_init(void);
extern amp_plugin_type *wait_for_plugin_init(unsigned long type, unsigned long fourcc);
extern void wait_for_plugin_exit(amp_plugin_type *pin);
extern int amp_plugin_exit(void);

unsigned long get_plugin_custom(unsigned long type, unsigned long fourcc);

#endif

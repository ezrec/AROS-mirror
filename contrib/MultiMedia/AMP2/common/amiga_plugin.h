/*
 *
 * amiga_plugin.h
 *
 */

#ifndef AMIGA_PLUGIN_H
#define AMIGA_PLUGIN_H

/* Functions */
#define FUNCTION_INIT (0x00000001)
#define FUNCTION_EXIT (0x00000002)

/* OS dependent message structure */
typedef struct {
  struct Message msg;          /* Mandatory AmigaOS message structure */
  struct MsgPortPPC *msg_port; /* Plugin message port, to send messages to plugin */
  unsigned long function;
  plugin_data data;
} plugin_msg_type;

#endif

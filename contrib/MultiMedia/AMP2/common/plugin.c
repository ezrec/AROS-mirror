/*
 *
 * plugin.c
 *
 */

#include <stdio.h>
#include <proto/exec.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else

#include "aros-inc.h"

#endif

#include "plugin.h"
#include "amiga_plugin.h"

static struct MsgPortPPC *amp_msg_port = NULL;
static struct MsgPortPPC *plugin_msg_port = NULL;
static plugin_msg_type plugin_msg;
static char amp_msg_port_name[20];

/* To access AMP functions */
plugin_data *amp = &plugin_msg.data;

static int plugin_attach(void)
{
  /* Find AMP message port */
  Forbid();
  amp_msg_port = FindPortPPC(amp_msg_port_name);
  Permit();
  if (amp_msg_port == NULL) {
    return PLUGIN_FAIL;
  }

  /* Create PLUGIN message port */
  plugin_msg_port = CreateMsgPortPPC();
  if (plugin_msg_port == NULL) {
    return PLUGIN_FAIL;
  }

  /* Init message structure */
  memset(&plugin_msg, 0, sizeof(plugin_msg_type));
  plugin_msg.msg.mn_ReplyPort = (struct MsgPort *)plugin_msg_port;
  plugin_msg.msg.mn_Length = sizeof(plugin_msg_type);
  plugin_msg.msg_port = plugin_msg_port;

  /* Send attach message */
  plugin_msg.data.ident = plugin_idents;
  plugin_msg.function = FUNCTION_INIT;
  PutMsgPPC(amp_msg_port, (struct Message *)&plugin_msg);

  /* Wait for reply */
  WaitPortPPC(plugin_msg_port);

  /* Remove message from queue */
  GetMsgPPC(plugin_msg_port);

  return PLUGIN_OK;
}

static void plugin_detach(void)
{
  /* Get message */
  for(;;) {
    plugin_msg_type *msg = NULL;
    unsigned long function;

    /* Wait for message */
    WaitPortPPC(plugin_msg_port);

    msg = (plugin_msg_type *)GetMsgPPC(plugin_msg_port);
    if (msg != NULL) {
      function = msg->function;
      ReplyMsgPPC((struct Message *)msg);
      if (function == FUNCTION_EXIT) {
        break;
      }
    }
  }

  DeleteMsgPortPPC(plugin_msg_port);
  plugin_msg_port = NULL;
  amp_msg_port = NULL;
}

/* Allocate and free aligned memory */
void *mallocalign (unsigned long size)
{
  return AllocVecPPC(size, MEMF_PUBLIC|MEMF_CLEAR, 32);
}

void freealign (void *buffer)
{
  FreeVecPPC(buffer);
}

/* Minimal main function */

int main(int argc, char **argv)
{
  if (argc == 3) {
    int serial = 0;

    /* Get serial */
    serial = argv[1][0] - '0';

    /* Only accept serials between 1 and 9 */
    if ((serial >= 1) && (serial <= 9)) {
      sprintf(amp_msg_port_name, "AMP_MSG_PORT_%04d", serial);

      /* Try to attach plugin, if success, wait for exit message */
      if (plugin_attach() == PLUGIN_OK) {
        /* Special */
        amp->plugin_path = argv[2];

        plugin_detach();
        return 0;
      }
    }
  }

  /* Print version information */
  printf("%s\n", &version[6]);

  return 0;
}

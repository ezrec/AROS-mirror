/*
 *
 * find_plugin.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <proto/exec.h>
#include <proto/dos.h>

#if defined(__AROS__)

#include "aros-inc.h"

#endif

#include <amiaudio.h>
#include "core.h"
#include "plugin.h"
#include "amiga_plugin.h"
#include "amp_plugin.h"

#include "../amigaos/video.h"
#include "../main/main.h"
#include "../main/prefs.h"

#define PLUGIN_AMP (0x24414d50) /* $AMP */
#define PLUGIN_ID  (0x50494e32) /* PIN2 */

#define VIDEO_PIN 0
#define AUDIO_PIN 1

amp_plugin_type pin[2];

/* Plugin message port, to send messages to plugin */
struct MsgPortPPC *pin_port[2] = { NULL, NULL };

static char cli[1024];
static char plugin_path[256+64];
static char plugin_file[256+64+64];

/* SCAN */

BPTR lock = NULL;
struct FileInfoBlock fiblock;

static unsigned char *scan_buf = NULL;
#define SCAN_BUF_SIZE (512*1024)

typedef struct {
  /* Type */
  unsigned long type;

  /* Fourcc */
  unsigned long fourcc;

  /* Filename */
  char filename[64];
} plugin_type;

plugin_type plugins[100]; /* FIXME: 7kb, allocate dynamically!!! */

struct MsgPortPPC *amp_msg_port = NULL;
struct MsgPortPPC *reply_msg_port = NULL;
static plugin_msg_type amp_msg; /* FIXME: Remove! */
static unsigned long serial = 0;

void plugin_put(struct MsgPortPPC *port, unsigned long function)
{
  /* Send attach message */
//  printf("Send message\n");
  amp_msg.function = function;
  PutMsgPPC(port, (struct Message *)&amp_msg);

  /* Wait for reply */
//  printf("Wait for reply\n");
  WaitPortPPC(reply_msg_port);

  /* Remove message from queue */
//  printf("Remove message from queue\n");
  GetMsgPPC(reply_msg_port);
}

int plugin_get(unsigned long function, unsigned long type, unsigned long fourcc)
{
  unsigned long msg_function = 0;
  plugin_msg_type *msg = NULL;
  int found = -1;

  /* Get message */
//  printf("Get message from plugin\n");
  for(;;) {
    /* Wait for message */
//    printf("Wait for message\n");
    WaitPortPPC(amp_msg_port);

/* FIXME: Add delay and timeout here */

    msg = (plugin_msg_type *)GetMsgPPC(amp_msg_port);
    if (msg != NULL) {
      msg_function = msg->function;
//      printf("Got message with function %08lx, wanted %08lx\n", msg_function, function);

      if (msg_function == FUNCTION_INIT) {

        msg->data.video_init = amp_video_init;
        msg->data.video_refresh = amp_video_refresh;
        msg->data.video_refresh_palette = amp_video_refresh_palette;
        msg->data.video_refresh_yuv2rgb = amp_video_refresh_yuv2rgb;
        msg->data.audio_init = amp_audio_init;
        msg->data.audio_refresh = amp_audio_refresh;

        {
          plugin_ident *plugin;

          /* Full search */
          plugin = msg->data.ident;
          if ((plugin->type == PLUGIN_AMP) && (plugin->fourcc == PLUGIN_ID)) {
            plugin++;
            while (plugin->type != 0) {
              debug_printf("%08lx, %08lx\n", plugin->type, plugin->fourcc);
              if (GET_COMMON(plugin->type) == GET_COMMON(type)) {
                if (plugin->fourcc == fourcc) {
                  debug_printf("FOUND!\n");
                  found = 0;
                  if (plugin->type & TYPE_VIDEO) {
                    pin[VIDEO_PIN].ident = plugin;
                    pin[VIDEO_PIN].data = &msg->data;
                    pin_port[VIDEO_PIN] = msg->msg_port;
                  } else {
                    pin[AUDIO_PIN].ident = plugin;
                    pin[AUDIO_PIN].data = &msg->data;
                    pin_port[AUDIO_PIN] = msg->msg_port;
                  }
                  break;
                }
              }
              plugin++;
            }
          }

          /* Types search */
          if (found != 0) {
            plugin = msg->data.ident;
            if ((plugin->type == PLUGIN_AMP) && (plugin->fourcc == PLUGIN_ID)) {
              plugin++;
              while (plugin->type != 0) {
                debug_printf("%08lx, %08lx\n", plugin->type, plugin->fourcc);
                if (GET_TYPES(plugin->type) == GET_TYPES(type)) {
                  if (plugin->fourcc == fourcc) {
                    debug_printf("FOUND!\n");
                    found = 0;
                    if (plugin->type & TYPE_VIDEO) {
                      pin[VIDEO_PIN].ident = plugin;
                      pin[VIDEO_PIN].data = &msg->data;
                      pin_port[VIDEO_PIN] = msg->msg_port;
                    } else {
                      pin[AUDIO_PIN].ident = plugin;
                      pin[AUDIO_PIN].data = &msg->data;
                      pin_port[AUDIO_PIN] = msg->msg_port;
                    }
                    break;
                  }
                }
                plugin++;
              }
            }
          }
        }
      }

      ReplyMsgPPC((struct Message *)msg);
      if (msg_function & function) {
        break;
      }
    }
  }

  if (found < 0) {
    /* Not found, stop */
    printf("Plugin is old or no longer support the current format!\n");
    plugin_put(msg->msg_port, FUNCTION_EXIT);
    pin_port[type] = NULL;
  }

  return found;
}

void scan_file(char *filename, char *file)
{
  plugin_ident *plugin = NULL;
  FILE *fp = NULL;
  int i;

  debug_printf("found: %s\n", filename);

  fp = fopen(filename, "r");
  if (fp != NULL) {
    int length = fread(scan_buf, 1, SCAN_BUF_SIZE, fp);

    i = 0;
    while (i < length) {
      if (*(unsigned long *)(scan_buf + i) == PLUGIN_AMP) {
        if (*(unsigned long *)(scan_buf + i + 4) == PLUGIN_ID) {
          plugin = (plugin_ident *)(scan_buf + i);
          plugin++;
          while (plugin->type != 0) {
            debug_printf("%08lx, %08lx\n", plugin->type, plugin->fourcc);
            plugins[serial].type = plugin->type;
            plugins[serial].fourcc = plugin->fourcc;
            strcpy(plugins[serial].filename, file);
            serial++;
            plugin++;
          }
          break;
        }
      }
      i++;
    }
    fclose(fp);
  }
}

void scan_dir(void)
{
  FILE *fp;

  serial = 0;
  scan_buf = malloc(SCAN_BUF_SIZE);
  memset(&fiblock, 0, sizeof(fiblock));

  /* Create path */
  strcpy(plugin_path, prefs.cwd);
  strcat(plugin_path, "Plugin/");
  debug_printf("scanning path: %s\n", plugin_path);

  lock = Lock(plugin_path, ACCESS_READ);
  if (lock) {
    if (Examine(lock, &fiblock)) {
      while (ExNext(lock, &fiblock)) {
        debug_printf("scanning file: %s\n", fiblock.fib_FileName);
        if (strstr(fiblock.fib_FileName, ".AMP") != NULL) {
          strcpy(plugin_file, plugin_path);
          strcat(plugin_file, fiblock.fib_FileName);
          scan_file(plugin_file, fiblock.fib_FileName);
        }
      }
    }
    UnLock(lock);
    lock = NULL;

    if (serial > 0) {
      strcpy(plugin_file, plugin_path);
      strcat(plugin_file, "PLUGIN.IDX");
      fp = fopen(plugin_file, "w");
      if (fp != NULL) {
        fwrite(plugins, sizeof(plugin_type), serial, fp);
        fclose(fp);
      }
    }
  }

  free(scan_buf);
  scan_buf = NULL;
}

void get_plugins(void)
{
  FILE *fp;

  /* Create path */
  strcpy(plugin_path, prefs.cwd);
  strcat(plugin_path, "Plugin/");

  strcpy(plugin_file, plugin_path);
  strcat(plugin_file, "PLUGIN.IDX");
  fp = fopen(plugin_file, "r");
  if (fp != NULL) {
    serial = fread(plugins, sizeof(plugin_type), 100, fp);
    verbose_printf("Number of plugins: %ld\n", serial);
    fclose(fp);
    if (serial > 0) {
      return;
    }
  }

  scan_dir();
}

unsigned long get_plugin_custom(unsigned long type, unsigned long fourcc)
{
  int i, retry = 1;

  {
    unsigned long val;
    char c;

    val = fourcc;
    debug_printf("FOURCC: %08lx, ", val);

    for (i=0; i<4; i++) {
      c = (val >> 24) & 0xff;
      debug_printf("%c", c);
      val <<= 8;
    }

    debug_printf("\n");
  }

  for (;;) {
    get_plugins();
    if (serial <= 0) {
      return 0;
    }

    /* Try any features */
    for (i=0; i<serial; i++) {
      if (GET_TYPES(plugins[i].type) == GET_TYPES(type)) {
        if (plugins[i].fourcc == fourcc) {
          debug_printf("Custom found in: '%s', %08lx\n", plugins[i].filename, plugins[i].type);
          return plugins[i].type;
        }
      }
    }

    if (retry == 0) {
      return 0;
    }
    retry = 0;

    scan_dir();
  }

  return 0;
}

static char msg_port_name[20];
static int port_number = 1; /* 1-9 are valid */
static char port_number_text[4];

int amp_plugin_init(void)
{
  struct MsgPortPPC *port;

  port_number = 1;
  for (;;) {
    sprintf(msg_port_name, "AMP_MSG_PORT_%04d", port_number);
    Forbid();
    port = FindPortPPC(msg_port_name);
    Permit();

    if (port == NULL) {
      break;
    } else {
      port_number++;
    }
  }

  if ((port_number < 1) || (port_number > 9)) {
    printf("Failed to find a free portt\n");
    return -1;
  }

  sprintf(port_number_text, " %d ", port_number);
  debug_printf("Free port %s (%s)\n", msg_port_name, port_number_text);

  reply_msg_port = CreateMsgPortPPC();
  if (reply_msg_port == NULL) {
    printf("Failed to allocate message port\n");
    return -1;
  }

  amp_msg_port = CreateMsgPortPPC();
  if (amp_msg_port == NULL) {
    printf("Failed to allocate message port\n");
    return -1;
  }

  #if !defined(__AROS__)
  amp_msg_port->mp_Port.mp_Node.ln_Name = msg_port_name;
  amp_msg_port->mp_Port.mp_Node.ln_Pri  = 1;
  #else
  amp_msg_port->mp_Node.ln_Name = msg_port_name;
  amp_msg_port->mp_Node.ln_Pri  = 1;
  #endif

  AddPortPPC(amp_msg_port);

  /* Setup plugin */
  memset(&amp_msg, 0, sizeof(amp_msg));
  amp_msg.msg.mn_ReplyPort = (struct MsgPort *)reply_msg_port;
  amp_msg.msg.mn_Length = sizeof(amp_msg);

  return 0;
}

amp_plugin_type *wait_for_plugin_init(unsigned long type, unsigned long fourcc)
{
  FILE *fp = NULL;
  int i, retry;

  memset(plugins, 0, sizeof(plugins));

  if ((prefs.lores == PREFS_ON) && (type & TYPE_VIDEO)) {
    type |= TYPE_LORES;
  }

  retry = 1;
  for (;;) {
    get_plugins();
    if (serial <= 0) {
      return NULL;
    }

    debug_printf("find suitable_plugin: %08lx, %08lx\n", type, fourcc);

    strcpy(plugin_file, "\0");

    /* Try exact match */
    for (i=0; i<serial; i++) {
      if (GET_COMMON(plugins[i].type) == GET_COMMON(type)) {
        if (plugins[i].fourcc == fourcc) {
          debug_printf("plugin found: '%s'\n", plugins[i].filename);
          strcpy(plugin_file, prefs.cwd);
          strcat(plugin_file, "Plugin/");
          strcat(plugin_file, plugins[i].filename);
          break;
        }
      }
    }

    if (strlen(plugin_file) == 0) {
      /* Try any features */
      for (i=0; i<serial; i++) {
        if (GET_TYPES(plugins[i].type) == GET_TYPES(type)) {
          if (plugins[i].fourcc == fourcc) {
            debug_printf("plugin found: '%s'\n", plugins[i].filename);
            strcpy(plugin_file, prefs.cwd);
            strcat(plugin_file, "Plugin/");
            strcat(plugin_file, plugins[i].filename);
            break;
          }
        }
      }
      if (strlen(plugin_file) == 0) {
        goto retry_scan;
      }
    }

    fp = fopen(plugin_file, "r");
    if (fp == NULL) {
      goto retry_scan;
    } else {
      fclose(fp);
    }

    sprintf(cli, "stack %d\n", (1024*512));
    strcat(cli, "run >NIL: \""); // Run AMP in the background
    strcat(cli, plugin_file);
    strcat(cli, "\"");

    strcat(cli, port_number_text);

    strcat(cli, "\"");
    strcat(cli, prefs.cwd);
    strcat(cli, "Plugin/");
    strcat(cli, "\"");

    if (prefs.debug == PREFS_ON) {
      strcat(cli, " >CON:0/0/800/200/DEBUG/AUTO/WAIT"); /* FIXME */
    } else {
      strcat(cli, " >NIL:"); /* FIXME */
    }

    debug_printf("%s\n", cli);
    Execute(cli, NULL, NULL);

    if (plugin_get(FUNCTION_INIT, type, fourcc) < 0) {
      goto retry_scan;
    }

    break; /* Exit the loop */

retry_scan:
    printf("no plugin found, retry: %d\n", retry);
    if (retry == 0) {
      return NULL;
    } else {
      retry = 0;
    }
    scan_dir();
  }

  if (type & TYPE_VIDEO) {
    return &pin[VIDEO_PIN];
  } else {
    return &pin[AUDIO_PIN];
  }
}

void wait_for_plugin_exit(amp_plugin_type *my_pin)
{
  int type;

  if (my_pin == &pin[VIDEO_PIN]) {
    type = VIDEO_PIN;
//    printf("Video exit\n");
  } else if (my_pin == &pin[AUDIO_PIN]) {
    type = AUDIO_PIN;
//    printf("Audio exit\n");
  } else {
    return;
  }

  plugin_put(pin_port[type], FUNCTION_EXIT);
  pin_port[type] = NULL;
}

int amp_plugin_exit(void)
{
  if (amp_msg_port != NULL) {
    RemPortPPC(amp_msg_port);

    DeleteMsgPortPPC(amp_msg_port);
    amp_msg_port = NULL;
  }

  if (reply_msg_port != NULL) {
    DeleteMsgPortPPC(reply_msg_port);
    reply_msg_port = NULL;
  }

  return 0;
}

/*
 * rp.c
 */

#include <stdio.h>
#include <stdlib.h>

#if defined(__AROS__)
#include <string.h>
#include <sys/types.h>
#endif

#include "types.h"
#include "dlfcn.h"

/*
videocodec rv30
  info "Linux RealPlayer 8 RV30 decoder"
  status working
  fourcc RV30,rv30
  driver realvid
  dll "drv3.so.6.0"
  out I420

videocodec rv20
  info "Linux RealPlayer 8 RV20 decoder"
  status working
  fourcc RV20,rv20
  driver realvid
  dll "drv2.so.6.0"
  out I420
*/

#define PRINTF if (1 == 0) printf

unsigned long (*rvyuv_custom_message)(unsigned long,unsigned long);
unsigned long (*rvyuv_free)(unsigned long);
unsigned long (*rvyuv_hive_message)(unsigned long,unsigned long);
unsigned long (*rvyuv_init)(unsigned long,unsigned long);
unsigned long (*rvyuv_transform)(unsigned long,unsigned long,unsigned long,unsigned long,unsigned long);

void *rv_handle=NULL;

void *__builtin_vec_new(unsigned long size) {
PRINTF("__builtin_vec_new: %ld\n", size);
     return malloc(size);
}

void __builtin_vec_delete(void *mem) {
PRINTF("__builtin_vec_delete\n");
     free(mem);
}

void __pure_virtual(void) {
     PRINTF("FATAL: __pure_virtual() called!\n");
//   exit(1);
}

void ___brk_addr(void) { /*exit(0);*/ }
char **__environ = { NULL };

/* we need exact positions */
struct rv_init_t {
     short unk1;
     short w;
     short h;
     short unk3;
     int unk2;
     int subformat;
     int unk5;
     int format;
} rv_init_t;

/* FIXME */

#if 0

struct object {
  void *pc_begin;
  void *pc_end;
  void *fde_begin;
  void *fde_array;
  size_t count;
  struct object *next;
};

/* Called either from crtbegin.o or a static constructor to register the
   unwind info for an object or translation unit, respectively.  */

extern void __register_frame_info (void *, struct object *);

/* Called from crtend.o to deregister the unwind info for an object.  */

extern void *__deregister_frame_info (void *);

#endif

void __register_frame_info(void) { /*printf("__register_frame_info\n");*/ }
void __deregister_frame_info(void) { /*printf("__deregister_frame_info\n");*/ }
void __gmon_start__(void) { /*printf("__gmon_start__\n");*/ }

void *__memcpy(void *dst, void *src, size_t size) { /*printf("__memcpy\n");*/ return memcpy(dst, src, size); }
void *__malloc(size_t size) { void *ptr = malloc(size); /*printf("__malloc: %ld @ %08lx\n", (u32)size, (u32)ptr);*/ return ptr; }
void *__memset(void *ptr, int val, size_t size) { /*printf("__memset: %08lx @ %08lx length %ld\n", (u32)val, (u32)ptr, (u32)size);*/ return memset(ptr, val, size); }
void __atexit(void) { /*printf("__atexit\n"); printf("FIXME\n");*/ }
void __free(void *ptr) { /*printf("__free\n");*/ free(ptr); }

static FILE *__stderr;
static int __fprintf(FILE *fp, const char *str, ...) { /*printf("__fprintf\n");*/ return 0; }

/* PPC only */
ssize_t __write(int fd, const void *buf, size_t count) { /*printf("__write\n");*/ return 0; }
int __strcmp(const char *a, const char *b) { /*printf("__strcmp: %s %s\n", a, b);*/ return strcmp(a, b); }
void __abort(void) { /*printf("__abort\n");*/ /* exit(0); */ }

reloc_t reloc[] = {
  { "__builtin_vec_new", &__builtin_vec_new },
  { "__builtin_vec_delete", &__builtin_vec_delete },
  { "__pure_virtual", &__pure_virtual },
  { "___brk_addr", &___brk_addr },
  { "__environ", &__environ },

/*
  { "memcpy", &memcpy },
  { "malloc", &malloc },
  { "memset", &memset },
  { "atexit", &atexit },
  { "free", &free },
*/

  { "memcpy", &__memcpy },
  { "malloc", &__malloc },
  { "memset", &__memset },
  { "atexit", &__atexit },
  { "free", &__free },

  { "stderr", &__stderr },
  { "fprintf", &__fprintf },

  /* FIXME */
  { "__register_frame_info", &__register_frame_info },
  { "__deregister_frame_info", &__deregister_frame_info },
  { "__gmon_start__", &__gmon_start__ },

/* PPC only */

  { "__write", &__write },
  { "strcmp", &__strcmp },
  { "abort", &__abort },

  { NULL, NULL},
};

/* MAIN */

static unsigned char *planes[3];
static void *context = NULL;

int real_decode(void *data, int len, int flags)
{
     unsigned long res;
     int *buff=(unsigned int *)((char*)data+len);
     unsigned long transform_out[5];
     unsigned long transform_in[6]={
          len,      // length of the packet (sub-packets appended)
          0,        // unknown, seems to be unused
          buff[0],  // number of sub-packets - 1
          (unsigned long)&buff[2], // table of sub-packet offsets
          0,        // unknown, seems to be unused
          buff[1],  // timestamp (the integer value from the stream)
     };

     if(len<=0 || flags&2) { return 0; } // skipped frame || hardframedrop

PRINTF("real_decode... %d with %08x\n", len, flags);

     res = rvyuv_transform((unsigned long)data, (unsigned long)planes[0], (unsigned long)transform_in, (unsigned long)transform_out, (unsigned long)context);

PRINTF("done: %ld (%08lx)\n", res, res);

      if (res != 0) {
        return 1;
      }

      return 0;
}

static void *handle = NULL;

static int real_load_plugin(int format, char *path)
{
  void (*func)(void);
  int ver;
  char dll[256];

  ver = ((format >> 24) & 0xff);

  PRINTF("version: %02x\n", ver);

  switch(ver) {
    case 0x20:
//      handle = dlopen("rpcodecs/drv2.so.6.0", RTLD_NOW);
sprintf(dll, "%sdrv2.so.6.0", path);
      handle = dlopen(dll, RTLD_NOW);
      break;

    case 0x30:
//      handle = dlopen("rpcodecs/drv3.so.6.0", RTLD_NOW);
sprintf(dll, "%sdrv3.so.6.0", path);
      handle = dlopen(dll, RTLD_NOW);
      break;

    default:
      printf("unsupported codec\n");
      return 1;
  }

  if (handle == NULL) {
    printf("failed to open library: %s\n", dlerror());
    return 1;
  }

  PRINTF("plugin loaded...\n");

  rvyuv_custom_message = dlsym(handle, "RV20toYUV420CustomMessage");
  rvyuv_free = dlsym(handle, "RV20toYUV420Free");
  rvyuv_hive_message = dlsym(handle, "RV20toYUV420HiveMessage");
  rvyuv_init = dlsym(handle, "RV20toYUV420Init");
  rvyuv_transform = dlsym(handle, "RV20toYUV420Transform");

  func = dlsym(handle, ".init");
  if (func) {
    PRINTF("init...\n");
    func();
    PRINTF("done...\n");
  }

  if (rvyuv_custom_message && rvyuv_free && rvyuv_hive_message && rvyuv_init && rvyuv_transform) {
    PRINTF("ok...\n");
  } else {
    PRINTF("error\n");
    return 1;
  }

  return 0;
}

int real_init(short w, short h, int subformat, int format, char *path)
{
      unsigned long res;
     struct rv_init_t init_data = {11, w, h, 0, 0, subformat, 1, format};

/* IMPORTANT: The codec relies on the planes being one memoryblock */
planes[0] = malloc(w * h * 4);
planes[1] = planes[0] + (w * h);
planes[2] = planes[1] + ((w / 2) * (h / 2));
memset(planes[0], 0, w * h * 4);

PRINTF("real_init... %dx%d with %08x (%08x -> %02x)\n", w, h, format, subformat, 1+((subformat>>16)&7));

     if (real_load_plugin(format, path) != 0) {
       return 1;
     }

PRINTF("%08lx\n", (u32)rvyuv_init);

     res = rvyuv_init((unsigned long)&init_data, (unsigned long)&context);

PRINTF("done: %ld (%08lx)\n", res, res);

      if (res != 0) {
        return 1;
      }

      // setup rv30 codec (codec sub-type and image dimensions):
      if (format >= 0x30000000) {
        unsigned long cmsg24[4] = {w, h, w, h};
        unsigned long cmsg_data[3] = {0x24, 1+((subformat>>16)&7), (unsigned long)&cmsg24};

PRINTF("custom message...\n");

        rvyuv_custom_message((unsigned long)cmsg_data, (unsigned long)context);

/*
         unsigned long cmsg24[4]={sh->disp_w,sh->disp_h,sh->disp_w,sh->disp_h};
         unsigned long cmsg_data[3]={0x24,1+((extrahdr[0]>>16)&7),&cmsg24};
         (*rvyuv_custom_message)(cmsg_data,sh->context);
*/
      }

     return 0;
}

/* */

#include <plugin.h>

static int video_w, video_h, video_gray;
static int real_error = 0;

static double current_time = 0.0;
static double current_frame_time = 0.0;
static double current_video_time = 0.0;

int decode_real(unsigned char *buf, unsigned long length, double sync_time)
{
  if (real_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    int result, len, flags;

    flags = *((unsigned int *)(buf + 0));
    len = *((unsigned int *)(buf + 4));

    buf += 8;
    length = len;

    result = real_decode(buf, length, flags);

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    if (result == 0) {
      amp->video_refresh(planes, current_time, current_time); /* Same time for both */
    }

    amp->total_frames++;

    current_time += current_frame_time;
    current_video_time += current_frame_time;
  }

  return PLUGIN_OK;
}

int init_real(unsigned long type, unsigned long fourcc)
{
  int width, height;

  width = (amp->width + 3) & ~3;
  height = (amp->height + 3) & ~3;

  video_w = width;
  video_h = height;
  video_gray = amp->gray;

  __stderr = stderr; /* huh ? */

  real_error = real_init(video_w, video_h, amp->subid, amp->id, amp->plugin_path);

  if (real_error != 0) {
    return PLUGIN_FAIL;
  }

  amp->total_frames = 0;
  amp->skiped_frames = 0;

  real_error = 0;

  current_time = 0.0;
  current_frame_time = 1.0 / amp->framerate;
  current_video_time = 0.0;

  /* FIXME: Is this correct ? */
  amp->video_refresh_yuv2rgb(6694222, -1643170, -3409833, 8460885, 4883779, -617108528, 0x80808080);

  if (amp->video_init(width, height, width, CMODE_YUV420) != PLUGIN_OK) {
    real_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int exit_real(void)
{
  if (handle != NULL) {
    void (*func)(void);
    func = dlsym(handle, ".fini");
    if (func) {
      PRINTF("fini...\n");
      func();
      PRINTF("done...\n");
    }

    dlclose(handle);
  }

  return PLUGIN_OK;
}

VERSION("REAL v1.00 (020902)");

#define FOURCC_RMOV FOURCC( 'R', 'M', 'O', 'V') /* AMP specific, RealMOVie */
#define FOURCC_rmov FOURCC( 'r', 'm', 'o', 'v')

IDENT_INIT
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_RMOV, init_real, decode_real, exit_real)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_rmov, init_real, decode_real, exit_real)
IDENT_EXIT

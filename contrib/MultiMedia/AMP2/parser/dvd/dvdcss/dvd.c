/*
 *
 * dvd.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#if defined(__AROS__)
#include "aros-inc.h"
#include <sys/types.h>
#endif

#define SCSIF_WRITE 0 /* intended data direction is out */
#define SCSIF_READ  1 /* intended data direction is in */

extern void ExecSCSICmd(unsigned char *cmd,int cmdsize,unsigned char *data,int datasize, unsigned char direction);

#include "dvd.h"
#include "decss.h"
#include "css-auth.h"

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned long __u32;

/* DVD struct types */
#define DVD_STRUCT_PHYSICAL  0x00
#define DVD_STRUCT_COPYRIGHT 0x01
#define DVD_STRUCT_DISCKEY   0x02
#define DVD_STRUCT_BCA       0x03
#define DVD_STRUCT_MANUFACT  0x04

struct dvd_layer {
  __u8 book_version   : 4;
  __u8 book_type      : 4;
  __u8 min_rate       : 4;
  __u8 disc_size      : 4;
  __u8 layer_type     : 4;
  __u8 track_path     : 1;
  __u8 nlayers        : 2;
  __u8 track_density  : 4;
  __u8 linear_density : 4;
  __u8 bca            : 1;
  __u32 start_sector;
  __u32 end_sector;
  __u32 end_sector_l0;
};

struct dvd_physical {
  __u8 type;
  __u8 layer_num;
  struct dvd_layer layer[4];
};

struct dvd_copyright {
  __u8 type;
  __u8 layer_num;
  __u8 cpst;
  __u8 rmi;
};

struct dvd_disckey {
  __u8 type;
  unsigned agid : 2;
  __u8 value[2048];
};

struct dvd_bca {
  __u8 type;
  int len;
  __u8 value[188];
};

struct dvd_manufact {
  __u8 type;
  __u8 layer_num;
  int len;
  __u8 value[2048];
};

typedef union {
  __u8 type;
  struct dvd_physical physical;
  struct dvd_copyright     copyright;
  struct dvd_disckey  disckey;
  struct dvd_bca      bca;
  struct dvd_manufact manufact;
} dvd_struct;

/* Authentication states */
#define DVD_LU_SEND_AGID        0
#define DVD_HOST_SEND_CHALLENGE 1
#define DVD_LU_SEND_KEY1        2
#define DVD_LU_SEND_CHALLENGE   3
#define DVD_HOST_SEND_KEY2      4

/* Termination states */
#define DVD_AUTH_ESTABLISHED    5
#define DVD_AUTH_FAILURE        6

/* Other functions */
#define DVD_LU_SEND_TITLE_KEY   7
#define DVD_LU_SEND_ASF         8
#define DVD_INVALIDATE_AGID     9
#define DVD_LU_SEND_RPC_STATE   10
#define DVD_HOST_SEND_RPC_STATE 11

/* State data */
typedef __u8 dvd_key[5];        /* 40-bit value, MSB is first elem. */
typedef __u8 dvd_challenge[10]; /* 80-bit value, MSB is first elem. */

struct dvd_lu_send_agid {
  __u8 type;
  unsigned agid : 2;
};

struct dvd_host_send_challenge {
  __u8 type;
  unsigned agid : 2;
  dvd_challenge chal;
};

struct dvd_send_key {
  __u8 type;
  unsigned agid : 2;
  dvd_key key;
};

struct dvd_lu_send_challenge {
  __u8 type;
  unsigned agid : 2;
  dvd_challenge chal;
};

#define DVD_CPM_NO_COPYRIGHT  0
#define DVD_CPM_COPYRIGHTED   1

#define DVD_CP_SEC_NONE       0
#define DVD_CP_SEC_EXIST      1

#define DVD_CGMS_UNRESTRICTED 0
#define DVD_CGMS_SINGLE       2
#define DVD_CGMS_RESTRICTED   3

struct dvd_lu_send_title_key {
  __u8 type;
  unsigned agid      : 2;
  dvd_key title_key;
  int lba;
  unsigned cpm       : 1;
  unsigned cp_sec    : 1;
  unsigned cgms      : 2;
};

struct dvd_lu_send_asf {
  __u8 type;
  unsigned agid : 2;
  unsigned asf  : 1;
};

struct dvd_host_send_rpcstate {
  __u8 type;
  __u8 pdrc;
};

struct dvd_lu_send_rpcstate {
  __u8 type : 2;
  __u8 vra  : 3;
  __u8 ucca : 3;
  __u8 region_mask;
  __u8 rpc_scheme;
};

typedef union {
  __u8 type;
  struct dvd_lu_send_agid        lsa;
  struct dvd_host_send_challenge hsc;
  struct dvd_send_key            lsk;
  struct dvd_lu_send_challenge   lsc;
  struct dvd_send_key            hsk;
  struct dvd_lu_send_title_key   lstk;
  struct dvd_lu_send_asf         lsasf;
  struct dvd_host_send_rpcstate  hrpcs;
  struct dvd_lu_send_rpcstate    lrpcs;
} dvd_authinfo;

static byte Challenge[10];
static struct block Key1;
static struct block Key2;
static struct block KeyCheck;
static int varient = -1;

static int authenticate_drive(const byte *key)
{
  int i;

  for (i=0; i<5; i++) {
    Key1.b[i] = key[4-i];
  }

  for (i = 0; i < 32; ++i) {
    CryptKey1(i, Challenge, &KeyCheck);
    if (memcmp(KeyCheck.b, Key1.b, 5) == 0) {
      varient = i;
/*
      printf("Drive Authentic - using varient %d\n", i);
*/
      return 1;
    }
  }

  if (varient == -1)
    printf("Drive would not Authenticate\n");

  return 0;
}

#define CDROM_PACKET_SIZE 12

#define CGC_DATA_UNKNOWN  0
#define CGC_DATA_WRITE    1
#define CGC_DATA_READ     2
#define CGC_DATA_NONE     3

struct cdrom_generic_command
{
  unsigned char cmd[CDROM_PACKET_SIZE];
  unsigned char *buffer;
  unsigned int  buflen;
  unsigned char data_direction;
};

#define GPCMD_READ_DVD_STRUCTURE 0xad
#define GPCMD_REPORT_KEY         0xa4
#define GPCMD_SEND_KEY           0xa3

typedef struct {
  __u16 report_key_length;
  __u8 reserved1;
  __u8 reserved2;

  /* BIG ENDIAN BITFIELD */
  __u8 type_code : 2;
  __u8 vra       : 3;
  __u8 ucca      : 3;

  __u8 region_mask;
  __u8 rpc_scheme;
  __u8 reserved3;
} rpc_state_t;

/* DVD handling */

#define copy_key(dest,src)    memcpy((dest), (src), sizeof(dvd_key))
#define copy_chal(dest,src)   memcpy((dest), (src), sizeof(dvd_challenge))

static void setup_report_key(struct cdrom_generic_command *cgc, unsigned agid, unsigned type)
{
  cgc->cmd[0] = GPCMD_REPORT_KEY;
  cgc->cmd[10] = type | (agid << 6);
  switch (type) {
    case 0: case 8: case 5: {
      cgc->buflen = 8;
      break;
    }
    case 1: {
      cgc->buflen = 16;
      break;
    }
    case 2: case 4: {
      cgc->buflen = 12;
      break;
    }
  }
  cgc->cmd[9] = cgc->buflen;
  cgc->data_direction = CGC_DATA_READ;
}

static void setup_send_key(struct cdrom_generic_command *cgc, unsigned agid, unsigned type)
{
  cgc->cmd[0] = GPCMD_SEND_KEY;
  cgc->cmd[10] = type | (agid << 6);
  switch (type) {
    case 1: {
      cgc->buflen = 16;
      break;
    }
    case 3: {
      cgc->buflen = 12;
      break;
    }
    case 6: {
      cgc->buflen = 8;
      break;
    }
  }
  cgc->cmd[9] = cgc->buflen;
  cgc->data_direction = CGC_DATA_WRITE;
}

static void init_cdrom_command(struct cdrom_generic_command *cgc, void *buf, int len, int type)
{
  memset(cgc, 0, sizeof(struct cdrom_generic_command));
  if (buf)
    memset(buf, 0, len);
  cgc->buffer = (char *) buf;
  cgc->buflen = len;
  cgc->data_direction = type;
}

static int generic_packet(struct cdrom_generic_command *cgc)
{
  unsigned char direction;

  if (cgc->data_direction == CGC_DATA_WRITE) {
    direction = SCSIF_WRITE;
  } else {
    direction = SCSIF_READ;
  }

  ExecSCSICmd(cgc->cmd, 12, cgc->buffer, cgc->buflen, direction);

  return 0;
}

static int dvd_do_auth(dvd_authinfo *ai)
{
  int ret;
  u_char buf[20];
  struct cdrom_generic_command cgc;
  rpc_state_t rpc_state;

  memset(buf, 0, sizeof(buf));
  init_cdrom_command(&cgc, buf, 0, CGC_DATA_READ);

  switch (ai->type) {
  /* LU data send */
    case DVD_LU_SEND_AGID:
      setup_report_key(&cgc, ai->lsa.agid, 0);

      if ((ret = generic_packet(&cgc)))
        return ret;

      ai->lsa.agid = buf[7] >> 6;
      /* Returning data, let host change state */
      break;

    case DVD_LU_SEND_KEY1:
      setup_report_key(&cgc, ai->lsk.agid, 2);

      if ((ret = generic_packet(&cgc)))
        return ret;

      copy_key(ai->lsk.key, &buf[4]);
      /* Returning data, let host change state */
      break;

    case DVD_LU_SEND_CHALLENGE:
      setup_report_key(&cgc, ai->lsc.agid, 1);

      if ((ret = generic_packet(&cgc)))
        return ret;

      copy_chal(ai->lsc.chal, &buf[4]);
      /* Returning data, let host change state */
      break;

    /* Post-auth key */
    case DVD_LU_SEND_TITLE_KEY:
      setup_report_key(&cgc, ai->lstk.agid, 4);
      cgc.cmd[5] = ai->lstk.lba;
      cgc.cmd[4] = ai->lstk.lba >> 8;
      cgc.cmd[3] = ai->lstk.lba >> 16;
      cgc.cmd[2] = ai->lstk.lba >> 24;

      if ((ret = generic_packet(&cgc)))
        return ret;

      ai->lstk.cpm = (buf[4] >> 7) & 1;
      ai->lstk.cp_sec = (buf[4] >> 6) & 1;
      ai->lstk.cgms = (buf[4] >> 4) & 3;
      copy_key(ai->lstk.title_key, &buf[5]);
      /* Returning data, let host change state */
      break;

    case DVD_LU_SEND_ASF:
      setup_report_key(&cgc, ai->lsasf.agid, 5);
          
      if ((ret = generic_packet(&cgc)))
        return ret;

      ai->lsasf.asf = buf[7] & 1;
      break;

    /* LU data receive (LU changes state) */
    case DVD_HOST_SEND_CHALLENGE:
      setup_send_key(&cgc, ai->hsc.agid, 1);
      buf[1] = 0xe;
      copy_chal(&buf[4], ai->hsc.chal);

      if ((ret = generic_packet(&cgc)))
        return ret;

      ai->type = DVD_LU_SEND_KEY1;
      break;

    case DVD_HOST_SEND_KEY2:
      setup_send_key(&cgc, ai->hsk.agid, 3);
      buf[1] = 0xa;
      copy_key(&buf[4], ai->hsk.key);

      if ((ret = generic_packet(&cgc))) {
        ai->type = DVD_AUTH_FAILURE;
        return ret;
      }
      ai->type = DVD_AUTH_ESTABLISHED;
      break;

    /* Misc */
    case DVD_INVALIDATE_AGID:
      setup_report_key(&cgc, ai->lsa.agid, 0x3f);
      if ((ret = generic_packet(&cgc)))
        return ret;
      break;

    /* Get region settings */
    case DVD_LU_SEND_RPC_STATE:
      setup_report_key(&cgc, 0, 8);
      memset(&rpc_state, 0, sizeof(rpc_state_t));
      cgc.buffer = (char *) &rpc_state;

      if ((ret = generic_packet(&cgc)))
        return ret;

      ai->lrpcs.type = rpc_state.type_code;
      ai->lrpcs.vra = rpc_state.vra;
      ai->lrpcs.ucca = rpc_state.ucca;
      ai->lrpcs.region_mask = rpc_state.region_mask;
      ai->lrpcs.rpc_scheme = rpc_state.rpc_scheme;
      break;

    /* Set region settings */
    case DVD_HOST_SEND_RPC_STATE:
      setup_send_key(&cgc, 0, 6);
      buf[1] = 6;
      buf[4] = ai->hrpcs.pdrc;

      if ((ret = generic_packet(&cgc)))
        return ret;
      break;

    default:
      return -1;
  }

  return 0;
}

static int dvd_read_copyright(dvd_struct *s)
{
  int ret;
  u_char buf[8];
  struct cdrom_generic_command cgc;

  init_cdrom_command(&cgc, buf, sizeof(buf), CGC_DATA_READ);
  cgc.cmd[0] = GPCMD_READ_DVD_STRUCTURE;
  cgc.cmd[6] = s->copyright.layer_num;
  cgc.cmd[7] = s->type;
  cgc.cmd[8] = cgc.buflen >> 8;
  cgc.cmd[9] = cgc.buflen & 0xff;

  if ((ret = generic_packet(&cgc)))
    return ret;

  s->copyright.cpst = buf[4];
  s->copyright.rmi = buf[5];

  return 0;
}

int is_dvd_encrypted(void)
{
  dvd_struct s;

  s.type = DVD_STRUCT_COPYRIGHT;
  s.copyright.layer_num = 0;

  dvd_read_copyright(&s);

  if (s.copyright.cpst) {
    return 1;
  }

  return 0;
}

static int dvd_read_disckey(dvd_struct *s)
{
  int ret, size;
  u_char *buf;
  struct cdrom_generic_command cgc;

  size = sizeof(s->disckey.value) + 4;

  if ((buf = (u_char *) malloc(size)) == NULL) {
    return -1;
  }

  init_cdrom_command(&cgc, buf, size, CGC_DATA_READ);
  cgc.cmd[0] = GPCMD_READ_DVD_STRUCTURE;
  cgc.cmd[7] = s->type;
  cgc.cmd[8] = size >> 8;
  cgc.cmd[9] = size & 0xff;
  cgc.cmd[10] = s->disckey.agid << 6;

  if (!(ret = generic_packet(&cgc))) {
    memcpy(s->disckey.value, &buf[4], sizeof(s->disckey.value));
  }

  free(buf);

  return ret;
}

static int GetDiscKey(int agid, char *key, unsigned char *disk_key)
{
  dvd_struct s;
  int i;

  s.type = DVD_STRUCT_DISCKEY;
  s.disckey.agid = agid;
  memset(s.disckey.value, 0, 2048);

  dvd_read_disckey(&s);
  for (i=0; i<2048; i++) {
    s.disckey.value[i] ^= key[4 - (i%5)];
  }

  memcpy(disk_key, s.disckey.value, 2048);

  return 1;
}

static int GetTitleKey(int agid, int lba, char *key, unsigned char *title_key)
{
  dvd_authinfo ai;
  int i;

  ai.type = DVD_LU_SEND_TITLE_KEY;
  ai.lstk.agid = agid;
  ai.lstk.lba = lba;

  if (dvd_do_auth(&ai)) {
    printf("GetTitleKey failed\n");
    return 0;
  }

  for (i=0; i<5; ++i) {
    ai.lstk.title_key[i] ^= key[4 - (i%5)];
  }

  memcpy(title_key, ai.lstk.title_key, 5);

  return 1;
}

static int GetASF()
{
  dvd_authinfo ai;

  ai.type = DVD_LU_SEND_ASF;
  ai.lsasf.agid = 0;
  ai.lsasf.asf = 0;

  if (dvd_do_auth(&ai)) {
    printf("GetASF failed\n");
    return 0;
  }

/*
  printf("%sAuthenticated\n", (ai.lsasf.asf) ? "" : "not ");
*/

  return 1;
}

static int hostauth (dvd_authinfo *ai)
{
  int i;

  switch (ai->type) {
    /* Host data receive (host changes state) */
    case DVD_LU_SEND_AGID:
      ai->type = DVD_HOST_SEND_CHALLENGE;
      break;

    case DVD_LU_SEND_KEY1:
      if (!authenticate_drive(ai->lsk.key)) {
        ai->type = DVD_AUTH_FAILURE;
        return -1;
      }
      ai->type = DVD_LU_SEND_CHALLENGE;
      break;

    case DVD_LU_SEND_CHALLENGE:
      for (i=0; i<10; ++i) {
        Challenge[i] = ai->hsc.chal[9-i];
      }
      CryptKey2(varient, Challenge, &Key2);
      ai->type = DVD_HOST_SEND_KEY2;
      break;

    /* Host data send */
    case DVD_HOST_SEND_CHALLENGE:
      for (i=0; i<10; ++i) {
        ai->hsc.chal[9-i] = Challenge[i];
      }
      /* Returning data, let LU change state */
      break;

    case DVD_HOST_SEND_KEY2:
      for (i=0; i<5; ++i) {
        ai->hsk.key[4-i] = Key2.b[i];
      }
      /* Returning data, let LU change state */
      break;

     default:
       return -1;
  }

  return 0;
}

static int authenticate(int title, int lba, unsigned char *key)
{
  dvd_authinfo ai;
  int i, rv, tries, agid;

  memset(&ai, 0, sizeof (ai));

  GetASF();

  /* Init sequence, request AGID */
  for (tries=1, rv=-1; rv==-1 && tries<4; ++tries) {
/*
    printf("Request AGID [%d]...\t", tries);
*/
    ai.type = DVD_LU_SEND_AGID;
    ai.lsa.agid = 0;
    rv = dvd_do_auth(&ai);

    if (rv == -1) {
      ai.type = DVD_INVALIDATE_AGID;
      ai.lsa.agid = 0;
      dvd_do_auth(&ai);
    }
  }

  if (tries == 4) {
    printf("Cannot get AGID\n");
    return -1;
  }

  for (i=0; i<10; ++i) {
    Challenge[i] = i;
  }

  /* Send AGID to host */
  if (hostauth(&ai) < 0) {
    printf("Send AGID to host failed\n");
    return -1;
  }

  /* Get challenge from host */
  if (hostauth(&ai) < 0) {
    printf("Get challenge from host failed\n");
    return -1;
  }
  agid = ai.lsa.agid;

  /* Send challenge to LU */
  if (dvd_do_auth(&ai) < 0) {
    printf("Send challenge to LU failed\n");
    return -1;
  }

  /* Get key1 from LU */
  if (dvd_do_auth(&ai) < 0) {
    printf("Get key1 from LU failed\n");
    return -1;
  }

  /* Send key1 to host */
  if (hostauth(&ai) < 0) {
    printf("Send key1 to host failed\n");
    return -1;
  }

  /* Get challenge from LU */
  if (dvd_do_auth(&ai) < 0) {
    printf("Get challenge from LU failed\n");
    return -1;
  }

  /* Send challenge to host */
  if (hostauth(&ai) < 0) {
    printf("Send challenge to host failed\n");
    return -1;
  }

  /* Get key2 from host */
  if (hostauth(&ai) < 0) {
    printf("Get key2 from host failed\n");
    return -1;
  }

  /* Send key2 to LU */
  if (dvd_do_auth(&ai) < 0) {
    printf("Send key2 to LU failed (expected)\n");
    return -1;
  }

  if (ai.type == DVD_AUTH_ESTABLISHED) {
    printf("DVD is authenticated\n");
  } else if (ai.type == DVD_AUTH_FAILURE) {
    printf("DVD authentication failed\n");
  }

  memcpy(Challenge, Key1.b, 5);
  memcpy(Challenge+5, Key2.b, 5);
  CryptBusKey(varient, Challenge, &KeyCheck);

  GetASF();

  if (title) {
    GetTitleKey(agid, lba, KeyCheck.b, key);
  } else {
    GetDiscKey(agid, KeyCheck.b, key);
  }

  GetASF();

  return 0;
}

int get_disk_key(unsigned char *disk_key)
{
  return authenticate(0, 0, disk_key);
}

int get_title_key(unsigned char *title_key, int lba)
{
  return authenticate(1, lba, title_key);
}

/*
 *
 * keyfile.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prefs.h"
#include "keyfile.h"
#include "version.h"

int check_keyfile()
{
  char fname[50], lname[50], *str, *ptr;
  unsigned long serial, *keyfile;
  int i, length, registered;
  FILE *fp;

  registered = PREFS_FALSE;

  fp = fopen("S:AMP.key", "r");
  if (fp == NULL) {
    char filename[256];
    strcpy(filename, prefs.cwd);
    strcat(filename, "AMP.key");
    fp = fopen(filename, "r");
  }

  if (fp != NULL) {
   fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    length /= 4;

    str = (char *)malloc(length + 1);
    keyfile = (unsigned long *)malloc(length*4);
    memset(str, 0, length + 1);

    fread(keyfile, sizeof(unsigned long), length, fp);
    fclose(fp);

    for (i=0; i<length; i++) {
      unsigned long data, value;

      value = keyfile[i];

      data = ( ((value&0xc0000000)>>24) |
               ((value&0x00300000)>>16) |
               ((value&0x00000c00)>>8) |
               (value&0x00000003) );

      str[i] = (char)data;
    }

    ptr = strstr(str, "AMPKEY1");
    if ((ptr != NULL) && ((ptr - str) == 111)) {
      ptr = strstr(ptr + 1, "AMPKEY1");
      if ((ptr != NULL) && ((length - (ptr - str)) == 80)) {

        /* Valid keyfile */
        ptr = strstr(str, "AMPKEY1");
        ptr += 8;

        /* Get firstname */
        i=0;
        while (ptr[i] != ' ') {
          fname[i]=ptr[i];
          i++;
        }
        fname[i]='\0';
        ptr += i+1;
  
        /* Get lastname */
        i=0;
        while (ptr[i] != ' ') {
          lname[i]=ptr[i];
          i++;
        }
        lname[i]='\0';
        ptr += i+1;

        /* Get serial */
        sscanf(ptr, "%ld", &serial);
        registered = PREFS_TRUE;
      }
    }
  }

  if (registered != PREFS_TRUE) {
    sprintf(prefs.registered, "%s (Unregistered)", &ver[6]);
  } else {
    sprintf(prefs.registered, "%s (%s %s, #%ld)", &ver[6], fname, lname, serial);
  }

  return registered;
}

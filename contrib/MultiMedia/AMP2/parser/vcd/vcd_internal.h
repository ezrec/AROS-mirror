/*
 *
 * vcd_internal.h
 *
 */

#ifdef SVCD
  #define SECTORS_AT_ONCE 30 /* (max 150 sectors/sec / 5 times per second = 30 sectors) */
#else
  #define SECTORS_AT_ONCE 15 /* (max 75 sectors/sec / 5 times per second = 15 sectors) */
#endif

  int decoders_running = 0, size, pos, type, i;
  int current_sector, sectors;
  int no_video, no_audio;
  unsigned char *data, *ptr, byte;
  double pts;

  data = malloc(RAW_SECTOR_SIZE*SECTORS_AT_ONCE);
  if (data == NULL) {
    return;
  }

  current_sector = min_sector;

  if (plugin_init() != CORE_OK) {
    return;
  }

  no_video = prefs.no_video;
  no_audio = prefs.no_audio;

  while (current_sector < max_sector) {
    /* Read a maximum of SECTORS_AT_ONCE sectors at once */
    sectors = max_sector - current_sector;
    if (sectors > SECTORS_AT_ONCE) {
      sectors = SECTORS_AT_ONCE;
    }

    ExecRead(current_sector, data, sectors);
    current_sector += sectors;

    /* Parse the read data */
    for (i=0; i<sectors; i++) {
      ptr = &data[sector_size * i];

      if (*(unsigned long *)ptr != 0x000001ba) {
        continue;
      }

#ifdef SVCD
      switch (ptr[17]) {
#else
      switch (ptr[15]) {
#endif
        case VIDEO_STREAM_ID:
          type = 0;
          if (no_video == PREFS_ON) {
            continue;
          }
        break;

        case AUDIO_STREAM_ID:
          type = 1;
          if (no_audio == PREFS_ON) {
            continue;
          }
        break;

        default:
          continue;
        break;
      }

#ifdef SVCD
      size = (ptr[18] << 8) + ptr[19];
      pts = -1.0;

      byte = ptr[21];

      /* 0x02 or 0x03 */
      if ((byte >> 6) >= 0x02) {
        /* 0x02 => 23 + 5 * 0 = 23, 0x03 => 23 + 5 * 1 = 28 */
        pos = 23 + 5 * ((byte >> 6) & 0x01);
        pts = BUF2PTS(&ptr[pos]);
      }

      pos = ptr[22] + 3;
      size -= pos;
      pos += 20;
#else
      size = (ptr[16] << 8) + ptr[17];
      pts = -1.0;
      pos = 0;
      ptr += 18;

      while (ptr[pos] & 0x80) {
        pos++;
      }

      byte = ptr[pos];

      if ((byte >> 6) == 0x01) {
        pos += 2;
        byte = ptr[pos];
      }

      /* FIXME: Can this one be optimized like for SVCD above? */
      if ((byte >> 4) == 0x02) {
        /* Get PTS */
        pts = BUF2PTS(&ptr[pos+0]);
        pos += 5;
      } else if ((byte >> 4) == 0x03) {
        /* Get DTS so I don't need to bother about frame reordering and such */
        pts = BUF2PTS(&ptr[pos+5]);
        pos += 10;
      } else {
        pos += 1;
      }

      size -= pos;
#endif

      while (amp_fwrite(type, ptr + pos, size, pts, current_sector + i) == BUF_FALSE) {
        /* Buffer full, can't write */
        if (decoders_running == 0) {
          if (no_video == PREFS_OFF) {
            if (plugin_video_init(SUBTYPE_MPEGX, FOURCC_NONE, 0, 0, 0, 0.0) != CORE_OK) {
              no_video = PREFS_ON;
            }
          }

          if (no_audio == PREFS_OFF) {
            if (plugin_audio_init(SUBTYPE_MPEGX, FOURCC_NONE, 0, 0, 0) != CORE_OK) {
              no_audio = PREFS_ON;
            }
          }

          if ((no_video == PREFS_ON) && (no_audio == PREFS_ON)) {
            goto get_out; /* EXIT */
          }

          if (plugin_start() != CORE_OK) {
            goto get_out; /* EXIT */
          }
          decoders_running = 1;
        }
      }
    }

    if (decoders_running) {
      int position, check;
      position = (int)((65535 * (long long)(video_ftell() - min_sector)) / (long long)(max_sector - min_sector));
      check = core_input(position);
      if (check >= 0) {
        current_sector = min_sector + ((max_sector - min_sector) * check) / 100;

        if (no_video == PREFS_OFF) {
          video_fseek();
        }

        if (no_audio == PREFS_OFF) {
          audio_fseek();
        }
      } else if (check == INPUT_QUIT) {
        goto get_out; /* EXIT */
      }
    }
  }

get_out: /* FIXME: Labels are bad */

  plugin_exit();

  free(data);

  #undef SECTORS_AT_ONCE


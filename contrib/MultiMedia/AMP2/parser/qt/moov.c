#include "quicktime.h"

int quicktime_moov_init(quicktime_moov_t *moov)
{
     int i;

     moov->total_tracks = 0;
     for(i = 0 ; i < MAXTRACKS; i++) moov->trak[i] = 0;
     quicktime_mvhd_init(&(moov->mvhd));
     quicktime_udta_init(&(moov->udta));
     quicktime_ctab_init(&(moov->ctab));
     return 0;
}

int quicktime_moov_delete(quicktime_moov_t *moov)
{
     while(moov->total_tracks) quicktime_delete_trak(moov);
     quicktime_mvhd_delete(&(moov->mvhd));
     quicktime_udta_delete(&(moov->udta));
     quicktime_ctab_delete(&(moov->ctab));
     return 0;
}

#include <zlib.h>

int quicktime_read_moov(quicktime_t *file, quicktime_moov_t *moov, quicktime_atom_t *parent_atom)
{
/* mandatory mvhd */
     quicktime_atom_t leaf_atom;

     do
     {
          quicktime_atom_read_header(file, &leaf_atom);

          if(quicktime_atom_is(&leaf_atom, "cmov")) 
          {
               quicktime_atom_t compressed_atom;

               unsigned char *cmov_buf = 0;
               unsigned char *moov_buf = 0;
               longest cmov_sz, tlen;
               int moov_sz;

               quicktime_atom_read_header(file, &compressed_atom);

               if(quicktime_atom_is(&compressed_atom, "dcom"))
               {
                    int zlibfourcc;
                    longest offset;

                    quicktime_read_char32(file, (char *)&zlibfourcc);

                    #define QT_zlib 0x7A6C6962
                    zlibfourcc = quicktime_atom_read_size((char *)&zlibfourcc);

                    if(compressed_atom.size - 4 > 0)
                    {
                         offset = file->ftell_position + compressed_atom.size - 4;
                         quicktime_fseek(file, offset);
                    }
               }

               quicktime_atom_read_header(file, &compressed_atom);

               if(quicktime_atom_is(&compressed_atom, "cmvd"))
               {
                    z_stream zstrm;
                    int zret;

                    /* read how large uncompressed moov will be */
                    quicktime_read_char32(file, (char *)&moov_sz);

                    moov_sz = quicktime_atom_read_size((char *)&moov_sz);
                    cmov_sz = compressed_atom.size - 4;

                    /* Allocate buffer for compressed header */
                    cmov_buf = (unsigned char *)malloc( cmov_sz );
                    if (cmov_buf == 0) 
                    {
                         return 0;
                    }

                    /* Read in compressed header */
                    tlen = quicktime_read_data(file, (char*)cmov_buf, cmov_sz);

                    if (tlen != 1)
                    { 
                         free(cmov_buf);
                         return 0;
                    }

                    /* Allocate buffer for decompressed header */
                    moov_sz += 16; /* slop?? */
                    moov_buf = (unsigned char *)malloc( moov_sz );

                    if (moov_buf == 0) 
                    {
                         return 0;
                    }

                    zstrm.zalloc          = (alloc_func)0;
                    zstrm.zfree           = (free_func)0;
                    zstrm.opaque          = (voidpf)0;
                    zstrm.next_in         = cmov_buf;
                    zstrm.avail_in        = cmov_sz;
                    zstrm.next_out        = moov_buf;
                    zstrm.avail_out       = moov_sz;

                    zret = inflateInit(&zstrm);

                    if (zret != Z_OK)
                    {
                         break;
                    }

                    zret = inflate(&zstrm, Z_NO_FLUSH);

                    if ((zret != Z_OK) && (zret != Z_STREAM_END))
                    {
                         break;
                    }

                    moov_sz = zstrm.total_out;
                    zret = inflateEnd(&zstrm);

                    file->decompressed_buffer_size = moov_sz;
                    file->decompressed_buffer = (char*)moov_buf;
                    file->decompressed_position = 8; // Passing the first moov
               }

          }
          else
          if(quicktime_atom_is(&leaf_atom, "mvhd"))
          {
               quicktime_read_mvhd(file, &(moov->mvhd));
          }
          else
          if(quicktime_atom_is(&leaf_atom, "clip"))
          {
               quicktime_atom_skip(file, &leaf_atom);
          }
          else
          if(quicktime_atom_is(&leaf_atom, "trak"))
          {
               quicktime_trak_t *trak = quicktime_add_trak(moov);
               quicktime_read_trak(file, trak, &leaf_atom);
          }
          else
          if(quicktime_atom_is(&leaf_atom, "udta"))
          {
               quicktime_read_udta(file, &(moov->udta), &leaf_atom);
               quicktime_atom_skip(file, &leaf_atom);
          }
          else
          if(quicktime_atom_is(&leaf_atom, "ctab"))
          {
               quicktime_read_ctab(file, &(moov->ctab));
          }
          else
          quicktime_atom_skip(file, &leaf_atom);
     }while((quicktime_position(file) < parent_atom->end && file->decompressed_buffer==NULL)
         || (quicktime_position(file) < file->decompressed_buffer_size && file->decompressed_buffer!=NULL));

     return 0;
}

void quicktime_update_durations(quicktime_moov_t *moov)
{
     
}

int quicktime_shift_offsets(quicktime_moov_t *moov, longest offset)
{
     int i;
     for(i = 0; i < moov->total_tracks; i++)
     {
          quicktime_trak_shift_offsets(moov->trak[i], offset);
     }
     return 0;
}

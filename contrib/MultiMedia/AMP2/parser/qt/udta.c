#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "quicktime.h"

int quicktime_udta_init(quicktime_udta_t *udta)
{
     udta->copyright = 0;
     udta->copyright_len = 0;
     udta->name = 0;
     udta->name_len = 0;
     udta->info = 0;
     udta->info_len = 0;
     return 0;
}

int quicktime_udta_delete(quicktime_udta_t *udta)
{
     if(udta->copyright_len)
     {
          free(udta->copyright);
     }
     if(udta->name_len)
     {
          free(udta->name);
     }
     if(udta->info_len)
     {
          free(udta->info);
     }
     quicktime_udta_init(udta);
     return 0;
}

int quicktime_read_udta(quicktime_t *file, quicktime_udta_t *udta, quicktime_atom_t *udta_atom)
{
     quicktime_atom_t leaf_atom;
     int result = 0;

     do
     {
          quicktime_atom_read_header(file, &leaf_atom);
          
          if(quicktime_atom_is(&leaf_atom, "©cpy"))
          {
               result += quicktime_read_udta_string(file, &(udta->copyright), &(udta->copyright_len));
          }
          else
          if(quicktime_atom_is(&leaf_atom, "©nam"))
          {
               result += quicktime_read_udta_string(file, &(udta->name), &(udta->name_len));
          }
          else
          if(quicktime_atom_is(&leaf_atom, "©inf"))
          {
               result += quicktime_read_udta_string(file, &(udta->info), &(udta->info_len));
          }
          else
          quicktime_atom_skip(file, &leaf_atom);
     }while(quicktime_position(file) < udta_atom->end);

     return result;
}

int quicktime_read_udta_string(quicktime_t *file, char **string, int *size)
{
     int result;

     if(*size) free(*string);
     *size = quicktime_read_int16(file);  /* Size of string */
     quicktime_read_int16(file);  /* Discard language code */
     *string = malloc(*size + 1);
     result = quicktime_read_data(file, *string, *size);
     (*string)[*size] = 0;
     return !result;
}

int quicktime_set_udta_string(char **string, int *size, char *new_string)
{
     if(*size) free(*string);
     *size = strlen(new_string + 1);
     *string = malloc(*size + 1);
     strcpy(*string, new_string);
     return 0;
}

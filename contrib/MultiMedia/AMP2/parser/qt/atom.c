#include "quicktime.h"

int quicktime_atom_reset(quicktime_atom_t *atom)
{
     atom->end = 0;
     atom->type[0] = atom->type[1] = atom->type[2] = atom->type[3] = 0;
     return 0;
}

int quicktime_atom_read_header(quicktime_t *file, quicktime_atom_t *atom)
{
     char header[10];

     quicktime_atom_reset(atom);

     atom->start = quicktime_position(file);

     if(!quicktime_read_data(file, header, HEADER_LENGTH)) return 1;
     quicktime_atom_read_type(header, atom->type);
     atom->size = quicktime_atom_read_size(header);
     atom->end = atom->start + atom->size;

/* Skip placeholder atom */
     if(quicktime_match_32(atom->type, "wide"))
     {
          atom->start = quicktime_position(file);
          quicktime_atom_reset(atom);
          if(!quicktime_read_data(file, header, HEADER_LENGTH)) return 1;
          quicktime_atom_read_type(header, atom->type);
          atom->size -= 8;
          if(atom->size <= 0)
          {
/* Wrapper ended.  Get new atom size */
               atom->size = quicktime_atom_read_size(header);
          }
          atom->end = atom->start + atom->size;
     }
     else
/* Get extended size */
     if(atom->size == 1)
     {
          if(!quicktime_read_data(file, header, HEADER_LENGTH)) return 1;
          atom->size = quicktime_atom_read_size64(header);
          atom->end = atom->start + atom->size;
     }

     return 0;
}

int quicktime_atom_is(quicktime_atom_t *atom, char *type)
{
     if(atom->type[0] == type[0] &&
          atom->type[1] == type[1] &&
          atom->type[2] == type[2] &&
          atom->type[3] == type[3])
     return 1;
     else
     return 0;
}

unsigned long quicktime_atom_read_size(char *data)
{
     unsigned long result;
     unsigned long a, b, c, d;
     
     a = (unsigned char)data[0];
     b = (unsigned char)data[1];
     c = (unsigned char)data[2];
     d = (unsigned char)data[3];

     result = (a << 24) | (b << 16) | (c << 8) | d;

// extended header is size 1
//   if(result < HEADER_LENGTH) result = HEADER_LENGTH;
     return result;
}

longest quicktime_atom_read_size64(char *data)
{
     ulongest result, a, b, c, d, e, f, g, h;

     a = (unsigned char)data[0];
     b = (unsigned char)data[1];
     c = (unsigned char)data[2];
     d = (unsigned char)data[3];
     e = (unsigned char)data[4];
     f = (unsigned char)data[5];
     g = (unsigned char)data[6];
     h = (unsigned char)data[7];

     result = (a << 56) | 
          (b << 48) | 
          (c << 40) | 
          (d << 32) | 
          (e << 24) | 
          (f << 16) | 
          (g << 8) | 
          h;

     if(result < HEADER_LENGTH) result = HEADER_LENGTH;
     return (longest)result;
}

void quicktime_atom_read_type(char *data, char *type)
{
     type[0] = data[4];
     type[1] = data[5];
     type[2] = data[6];
     type[3] = data[7];
}

int quicktime_atom_skip(quicktime_t *file, quicktime_atom_t *atom)
{
     if(atom->start == atom->end) atom->end++;
     return quicktime_set_position(file, atom->end);
}

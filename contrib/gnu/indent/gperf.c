/* C code produced by gperf version 2.7 */
/* Command-line: gperf -D -c -l -p -t -T -g -j1 -o -K rwd -N is_reserved indent.gperf  */

#define TOTAL_KEYWORDS 32
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 35
/* maximum key range = 33, duplicates = 0 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char asso_values[] =
    {
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 19, 11,
       1,  0, 20, 19,  3,  9, 36,  0,  9,  2,
      11,  0, 36, 36,  3,  4,  0, 19, 14,  0,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36
    };
  return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#endif
struct templ *
is_reserved (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char lengthtable[] =
    {
       2,  4,  5,  4,  6,  7,  5,  6,  6,  3,  6,  8,  4,  5,
       6,  4,  4,  6,  6,  8,  4,  5,  5,  3,  7,  8,  6,  6,
       2,  4,  6,  5
    };
  static struct templ wordlist[] =
    {
      {"do", rw_sp_nparen,},
      {"else", rw_sp_else,},
      {"while", rw_sp_paren,},
      {"enum", rw_enum,},
      {"double", rw_decl,},
      {"default", rw_case,},
      {"short", rw_decl,},
      {"struct", rw_struct_like,},
      {"signed", rw_decl,},
      {"int", rw_decl,},
      {"switch", rw_switch,},
      {"register", rw_decl,},
      {"case", rw_case,},
      {"const", rw_decl,},
      {"extern", rw_decl,},
      {"char", rw_decl,},
      {"void", rw_decl,},
      {"return", rw_return,},
      {"static", rw_decl,},
      {"volatile", rw_decl,},
      {"goto", rw_break,},
      {"break", rw_break,},
      {"float", rw_decl,},
      {"for", rw_sp_paren,},
      {"typedef", rw_decl,},
      {"unsigned", rw_decl,},
      {"va_dcl", rw_decl,},
      {"sizeof", rw_sizeof,},
      {"if", rw_sp_paren,},
      {"long", rw_decl,},
      {"global", rw_decl,},
      {"union", rw_struct_like,}
    };

  static short lookup[] =
    {
      -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
      25, 26, 27, 28, 29, -1, 30, 31
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              if (len == lengthtable[index])
                {
                  register const char *s = wordlist[index].rwd;

                  if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                    return &wordlist[index];
                }
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const unsigned char *lengthptr = &lengthtable[TOTAL_KEYWORDS + lookup[offset]];
              register struct templ *wordptr = &wordlist[TOTAL_KEYWORDS + lookup[offset]];
              register struct templ *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  if (len == *lengthptr)
                    {
                      register const char *s = wordptr->rwd;

                      if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                        return wordptr;
                    }
                  lengthptr++;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}

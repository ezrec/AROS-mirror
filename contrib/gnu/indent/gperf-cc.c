/* C code produced by gperf version 2.7 */
/* Command-line: gperf -D -c -l -p -t -T -g -j1 -o -K rwd -N is_reserved_cc -H hash_cc indent-cc.gperf  */

#define TOTAL_KEYWORDS 48
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 63
/* maximum key range = 60, duplicates = 1 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash_cc (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char asso_values[] =
    {
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,  8, 18,
      14,  0,  9,  5,  0, 26, 64,  3,  0,  4,
       1,  5, 64, 64, 18,  4, 33, 11, 45,  1,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64
    };
  return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

#ifdef __GNUC__
__inline
#endif
struct templ *
is_reserved_cc (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char lengthtable[] =
    {
       4,  3,  5,  6,  4,  4,  6,  6,  4,  9,  4,  6,  5,  5,
       5,  6,  6,  6,  2,  4,  5,  6,  6,  5,  6,  6,  3,  8,
       6,  8,  7,  2,  5,  4,  8,  5,  6,  8,  5,  6,  7,  6,
       7,  8,  7,  5,  3,  4
    };
  static struct templ wordlist[] =
    {
      {"else", rw_sp_else,},
      {"new", rw_return,},
      {"while", rw_sp_paren,},
      {"extern", rw_decl,},
      {"enum", rw_enum,},
      {"long", rw_decl,},
      {"switch", rw_switch,},
      {"global", rw_decl,},
      {"bool", rw_decl,},
      {"signature", rw_struct_like,},
      {"goto", rw_break,},
      {"headof", rw_sizeof,},
      {"break", rw_break,},
      {"union", rw_struct_like,},
      {"sigof", rw_sizeof,},
      {"sizeof", rw_sizeof,},
      {"double", rw_decl,},
      {"delete", rw_return,},
      {"do", rw_sp_nparen,},
      {"case", rw_case,},
      {"catch", rw_sp_paren,},
      {"signed", rw_decl,},
      {"return", rw_return,},
      {"class", rw_struct_like,},
      {"static", rw_decl,},
      {"friend", rw_decl,},
      {"for", rw_sp_paren,},
      {"operator", rw_operator,},
      {"inline", rw_decl,},
      {"unsigned", rw_decl,},
      {"classof", rw_sizeof,},
      {"if", rw_sp_paren,},
      {"throw", rw_return,},
      {"char", rw_decl,},
      {"template", rw_decl,},
      {"short", rw_decl,},
      {"struct", rw_struct_like,},
      {"register", rw_decl,},
      {"float", rw_decl,},
      {"typeof", rw_sizeof,},
      {"typedef", rw_decl,},
      {"va_dcl", rw_decl,},
      {"virtual", rw_decl,},
      {"volatile", rw_decl,},
      {"default", rw_case,},
      {"const", rw_decl,},
      {"int", rw_decl,},
      {"void", rw_decl,}
    };

  static short lookup[] =
    {
       -1,  -1,  -1,  -1,   0,   1,   2,   3,   4,   5,
        6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
      -84,  18,  19,  20,  21,  22,  -1,  23,  24,  25,
       26,  27,  28,  29,  30, -32,  -2,  31,  -1,  32,
       33,  34,  35,  36,  37,  -1,  -1,  38,  39,  40,
       -1,  41,  42,  43,  44,  -1,  45,  -1,  -1,  -1,
       -1,  -1,  46,  47
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_cc (str, len);

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

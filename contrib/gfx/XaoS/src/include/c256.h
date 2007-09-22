#ifdef cpixel_t
#undef cpixel_t
#undef cppixel_t
#undef bpp
#endif
#define cpixel_t pixel8_t
#define cppixel_t ppixel8_t
#define bpp 1
#define bpp1
#include <generic.h>

#ifdef PROFILE
external int profile[32][4];
#define StartProfile(n) ppctimer (profile[n]);
#define EndProfile(n)                                                       \
{                                                                           \
  unsigned int prof_end[2];                                                 \
  unsigned int prof_time;                                                   \
                                                                            \
  ppctimer (prof_end);                                                      \
  if (prof_end[1] >= profile[n][1])                                         \
    prof_time = (((prof_end[1] - profile[n][1]) << 2) / bus_MHz);           \
  else                                                                      \
    prof_time = (((prof_end[1] - profile[n][1]) << 2) / bus_MHz + 1000000); \
  profile[n][2] += prof_time;                                               \
}
#else
#define StartProfile(n)
#define EndProfile(n)
#endif

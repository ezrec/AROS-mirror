#define CFG_DRIVER 0

extern LONG Config[];

const char *GetRomPath(int driver, int path_num);
const char *GetSamplePath(int driver, int path_num);

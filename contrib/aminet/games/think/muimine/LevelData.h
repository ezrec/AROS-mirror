#ifndef __LEVELDATA_H
#define __LEVELDATA_H


/*
    struct LevelData holds data describing a difficulty level
*/
#define LEVEL_NAME_SIZE     20
#define BT_NAME_SIZE        24

struct BestTime
{
    LONG    Time;               // time to clear mine field (-ve if not set)
    char    Name[BT_NAME_SIZE]; // name of player who achieved this time
};

struct LevelData
{
    ULONG   Level;      // difficulty level as in MUIA_MFWindow_Level
    ULONG   MenuId;     // menu item id if level in menu or 0 if not
    Object *MenuItem;   // menu item object when in level menu or NULL
    char    Name[LEVEL_NAME_SIZE];  // name of this level
    struct BestTime BestTimes[3];   // best times for this level
};


/*
    struct LevelDataList holds an allocated list of LevelData's
*/
struct LevelDataList
{
    LONG                NumLevels;      // number of allocated entries
    ULONG               Flags;          // flags: private
    ULONG               CurrentLevel;   // level being played
    struct LevelData *  LevelList;      // pointer to the allocated array
};


/*
    level macros
*/
#define MAKE_LEVEL(w, h, m) (ULONG)((w) | (h) << 8 | (m) << 16)

#define LVL_WIDTH_MASK      0x000000FF
#define LVL_HEIGHT_MASK     0x0000FF00
#define LVL_MINES_MASK      0x0FFF0000

#define LVL_WIDTH(l)        ((l) & LVL_WIDTH_MASK)
#define LVL_HEIGHT(l)       (((l) & LVL_HEIGHT_MASK) >> 8)
#define LVL_MINES(l)        (((l) & LVL_MINES_MASK) >> 16)

#define LVL_SET_WIDTH(l, w)  (((l) & ~LVL_WIDTH_MASK)  | ((w) & LVL_WIDTH_MASK))
#define LVL_SET_HEIGHT(l, h) (((l) & ~LVL_HEIGHT_MASK) | (((h) << 8) & LVL_HEIGHT_MASK))
#define LVL_SET_MINES(l, m)  (((l) & ~LVL_MINES_MASK)  | (((m) << 16) & LVL_MINES_MASK))


/*
    function prototypes
*/
BOOL LoadLevelData(struct LevelDataList * lList, STRPTR fileName);
BOOL SaveLevelData(struct LevelDataList * lList, STRPTR fileName);
BOOL InitLevelDataList(struct LevelDataList * lList, int numLevels);
void DisposeLevelData(struct LevelDataList * lList);

struct LevelData * FindLevel(struct LevelDataList * lList, ULONG level);
struct LevelData * FindMenuId(struct LevelDataList * lList, ULONG menuId);
struct LevelData * FindCurrentLevel(struct LevelDataList * lList);


#endif  /* __LEVELDATA_H */


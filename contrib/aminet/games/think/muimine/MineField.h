#ifndef  __MINEFIELD_H_
#define  __MINEFIELD_H_
/*
    header file for X-Mines type mine field MUI custom class
*/


/********************************************************************

    class attributes

********************************************************************/

/*
    these attributes are set at initialization only
*/
#define MUIA_MineField_Width        (MINEFIELD_TAG_BASE | 0x0001)
#define MUIA_MineField_Height       (MINEFIELD_TAG_BASE | 0x0002)
#define MUIA_MineField_NumMines     (MINEFIELD_TAG_BASE | 0x0003)
#define MUIA_MineField_ImageFile    (MINEFIELD_TAG_BASE | 0x0004)

/*
    these attributes are ISG
*/
#define MUIA_MineField_SafeStart    (MINEFIELD_TAG_BASE | 0x0008)

/*
    these attributes are set at game initialization and by the input
    handler and are used for notification only do not set them yourself
*/
#define MUIA_MineField_TimeTaken        (MINEFIELD_TAG_BASE | 0x0010)
#define MUIA_MineField_MinesLeft        (MINEFIELD_TAG_BASE | 0x0011)
#define MUIA_MineField_GameInitialized  (MINEFIELD_TAG_BASE | 0x0018)
#define MUIA_MineField_GameRunning      (MINEFIELD_TAG_BASE | 0x0019)
#define MUIA_MineField_GameWon          (MINEFIELD_TAG_BASE | 0x001A)
#define MUIA_MineField_GameLost         (MINEFIELD_TAG_BASE | 0x001B)
#define MUIA_MineField_MouseDown        (MINEFIELD_TAG_BASE | 0x0020)


/********************************************************************

    class methods

********************************************************************/

#define MUIM_MineField_Init         (MINEFIELD_TAG_BASE | 0x0801)
#define MUIM_MineField_TimerTick    (MINEFIELD_TAG_BASE | 0x0802)

struct MUIP_MineField_Init          { STACKED ULONG MethodID; };
struct MUIP_MineField_TimerTick     { STACKED ULONG MethodID; };



/********************************************************************

    function prototypes

********************************************************************/

struct MUI_CustomClass * CreateMineFieldClass(void);
void DeleteMineFieldClass(struct MUI_CustomClass * mcc);


#endif /* __MINEFIELD_H_ */

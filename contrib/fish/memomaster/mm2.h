/*Header file for mm2 */


#define LISTEMPTY (MemListPtr->mlh_Head->mln_Succ == NULL)
#define MEMOS_IN_BLOCK 6
#define MIN(a,b) ((a)<=(b)?(a):(b))

/*---------------------------------------------------------------------
 * Defines for GDisplay code
 *
 */
#define MODE_CHOICE 0
#define MODE_INFO 1
#define MODE_ERROR 2

/*----------------------------------------------------------------------
 * Defines for deledadd.c
 *
 */
#define DEA_SGAD_H     8
#define DEA_BGAD_H    11
#define DEA_BGAD_W    55
#define DEA_DATE_W    85
#define DEA_NOTI_W  30
#define DEA_TYPE_W    20
#define DEA_MEMO_W   490


/*----------------------------------------------------------------------
 * Structure definitions etc
 *
 */
struct Memo_Item
  {
  char mi_Date[10];
  char mi_Notice[3];
  char mi_Type[2];
  char mi_Text[61];
  };

struct MI_Mem
  {
  struct MinNode mim_Node;
  struct Memo_Item mim_MI;
  char mim_Select;
  char mim_Pad;
  };

struct MM_Config
  {
  char cols[4]; 	/*Colour register redirection*/
  };



/* defines for tool gadget ids - must be 49 or lower */

#define circle_n  1
#define circle_f  2
#define square_n  3
#define square_f  4
#define line      5
#define oval_n    6
#define oval_f    7
#define freestyle 8
#define rectangle_n	9
#define rectangle_f	10
#define triangle_n	11
#define triangle_f	12

/* fill gadget's id must be 50 or greater */

#define fill_1   50
#define fill_2   51
#define fill_3   52
#define fill_4   53


#define gad_w     10
#define gad_h     20
#define gad_depth  3


#define normal 1
#define filled 0

#define total_gadgets 16


struct Gadget *get_gadgets(void);
struct Gadget *get_pal(void);

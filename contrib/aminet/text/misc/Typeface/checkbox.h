/* checkbox gadget subclass */

extern Class *CheckClass;
Class *InitCheckClass(void);
void FreeCheckClass(void);

#ifdef CheckBoxObject
#undef CheckBoxObject
#endif

#define CheckBoxObject NewObject(CheckClass,NULL

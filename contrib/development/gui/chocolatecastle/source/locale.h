/* ChocolateCastle */
/* Localization */

#include <proto/locale.h>

extern struct Library *LocaleBase;
extern struct Catalog *Cat;

#define LS(id, txt) GetCatalogStr(Cat, id, (STRPTR)txt)

extern STRPTR TodoRadioEntries[3];
extern STRPTR SuperclassRadioEntries[3];
extern STRPTR ReggaeClassTypeEntries[9];

void InitializeStrings(void);

#define MSG_ERROR_REQUESTER_OK_BUTTON                  0
#define MSG_INFO_REQUESTER_OK_BUTTON                   1
#define MSG_MAIN_WINDOW_TITLE                          2
#define MSG_PROJECT_SELECTOR_BUTTON_MUI_CLASS          3
#define MSG_PROJECT_SELECTOR_BUTTON_REGGAE_CLASS       4
#define MSG_APPLICATION_DESCRIPTION                    5
#define MSG_EDITOR_BUTTON_ADD                          6
#define MSG_EDITOR_BUTTON_ADD_HOTKEY                   7
#define MSG_EDITOR_BUTTON_EDIT                         8
#define MSG_EDITOR_BUTTON_EDIT_HOTKEY                  9
#define MSG_EDITOR_BUTTON_DELETE                      10
#define MSG_EDITOR_BUTTON_DELETE_HOTKEY               11
#define MSG_EDITOR_BUTTON_OK_HOTKEY                   12
#define MSG_EDITOR_BUTTON_OK                          13
#define MSG_PROJECT_SOURCE_DIR_LABEL                  14
#define MSG_PROJECT_SOURCE_DIR_REQUEST_TITLE          15
#define MSG_PROJECT_MODULE_NAME_LABEL                 16
#define MSG_PROJECT_TODO_REMARKS_GROUP_TITLE          17
#define MSG_PROJECT_GENERATE_BUTTON                   18
#define MSG_PROJECT_GENERATE_BUTTON_HOTKEY            19
#define MSG_PROJECT_SAVE_BUTTON                       20
#define MSG_PROJECT_SAVE_BUTTON_HOTKEY                21
#define MSG_PROJECT_LOAD_BUTTON                       22
#define MSG_PROJECT_LOAD_BUTTON_HOTKEY                23
#define MSG_PROJECT_SAVE_REQUESTER_TITLE              24
#define MSG_PROJECT_SAVE_REQUESTER_BUTTON             25
#define MSG_PROJECT_SAVE_OPEN_ERROR                   26
#define MSG_PROJECT_SAVE_DIRLOCK_ERROR                27
#define MSG_PROJECT_INFO_PROJECT_SAVED                28
#define MSG_PROJECT_LOAD_REQUESTER_TITLE              29
#define MSG_PROJECT_LOAD_REQUESTER_BUTTON             30
#define MSG_PROJECT_LOAD_OPEN_ERROR                   31
#define MSG_PROJECT_LOAD_DIRLOCK_ERROR                32
#define MSG_PROJECT_INFO_CODE_GENERATED               33
#define MSG_METHOD_EDITOR_METHOD_NAME_LABEL           34
#define MSG_METHOD_EDITOR_METHOD_FUNCTION_LABEL       35
#define MSG_METHOD_EDITOR_METHOD_STRUCTURE_LABEL      36
#define MSG_METHOD_EDITOR_METHOD_IDENTIFIER_LABEL     37
#define MSG_METHOD_EDITOR_ERROR_MISSING_NAME          38
#define MSG_METHOD_EDITOR_ERROR_NAME_DIGIT            39
#define MSG_METHOD_EDITOR_ERROR_MISSING_FUNCTION      40
#define MSG_METHOD_EDITOR_ERROR_FUNCTION_DIGIT        41
#define MSG_METHOD_EDITOR_ERROR_STRUCTURE_DIGIT       42
#define MSG_METHOD_EDITOR_ERROR_MISSING_IDENTIFIER    43
#define MSG_METHOD_EDITOR_WINDOW_TITLE_NEW_METHOD     44
#define MSG_METHOD_LIST_HEADER_NAME                   45
#define MSG_METHOD_LIST_HEADER_IDENTIFIER             46
#define MSG_METHOD_LIST_HEADER_FUNCTION               47
#define MSG_METHOD_LIST_HEADER_STRUCTURE              48
#define MSG_MUI_GENERATOR_SUPERCLASS_LABEL            49
#define MSG_MUI_GENERATOR_SUPERCLASS_PTRNAME_LABEL    50
#define MSG_MUI_GENERATOR_SUPERCLASS_CNAME_LABEL      51
#define MSG_MUI_GENERATOR_METHODS_FRAME_TITLE         52
#define MSG_MUI_GENERATOR_WINDOW_TITLE                53
#define MSG_REGGAE_EDITOR_METHODS_FRAME_TITLE         54
#define MSG_REGGAE_GENERATOR_WINDOW_TITLE             55
#define MSG_ERROR_REQUESTER_UNEXPECTED_EOF            56
#define MSG_ERROR_REQUESTER_SYNTAX_ERROR              57
#define MSG_TODO_REMARKS_RADIO_WARNINGS               58
#define MSG_TODO_REMARKS_RADIO_COMMENTS               59
#define MSG_SUPERCLASS_RADIO_PRIVATE                  60
#define MSG_SUPERCLASS_RADIO_PUBLIC                   61
#define MSG_BUTTON_REGGAE_CLASS_HOTKEY                62
#define MSG_BUTTON_MUI_CLASS_HOTKEY                   63
#define MSG_REGGAE_GENERATOR_EXTERNAL_CLASS           64
#define MSG_GENERATE_ERROR_NO_MODULE_NAME             65
#define MSG_GENERATE_ERROR_MODULE_NAME_DIGIT          66
#define MSG_GENERATOR_AUTODOC_CHECKMARK_LABEL         67
#define MSG_MODULE_UNNAMED                            68
#define MSG_LIBRARY_GROUP_NAME_LABEL                  69
#define MSG_LIBRARY_GROUP_VERSION_LABEL               70
#define MSG_LIBRARY_GROUP_REVISION_LABEL              71
#define MSG_LIBRARY_GROUP_COPYRIGHT_LABEL             72
#define MSG_REGGAE_GENERATOR_CLASS_TYPE_LABEL         73
#define MSG_REGGAE_CLASS_TYPE_STREAM                  74
#define MSG_REGGAE_CLASS_TYPE_DEMUXER                 75
#define MSG_REGGAE_CLASS_TYPE_DECODER                 76
#define MSG_REGGAE_CLASS_TYPE_FILTER                  77
#define MSG_REGGAE_CLASS_TYPE_ENCODER                 78
#define MSG_REGGAE_CLASS_TYPE_MULTIPLEXER             79
#define MSG_REGGAE_CLASS_TYPE_OUTPUT                  80
#define MSG_REGGAE_CLASS_TYPE_NO_TYPE                 81
#define MSG_PROJECT_SOURCE_DIR_HELP                   82
#define MSG_PROJECT_MODULE_NAME_HELP                  83
#define MSG_GENERATOR_AUTODOC_CHECKMARK_HELP          84
#define MSG_PROJECT_TODO_REMARKS_HELP                 85
#define MSG_REGGAE_GENERATOR_EXTERNAL_CLASS_HELP      86
#define MSG_LIBRARY_GROUP_NAME_HELP                   87
#define MSG_LIBRARY_GROUP_VERSION_HELP                88
#define MSG_LIBRARY_GROUP_COPYRIGHT_HELP              89
#define MSG_LIBRARY_GROUP_REVISION_HELP               90


/* Prototypes for functions defined in
settings.c
 */

extern UBYTE SettingsFullName[256];

extern UBYTE ARexxCommandSettingsFullName[256];

BOOL LoadSettings(UBYTE * , BOOL );

BOOL LoadSettingsFR(void);

void LoadSettingsDefault(void);

BOOL SaveSettings(UBYTE * , BOOL );

BOOL SaveSettingsFR(void);

void SaveSettingsDefault(void);

BOOL LoadARexxCommands(UBYTE * );

BOOL LoadARexxCommandsFR(void);

void LoadARexxCommandsDefault(void);

BOOL SaveARexxCommands(UBYTE * );

BOOL SaveARexxCommandsFR(void);

void SaveARexxCommandsDefault(void);


void INIT_6_MUI_Chess_Class();
void INIT_6_MUI_Board_Class();
void INIT_6_MUI_Field_Class();

void EXIT_6_MUI_Chess_Class();
void EXIT_6_MUI_Board_Class();
void EXIT_6_MUI_Field_Class();

void InitModules()
{
INIT_6_MUI_Chess_Class();
INIT_6_MUI_Board_Class();
INIT_6_MUI_Field_Class();
}

void CleanupModules()
{
EXIT_6_MUI_Chess_Class();
EXIT_6_MUI_Board_Class();
EXIT_6_MUI_Field_Class();
}


//BomberVarious
void StartWait ();
void StartFor (int n);
void _Delay (int n);
void FadeOut ();

//BomberSounds
void LoadSounds ();

//BomberKeyboard
void InitKeyboard ();
void SaveKeys ();
void CheckKeyboard ();
void FlushKeys ();
char getch ();

//BomberSprites
void LoadSprites ();
unsigned char _ReadPixel (int x, int y);
void DrawBlock (int x, int y, int x1, int y1);
void DrawTSprite (int x, int y, int x1, int y1, int xs, int ys);

//BomberMap
void CreateMap ();
void DrawMap ();
void BlowBlock (int x, int y);

//BomberPlayers
void DrawPlayers ();
void MovePlayers ();
void KillPlayer (int x, int y);

//BomberExpl
void SetupExpl (int x, int y, int r);
void DoExpl ();

//BomberBonus
void ClearBonus ();
void DrawBonus ();
void CheckBonus ();

//BomberBombs
void InitBombs ();
void DropBombs ();
void CheckBombs ();
void ShortBomb (int x, int y);
void DrawBombs ();

//BomberMidi
void PlayMidi (char *FileName);
void StopMidi ();
void UnloadMidi ();

//BomberMenu
void InitMenu ();
void DrawNumber ();
void MenuLoop ();

//BomberSetup
void InitSetup ();
void SetupDisplay ();
void SetupLoop ();

//BomberWinLose
void InitWinLose ();
void WinLoseLoop ();

//Main game
void ClearGame ();
void NewGame ();
void InitGame ();
void CloseGame ();
void GameLoop ();
void GameFrame ();


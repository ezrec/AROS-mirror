//State indicators
#define         IN_MENU  1
#define         IN_GAME  2
#define         IN_WNLS  3
#define         IN_SETUP 4
int             State = IN_MENU;

//For graphics
unsigned char   *SpriteBuf;
unsigned char   *BlocksBuf;
ULONG           Palette [256];

//Variables
unsigned char   Grid [17][17],
                BonusGrid [17][17];
int             PlayerX [5], 
                PlayerY [5],
                MaxRange [5],
                MaxBombs [5],
                PlayerDead [5],
                PlayerDir [5],
                PlayerFrame [5],
                PlayerWon [5];
int             AllowBombs,
                TimeOut,
                Frame,
                ComputerPlays = 0,
                Players = 2;

//Startup directory
char            StartDir [1024];

#include <stdlib.h>

//Function prototypes
#include "BomberInclude.c"

#include "BomberVarious.c"
#include "BomberSounds.c"
#include "BomberKeyboard.c"
#include "BomberSprites.c"
#include "BomberMap.c"
#include "BomberPlayers.c"
#include "BomberExpl.c"
#include "BomberBonus.c"
#include "BomberBombs.c"
#include "BomberMidi.c"
#include "BomberSetup.c"
#include "BomberWinLose.c"
#include "BomberMenu.c"

//Clear all scores
void ClearGame ()
{
  int p;

  //Nobody has won yet
  for (p = 0; p < Players; p++) PlayerWon [p] = 0;
}

//Start a new game
void NewGame ()
{
  int p;

  //Player one, start at bottom right
  PlayerX [0] = PlayerY [0] = (15 << 4) + 8;
  //Player two start at top left
  PlayerX [1] = PlayerY [1] = (1 << 4) + 8;
  //Player three, start at bottom left
  PlayerX [2] = PlayerX [1];
  PlayerY [2] = PlayerY [0];
  //Player four start at top right
  PlayerX [3] = PlayerX [0];
  PlayerY [3] = PlayerY [1];
  //Draw to the backbuffer
  TargetData = BackBitmapData;
  //Initialize bombs
  InitBombs ();
  //Clear the map
  CreateMap ();
  //Clear the bonuses
  ClearBonus ();
  //Set time out at 3 minutes = 2770 frames
  TimeOut = 2770;
  AllowBombs = -1;
  //Player is not dead, looking down
  for (p = 0; p < Players; p++) {
    PlayerDead [p] = 0;
    PlayerDir [p] = 0;
  }
  //Wait until enter has been released
  do
  {
    HandleIntuiMessages();
  } while (GetAsyncKeyState (VK_RETURN));
  
  CheckKeyboard ();
}

//Check whether the game should end
void CheckEndGame ()
{
  int   LivingPlayers = Players, p;

  //Move towards timeout
  TimeOut--;

  //Count amount of living players
  for (p = 0; p < Players; p++)
    if (PlayerDead [p]) LivingPlayers--;
  //Set timeout and disallow bombs
  if (LivingPlayers < 2) {
    if (TimeOut > 60) TimeOut = 60;
    AllowBombs = 0;
  }

  //If timeout has been reached, show win/lose screen
  if (TimeOut == 0) {
    State = IN_WNLS;
    InitWinLose ();
  }
}

//Initialize the game
void InitGame ()
{
  srand ((unsigned)time (NULL));
  InitKeyboard ();
  CheckKeyboard ();
  GetStartDir ();
  LoadSprites ();
  LoadBlocks ();
  LoadSounds ();
  InitMenu ();
}

//Shut down the game
void CloseGame ()
{
  UnloadMidi ();
}

//The actual game itself
void GameLoop ()
{
  StartWait ();

  //Repeat music
  if (!Playing) PlayMidi ("BOMBER.MID");

  //The game logic
  DrawMap ();
  MovePlayers ();
  DropBombs ();
  CheckBonus ();
  CheckBombs ();
  DoExpl ();
  DrawBonus ();
  DrawBombs ();
  DrawPlayers ();
  CheckEndGame ();
  FadeOut ();
  Frame++;

  //Limit to 15fps
  WaitFor (65); /* 65 */ /* 1000 / FPS */
  //Escape quits to menu
  if (Esc) {
    StopMidi ();
    InitMenu ();
    State = IN_MENU;
  }
}

//Do one game frame
void GameFrame ()
{
  //Read from keyboard
  CheckKeyboard ();

  //Call appropriate frame
  if (State == IN_MENU) MenuLoop ();
  if (State == IN_SETUP) SetupLoop ();
  if (State == IN_GAME) GameLoop ();
  if (State == IN_WNLS) WinLoseLoop ();
}

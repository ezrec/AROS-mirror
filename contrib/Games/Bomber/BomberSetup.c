int     WhichPlayer, WhichKey;

//Initialize the menu
void InitSetup ()
{
  //Draw to front buffer
  TargetData = MainBitmapData;

  //Clear buffer
  memset ((char *)TargetData, 0, WIDTH * HEIGHT);

  //Display title
  Blit (35, 32, 0, 80, 203, 29);

  FlushKeys ();
  WhichPlayer = 0;
  WhichKey = 0;
}

//Display the number of players
void SetupDisplay ()
{
  //Display "player"
  Blit (96, 100, 64, 108, 48, 15);

  //Display number
  Blit (160, 100, WhichPlayer * 8, 168, 8, 15);

  //Display directive
  Blit (60, 132, 64, 123 + WhichKey * 15, 152, 15);
}

//Do the menu stuff
void SetupLoop ()
{
  int Key = getch ();

  //Display instructions
  SetupDisplay ();

  //Add a new key
  if (Key != -1) {
    ChosenKeys [WhichPlayer * 5 + WhichKey] = Key;
    //Move on to next one
    WhichKey++;
    if (WhichKey == 5) {
      WhichKey = 0;
      WhichPlayer++;
    }
  }

  //Exit if we're here
  if (WhichPlayer == 3) {
    SaveKeys ();
    State = IN_MENU;
    InitMenu ();
  }
}

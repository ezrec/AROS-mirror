int     MenuChoice, MenuFrame, MenuPlayers = 2;

//Initialize the menu
void InitMenu ()
{
  int   i;

  //Draw to front buffer
  TargetData = MainBitmapData;

  //Clear buffer
  memset ((char *)TargetData, 0, WIDTH * HEIGHT);

  //Display title
  Blit (35, 32, 0, 80, 203, 29);

  //Display menu
  for (i = 0; i < 4; i++)
    Blit (112, 101 + i * 32, 0, 109 + i * 15, 64, 15);

  //Choose top
  MenuChoice = 0;
  MenuFrame = 0;
  FlushKeys ();
}

//Display the number of players
void DrawNumber ()
{
  int   yt;

  //Display menu
  for (yt = 0; yt < 15; yt++)
    memcpy ((char *)TargetData + (101 + yt + 32) * WIDTH + 179,
            (char *)BlocksBuf + (168 + yt) * 320 + (MenuPlayers - 1) * 8,
            8);
}

//Do the menu stuff
void MenuLoop ()
{
  int   Key;

  //Clear old bomb
  DrawBlock (5, (3 + MenuChoice) * 2, 10, 2);

  //Read from keyboard
  Key = getch ();

  //Move cursor
  if (Key == VK_UP) {
    MenuChoice--;
    if (MenuChoice == -1) MenuChoice = 3;
  }
  if (Key == VK_DOWN) {
    MenuChoice++;
    if (MenuChoice == 4) MenuChoice = 0;
  }

  //Update number of players
  if (MenuChoice == 1) {
    if (Key == VK_LEFT) {
      MenuPlayers--;
      if (MenuPlayers == 1) MenuPlayers = 4;
    }
    if (Key == VK_RIGHT) {
      MenuPlayers++;
      if (MenuPlayers == 5) MenuPlayers = 2;
    }
  }

  //Display new bomb
  MenuFrame ^= 1;
  DrawBlock (5, (3 + MenuChoice) * 2, 10, MenuFrame);
  DrawNumber ();

  //Enter key
  if (Key == VK_RETURN) {
    switch (MenuChoice) {
      //Start game
      case 0:
      case 1:
        //Get number of players
        Players = MenuPlayers;
        ComputerPlays = 0;
        if (MenuPlayers == 1) {
          Players++;
          ComputerPlays = -1;
        }

        //Start the game
        ClearGame ();

        State = IN_GAME;
        NewGame ();

        break;
      //Setup
      case 2:
        State = IN_SETUP;
        InitSetup ();
        break;
      //Quit
      case 3:
        Done = -1;
        break;
    }
  }
}

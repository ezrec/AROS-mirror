int LimitReached = 0;

//Initialize the menu
void InitWinLose ()
{
  int   p, w;
  int   LivingPlayers = Players;

  //Stop MIDI sound
  StopMidi ();

  //Count amount of living players
  for (p = 0; p < Players; p++)
    if (PlayerDead [p]) LivingPlayers--;

  //Reward player
  if (LivingPlayers < 2) {
    for (p = 0; p < Players; p++) {
      if (!PlayerDead [p]) {
        LimitReached = ++PlayerWon [p];
      }
    }
  }

  //Draw to front buffer
  TargetData = MainBitmapData;

  //Clear buffer
  memset ((char *)TargetData, 0, WIDTH*HEIGHT);

  //Display title
  Blit (35, 32, 0, 80, 203, 29);

  //Display players
  for (p = 0; p < Players; p++) {
    if (PlayerDead [p])
      Blit (60, 90 + p * 32, 232, p * 24, 16, 24);
    else
      Blit (60, 90 + p * 32, 208, p * 24, 16, 24);
    for (w = 1; w <= PlayerWon [p]; w++)
      Blit (92 + w * 24, 90 + p * 32, 160, 48, 16, 24);
  }
}

//Do the win/lose stuff
void WinLoseLoop ()
{
  int Key = getch ();

  //Only enter will suffice
  if (Key == VK_RETURN) {
    if (LimitReached == 5) {
      State = IN_MENU;
      InitMenu ();
    } else {
      State = IN_GAME;
      NewGame ();
    }
  }
}

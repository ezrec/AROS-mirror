//Variables
int ExplX [50];     //x-coordinate
int ExplY [50];     //y-coordinate
int ExplL [50][4];  //length of each ray
int ExplF [50];     //Frame counter

//Sets up an explosion at given coordinates with given length
void SetupExpl (int x, int y, int r)
{
  int   i, l;

  //Check for an available explosion
  for (i = 0; i < 50; i++) {
    if (ExplX [i] == 0) break;
  }

  //Store information
  ExplX [i] = x;
  ExplY [i] = y;
  ExplF [i] = -3;
  r++;

  //Check length of rays
  for (l = 1; l < r; l++) { ExplL [i][0] = l; if (Grid [x][y + l]) break; }
  for (l = 1; l < r; l++) { ExplL [i][1] = l; if (Grid [x + l][y]) break; }
  for (l = 1; l < r; l++) { ExplL [i][2] = l; if (Grid [x - l][y]) break; }
  for (l = 1; l < r; l++) { ExplL [i][3] = l; if (Grid [x][y - l]) break; }

  //Play the explosion sound
#if !NO_SOUND  
  PlaySound ((LPCSTR)Expl, NULL, SND_ASYNC | SND_MEMORY);
#endif

  //Clear bonuses and ignite other bombs
  BonusGrid [x][y] = 0;
  for (l = 1; l <= ExplL [i][0]; l++) {
    BonusGrid [x][y + l] = 0;
    ShortBomb (x, y + l);
  }
  for (l = 1; l <= ExplL [i][1]; l++) {
    BonusGrid [x + l][y] = 0;
    ShortBomb (x + l, y);
  }
  for (l = 1; l <= ExplL [i][2]; l++) {
    BonusGrid [x - l][y] = 0;
    ShortBomb (x - l, y);
  }
  for (l = 1; l <= ExplL [i][3]; l++) {
    BonusGrid [x][y - l] = 0;
    ShortBomb (x, y - l);
  }
}

//Updates and draws the explosions
void DoExpl ()
{
  int   i, l, x, y, f;

  //Kill player
  for (i = 0; i < 50; i++) if (ExplX [i] > 0) {
    //Coordinates
    x = ExplX [i];
    y = ExplY [i];
    f = abs (ExplF [i]);

    //Four directions
    for (l = 0; l <= ExplL [i][0]; l++) KillPlayer (x, y + l);
    for (l = 1; l <= ExplL [i][1]; l++) KillPlayer (x + l, y);
    for (l = 1; l <= ExplL [i][2]; l++) KillPlayer (x - l, y);
    for (l = 1; l <= ExplL [i][3]; l++) KillPlayer (x, y - l);
  }

  //Draw all explosion tips
  for (i = 0; i < 50; i++) if (ExplX [i] > 0) {
    //Coordinates
    x = ExplX [i];
    y = ExplY [i];
    f = abs (ExplF [i]);

    //End of the rays
    if (Grid [x][y + ExplL [i][0]] == 0) DrawBlock (x, y + ExplL [i][0], f + 5, 1);
    if (Grid [x + ExplL [i][1]][y] == 0) DrawBlock (x + ExplL [i][1], y, f + 5, 2);
    if (Grid [x - ExplL [i][2]][y] == 0) DrawBlock (x - ExplL [i][2], y, f + 5, 3);
    if (Grid [x][y - ExplL [i][3]] == 0) DrawBlock (x, y - ExplL [i][3], f + 5, 4);
    if (Grid [x][y + ExplL [i][0]] == 2) DrawBlock (x, y + ExplL [i][0], f + 5, 0);
    if (Grid [x + ExplL [i][1]][y] == 2) DrawBlock (x + ExplL [i][1], y, f + 5, 0);
    if (Grid [x - ExplL [i][2]][y] == 2) DrawBlock (x - ExplL [i][2], y, f + 5, 0);
    if (Grid [x][y - ExplL [i][3]] == 2) DrawBlock (x, y - ExplL [i][3], f + 5, 0);
  }
  
  //Draw all explosion rays
  for (i = 0; i < 50; i++) if (ExplX [i] > 0) {
    //Coordinates
    x = ExplX [i];
    y = ExplY [i];
    f = abs (ExplF [i]);

    //Four directions
    for (l = 1; l < ExplL [i][0]; l++) DrawBlock (x, y + l, f, 2);
    for (l = 1; l < ExplL [i][1]; l++) DrawBlock (x + l, y, f, 3);
    for (l = 1; l < ExplL [i][2]; l++) DrawBlock (x - l, y, f, 3);
    for (l = 1; l < ExplL [i][3]; l++) DrawBlock (x, y - l, f, 2);
  }
  
  //Center and remove
  for (i = 0; i < 50; i++) if (ExplX [i] > 0) {
    //Coordinates
    x = ExplX [i];
    y = ExplY [i];
    f = abs (ExplF [i]);

    //Center
    DrawBlock (x, y, f, 1);

    //Time to remove the explosion?
    if (++ExplF [i] == 5) {
      //Remove the actual explosion
      ExplX [i] = 0;
      //Remove any block being blown up
      if (Grid [x][y + ExplL [i][0]] == 2) BlowBlock (x, y + ExplL [i][0]);
      if (Grid [x + ExplL [i][1]][y] == 2) BlowBlock (x + ExplL [i][1], y);
      if (Grid [x - ExplL [i][2]][y] == 2) BlowBlock (x - ExplL [i][2], y);
      if (Grid [x][y - ExplL [i][3]] == 2) BlowBlock (x, y - ExplL [i][3]);
      //Remove the bomb
      Grid [x][y] = 0;
    }
  }
}

int     BombX [5][50];
int     BombY [5][50];
int     BombFuse [5][50];

//Clears the bombs
void InitBombs ()
{
  int b, p;

  for (p = 0; p < Players; p++) {
    MaxBombs [p] = 1;
    MaxRange [p] = 2;
    for (b = 0; b < 50; b++) {
      BombFuse [p][b] = -1;
    }
  }
}

//Check whether the players have dropped a bomb
void DropBombs ()
{
  int b, p, xp, yp;

  //Only if bombs are allowed
  if (!AllowBombs) return;

  //For both players
  for (p = 0; p < Players; p++) {
    //Player coordinates
    xp = PlayerX [p] >> 4;
    yp = PlayerY [p] >> 4;
    //If the fire key is pressed and the player is alive
    if (Fire [p] && !PlayerDead [p])
    //If the place is empty
    if (Grid [xp][yp] == 0) {
      //Search for an available bomb
      for (b = 0; b < MaxBombs [p]; b++) {
        if (BombFuse [p][b] == -1) {
          //Actually drop the bomb
          BombX [p][b] = xp;
          BombY [p][b] = yp;
          BombFuse [p][b] = 46;
          Grid [xp][yp] = 3;
          //Stop looking
          break;
        }
      }
    }
  }
}

//Check whether the bombs should explode
void CheckBombs ()
{
  int b, p;

  //For both players
  for (p = 0; p < Players; p++) {
    //Search for a waiting bomb
    for (b = 0; b < MaxBombs [p]; b++) {
      //If it's still burning, decrease the fuse length
      if (BombFuse [p][b] > 0) 
        BombFuse [p][b]--;
      //It is's going to explode
      else if (BombFuse [p][b] == 0) {
        //Set up an explosion
        SetupExpl (BombX [p][b], BombY [p][b], MaxRange [p]);
        //Make the bomb available
        BombFuse [p][b] = -1;
      }
    }
  }
}

//Makes a certain bomb explode
void ShortBomb (int x, int y)
{
  int b, p;

  //Can't explode if it's out of range
  if (x == 0) return;

  //For both players
  for (p = 0; p < Players; p++) {
    //Search for a waiting bomb
    for (b = 0; b < MaxBombs [p]; b++) {
      //If this is the one, blow it
      if (BombX [p][b] == x &&
          BombY [p][b] == y &&
          BombFuse [p][b] > 0) 
        BombFuse [p][b] = 0;
    }
  }
}

//Actually draw the bombs
void DrawBombs ()
{
  int b, p;

  //For both players
  for (p = 0; p < Players; p++) {
    //Search for a waiting bomb
    for (b = 0; b < MaxBombs [p]; b++) {
      if (BombFuse [p][b] > 0)
        DrawBlock (BombX [p][b], 
                   BombY [p][b],
                   3 + ((BombFuse [p][b] & 2) >> 1),
                   0);
    }
  }
}

//Clear the bonuses
void ClearBonus ()
{
  int   x, y;
  for (y = 0; y < 17; y++) for (x = 0; x < 17; x++)
    BonusGrid [x][y] = 0;
}

//Draw all bonuses
void DrawBonus ()
{
  int   x, y;
  for (y = 0; y < 17; y++) for (x = 0; x < 17; x++)
    if (BonusGrid [x][y]) DrawBlock (x, y, BonusGrid [x][y] + (Frame & 2), 4);
}

//Check whether a player receives a bonus
void CheckBonus ()
{
  int   p, x, y;

  for (p = 0; p < Players; p++) {
    //Calculate coordinates
    x = PlayerX [p] >> 4;
    y = PlayerY [p] >> 4;

    //If there is a bonus
    if (BonusGrid [x][y]) {
      //Take appropriate action
      if (BonusGrid [x][y] == 1) if (MaxRange [p] < 15) MaxRange [p]++;
      if (BonusGrid [x][y] == 2) if (MaxBombs [p] < 8)  MaxBombs [p]++;
      //Play sound
#if !NO_SOUND
      PlaySound ((LPCSTR)Bonus, NULL, SND_ASYNC | SND_MEMORY);
#endif
      //Remove the bonus
      BonusGrid [x][y] = 0;
      DrawBlock (x, y, 0, 0);
    }
  }
}

//Draw the players
void DrawPlayers ()
{
  int   f, sx, i, j, p;
  int   Order [5], Y [5];

  //Sort back to front
  for (i = 0; i < Players; i++) {
    Order [i] = i;
    Y [i] = PlayerY [i];
  }

  for (i = 0; i < Players; i++) {
    for (j = i + 1; j < Players; j++) {
      if (Y [j] < Y [i]) {
        p = Y [i];
        Y [i] = Y [j];
        Y [j] = p;
        p = Order [i];
        Order [i] = Order [j];
        Order [j] = p;
      }
    }
  }
  
  //Draw back to front
  for (i = 0; i < Players; i++) {
    p = Order [i];
    if (PlayerDead [p]) {
      if (PlayerFrame [p] < 8)
      DrawTSprite (PlayerX [p] - 12,
                   PlayerY [p] - 16,
                   24 * PlayerFrame [p]++,
                   p * 48 + 24,
                   25, 25);
    } else {
      f = PlayerFrame [p];
      if (f == 2) f = 0;
      if (f == 3) f = 2;
      sx = PlayerDir [p] * 72 + f * 24;
      DrawTSprite (PlayerX [p] - 12, 
                   PlayerY [p] - 16,
                   sx, p * 48, 24, 24);
    }
  }
}

//Move the players
void MovePlayers ()
{
  int canMove, tryDir;
  int p, px, py, ox, oy;

  for (p = 0; p < Players; p++) if (!PlayerDead [p]) {
    //Assume nothing
    canMove = 0;
    tryDir = -1;
    px = PlayerX [p];
    py = PlayerY [p];

    //Try the direction
    if (Down  [p]) tryDir = 0;
    if (Right [p]) tryDir = 1;
    if (Left  [p]) tryDir = 2;
    if (Up    [p]) tryDir = 3;

    //Check the map
    ox = oy = 0;
    if (tryDir == 0) { oy =   8; }
    if (tryDir == 1) { ox =   8; }
    if (tryDir == 2) { ox = -12; }
    if (tryDir == 3) { oy = -12; }

    //If the grid is empty or the spot is the same, movement is possible
    canMove = (Grid [(px + ox) >> 4][(py + oy) >> 4] == 0);
    if (tryDir == 0 || tryDir == 3) canMove |= (((py + oy) >> 4) == (py >> 4));
    if (tryDir == 1 || tryDir == 2) canMove |= (((px + ox) >> 4) == (px >> 4));

    //Update player
    if (tryDir >= 0) PlayerDir [p] = tryDir;
    if (canMove && tryDir >= 0) {
      if (tryDir == 0) { oy =  4; PlayerX [p] = (PlayerX [p] & 0xFF0) + 8; }
      if (tryDir == 1) { ox =  4; PlayerY [p] = (PlayerY [p] & 0xFF0) + 8; }
      if (tryDir == 2) { ox = -4; PlayerY [p] = (PlayerY [p] & 0xFF0) + 8; }
      if (tryDir == 3) { oy = -4; PlayerX [p] = (PlayerX [p] & 0xFF0) + 8; }
      PlayerX [p] += ox;
      PlayerY [p] += oy;
      PlayerFrame [p]++;
      PlayerFrame [p] &= 3;
    } else {
      PlayerFrame [p] = 0;
    }
  }
}

//Kill a player at a specific location
void KillPlayer (int x, int y)
{
  int   p, tx, ty;

  //Check all players
  for (p = 0; p < Players; p++) {
    //if they're not dead
    if (!PlayerDead [p]) {
      tx = PlayerX [p] >> 4;
      ty = PlayerY [p] >> 4;
      //They can be killed
      if (tx == x && ty == y) {
        PlayerDead [p] = -1;
        PlayerFrame [p] = 0;
#if !NO_SOUND
        PlaySound ((LPCSTR)Lose, NULL, SND_ASYNC | SND_MEMORY);
#endif
      }
    }
  }
}

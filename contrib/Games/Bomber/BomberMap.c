//Create the map
void CreateMap ()
{
  int t, x, y;

  //Fill the grid
  for (y = 0; y < 17; y++) for (x = 0; x < 17; x++) {
    Grid [x][y] = 0;
    if (x == 0 || y == 0 || x == 16 || y == 16) Grid [x][y] = 1;
    if ((x & 1) == 0 && (y & 1) == 0) Grid [x][y] = 1;
  }

  //Random walls
  for (t = 0; t < 192; t++) {
    x = (rand () & 15);
    y = (rand () & 15);
    if (!Grid [x][y]) Grid [x][y] = 2;
  }

  //Opening
  Grid [ 1][ 1] = Grid [ 2][ 1] = Grid [ 1][ 2] =
  Grid [ 1][15] = Grid [ 2][15] = Grid [ 1][14] =
  Grid [15][ 1] = Grid [14][ 1] = Grid [15][ 2] =
  Grid [15][15] = Grid [15][14] = Grid [14][15] = 0;

  //Draw
  for (y = 0; y < 17; y++) for (x = 0; x < 17; x++) {
    DrawBlock (x, y, Grid [x][y], 0);
  }
}

//Draw the map
void DrawMap ()
{
  //Copy the map from the background buffer

  memcpy(MainBitmapData, BackBitmapData, WIDTH * HEIGHT);
}

//Removes a block from the map, adding a bonus?
void BlowBlock (int x, int y)
{
  Grid [x][y] = 0;
  if ((rand () & 63) > 48) BonusGrid [x][y] = (rand () & 1) + 1;
}

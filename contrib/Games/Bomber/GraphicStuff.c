//Creates a new bitmap to draw to
void InitGraph ()
{
  UBYTE *mem;
  
  mem = malloc(WIDTH * HEIGHT * 2);
  if (!mem) cleanup("Out of memory!");
  
  MainBitmapData = mem;
  BackBitmapData = mem + (WIDTH * HEIGHT);
  
}

//Return old bitmap
void CloseGraph ()
{

}

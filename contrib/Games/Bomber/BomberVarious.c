int start;

int GetTickCount(void)
{
    static int first = 1;
    static ULONG startsec;
    ULONG sec, mic;
    int retval;
        
    CurrentTime(&sec, &mic);
    
    if (first)
    {
    	startsec = sec; 
	first = 0;
    }

    retval = (sec - startsec) * 1000 +
    	     mic / 1000;
	     
    return retval;
    
}

//Store start
void StartWait ()
{
  start = GetTickCount ();
}

//Wait for n milliseconds
void WaitFor (int n)
{
  int current;
  do
    current = GetTickCount ();
  while (current < start + n);
}

//Wait for n milliseconds
void _Delay (int n)
{
  StartWait ();
  WaitFor (n);
}

//Fade out
void FadeOut ()
{
#if 0
  int           o;
  unsigned char *Pointer;
  unsigned char f;

  if (TimeOut < 16) {
    f = (16 - TimeOut) << 4;
    Pointer = MainBitmapData;
    for (o = 0; o < 221952; o++) {
      if (Pointer [o] > f) 
        Pointer [o] -= f;
      else
        Pointer [o] = 0;
    }
  }
#endif
}

//Get current directory
void GetStartDir ()
{
#if 0
  GetCurrentDirectory (1024, StartDir);
  if (StartDir [strlen (StartDir)] != '\\')
    strcat (StartDir, "\\");
#endif
}

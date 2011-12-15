#include <string.h> /* memset() */

//Load the sprites
void LoadSprites ()
{
  FILE          *SSFFile;
  unsigned char RGBPal [256][3];
  int           DAC;

  //Assign memory to sprite buffer
  SpriteBuf = (char *)malloc (64000);

  //Load graphics data from file
  SSFFile = fopen ("Bomber0.ssf", "rb");
  fread (SpriteBuf, 7, 1, SSFFile);
  fread (SpriteBuf, 320, 200, SSFFile);
  fread (RGBPal, 3, 256, SSFFile);
  fclose (SSFFile);

  //Convert palette
  for (DAC = 0; DAC < 256; DAC++) {
    RGBPal [DAC][0] <<= 2;
    RGBPal [DAC][1] <<= 2;
    RGBPal [DAC][2] <<= 2;
    Palette [DAC] = (RGBPal [DAC][2]) +
                    (RGBPal [DAC][1] << 8) +
                    (RGBPal [DAC][0] << 16);
  }
}

//Load the sprites
void LoadBlocks ()
{
  FILE          *SSFFile;
  unsigned char RGBPal [256][3];
#if 0
  unsigned char Pixel;
  int           DAC, o1, o2;
#endif
  char          *RawBuf;

  //Assign memory to sprite buffer
  RawBuf = (char *)malloc (64000);

  //Load graphics data from file
  SSFFile = fopen ("Bomber1.ssf", "rb");
  fread (RawBuf, 7, 1, SSFFile);
  fread (RawBuf, 320, 200, SSFFile);
  fread (RGBPal, 3, 256, SSFFile);
  fclose (SSFFile);

  BlocksBuf = RawBuf;
  
#if 0
  //Convert palette
  for (DAC = 0; DAC < 256; DAC++) {
    RGBPal [DAC][0] <<= 2;
    RGBPal [DAC][1] <<= 2;
    RGBPal [DAC][2] <<= 2;
    Palette [DAC] = (RGBPal [DAC][0]) +
                    (RGBPal [DAC][1] << 8) +
                    (RGBPal [DAC][2] << 16);
  }

  //Save as 24bit data
  o2 = 0;
  for (o1 = 0; o1 < 64000; o1++) {
    Pixel = RawBuf [o1];
    BlocksBuf [o2++] = RGBPal [Pixel][2];
    BlocksBuf [o2++] = RGBPal [Pixel][1];
    BlocksBuf [o2++] = RGBPal [Pixel][0];
  }
#endif
}

//This reads a pixel from the sprite buffer
unsigned char _ReadPixel (int x, int y)
{
  return SpriteBuf [(y << 8) + (y << 6) + x];
}

void Blit (int x, int y, int x1, int y1, int w, int h)
{
  int   yt;

#if 0
  x *= 3;
  x1 *= 3;
  w *= 3;
#endif

  //Do blits
  for (yt = 0; yt < h; yt++)
    memcpy ((char *)TargetData + (y + yt) * WIDTH + x,
            (char *)BlocksBuf + (y1 + yt) * 320 + x1,
            w);

}

//This draws a sprite onto the device context
void DrawBlock (int x, int y, int x1, int y1)
{
  int   yt;

  //Convert coordinates
  x *= 16;
  y *= 16;
  x1 *= 16;
  y1 *= 16;

  //Draw each pixel
  for (yt = 0; yt < 16; yt++)
    memcpy ((char *)TargetData + (y + yt) * WIDTH + x,
            (char *)BlocksBuf + (y1 + yt) * 320 + x1,
            16);
}

//This draws a transparent sprite onto the device context
void DrawTSprite (int x, int y, int x1, int y1, int xs, int ys)
{
  int           xt, yt;
  unsigned char c;

  for (xt = 0; xt < xs; xt++) for (yt = 0; yt < ys; yt++) {
    c = _ReadPixel (x1 + xt, y1 + yt);
    if (c) MainBitmapData[(y + yt) * WIDTH + x + xt] = c;
  }
}

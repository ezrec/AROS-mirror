#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "SDL.h"
#include "piece.h"
#include "font.h"

SDL_Surface *video, *layer;
SDL_Rect layerRect;

unsigned char *vBuffer = NULL;

void
pceLCDDispStop ()
{
}

void
pceLCDDispStart ()
{
}

void
initSDL ()
{
  SDL_PixelFormat *pfrm;

  if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
      fprintf (stderr, "Couldn't initialize SDL: %s\n", SDL_GetError ());
      exit (1);
    }
  atexit (SDL_Quit);

  if (fullscreen == 1)
    {
      video =
	SDL_SetVideoMode (SDL_WIDTH, SDL_HEIGHT, 8,
			  SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWPALETTE |
			  SDL_FULLSCREEN);
      SDL_ShowCursor (0);
    }
  else
    video =
      SDL_SetVideoMode (SDL_WIDTH, SDL_HEIGHT, 8,
			SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWPALETTE);

  if (video == NULL)
    {
      fprintf (stderr, "Couldn't set video mode: %s\n", SDL_GetError ());
      exit (1);
    }

  pfrm = video->format;
  layer =
    SDL_CreateRGBSurface (SDL_SWSURFACE, SDL_WIDTH, SDL_HEIGHT, 8,
			  pfrm->Rmask, pfrm->Gmask, pfrm->Bmask, pfrm->Amask);
  if (layer == NULL)
    {
      fprintf (stderr, "Couldn't create surface: %s\n", SDL_GetError ());
      exit (1);
    }

  layerRect.x = 0;
  layerRect.y = 0;
  layerRect.w = SDL_WIDTH;
  layerRect.h = SDL_HEIGHT;

  {
    static SDL_Color pltTbl[4] = {
      {255, 255, 255},
      {170, 170, 170},
      {85, 85, 85},
      {0, 0, 0}
    };
    SDL_SetColors (video, pltTbl, 0, 4);
    SDL_SetColors (layer, pltTbl, 0, 4);
  }
}

void
pceLCDTrans ()
{
  int x, y;
  unsigned char *vbi, *bi;

  bi = layer->pixels;
  for (y = 0; y < SDL_HEIGHT; y++)
    {
      vbi = vBuffer + (y / zoom) * 128;
      for (x = 0; x < SDL_WIDTH; x++)
	{
	  *bi++ = *(vbi + x / zoom);
	}
      bi += layer->pitch - SDL_WIDTH;
    }

  SDL_BlitSurface (layer, NULL, video, &layerRect);
  SDL_Flip (video);
}

unsigned char *keys;

int
pcePadGet ()
{
  static int pad = 0;
  int i = 0, op = pad & 0x00ff;

  int k[] = {
    SDLK_PAGEUP, SDLK_PAGEDOWN, SDLK_LEFT, SDLK_RIGHT,
    SDLK_KP4, SDLK_KP6,
    SDLK_x, SDLK_z, SDLK_SPACE, SDLK_RETURN,
    SDLK_ESCAPE, SDLK_LSHIFT, SDLK_RSHIFT,
    SDLK_PLUS, SDLK_MINUS
  };

  int p[] = {
    PAD_UP, PAD_DN, PAD_LF, PAD_RI,
    PAD_LF, PAD_RI,
    PAD_A, PAD_B, PAD_A, PAD_B,
    PAD_C, PAD_D, PAD_D, -1
  };

  pad = 0;

  do
    {
      if (keys[k[i]] == SDL_PRESSED)
	{
	  pad |= p[i];
	}
      i++;
    }
  while (p[i] >= 0);

  pad |= (pad & (~op)) << 8;

  return pad;
}

int interval = 0;

void
pceAppSetProcPeriod (int period)
{
  interval = period;
}

int exec = 1;

void
pceAppReqExit (int c)
{
  exec = 0;
}

unsigned char *
pceLCDSetBuffer (unsigned char *pbuff)
{
  if (pbuff)
    {
      vBuffer = pbuff;
    }
  return vBuffer;
}

int font_posX = 0, font_posY = 0, font_width = 4, font_height = 6;
unsigned char font_fgcolor = 3, font_bgcolor = 0, font_bgclear = 0;
const char *font_adr = FONT6;

void
pceFontSetType (int type)
{
  const int width[] = { 5, 8, 4 };
  const int height[] = { 10, 16, 6 };
  const char *adr[] = { FONT6, FONT16, FONT6 };

  type &= 3;
  font_width = width[type];
  font_height = height[type];
  font_adr = adr[type];
}

void
pceFontSetTxColor (int color)
{
  font_fgcolor = (unsigned char) color;
}

void
pceFontSetBkColor (int color)
{
  if (color >= 0)
    {
      font_bgcolor = (unsigned char) color;
      font_bgclear = 0;
    }
  else
    {
      font_bgclear = 1;
    }
}

void
pceFontSetPos (int x, int y)
{
  font_posX = x;
  font_posY = y;
}

int
pceFontPrintf (const char *fmt, ...)
{
  unsigned char *adr = vBuffer + font_posX + font_posY * 128;
  unsigned char *pC;
  char c[1024];
  va_list argp;

  va_start (argp, fmt);
  vsprintf (c, fmt, argp);
  va_end (argp);

  pC = c;
  while (*pC)
    {
      int i, x, y;
      const unsigned char *sAdr;
      if (*pC >= 0x20 && *pC < 0x80)
	{
	  i = *pC - 0x20;
	}
      else
	{
	  i = 0;
	}
      sAdr = font_adr + (i & 15) + (i >> 4) * 16 * 16;
      for (y = 0; y < font_height; y++)
	{
	  unsigned char c = *sAdr;
	  for (x = 0; x < font_width; x++)
	    {
	      if (c & 0x80)
		{
		  *adr = font_fgcolor;
		}
	      else if (font_bgclear == 0)
		{
		  *adr = font_bgcolor;
		}
	      adr++;
	      c <<= 1;
	    }
	  adr += 128 - font_width;
	  sAdr += 16;
	}
      adr -= 128 * font_height - font_width;
      pC++;
    }
}

int
pceFileOpen (int * pfa, const char *fname, int mode)
{
  if (mode == FOMD_RD)
    {
      *pfa = open (fname, O_RDONLY);
    }
  else if (mode == FOMD_WR)
    {
      *pfa = open (fname, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    }

  if (*pfa >= 0)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

int
pceFileReadSct (int * pfa, void *ptr, int sct, int len)
{
  return read (*pfa, ptr, len);
}

int
pceFileWriteSct (int * pfa, const void *ptr, int sct, int len)
{
  return write (*pfa, ptr, len);
}

int
pceFileClose (int * pfa)
{
  close (*pfa);
  return 0;
}

int
main (int argc, char *argv[])
{
  SDL_Event event;
  long nextTick, wait;
  int cnt = 0;
  int pzoom;
  char *tail;
  zoom = 3;
  fullscreen = 0;
  if (argc > 1)
    {
      if (strcmp (argv[1], "f") == 0)
	fullscreen = 1;
      else
	{
	  pzoom = strtol (argv[1], &tail, 0);
	  if (pzoom >= 1)
	    zoom = pzoom;
	}
    }

  initSDL ();
  pceAppInit ();

  SDL_WM_SetCaption (PACKAGE_STRING, NULL);

  nextTick = SDL_GetTicks () + interval;
  while (exec)
    {
      SDL_PollEvent (&event);
      keys = SDL_GetKeyState (NULL);

      wait = nextTick - SDL_GetTicks ();
      if (wait > 0)
	{
	  SDL_Delay (wait);
	}

      pceAppProc (cnt);
      //      SDL_Flip(video);

      nextTick += interval;
      cnt++;

      if ((keys[SDLK_ESCAPE] == SDL_PRESSED
	   && (keys[SDLK_LSHIFT] == SDL_PRESSED
	       || keys[SDLK_RSHIFT] == SDL_PRESSED))
	  || event.type == SDL_QUIT)
	{
	  exec = 0;
	}
    }

  pceAppExit ();
}

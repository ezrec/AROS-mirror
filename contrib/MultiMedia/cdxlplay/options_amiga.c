#include "cdxlplay.h"
#include <dos/dos.h>
#include <proto/dos.h>
#ifdef __AMIGAOS4__
#include <inline4/dos.h>
#endif

static const char template[] = "V=VERSION/S,CDXLFILE,R=RATE/N/K,F=FPS/N/K,L=LOOP/S,"
	"W=WIDTH/N/K,H=HEIGHT/N/K,FS=FULLSCREEN/S,P=PIXELRATIO/N/K";

struct args_struct {
	int32_t version;
	const char *filename;
	int32_t *rate;
	int32_t *fps;
	int32_t loop;
	int32_t *width;
	int32_t *height;
	int32_t fullscreen;
	const char *pixel_ratio;
};

static void *rd_args;

int get_options (int argc, char *argv[], struct player_data *pd) {
	struct args_struct args;
	memset(&args, 0, sizeof(args));
	rd_args = ReadArgs(template, (void *)&args, NULL);
	if (!rd_args) {
		PrintFault(IoErr(), PROGNAME);
		return 0;
	}
	if (args.version) {
		SDL_version sdl_ver;
		SDL_VERSION(&sdl_ver);
		printf("%s version %s\nSDL version %d.%d.%d\nBuild date: %s\n",
			PROGNAME, VSTRING, sdl_ver.major, sdl_ver.minor, sdl_ver.patch, __DATE__);
		return 0;
	}
	pd->filename = args.filename;
	if (args.rate) {
		pd->freq = *args.rate;
		if (pd->freq < 4000) pd->freq = 4000;
		else if (pd->freq > 96000) pd->freq = 96000;
	}
	if (args.fps) {
		pd->fps = *args.fps;
		if (pd->fps < 5) pd->fps = 5;
		else if (pd->fps > 100) pd->fps = 100;
	}
	if (args.loop) {
		pd->loop = -1;
	}
	if (args.width) {
		pd->width = *args.width;
	}
	if (args.height) {
		pd->height = *args.height;
	}
	if (args.fullscreen) {
		pd->sdl_video_flags |= SDL_FULLSCREEN;
		pd->sdl_video_flags &= ~SDL_SWSURFACE;
	}
	if (args.pixel_ratio) {
		if (!strcmp(args.pixel_ratio, "1:1"))
			pd->pixel_ratio = 0x10000;
		else if (!strcmp(args.pixel_ratio, "1:2"))
			pd->pixel_ratio = 0x08000;
		else if (!strcmp(args.pixel_ratio, "1:4"))
			pd->pixel_ratio = 0x04000;
		else if (!strcmp(args.pixel_ratio, "2:1"))
			pd->pixel_ratio = 0x20000;
		else
			fprintf(stderr, "Unsupported pixel ratio: %s\n", args.pixel_ratio);
	}
	if (!pd->filename) {
		PrintFault(ERROR_REQUIRED_ARG_MISSING, PROGNAME);
		return 0;
	}
	return 1;
}

void free_options (void) {
	if (rd_args) {
		FreeArgs(rd_args);
		rd_args = NULL;
	}
}

#include "cdxlplay.h"
#include "getopt.h"

static const char short_opts[] = "-?vr:f:lw:h:sp:";

static struct option long_opts[] = {
	{ "help",       no_argument,       NULL, '?' },
	{ "version",    no_argument,       NULL, 'v' },
	{ "rate",       required_argument, NULL, 'r' },
	{ "fps",        required_argument, NULL, 'f' },
	{ "loop",       no_argument,       NULL, 'l' },
	{ "width",      required_argument, NULL, 'w' },
	{ "height",     required_argument, NULL, 'h' },
	{ "fullscreen", no_argument,       NULL, 's' },
	{ "pixelratio", required_argument, NULL, 'p' },
	{ 0 }
};

int get_options (int argc, char *argv[], struct player_data *pd) {
	int c, index, show_usage, show_version;
	const char *arg;
	show_usage = 0;
	show_version = 0;
	while ((c = getopt_long(argc, argv, short_opts, long_opts, &index)) != -1) {
		arg = optarg;
		if (arg && *arg == '=') arg++;
		switch (c) {
			case '\1':
				pd->filename = arg;
				break;
			case '?':
				show_usage = 1;
				break;
			case 'v':
				show_version = 1;
				break;
			case 'r':
				pd->freq = atoi(arg);
				if (pd->freq < 4000) pd->freq = 4000;
				else if (pd->freq > 96000) pd->freq = 96000;
				break;
			case 'f':
				pd->fps = atoi(arg);
				if (pd->fps < 5) pd->fps = 5;
				else if (pd->fps > 100) pd->fps = 100;
				break;
			case 'l':
				pd->loop = -1;
				break;
			case 'w':
				pd->width = atoi(arg);
				break;
			case 'h':
				pd->height = atoi(arg);
				break;
			case 's':
				pd->sdl_video_flags |= SDL_FULLSCREEN;
				pd->sdl_video_flags &= ~SDL_RESIZABLE;
				break;
			case 'p':
				if (!strcmp(arg, "1:1"))
					pd->pixel_ratio = 0x10000;
				else if (!strcmp(arg, "1:2"))
					pd->pixel_ratio = 0x08000;
				else if (!strcmp(arg, "1:4"))
					pd->pixel_ratio = 0x04000;
				else if (!strcmp(arg, "2:1"))
					pd->pixel_ratio = 0x20000;
				else
					fprintf(stderr, "Unsupported pixel ratio: %s\n", arg);
				break;
		}
	}
	if (show_version) {
		SDL_version sdl_ver;
		SDL_VERSION(&sdl_ver);
		printf("%s version %s\nSDL version %d.%d.%d\nBuild date: %s\n",
			PROGNAME, VSTRING, sdl_ver.major, sdl_ver.minor, sdl_ver.patch, __DATE__);
		return 0;
	}
	if (!pd->filename || show_usage) {
		printf("Usage: %s [OPTIONS] CDXLFILE\n", PROGNAME);
		printf("Options:\n"
			"  -?,        --help           Display this information\n"
			"  -v,        --version        Display version information\n"
			"  -r=<rate>, --rate=<rate>    Set sample rate for audio (defaults to 11025 Hz)\n"
			"  -f=<fps>,  --fps=<fps>      Set framerate for playback (defaults to 50 fps)\n"
			"  -l,        --loop           Play animation as a loop\n"
			"  -w=<size>, --width=<size>   Display width (defaults to CDXL width)\n"
			"  -h=<size>, --height=<size>  Display height (defaults to CDXL height)\n"
			"  -s,        --fullscreen     Fullscreen mode\n"
			"  -p=<r>,    --pixelratio=<r> Pixel ratio (defaults to 1:1)\n");
		printf("Supported pixel ratios:\n"
			"  1:1  lowres/highres laced\n"
			"  1:2  highres/super highres laced\n"
			"  1:4  super highres\n"
			"  2:1  lowres laced\n");
		return 0;
	}
	return 1;
}

void free_options (void) {
	/* do nothing */
}

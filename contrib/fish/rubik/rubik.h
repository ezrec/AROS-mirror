
#include <aros/oldprograms.h>

#define LEFT		32
#define TOP		4
#define WIDTH		256L
#define HEIGHT		192L
#define TMPWIDTH	512L
#define TMPHEIGHT	512L
#define MAXHINOLACE	200L
#define N_BIT_PLANES	3L
#define CLEAR		MEMF_CHIP | MEMF_CLEAR

#define MAXFRAMES	26L

#define YES	1
#define NO	0

#define SINA 0x0404
#define COSA 0x3fdf

struct Matrix {WORD uv11, uv12, uv13, uv21, uv22, uv23, uv31, uv32, uv33;};

struct Vector {WORD x, y, z;};

struct Tile {
	struct Vector **vertexstart;
	WORD color;
};

struct Objectinfo {
	struct Matrix *matrix;
	struct Vector *position;
	WORD numpoints;
	struct Vector **points;
	WORD numtiles;
	struct Tile **tiles;
	void (*procedure)();

	WORD bufpointsize;
	struct Vector *bufpoints;
	WORD pptrbufsize;
	struct Vector **pptrbuf;
	WORD colorbufsize;
	WORD *colorbuf;
};

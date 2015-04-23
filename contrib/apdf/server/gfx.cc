#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include "gfx.h"

#define DEBUG_GFXSTATE_CLIP(x)	;
#define DEBUG_GFXSTATE_SAVE(x)	;
#define DEBUG_BEZIER(x)		;
#define DEBUG_BLIT(x)		;
#define DEBUG_BLITMASK(x)	;
#define DEBUG_FILL(x)		;
#define DEBUG_SPANS(x)		;
#define DEBUG_LOOPS(x)

#define dprintf printf

static inline int scale(int a, int b, int c) { return a * b / c; }
static inline int round(double x) { return int(x + 131072.5) - 131072; }

void DashState::init(int dash_length, const int* dash_pattern, int dash_start)
{
	delete [] array;
	length = dash_length;
	array = dash_pattern;
	if (dash_pattern)
	{
		int i = 0;
		bool on1 = true;
		while (dash_start >= dash_pattern[i])
		{
			dash_start -= dash_pattern[i];
			if (++i == length)
				i = 0;
			on1 = !on1;
		}
		index = i;
		num = dash_pattern[i] - dash_start;
		on = on1;
	}
}


ABitmap::ABitmap(int w, int h, PixFmt* pfmt, int x, int y)
: width(w), height(h), modulo(w), bpp(pfmt->bytes_per_pixel()), pixfmt(pfmt), xoffset(x), yoffset(y), owner(true)
{
	data = new int8u [w * h * bpp];
}

ABitmap::ABitmap(ABitmap& bm, int x, int y, int w, int h)
: owner(false), pixfmt(bm.pixfmt), bpp(bm.bpp), modulo(bm.modulo)
{
	if (x < bm.xoffset)
	{
		w -= bm.xoffset - x;
		x = bm.xoffset;
	}

	if (y < bm.yoffset)
	{
		h -= bm.yoffset - y;
		y = bm.yoffset;
	}

	xoffset = x;
	yoffset = y;
	x -= bm.xoffset;
	y -= bm.yoffset;

	if (x + w > bm.width)
		w = bm.width - x;

	if (y + h > bm.height)
		h = bm.height - y;

	width = w;
	height = h;

	data = bm.data + (y * modulo + x) * bpp;
}

ABitmap::~ABitmap()
{
	if (owner)
		delete [] data;
}

void ABitmap::init(int8u* dat, int x, int y, int w, int h, int mod, PixFmt* fmt)
{
	if (owner)
		delete [] data;
	owner = false;
	data = dat;
	width = w;
	height = h;
	xoffset = x;
	yoffset = y;
	modulo = mod;
	pixfmt = fmt;
	bpp = pixfmt->bytes_per_pixel();
}

void ABitmap::fill(int x1, int y1, int x2, int y2, Color color)
{
	int w = x2 - x1 + 1;
	int h = y2 - y1 + 1;
	int x = x1 - xoffset;
	int y = y1 - yoffset;

	if (x < width && y < height && x + w > 0 && y + h > 0)
	{
		if (x < 0)
		{
			w += x;
			x = 0;
		}

		if (y < 0)
		{
			h += y;
			y = 0;
		}

		if (x + w > width)
			w = width - x;

		if (y + h > height)
			h = height - y;

		pixfmt->rectfill(data + (y * modulo + x) * bpp, modulo - w, w, h, pixfmt->color_to_int(color));
	}
}

void ABitmap::fill(int x, int y, ARegion* region, Color c)
{
	DEBUG_FILL(printf("fill: x %d y %d region %p [%p] (%d,%d) %d×%d bitmap (%d,%d) %d×%d\n",
				x, y, region, region->rows,
				region->x0, region->y0, region->width, region->height,
				xoffset, yoffset, width, height));
	x -=/* region->x0 +*/ xoffset;
	y -= region->y0 + yoffset;

	if (y < height && y + region->height > 0 &&
			x - region->x0 < width && x - region->x0 + region->width > 0)
	{
		ARegion::Row** p = region->rows;
		int h = region->height;

		if (y < 0)
		{
			h += y;
			p -= y;
			y = 0;
		}
		if (y + h > height)
			h = height - y;

		pixfmt->regionfill(data + y * (modulo * bpp), modulo * bpp, x, width, h, p, c);
	}
}

void ABitmap::blit(int x1, int y1, ABitmap* bm, int x2, int y2, int w, int h)
{
	DEBUG_BLIT(printf("blit: x1 %d y1 %d x2 %d y2 %d w %d h %d\n", x1, y1, x2, y2, w, h));
	DEBUG_BLIT(printf("blit: dst: xo %d yo %d w %d h %d\n",
				xoffset, yoffset, width, height));
	DEBUG_BLIT(printf("blit: src: xo %d yo %d w %d h %d\n",
				bm->xoffset, bm->yoffset, bm->width, bm->height));

	x2 -= bm->xoffset;
	y2 -= bm->yoffset;

	if (x2 < bm->width && y2 < bm->height && x2 + w > 0 && y2 + h > 0)
	{
		if (x2 < 0)
		{
			x1 -= x2;
			w += x2;
			x2 = 0;
		}
		if (y2 < 0)
		{
			y1 -= y2;
			h += y2;
			y2 = 0;
		}
		if (x2 + w > bm->width)
			w = bm->width - x2;
		if (y2 + h > bm->height)
			h = bm->height - y2;

		x1 -= xoffset;
		y1 -= yoffset;

		if (x1 < width && y1 < height && x1 + w > 0 && y1 + h > 0)
		{
			if (x1 < 0)
			{
				w += x1;
				x2 -= x1;
				x1 = 0;
			}
			if (x1 + w > width)
				w = width - x1;

			if (y1 < 0)
			{
				h += y1;
				y2 -= y1;
				y1 = 0;
			}
			if (y1 + h > height)
				h = height - y1;

			DEBUG_BLIT(printf("blit: clipped x1 %d y1 %d x2 %d y2 %d w %d h %d\n",
						x1, y1, x2, y2, w, h));

			pixfmt->blit(data + (y1 * modulo + x1) * bpp, modulo - w,
					bm->data + (y2 * bm->modulo + x2) * bm->bpp, bm->modulo - w, w, h);
		}
	}
}


void ABitmap::blit(int x1, int y1, ABitmap* bm, int x2, int y2, ARegion* region)
{
	DEBUG_BLITMASK(printf("blitmsk: x1 %d y1 %d x2 %d y2 %d mask x %d y %d w %d h %d\n",
				x1, y1, x2, y2,
				region->x0, region->y0, region->width, region->height));
	DEBUG_BLITMASK(printf("blitmsk: dst: xo %d yo %d w %d h %d\n",
				xoffset, yoffset, width, height));
	DEBUG_BLITMASK(printf("blitmsk: src: xo %d yo %d w %d h %d\n",
				bm->xoffset, bm->yoffset, bm->width, bm->height));

	x1 -= region->x0 + xoffset;
	y1 -= region->y0 + yoffset;
	x2 -= region->x0 + bm->xoffset;
	y2 -= region->y0 + bm->yoffset;

	int w = region->width;
	int h = region->height;

	ARegion::Row** rows = region->rows;
	int dx = region->x0;

	if (x1 < 0)
	{
		dx += x1;
		w += x1;
		x2 -= x1;
		x1 = 0;
	}

	if (x2 < 0)
	{
		dx += x2;
		w += x2;
		x1 -= x2;
		x2 = 0;
	}

	if (x1 + w > width)
		w = width - x1;

	if (x2 + w > bm->width)
		w = bm->width - x2;

	if (w < 0)
		return;

	if (y1 < 0)
	{
		rows -= y1;
		h += y1;
		y2 -= y1;
		y1 = 0;
	}

	if (y2 < 0)
	{
		rows -= y2;
		h += y2;
		y1 -= y2;
		y2 = 0;
	}

	if (y1 + h > height)
		h = height - y1;

	if (y2 + h > bm->height)
		h = bm->height - y2;

	if (h < 0)
		return;

	DEBUG_BLITMASK(printf("blitmsk: clipped x1 %d y1 %d x2 %d y2 %d w %d h %d dx %d\n",
				x1, y1, x2, y2, w, h, dx));

	pixfmt->blit(data + (y1 * modulo + x1) * bpp, modulo * bpp,
			bm->data + (y2 * bm->modulo + x2) * bpp, bm->modulo * bpp,
			w, h, dx, rows);
}

void ABitmap::int_line(int x1, int y1, int x2, int y2, int32u c, DashState& ds)
{
#if LINEDB
	int u1 = x1;
	int v1 = y1;
	int u2 = x2;
	int v2 = y2;
	static bool debug = false;
#else
	const bool debug = false;
#endif
	if (y1 == y2)
	{
		if (y1 < 0 || y1 >= height)
			return;
		if (x1 > x2)
		{
			int t = x2; x2 = x1; x1 = t;
		}
		if (x2 < 0 || x1 >= width)
			return;
		if (x1 < 0)
			x1 = 0;
		if (x2 >= width)
			x2 = width - 1;
		pixfmt->hline_dash_func(data + y1 * modulo * bpp, x1, x2 - x1 + 1, c, ds);
	}
	else if (x1 == x2)
	{
		if (x1 < 0 || x1 >= width)
			return;
		if (y1 > y2)
		{
			int t = y2; y2 = y1; y1 = t;
		}
		if (y2 < 0 || y1 >= height)
			return;
		if (y1 < 0)
			y1 = 0;
		if (y2 >= height)
			y2 = height - 1;
		pixfmt->vline_dash_func(data, x1, y1, y2 - y1 + 1, modulo, c, ds);
	}
	else
	{
		if (x1 > x2)
		{
			int t = x1; x1 = x2; x2 = t;
			int u = y1; y1 = y2; y2 = u;
		}
		if (x2 < 0 || x1 >= width)
			return;
		if (debug)
			printf("x1=%d y1=%d x2=%d y2=%d, w=%d\n", x1, y1, x2, y2, width);
		if (x1 < 0)
		{
			y1 += scale(y1 - y2, x1, x2 - x1);
			x1 = 0;
			if (debug)
				printf("clip x1: x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
		}
		if (x2 >= width)
		{
			y2 += scale(y1 - y2, x2 - width + 1, x2 - x1);
			x2 = width - 1;
			if (debug)
				printf("clip x2: x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
		}
		if (y1 > y2)
		{
			int t = y1; y1 = y2; y2 = t;
			int u = x1; x1 = x2; x2 = u;
		}
		if (y2 < 0 || y1 >= height)
			return;
		if (debug)
			printf("x1=%d y1=%d x2=%d y2=%d, h=%d\n", x1, y1, x2, y2, height);
		if (y1 < 0)
		{
			x1 += scale(x1 - x2, y1, y2 - y1);
			y1 = 0;
			if (debug)
				printf("clip y1: x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
		}
		if (y2 >= height)
		{
			x2 += scale(x1 - x2, y2 - height + 1, y2 - y1);
			y2 = height - 1;
			if (debug)
				printf("clip y2: x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
		}
#if LINEDB
		if (x1 < 0 || x2 < 0 || x1 >= width || x2 > width)
		{
			if (!debug)
			{
				debug = true;
				line(bm, u1, v1, u2, v2, color, ds);
				debug = false;
			}
			return;
		}
#else
		//~ to remove
		if (x1 < 0 || x2 < 0 || x1 >= width || x2 >= width)
		{
			printf("Line clipping error\n");
			return;
		}
#endif
		pixfmt->line_dash_func(data, modulo, x1, y1, x2, y2, c, ds);
	}
}

void ABitmap::line(int x1, int y1, int x2, int y2, Color color, DashState& ds)
{
	int32u c = pixfmt->color_to_int(color);
	x1 -= xoffset;
	y1 -= yoffset;
	x2 -= xoffset;
	y2 -= yoffset;
	int_line(x1, y1, x2, y2, c, ds);
}

void ABitmap::bezier(int x1, int y1, int x2, int y2, int x3, int y3, Color color, DashState& ds)
{
	const int max_levels = 15;

	struct
	{
		int x;
		int y;
	} *s, stack[max_levels];

	s = stack;
	int32u c = pixfmt->color_to_int(color);

	x1 -= xoffset;
	y1 -= yoffset;
	x2 -= xoffset;
	y2 -= yoffset;
	x3 -= xoffset;
	y3 -= yoffset;
	x1 <<= 8;
	y1 <<= 8;
	x2 <<= 8;
	y2 <<= 8;
	x3 <<= 8;
	y3 <<= 8;

	DEBUG_LOOPS(int n = 0);

	for (;;)
	{
		DEBUG_LOOPS(++n);

		DEBUG_BEZIER(printf("bezier: %d,%d %d,%d %d,%d level %d\n",
					x1, y1, x2, y2, x3, y3, (s - stack) / 2));
		int xmin = x1;
		int ymin = y1;
		int xmax = x1;
		int ymax = y1;
		if (x2 < xmin) xmin = x2; else if (x2 > xmax) xmax = x2;
		if (y2 < ymin) ymin = y2; else if (y2 > ymax) ymax = y2;
		if (x3 < xmin) xmin = x3; else if (x3 > xmax) xmax = x3;
		if (y3 < ymin) ymin = y3; else if (y3 > ymax) ymax = y3;

		if (xmax >= 0 && ymax >= 0 && xmin < width << 8 && ymin < height << 8)
		{
			DEBUG_LOOPS(if (n > 200) printf("too many iterations: %d,%d %d,%d %d,%d\n", x1,y1,x2,y2,x3,y3); else)
			if (s < stack + max_levels - 2)
			{
				int dx = x1 + x3 - 2 * x2;
				if (dx < 0) dx = -dx;
				int dy = y1 + y3 - 2 * y2;
				if (dy < 0) dy = -dy;
				if (dx < dy) dx = dy;

				if (dx > 64)
				{
					DEBUG_BEZIER(printf("bezier: split\n"));
					s->x = x1;
					s->y = y1;
					++s;
					s->x = (x1 + x2) >> 1;
					s->y = (y1 + y2) >> 1;
					x2 = (x2 + x3) >> 1;
					y2 = (y2 + y3) >> 1;
					x1 = (s->x + x2) >> 1;
					y1 = (s->y + y2) >> 1;
					++s;
					continue;
				}
			}

			int midx = (x1 + 2 * x2 + x3 + 512) >> 2 + 8;
			int midy = (y1 + 2 * y2 + y3 + 512) >> 2 + 8;

			int_line(x1 + 128 >> 8, y1 + 128 >> 8, midx, midy, c, ds);
			int_line(midx, midy, x3 + 128 >> 8, y3 + 128 >> 8, c, ds);
		}

		if (s == stack)
			break;

		x3 = x1;
		y3 = y1;
		--s;
		x2 = s->x;
		y2 = s->y;
		--s;
		x1 = s->x;
		y1 = s->y;
	}
	DEBUG_BEZIER(printf("bezier: done\n"));
}

void ABitmap::bezier(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color color, DashState& ds)
{
	const int max_levels = 22;

	struct
	{
		int x;
		int y;
	} *s, stack[max_levels];

	s = stack;
	int32u c = pixfmt->color_to_int(color);

	x1 -= xoffset;
	y1 -= yoffset;
	x2 -= xoffset;
	y2 -= yoffset;
	x3 -= xoffset;
	y3 -= yoffset;
	x4 -= xoffset;
	y4 -= yoffset;
	x1 <<= 8;
	y1 <<= 8;
	x2 <<= 8;
	y2 <<= 8;
	x3 <<= 8;
	y3 <<= 8;
	x4 <<= 8;
	y4 <<= 8;

	DEBUG_LOOPS(int n = 0);

	for (;;)
	{
		int xmin = x1;
		int ymin = y1;
		int xmax = x1;
		int ymax = y1;
		if (x2 < xmin) xmin = x2; else if (x2 > xmax) xmax = x2;
		if (y2 < ymin) ymin = y2; else if (y2 > ymax) ymax = y2;
		if (x3 < xmin) xmin = x3; else if (x3 > xmax) xmax = x3;
		if (y3 < ymin) ymin = y3; else if (y3 > ymax) ymax = y3;
		if (x4 < xmin) xmin = x4; else if (x4 > xmax) xmax = x4;
		if (y4 < ymin) ymin = y4; else if (y4 > ymax) ymax = y4;

		DEBUG_LOOPS(if (++n > 400) printf("too many iterations: %d,%d %d,%d %d,%d %d,%d\n", x1,y1,x2,y2,x3,y3,x4,y4); else)
		if (xmax >= 0 && ymax >= 0 && xmin < width << 8 && ymin < height << 8)
		{
			if (s < stack + max_levels - 3)
			{
				int dx = x1 + x4 - x2 - x3;
				if (dx < 0) dx = -dx;
				int dy = y1 + y4 - y2 - y3;
				if (dy < 0) dy = -dy;
				if (dx < dy) dx = dy;

				if (dx > 64)
				{
					int xa = (x2 + x3) >> 1;
					int ya = (y2 + y3) >> 1;
					s[0].x = x1;
					s[0].y = y1;
					s[1].x = (x1 + x2) >> 1;
					s[1].y = (y1 + y2) >> 1;
					x3 = (x3 + x4) >> 1;
					y3 = (y3 + y4) >> 1;
					s[2].x = (s[1].x + xa) >> 1;
					s[2].y = (s[1].y + ya) >> 1;
					x2 = (xa + x3) >> 1;
					y2 = (ya + y3) >> 1;
					x1 = (s[2].x + x2) >> 1;
					y1 = (s[2].y + y2) >> 1;
					s += 3;
					continue;
				}
			}

			int midx = (x1 + 3 * (x2 + x3) + x4 + 1024) >> 3 + 8;
			int midy = (y1 + 3 * (y2 + y3) + y4 + 1024) >> 3 + 8;

			int_line(x1 + 128 >> 8, y1 + 128 >> 8, midx, midy, c, ds);
			int_line(midx, midy, x4 + 128 >> 8, y4 + 128 >> 8, c, ds);
		}

		if (s == stack)
			break;

		x4 = x1;
		y4 = y1;
		--s;
		x3 = s->x;
		y3 = s->y;
		--s;
		x2 = s->x;
		y2 = s->y;
		--s;
		x1 = s->x;
		y1 = s->y;
	}
}

	class StrokeScanner : public APath::Scanner
	{
	public:
		StrokeScanner(ABitmap& bm, int x1, int y1, Color c)
		: bitmap(bm), x(x1), y(y1), xi(x1), yi(y1), color(c), open_flag(false) {}

		void open(APath::Part*) { open_flag = true; }

		void open()
		{
			if (open_flag)
			{
				x0 = x;
				y0 = y;
				x0i = xi;
				y0i = yi;
				open_flag = false;
			}
		}
		void move(double x1, double y1)
		{
//printf("move %f %f\n", x1, y1);
			x += x1;
			y += y1;
			xi = round(x);
			yi = round(y);
			open();
		}
		void line(double x1, double y1)
		{
//printf("line %f %f\n", x1, y1);
			x += x1;
			y += y1;
			int X = round(x);
			int Y = round(y);
			bitmap.line(xi, yi, X, Y, color, ds);
			xi = X;
			yi = Y;
			open();
		}
		void bezier(double x1, double y1, double x2, double y2)
		{
//printf("bezier %f %f\n", x2, y2);
			int X1 = round(x + x1);
			int Y1 = round(y + y1);
			x += x2;
			y += y2;
			int X2 = round(x);
			int Y2 = round(y);
			bitmap.bezier(xi, yi, X1, Y1, X2, Y2, color, ds);
			xi = X2;
			yi = Y2;
			open();
		}
		void bezier(double x1, double y1, double x2, double y2, double x3, double y3)
		{
//printf("bezier2 %f %f\n", x3, y3);
			int X1 = round(x + x1);
			int Y1 = round(y + y1);
			int X2 = round(x + x2);
			int Y2 = round(y + y2);
			x += x3;
			y += y3;
			int X3 = round(x);
			int Y3 = round(y);
			bitmap.bezier(xi, yi, X1, Y1, X2, Y2, X3, Y3, color, ds);
			xi = X3;
			yi = Y3;
			open();
		}
		void close()
		{
//printf("close\n");
			if (xi != x0i || yi != y0i)
			{
				bitmap.line(xi, yi, x0i, y0i, color, ds);
				xi = x0i;
				yi = y0i;
			}
			x = x0;
			y = y0;
		}

		double x;
		double y;
		double x0;
		double y0;
		int xi;
		int yi;
		int x0i;
		int y0i;
		ABitmap& bitmap;
		DashState ds;
		Color color;
		bool open_flag;
	};

void ABitmap::stroke(int x, int y, APath* path, Color c)
{
	StrokeScanner scanner(*this, x, y, c);
	scanner.scan(path);
}


ARegion::ARegion()
: x0(0), y0(0), width(0), height(0), rows(NULL)
{
}

ARegion::~ARegion()
{
	clear();
}

void ARegion::clear()
{
	int n = height;
	for (int k = 0; k < n; ++k)
		free(rows[k]);
	free(rows);
	rows = NULL;
	x0 = 0;
	y0 = 0;
	width = 0;
	height = 0;
}

ARegion::Row** ARegion::alloc_rows(int num)
{
	DEBUG_SPANS(dprintf("ARegion::alloc_rows(%d)\n", num));

	rows = static_cast<Row**>(malloc(num * sizeof(*rows)));
	if (!rows)
		throw "No memory.";
	Row** p = rows;
	height = num;
	while (--num >= 0)
		*p++ = NULL;

	return rows;
}

ARegion::Span* ARegion::add_spans(int y, int num_spans)
{
	DEBUG_SPANS(dprintf("ARegion::add_spans(%d,%d) y0 %d\n", y, num_spans, y0));
	Row** p = &rows[y + y0];
	Row* q = *p;
	int old_spans = q ? q->count : 0;
	q = static_cast<Row*>(realloc(q, sizeof(Row) + (old_spans + num_spans) * sizeof(Span)));
	if (!q)
		throw "No memory.";
	q->count = old_spans + num_spans;
	*p = q;
	return q->spans + old_spans;
}


AGfxState::AGfxState(ABitmap* bm)
: bitmap(bm), clip(NULL), save_list(NULL)
{
}

AGfxState::~AGfxState()
{
	reset();
}

void AGfxState::reset()
{
	while (save_list)
		pop();
	while (clip)
		pop_clip();
}

AGfxState::Save *AGfxState::push()
{
	Save *s = new Save;
	s->prev = save_list;
	s->clip = clip;
	save_list = s;
	DEBUG_GFXSTATE_SAVE(dprintf("AGfxState::push: save %p clip %p old %p\n", s, clip, s->prev));
	return s->prev;
}

void AGfxState::pop()
{
	Save *s = save_list;
	DEBUG_GFXSTATE_SAVE(dprintf("AGfxState::pop: top %p\n", s));
	if (s)
	{
		while (clip != s->clip)
			pop_clip();
		save_list = s->prev;
		delete s;
		DEBUG_GFXSTATE_SAVE(dprintf("AGfxState::pop: new top %p\n", save_list));
	}
}

void AGfxState::pop(Save *s)
{
	DEBUG_GFXSTATE_SAVE(dprintf("AGfxState::pop: save %p\n", s));
	while (save_list && save_list != s)
	{
		pop();
	}
}

void AGfxState::pop_clip()
{
	ClipState *s = clip;
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::pop_clip: %p\n", s));
	if (s)
	{
		if (s->mask)
		{
			DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::pop_clip: blit %p through mask %p at %d,%d\n",
						bitmap, s->mask, s->mask_x, s->mask_y));
			if (s->bitmap && bitmap)
				s->bitmap->blit(s->mask_x, s->mask_y,
						bitmap, s->mask_x, s->mask_y, s->mask);
			delete s->mask;
		}

		delete bitmap;
		bitmap = s->bitmap;
		clip = s->prev;
		delete s;
		DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::pop_clip: new top %p\n", clip));
	}
}

void AGfxState::add_clip(int x1, int y1, int x2, int y2)
{
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: (%d,%d)-(%d,%d)\n", x1, y1, x2, y2));
	std::auto_ptr<ClipState> s(new ClipState);
	s->prev = clip;
	s->mask = NULL;
	s->bitmap = bitmap;
	if (bitmap && bitmap->width > 0 && bitmap->height > 0 && x2 > x1 && y2 > y1)
		bitmap = new ABitmap(*bitmap, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
	else
		bitmap = NULL;
	clip = s.release();
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: clip %p bitmap %p\n", clip, bitmap));
}

void AGfxState::add_clip(int x, int y, ARegion *mask)
{
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: mask %p at %d,%d\n", mask, x, y));
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: mask %d×%d, offset %d,%d\n",
		mask->width, mask->height, mask->x0, mask->y0));
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: bitmap %p %d×%d, offset %d,%d\n",
		bitmap, bitmap->width, bitmap->height, bitmap->xoffset, bitmap->yoffset));
	std::auto_ptr<ClipState> s(new ClipState);
	s->prev = clip;
	s->mask = mask;
	s->mask_x = x;
	s->mask_y = y;
	s->bitmap = bitmap;

	if (bitmap)
	{
		x -= mask->x0 + bitmap->xoffset;
		y -= mask->y0 + bitmap->yoffset;
	}

	int w = mask->width;
	int h = mask->height;
	if (!bitmap || bitmap->width <= 0 || bitmap->height <= 0 ||
			x >= bitmap->width || y >= bitmap->height || x + w <= 0 || y + h <= 0)
	{
		delete mask;
		s->mask = NULL;
		bitmap = NULL;
	}
	else
	{
		if (x < 0)
		{
			w += x;
			x = 0;
		}
		if (y < 0)
		{
			h += y;
			y = 0;
		}
		if (x + w > bitmap->width)
			w = bitmap->width - x;
		if (y + h > bitmap->height)
			h = bitmap->height - y;

		x += bitmap->xoffset;
		y += bitmap->yoffset;

		DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: create %d×%d bitmap, offset %d,%d\n",
			  w, h, x, y));
		bitmap = new ABitmap(w, h, bitmap->pixfmt, x, y);

		DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip; blit\n"));
		bitmap->blit(x, y, s->bitmap, x, y, w, h);
	}
	clip = s.release();
	DEBUG_GFXSTATE_CLIP(dprintf("AGfxState::add_clip: clip %p bitmap %p\n", clip, bitmap));
}

void AGfxState::stroke(int x, int y, APath* path, Color c, double lw, CapStyle cap, JoinStyle join, double ml, DashState& ds)
{
	if (!bitmap)
	{
	}
	else if (lw)
	{
#ifdef __AROS__
		// FIXME AROS
		APath * path1;
#else
		std::auto_ptr<APath> path1;
#endif
		if (ds.is_solid())
		{
			path1 = path->thicken(lw, cap, join, ml);
		}
		else
		{
			path1 = path->make_dashes(ds);
			path1 = path1->thicken(lw, cap, join, ml);
		}
		std::auto_ptr<ARegion> region(path1->make_region());
		if (region.get())
			bitmap->fill(x, y, region.get(), c);
	}
	else
	{
		bitmap->stroke(x, y, path, c);
	}
}


#if 0
ColorTable::~ColorTable()
{
	delete [] entries;
	delete color_allocator;
}

int8u ColorTable::find(Color x)
{
	unsigned n = num;
	entry *p = entries;
	while (n != 0)
	{
		unsigned k = n / 2;
		entry *q = p + k;
		if (x > q->value)
		{
			p = q + 1;
			n -= k + 1;
		}
		else if(x != q->value)
			n = k;
		else
			return q->pen;
	}
	if (num == max_entries)
	{
		unsigned k = p - entries;
		unsigned new_max_entries = (max_entries * 3) / 2 + 16;
		entry *q = new entry[new_max_entries];
		memcpy(q, entries, max_entries * sizeof(*q));
		delete [] entries;
		entries = q;
		p = q + k;
		max_entries = new_max_entries;
	}
	if (p != entries + num)
		memmove(p + 1, p, (entries + num - p) * sizeof(entry));
	++num;
	int c = color_allocator->allocate(x);
	p->pen = int8u(c);
	p->value = x;
	pens[c] = x;
	return p->pen;
}

void ColorTable::allocate(int n, ColorEntry *p) {
	color_allocator->allocate(n, p);
	for (; --n >= 0; ++p)
		pens[p->index] =
			((p->color.r & 0xff000000UL) >> 8) |
			((p->color.g & 0xff000000UL) >> 16) |
			((p->color.b & 0xff000000UL) >> 24);
}
#endif

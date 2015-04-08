#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include "gfx.h"


template<class T> void rectfill(int8u* q, int add, int w, int h, int32u color)
{
	T* p = reinterpret_cast<T*>(q);
	T c = T(color);

	while (--h >= 0)
	{
		for (int k = 0; k < w; ++k)
			*p++ = c;

		p += add;
	}
}

//template<> void rectfill<int16u>(int8u* q, int add, int w, int h, int32u color);

template<class T> void hline(int8u* q, int x, int w, int32u color)
{
	T* p = reinterpret_cast<T*>(q) + x;
	T c = T(color);
	for (int k = 0; k < w; ++k)
		*p++ = c;
}

template<class T> void hline_dash(int8u* q, int x, int w, int32u color, DashState& ds)
{
	T* p = reinterpret_cast<T*>(q) + x;
	T c = T(color);
	if (ds.is_solid())
	{
		while (--w >= 0)
			*p++ = c;
	}
	else if (w > 0)
	{
		while (--w >= 0)
		{
			if (ds.is_on())
			{
				*p = color;
			}
			++p;
			ds.advance(1 << DashState::fract_bits);
		}
	}
}

template<class T> void vline_dash(int8u* q, int x, int y, int h, int modulo, int32u color, DashState& ds)
{
	T* p = reinterpret_cast<T*>(q) + x + y * modulo;
	T c = T(color);
	if (ds.is_solid())
	{
		while (--h >= 0)
		{
			*p = c;
			p += modulo;
		}
	}
	else if (h > 0)
	{
		while (--h >= 0)
		{
			if (ds.is_on())
			{
				*p = color;
			}
			p += modulo;
			ds.advance(1 << DashState::fract_bits);
		}
	}
}

template<class T>
void line_dash(int8u* q, int modulo, int x1, int y1, int x2, int y2, int32u color, DashState& ds)
{
	T* p = reinterpret_cast<T*>(q);
	T c = T(color);
	int dx = x2 - x1;
	int adx = dx < 0 ? -dx : dx;
	int dy = y2 - y1;
	int ady = dy < 0 ? -dy : dy;

	if (adx > ady)
	{
		p += x1;
		int sx = dx < 0 ? -1 : 1;
		int sy = (ady << 16) / adx;
		int ry = ((ady << 16) % adx) >> 1;
		if (dy < 0)
		{
			sy = -sy;
			ry = -ry;
		}
		y1 <<= 16;
		y1 += ry + (1 << 15);
		do
		{
			p[(y1 >> 16) * modulo] = c;
			y1 += sy;
			p += sx;
		}
		while (--adx >= 0);
	}
	else
	{
		p += y1 * modulo;
		if (dy < 0)
			modulo = -modulo;
		int sx = (adx << 16) / ady;
		int rx = ((adx << 16) % ady) >> 1;
		if (dx < 0)
		{
			sx = -sx;
			rx = -rx;
		}
		x1 <<= 16;
		x1 += rx + (1 << 15);
		do
		{
			p[x1 >> 16] = c;
			x1 += sx;
			p += modulo;
		}
		while (--ady >= 0);
	}
}


template<class T> void blit(int8u* dst, int add1, const int8u* src, int add2, int w, int h)
{
	T* p = reinterpret_cast<T*>(dst);
	const T* q = reinterpret_cast<const T*>(src);

	while (--h >= 0)
	{
		for (int k = 0; k < w; ++k)
			*p++ = *q++;

		p += add1;
		q += add2;
	}
}

//template<> void blit<int16u>(int8u* dst, int add1, const int8u* src, int add2, int w, int h);


template<class T> void hline_blit(int8u* dst, const int8u* src, int x, int w)
{
	T* p = reinterpret_cast<T*>(dst) + x;
	const T* q = reinterpret_cast<const T*>(src) + x;
	for (int k = 0; k < w; ++k)
		*p++ = *q++;
}

#if 0
template void rectfill<int8u>(int8u* q, int add, int w, int h, int32u color);
template void hline<int8u>(int8u* q, int x, int w, int32u color);
template void hline_dash<int8u>(int8u* q, int x, int w, int32u color, DashState& ds);
template void vline_dash<int8u>(int8u* q, int x, int y, int h, int modulo, int32u color, DashState& ds);
template void line_dash<int8u>(int8u* q, int modulo, int x1, int y1, int x2, int y2, int32u color, DashState& ds);
template void blit<int8u>(int8u* dst, int add1, const int8u* src, int add2, int w, int h);
template void hline_blit<int8u>(int8u* dst, int x1, const int8u* src, int x2, int w);

template void rectfill<int16u>(int8u* q, int add, int w, int h, int32u color);
template void hline<int16u>(int8u* q, int x, int w, int32u color);
template void hline_dash<int16u>(int8u* q, int x, int w, int32u color, DashState& ds);
template void vline_dash<int16u>(int8u* q, int x, int y, int h, int modulo, int32u color, DashState& ds);
template void line_dash<int16u>(int8u* q, int modulo, int x1, int y1, int x2, int y2, int32u color, DashState& ds);
template void blit<int16u>(int8u* dst, int add1, const int8u* src, int add2, int w, int h);
template void hline_blit<int16u>(int8u* dst, int x1, const int8u* src, int x2, int w);

template void rectfill<Color24>(int8u* q, int add, int w, int h, int32u color);
template void hline<Color24>(int8u* q, int x, int w, int32u color);
template void hline_dash<Color24>(int8u* q, int x, int w, int32u color, DashState& ds);
template void vline_dash<Color24>(int8u* q, int x, int y, int h, int modulo, int32u color, DashState& ds);
template void line_dash<Color24>(int8u* q, int modulo, int x1, int y1, int x2, int y2, int32u color, DashState& ds);
template void blit<Color24>(int8u* dst, int add1, const int8u* src, int add2, int w, int h);
template void hline_blit<Color24>(int8u* dst, int x1, const int8u* src, int x2, int w);

template void rectfill<int32u>(int8u* q, int add, int w, int h, int32u color);
template void hline<int32u>(int8u* q, int x, int w, int32u color);
template void hline_dash<int32u>(int8u* q, int x, int w, int32u color, DashState& ds);
template void vline_dash<int32u>(int8u* q, int x, int y, int h, int modulo, int32u color, DashState& ds);
template void line_dash<int32u>(int8u* q, int modulo, int x1, int y1, int x2, int y2, int32u color, DashState& ds);
template void blit<int32u>(int8u* dst, int add1, const int8u* src, int add2, int w, int h);
template void hline_blit<int32u>(int8u* dst, int x1, const int8u* src, int x2, int w);
#endif

class TrueColorPixFmt : public PixFmt
{
public:
	TrueColorPixFmt(FmtId id1) : PixFmt(id1) {}

	virtual void regionfill(int8u* q, int m, int x, int w, int h, ARegion::Row** p, Color c);
	virtual void blit(int8u* q, int m, const int8u* p, int m1, int w, int h, int dx, ARegion::Row** rows);

	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c) =0;
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha) =0;
};

class PixFmt32 : public TrueColorPixFmt
{
public:
	PixFmt32(FmtId id1) : TrueColorPixFmt(id1)
	{
		bpp = 4;
		rectfill_func = ::rectfill<int32u>;
		blit_func = ::blit<int32u>;
		hline_func = ::hline<int32u>;
		hline_blit_func = ::hline_blit<int32u>;
		hline_dash_func = ::hline_dash<int32u>;
		vline_dash_func = ::vline_dash<int32u>;
		line_dash_func = ::line_dash<int32u>;
	}

	virtual Color get_pixel(const int8u* p);
	virtual void put_pixel(int8u* p, Color);
};

Color PixFmt32::get_pixel(const int8u* p)
{
	return int_to_color(*(int32u*)p);
}

void PixFmt32::put_pixel(int8u* p, Color c)
{
	*(int32u*)p = color_to_int(c);
}

class PixFmt24 : public TrueColorPixFmt
{
public:
	PixFmt24(FmtId id1) : TrueColorPixFmt(id1)
	{
		bpp = 3;
		rectfill_func = ::rectfill<Color24>;
		blit_func = ::blit<Color24>;
		hline_func = ::hline<Color24>;
		hline_blit_func = ::hline_blit<Color24>;
		hline_dash_func = ::hline_dash<Color24>;
		vline_dash_func = ::vline_dash<Color24>;
		line_dash_func = ::line_dash<Color24>;
	}

	virtual Color get_pixel(const int8u* p);
	virtual void put_pixel(int8u* p, Color);
};

Color PixFmt24::get_pixel(const int8u* p)
{
	return int_to_color(*(Color24*)p);
}

void PixFmt24::put_pixel(int8u* p, Color c)
{
	*(Color24*)p = color_to_int(c);
}


class PixFmt16 : public TrueColorPixFmt
{
public:
	PixFmt16(FmtId id1) : TrueColorPixFmt(id1)
	{
		bpp = 2;
		rectfill_func = ::rectfill<int16u>;
		blit_func = ::blit<int16u>;
		hline_func = ::hline<int16u>;
		hline_blit_func = ::hline_blit<int16u>;
		hline_dash_func = ::hline_dash<int16u>;
		vline_dash_func = ::vline_dash<int16u>;
		line_dash_func = ::line_dash<int16u>;
	}

	virtual Color get_pixel(const int8u* p);
	virtual void put_pixel(int8u* p, Color);
};

Color PixFmt16::get_pixel(const int8u* p)
{
	return int_to_color(*(int16u*)p);
}

void PixFmt16::put_pixel(int8u* p, Color c)
{
	*(int16u*)p = color_to_int(c);
}


class PixFmt8 : public TrueColorPixFmt
{
public:
	PixFmt8(FmtId id1) : TrueColorPixFmt(id1)
	{
		bpp = 1;
		rectfill_func = ::rectfill<int8u>;
		blit_func = ::blit<int8u>;
		hline_func = ::hline<int8u>;
		hline_blit_func = ::hline_blit<int8u>;
		hline_dash_func = ::hline_dash<int8u>;
		vline_dash_func = ::vline_dash<int8u>;
		line_dash_func = ::line_dash<int8u>;
	}

	virtual Color get_pixel(const int8u* p);
	virtual void put_pixel(int8u* p, Color);
};

Color PixFmt8::get_pixel(const int8u* p)
{
	return int_to_color(*p);
}

void PixFmt8::put_pixel(int8u* p, Color c)
{
	*p = color_to_int(c);
}


void TrueColorPixFmt::regionfill(int8u* q, int m, int x, int w, int h, ARegion::Row** p, Color c)
{
	int32u c1 = color_to_int(c);

	while (--h >= 0)
	{
		ARegion::Row* row = *p++;
		if (row)
		{
			int n = row->count;
			ARegion::Span *span = row->spans;

			while (--n >= 0)
			{
				int alpha = span->alpha;
				if (alpha)
				{
					int x1 = span->x + x;
					int w1 = span->length;
					if (x1 < w && x1 + w1 > 0)
					{
						if (x1 < 0)
						{
							w1 += x1;
							x1 = 0;
						}
						if (x1 + w1 > w)
							w1 = w - x1;
						if (alpha == 255)
							hline(q, x1, w1, c1);
						else
							hline_alpha(q, x1, w1, alpha, c);
					}
				}
				++span;
			}

		}
		q += m;
	}
}

void TrueColorPixFmt::blit(int8u* q, int m, const int8u* p, int m1, int w, int h, int dx, ARegion::Row** rows)
{
	while (--h >= 0)
	{
		ARegion::Row* row = *rows++;
		if (row)
		{
			int n = row->count;
			ARegion::Span *span = row->spans;

			while (--n >= 0)
			{
				int alpha = span->alpha;
				if (alpha)
				{
					int x = span->x + dx;
					int w1 = span->length;
					if (x + w1 > 0 && x < w)
					{
						if (x < 0)
						{
							w1 += x;
							x = 0;
						}
						if (x + w1 > w)
							w1 = w - x;
						if (alpha == 255)
							hline_blit(q, p, x, w1);
						else
							hline_alpha_blit(q, p, x, w1, alpha);
					}
				}
				++span;
			}

		}
		q += m;
		p += m1;
	}
}

class PixFmtARGB32 : public PixFmt32
{
public:
	PixFmtARGB32() : PixFmt32(argb32) {}

	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtARGB32::color_to_int(Color c)
{
	if (big_endian)
		return c;
	else
		return  ((c >>  8) & 0x0000ff00) |
			((c <<  8) & 0x00ff0000) |
			((c << 24) & 0xff000000);
}

Color PixFmtARGB32::int_to_color(int32u c)
{
	if (big_endian)
		return c;
	else
		return  ((c >> 24) & 0x000000ff) |
			((c >>  8) & 0x0000ff00) |
			((c <<  8) & 0x00ff0000);
}

int32u PixFmtARGB32::rgb_to_int(int8u r, int8u g, int8u b)
{
	if (big_endian)
		return (r << 16) | (g << 8) | b;
	else
		return (r << 8) | (g << 16) | (b << 24);
}

void PixFmtARGB32::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	p += x * 4;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		p[1] = (r * u + p[1] * v) >> 8;
		p[2] = (g * u + p[2] * v) >> 8;
		p[3] = (b * u + p[3] * v) >> 8;
		p += 4;
	}
}

void PixFmtARGB32::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x * 4;
	q += x * 4;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		p[1] = (q[1] * u + p[1] * v) >> 8;
		p[2] = (q[2] * u + p[2] * v) >> 8;
		p[3] = (q[3] * u + p[3] * v) >> 8;
		p += 4;
		q += 4;
	}
}

PixFmtARGB32 pixfmt_argb32;


class PixFmtRGBA32 : public PixFmt32
{
public:
	PixFmtRGBA32() : PixFmt32(rgba32) {}

	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtRGBA32::color_to_int(Color c)
{
	if (big_endian)
		return c << 8;
	else
		return  ((c >> 16) & 0x0000ff) |
			(c & 0x00ff00) |
			((c << 16) & 0xff0000);
}

Color PixFmtRGBA32::int_to_color(int32u c)
{
	if (big_endian)
		return c >> 8;
	else
		return  ((c >> 16) & 0x000000ff) |
			(c & 0x00ff00) |
			((c << 16) & 0x00ff0000);
}

int32u PixFmtRGBA32::rgb_to_int(int8u r, int8u g, int8u b)
{
	if (big_endian)
		return (r << 24) | (g << 16) | (b << 8);
	else
		return r | (g << 8) | (b << 16);
}

void PixFmtRGBA32::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	p += x * 4;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		p[0] = (r * u + p[0] * v) >> 8;
		p[1] = (g * u + p[1] * v) >> 8;
		p[2] = (b * u + p[2] * v) >> 8;
		p += 4;
	}
}

void PixFmtRGBA32::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x * 4;
	q += x * 4;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		p[0] = (q[0] * u + p[0] * v) >> 8;
		p[1] = (q[1] * u + p[1] * v) >> 8;
		p[2] = (q[2] * u + p[2] * v) >> 8;
		p += 4;
		q += 4;
	}
}

PixFmtRGBA32 pixfmt_rgba32;


class PixFmtBGRA32 : public PixFmt32
{
public:
	PixFmtBGRA32() : PixFmt32(bgra32) {}

	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtBGRA32::color_to_int(Color c)
{
	if (big_endian)
		return  ((c >>  8) & 0x0000ff00) |
			((c <<  8) & 0x00ff0000) |
			((c << 24) & 0xff000000);
	else
		return c;
}

Color PixFmtBGRA32::int_to_color(int32u c)
{
	if (big_endian)
		return  ((c >> 24) & 0x000000ff) |
			((c >>  8) & 0x0000ff00) |
			((c <<  8) & 0x00ff0000);
	else
		return c;
}

int32u PixFmtBGRA32::rgb_to_int(int8u r, int8u g, int8u b)
{
	if (big_endian)
		return (b << 24) | (g << 16) | (r << 8);
	else
		return b | (g << 8) | (r << 16);
}

void PixFmtBGRA32::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	p += x * 4;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		p[2] = (r * u + p[2] * v) >> 8;
		p[1] = (g * u + p[1] * v) >> 8;
		p[0] = (b * u + p[0] * v) >> 8;
		p += 4;
	}
}

void PixFmtBGRA32::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x * 4;
	q += x * 4;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		p[2] = (q[2] * u + p[2] * v) >> 8;
		p[1] = (q[1] * u + p[1] * v) >> 8;
		p[0] = (q[0] * u + p[0] * v) >> 8;
		p += 4;
		q += 4;
	}
}

PixFmtBGRA32 pixfmt_bgra32;


class PixFmtRGB24 : public PixFmt24
{
public:
	PixFmtRGB24() : PixFmt24(rgb24) {}

	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtRGB24::color_to_int(Color c)
{
	return c;
}

Color PixFmtRGB24::int_to_color(int32u c)
{
	return c;
}

int32u PixFmtRGB24::rgb_to_int(int8u r, int8u g, int8u b)
{
	return (r << 16) | (g << 8) | b;
}

void PixFmtRGB24::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	p += x * 3;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		p[0] = (r * u + p[0] * v) >> 8;
		p[1] = (g * u + p[1] * v) >> 8;
		p[2] = (b * u + p[2] * v) >> 8;
		p += 3;
	}
}

void PixFmtRGB24::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x * 3;
	q += x * 3;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		p[0] = (q[0] * u + p[0] * v) >> 8;
		p[1] = (q[1] * u + p[1] * v) >> 8;
		p[2] = (q[2] * u + p[2] * v) >> 8;
		p += 3;
		q += 3;
	}
}

PixFmtRGB24 pixfmt_rgb24;


class PixFmtBGR24 : public PixFmt24
{
public:
	PixFmtBGR24() : PixFmt24(bgr24) {}

	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtBGR24::color_to_int(Color c)
{
	return ((c & 0xff0000) >> 16) | (c & 0x00ff00) | ((c & 0x0000ff) << 16);
}

Color PixFmtBGR24::int_to_color(int32u c)
{
	return ((c & 0xff0000) >> 16) | (c & 0x00ff00) | ((c & 0x0000ff) << 16);
}

int32u PixFmtBGR24::rgb_to_int(int8u r, int8u g, int8u b)
{
	return (b << 16) | (g << 8) | r;
}

void PixFmtBGR24::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	p += x * 3;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		p[2] = (r * u + p[2] * v) >> 8;
		p[1] = (g * u + p[1] * v) >> 8;
		p[0] = (b * u + p[0] * v) >> 8;
		p += 3;
	}
}

void PixFmtBGR24::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x * 3;
	q += x * 3;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		p[2] = (q[2] * u + p[2] * v) >> 8;
		p[1] = (q[1] * u + p[1] * v) >> 8;
		p[0] = (q[0] * u + p[0] * v) >> 8;
		p += 3;
		q += 3;
	}
}

PixFmtBGR24 pixfmt_bgr24;


class PixFmtRGB16 : public PixFmt16
{
public:
	PixFmtRGB16() : PixFmt16(rgb16) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtRGB16::color_to_int(Color c)
{
	if (big_endian)
		return ((c >> 8) & 0xf800) | ((c >> 5) & 0x07e0) | ((c >> 3) & 0x001f);
	else
		return ((c >> 16) & 0x00f8) | ((c >> 13) & 0x0007) | ((c << 3) & 0xe000) | ((c << 5) & 0x1f00);
}

Color PixFmtRGB16::int_to_color(int32u c)
{
	if (big_endian)
		return ((c & 0xf800) << 8) | ((c & 0x07e0) << 5) | ((c & 0x001f) << 3);
	else
		return ((c & 0x00f8) << 16) | ((c & 0x0007) << 13) | ((c & 0xe000) >> 3) | ((c & 0x1f00) >> 5);
}

int32u PixFmtRGB16::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 2;
	int b1 = b >> 3;
	if (big_endian)
		return (r1 << 11) | (g1 << 5) | b1;
	else
		return (r1 << 3) | (g1 >> 3) | ((g1 & 7) << 13) | (b1 << 8);
}

void PixFmtRGB16::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (big_endian)
		{
			r1 = (t & 0xf800) >> 8;
			g1 = (t & 0x07e0) >> 3;
			b1 = (t & 0x001f) << 3;
		}
		else
		{
			r1 = (t & 0x00f8);
			g1 = ((t & 0x0007) << 5) | ((t & 0xe000) >> 11);
			b1 = (t & 0x1f00) >> 5;
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (big_endian)
		{
			t = ((r1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t = (r1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((b1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtRGB16::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (big_endian)
		{
			r1 = (t1 & 0xf800) >> 8;
			g1 = (t1 & 0x07e0) >> 3;
			b1 = (t1 & 0x001f) << 3;
			r2 = (t2 & 0xf800) >> 8;
			g2 = (t2 & 0x07e0) >> 3;
			b2 = (t2 & 0x001f) << 3;
		}
		else
		{
			r1 =  (t1 & 0x00f8);
			g1 = ((t1 & 0x0007) << 5) | ((t1 & 0xe000) >> 11);
			b1 =  (t1 & 0x1f00) >> 5;
			r2 =  (t2 & 0x00f8);
			g2 = ((t2 & 0x0007) << 5) | ((t2 & 0xe000) >> 11);
			b2 =  (t2 & 0x1f00) >> 5;
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (big_endian)
		{
			t1 = ((r1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t1 = (r1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((b1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtRGB16 pixfmt_rgb16;

class PixFmtRGB16pc : public PixFmt16
{
public:
	PixFmtRGB16pc() : PixFmt16(rgb16pc) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtRGB16pc::color_to_int(Color c)
{
	if (!big_endian)
		return ((c >> 8) & 0xf800) | ((c >> 5) & 0x07e0) | ((c >> 3) & 0x001f);
	else
		return ((c >> 16) & 0x00f8) | ((c >> 13) & 0x0007) | ((c << 3) & 0xe000) | ((c << 5) & 0x1f00);
}

Color PixFmtRGB16pc::int_to_color(int32u c)
{
	if (!big_endian)
		return ((c & 0xf800) << 8) | ((c & 0x07e0) << 5) | ((c & 0x001f) << 3);
	else
		return ((c & 0x00f8) << 16) | ((c & 0x0007) << 13) | ((c & 0xe000) >> 3) | ((c & 0x1f00) >> 5);
}

int32u PixFmtRGB16pc::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 2;
	int b1 = b >> 3;
	if (big_endian)
		return (r1 << 3) | (g1 >> 3) | ((g1 & 7) << 13) | (b1 << 8);
	else
		return (r1 << 11) | (g1 << 5) | b1;
}

void PixFmtRGB16pc::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (!big_endian)
		{
			r1 = (t & 0xf800) >> 8;
			g1 = (t & 0x07e0) >> 3;
			b1 = (t & 0x001f) << 3;
		}
		else
		{
			r1 = (t & 0x00f8);
			g1 = ((t & 0x0007) << 5) | ((t & 0xe000) >> 11);
			b1 = (t & 0x1f00) >> 5;
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t = ((r1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t = (r1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((b1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtRGB16pc::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (!big_endian)
		{
			r1 = (t1 & 0xf800) >> 8;
			g1 = (t1 & 0x07e0) >> 3;
			b1 = (t1 & 0x001f) << 3;
			r2 = (t2 & 0xf800) >> 8;
			g2 = (t2 & 0x07e0) >> 3;
			b2 = (t2 & 0x001f) << 3;
		}
		else
		{
			r1 =  (t1 & 0x00f8);
			g1 = ((t1 & 0x0007) << 5) | ((t1 & 0xe000) >> 11);
			b1 =  (t1 & 0x1f00) >> 5;
			r2 =  (t2 & 0x00f8);
			g2 = ((t2 & 0x0007) << 5) | ((t2 & 0xe000) >> 11);
			b2 =  (t2 & 0x1f00) >> 5;
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t1 = ((r1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t1 = (r1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((b1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtRGB16pc pixfmt_rgb16pc;


class PixFmtBGR16 : public PixFmt16
{
public:
	PixFmtBGR16() : PixFmt16(bgr16) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtBGR16::color_to_int(Color c)
{
	if (big_endian)
		return ((c << 8) & 0xf800) | ((c >> 5) & 0x07e0) | ((c >> 19) & 0x001f);
	else
		return (c & 0x00f8) | ((c >> 13) & 0x0007) | ((c << 3) & 0xe000) | ((c >> 11) & 0x1f00);
}

Color PixFmtBGR16::int_to_color(int32u c)
{
	if (big_endian)
		return ((c & 0xf800) >> 8) | ((c & 0x07e0) << 5) | ((c & 0x001f) << 19);
	else
		return (c & 0x00f8) | ((c & 0x0007) << 13) | ((c & 0xe000) >> 3) | ((c & 0x1f00) << 11);
}

int32u PixFmtBGR16::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 2;
	int b1 = b >> 3;
	if (big_endian)
		return (b1 << 11) | (g1 << 5) | r1;
	else
		return (b1 << 3) | (g1 >> 3) | ((g1 & 7) << 13) | (r1 << 8);
}

void PixFmtBGR16::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (big_endian)
		{
			r1 = (t & 0x001f) << 3;
			g1 = (t & 0x07e0) >> 3;
			b1 = (t & 0xf800) >> 8;
		}
		else
		{
			r1 = (t & 0x1f00) >> 5;
			g1 = ((t & 0x0007) << 5) | ((t & 0xe000) >> 11);
			b1 = (t & 0x00f8);
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (big_endian)
		{
			t = ((b1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t = (b1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((r1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtBGR16::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (big_endian)
		{
			r1 = (t1 & 0x001f) << 3;
			g1 = (t1 & 0x07e0) >> 3;
			b1 = (t1 & 0xf800) >> 8;
			r2 = (t2 & 0x001f) << 3;
			g2 = (t2 & 0x07e0) >> 3;
			b2 = (t2 & 0xf800) >> 8;
		}
		else
		{
			r1 =  (t1 & 0x1f00) >> 5;
			g1 = ((t1 & 0x0007) << 5) | ((t1 & 0xe000) >> 11);
			b1 =  (t1 & 0x00f8);
			r2 =  (t2 & 0x1f00) >> 5;
			g2 = ((t2 & 0x0007) << 5) | ((t2 & 0xe000) >> 11);
			b2 =  (t2 & 0x00f8);
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (big_endian)
		{
			t1 = ((b1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t1 = (b1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((r1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtBGR16 pixfmt_bgr16;


class PixFmtBGR16pc : public PixFmt16
{
public:
	PixFmtBGR16pc() : PixFmt16(bgr16pc) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtBGR16pc::color_to_int(Color c)
{
	if (!big_endian)
		return ((c << 8) & 0xf800) | ((c >> 5) & 0x07e0) | ((c >> 19) & 0x001f);
	else
		return (c & 0x00f8) | ((c >> 13) & 0x0007) | ((c << 3) & 0xe000) | ((c >> 11) & 0x1f00);
}

Color PixFmtBGR16pc::int_to_color(int32u c)
{
	if (!big_endian)
		return ((c & 0xf800) >> 8) | ((c & 0x07e0) << 5) | ((c & 0x001f) << 19);
	else
		return (c & 0x00f8) | ((c & 0x0007) << 13) | ((c & 0xe000) >> 3) | ((c & 0x1f00) << 11);
}

int32u PixFmtBGR16pc::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 2;
	int b1 = b >> 3;
	if (big_endian)
		return (b1 << 3) | (g1 >> 3) | ((g1 & 7) << 13) | (r1 << 8);
	else
		return (b1 << 11) | (g1 << 5) | r1;
}

void PixFmtBGR16pc::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (!big_endian)
		{
			r1 = (t & 0x001f) << 3;
			g1 = (t & 0x07e0) >> 3;
			b1 = (t & 0xf800) >> 8;
		}
		else
		{
			r1 = (t & 0x1f00) >> 5;
			g1 = ((t & 0x0007) << 5) | ((t & 0xe000) >> 11);
			b1 = (t & 0x00f8);
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t = ((b1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t = (b1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((r1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtBGR16pc::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (!big_endian)
		{
			r1 = (t1 & 0x001f) << 3;
			g1 = (t1 & 0x07e0) >> 3;
			b1 = (t1 & 0xf800) >> 8;
			r2 = (t2 & 0x001f) << 3;
			g2 = (t2 & 0x07e0) >> 3;
			b2 = (t2 & 0xf800) >> 8;
		}
		else
		{
			r1 =  (t1 & 0x1f00) >> 5;
			g1 = ((t1 & 0x0007) << 5) | ((t1 & 0xe000) >> 11);
			b1 =  (t1 & 0x00f8);
			r2 =  (t2 & 0x1f00) >> 5;
			g2 = ((t2 & 0x0007) << 5) | ((t2 & 0xe000) >> 11);
			b2 =  (t2 & 0x00f8);
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t1 = ((b1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t1 = (b1 & 0xf8) | ((g1 & 0xe0) >> 5) | ((g1 & 0x1c) << 11) | ((r1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtBGR16pc pixfmt_bgr16pc;


class PixFmtRGB15 : public PixFmt16
{
public:
	PixFmtRGB15() : PixFmt16(rgb15) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtRGB15::color_to_int(Color c)
{
	if (big_endian)
		return ((c >> 9) & 0x7c00) | ((c >> 6) & 0x03e0) | ((c >> 3) & 0x001f);
	else
		return ((c >> 17) & 0x007c) | ((c >> 14) & 0x0003) | ((c << 2) & 0xe000) | ((c << 5) & 0x1f00);
}

Color PixFmtRGB15::int_to_color(int32u c)
{
	if (big_endian)
		return ((c & 0x7c00) << 9) | ((c & 0x03e0) << 6) | ((c & 0x001f) << 3);
	else
		return ((c & 0x007c) << 17) | ((c & 0x0003) << 14) | ((c & 0xe000) >> 2) | ((c & 0x1f00) >> 5);
}

int32u PixFmtRGB15::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 3;
	int b1 = b >> 3;
	if (big_endian)
		return (r1 << 10) | (g1 << 5) | b1;
	else
		return (r1 << 2) | (g1 >> 3) | ((g1 & 7) << 13) | (b1 << 8);
}

void PixFmtRGB15::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (big_endian)
		{
			r1 = (t & 0x7c00) >> 7;
			g1 = (t & 0x03e0) >> 2;
			b1 = (t & 0x001f) << 3;
		}
		else
		{
			r1 = (t & 0x007c) << 1;
			g1 = ((t & 0x0003) << 6) | ((t & 0xe000) >> 10);
			b1 = (t & 0x1f00) >> 5;
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (big_endian)
		{
			t = ((r1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t = ((r1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x38) << 10) | ((b1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtRGB15::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (big_endian)
		{
			r1 = (t1 & 0x7c00) >> 7;
			g1 = (t1 & 0x03e0) >> 2;
			b1 = (t1 & 0x001f) << 3;
			r2 = (t2 & 0x7c00) >> 7;
			g2 = (t2 & 0x03e0) >> 2;
			b2 = (t2 & 0x001f) << 3;
		}
		else
		{
			r1 = ((t1 & 0x007c) << 1);
			g1 = ((t1 & 0x0003) << 6) | ((t1 & 0xe000) >> 10);
			b1 =  (t1 & 0x1f00) >> 5;
			r2 = ((t2 & 0x007c) << 1);
			g2 = ((t2 & 0x0003) << 6) | ((t2 & 0xe000) >> 10);
			b2 =  (t2 & 0x1f00) >> 5;
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (big_endian)
		{
			t1 = ((r1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t1 = ((r1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x18) << 10) | ((b1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtRGB15 pixfmt_rgb15;


class PixFmtRGB15pc : public PixFmt16
{
public:
	PixFmtRGB15pc() : PixFmt16(rgb15pc) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtRGB15pc::color_to_int(Color c)
{
	if (!big_endian)
		return ((c >> 9) & 0x7c00) | ((c >> 6) & 0x03e0) | ((c >> 3) & 0x001f);
	else
		return ((c >> 17) & 0x007c) | ((c >> 14) & 0x0003) | ((c << 2) & 0xe000) | ((c << 5) & 0x1f00);
}

Color PixFmtRGB15pc::int_to_color(int32u c)
{
	if (!big_endian)
		return ((c & 0x7c00) << 9) | ((c & 0x03e0) << 6) | ((c & 0x001f) << 3);
	else
		return ((c & 0x007c) << 17) | ((c & 0x0003) << 14) | ((c & 0xe000) >> 2) | ((c & 0x1f00) >> 5);
}

int32u PixFmtRGB15pc::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 3;
	int b1 = b >> 3;
	if (big_endian)
		return (r1 << 2) | (g1 >> 3) | ((g1 & 7) << 13) | (b1 << 8);
	else
		return (r1 << 10) | (g1 << 5) | b1;
}

void PixFmtRGB15pc::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (!big_endian)
		{
			r1 = (t & 0x7c00) >> 7;
			g1 = (t & 0x03e0) >> 2;
			b1 = (t & 0x001f) << 3;
		}
		else
		{
			r1 = (t & 0x007c) << 1;
			g1 = ((t & 0x0003) << 6) | ((t & 0xe000) >> 10);
			b1 = (t & 0x1f00) >> 5;
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t = ((r1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t = ((r1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x38) << 10) | ((b1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtRGB15pc::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (!big_endian)
		{
			r1 = (t1 & 0x7c00) >> 7;
			g1 = (t1 & 0x03e0) >> 2;
			b1 = (t1 & 0x001f) << 3;
			r2 = (t2 & 0x7c00) >> 7;
			g2 = (t2 & 0x03e0) >> 2;
			b2 = (t2 & 0x001f) << 3;
		}
		else
		{
			r1 = ((t1 & 0x007c) << 1);
			g1 = ((t1 & 0x0003) << 6) | ((t1 & 0xe000) >> 10);
			b1 =  (t1 & 0x1f00) >> 5;
			r2 = ((t2 & 0x007c) << 1);
			g2 = ((t2 & 0x0003) << 6) | ((t2 & 0xe000) >> 10);
			b2 =  (t2 & 0x1f00) >> 5;
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t1 = ((r1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((b1 & 0xf8) >> 3);
		}
		else
		{
			t1 = ((r1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x18) << 10) | ((b1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtRGB15pc pixfmt_rgb15pc;


class PixFmtBGR15 : public PixFmt16
{
public:
	PixFmtBGR15() : PixFmt16(bgr15) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtBGR15::color_to_int(Color c)
{
	if (big_endian)
		return ((c << 15) & 0x7c00) | ((c >> 6) & 0x03e0) | ((c >> 19) & 0x001f);
	else
		return ((c >> 1) & 0x007c) | ((c >> 14) & 0x0003) | ((c << 2) & 0xe000) | ((c << 11) & 0x1f00);
}

Color PixFmtBGR15::int_to_color(int32u c)
{
	if (big_endian)
		return ((c & 0x7c00) >> 7) | ((c & 0x03e0) << 6) | ((c & 0x001f) << 19);
	else
		return ((c & 0x007c) << 1) | ((c & 0x0003) << 14) | ((c & 0xe000) >> 2) | ((c & 0x1f00) << 11);
}

int32u PixFmtBGR15::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 3;
	int b1 = b >> 3;
	if (big_endian)
		return (b1 << 10) | (g1 << 5) | r1;
	else
		return (b1 << 2) | (g1 >> 3) | ((g1 & 7) << 13) | (r1 << 8);
}

void PixFmtBGR15::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (big_endian)
		{
			r1 = (t & 0x001f) << 3;
			g1 = (t & 0x03e0) >> 2;
			b1 = (t & 0x7c00) >> 7;
		}
		else
		{
			r1 = (t & 0x1f00) >> 5;
			g1 = ((t & 0x0003) << 6) | ((t & 0xe000) >> 10);
			b1 = (t & 0x007c) << 1;
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (big_endian)
		{
			t = ((b1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t = ((b1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x38) << 10) | ((r1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtBGR15::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (big_endian)
		{
			r1 = (t1 & 0x001f) << 3;
			g1 = (t1 & 0x03e0) >> 2;
			b1 = (t1 & 0x7c00) >> 7;
			r2 = (t2 & 0x001f) << 3;
			g2 = (t2 & 0x03e0) >> 2;
			b2 = (t2 & 0x7c00) >> 7;
		}
		else
		{
			r1 =  (t1 & 0x1f00) >> 5;
			g1 = ((t1 & 0x0003) << 6) | ((t1 & 0xe000) >> 10);
			b1 = ((t1 & 0x007c) << 1);
			r2 =  (t2 & 0x1f00) >> 5;
			g2 = ((t2 & 0x0003) << 6) | ((t2 & 0xe000) >> 10);
			b2 = ((t2 & 0x007c) << 1);
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (big_endian)
		{
			t1 = ((b1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t1 = ((b1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x18) << 10) | ((r1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtBGR15 pixfmt_bgr15;


class PixFmtBGR15pc : public PixFmt16
{
public:
	PixFmtBGR15pc() : PixFmt16(bgr15pc) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtBGR15pc::color_to_int(Color c)
{
	if (!big_endian)
		return ((c << 15) & 0x7c00) | ((c >> 6) & 0x03e0) | ((c >> 19) & 0x001f);
	else
		return ((c >> 1) & 0x007c) | ((c >> 14) & 0x0003) | ((c << 2) & 0xe000) | ((c << 11) & 0x1f00);
}

Color PixFmtBGR15pc::int_to_color(int32u c)
{
	if (!big_endian)
		return ((c & 0x7c00) >> 7) | ((c & 0x03e0) << 6) | ((c & 0x001f) << 19);
	else
		return ((c & 0x007c) << 1) | ((c & 0x0003) << 14) | ((c & 0xe000) >> 2) | ((c & 0x1f00) << 11);
}

int32u PixFmtBGR15pc::rgb_to_int(int8u r, int8u g, int8u b)
{
	int r1 = r >> 3;
	int g1 = g >> 3;
	int b1 = b >> 3;
	if (big_endian)
		return (b1 << 2) | (g1 >> 3) | ((g1 & 7) << 13) | (r1 << 8);
	else
		return (b1 << 10) | (g1 << 5) | r1;
}

void PixFmtBGR15pc::hline_alpha(int8u* p, int x, int w, int alpha, Color c)
{
	int16u* q = reinterpret_cast<int16u*>(p) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	short g = short((c >> 8) & 0xff);
	short b = short(c & 0xff);
	while (--w >= 0)
	{
		int16u t = *q;
		int r1, g1, b1;

		if (!big_endian)
		{
			r1 = (t & 0x001f) << 3;
			g1 = (t & 0x03e0) >> 2;
			b1 = (t & 0x7c00) >> 7;
		}
		else
		{
			r1 = (t & 0x1f00) >> 5;
			g1 = ((t & 0x0003) << 6) | ((t & 0xe000) >> 10);
			b1 = (t & 0x007c) << 1;
		}

		r1 = (r * u + r1 * v) >> 8;
		g1 = (g * u + g1 * v) >> 8;
		b1 = (b * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t = ((b1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t = ((b1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x38) << 10) | ((r1 & 0xf8) << 5);
		}

		*q++ = t;
	}
}

void PixFmtBGR15pc::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	int16u* p1 = reinterpret_cast<int16u*>(p) + x;
	const int16u* q1 = reinterpret_cast<const int16u*>(q) + x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		int16u t1 = *p1;
		int16u t2 = *q1++;
		int r1, g1, b1;
		int r2, g2, b2;

		if (!big_endian)
		{
			r1 = (t1 & 0x001f) << 3;
			g1 = (t1 & 0x03e0) >> 2;
			b1 = (t1 & 0x7c00) >> 7;
			r2 = (t2 & 0x001f) << 3;
			g2 = (t2 & 0x03e0) >> 2;
			b2 = (t2 & 0x7c00) >> 7;
		}
		else
		{
			r1 =  (t1 & 0x1f00) >> 5;
			g1 = ((t1 & 0x0003) << 6) | ((t1 & 0xe000) >> 10);
			b1 = ((t1 & 0x007c) << 1);
			r2 =  (t2 & 0x1f00) >> 5;
			g2 = ((t2 & 0x0003) << 6) | ((t2 & 0xe000) >> 10);
			b2 = ((t2 & 0x007c) << 1);
		}

		r1 = (r2 * u + r1 * v) >> 8;
		g1 = (g2 * u + g1 * v) >> 8;
		b1 = (b2 * u + b1 * v) >> 8;

		if (!big_endian)
		{
			t1 = ((b1 & 0xf8) << 7) | ((g1 & 0xf8) << 2) | ((r1 & 0xf8) >> 3);
		}
		else
		{
			t1 = ((b1 & 0xf8) >> 1) | ((g1 & 0xc0) >> 6) | ((g1 & 0x18) << 10) | ((r1 & 0xf8) << 5);
		}

		*p1++ = t1;
	}
}

PixFmtBGR15pc pixfmt_bgr15pc;


class PixFmtR8 : public PixFmt8
{
public:
	PixFmtR8() : PixFmt8(red8) {}
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
};

int32u PixFmtR8::color_to_int(Color c)
{
	return (c >> 16) & 0xff;
}

Color PixFmtR8::int_to_color(int32u c)
{
	return c << 16;
}

int32u PixFmtR8::rgb_to_int(int8u r, int8u g, int8u b)
{
	return r;
}

void PixFmtR8::hline_alpha(int8u* q, int x, int w, int alpha, Color c)
{
	q += x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = short((c >> 16) & 0xff);
	while (--w >= 0)
	{
		*q = (r * u + *q * v) >> 8;
		++q;
	}
}

void PixFmtR8::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x;
	q += x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		*p = (*q * u + *p * v) >> 8;
		++p;
		++q;
	}
}

PixFmtR8 pixfmt_r8;


class PixFmtGray8 : public PixFmt8
{
public:
	PixFmtGray8(int8u* m);
	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual void hline_alpha(int8u* p, int x, int w, int alpha, Color c);
	virtual void hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha);
private:
	int8u map[256];
	Color pen[256];
};

PixFmtGray8::PixFmtGray8(int8u* m) : PixFmt8(gray8)
{
	memcpy(map, m, 256);

	for (int k = 0; k < 256; ++k)
	{
		pen[m[k]] = k * 0x010101;
	}
}

int32u PixFmtGray8::color_to_int(Color c)
{
	return map[(((c >> 16) & 0xff) * 11 + ((c >> 4) & 0xff0) + (c & 0xff) * 5) >> 5];
}

Color PixFmtGray8::int_to_color(int32u c)
{
	return pen[c];
}

int32u PixFmtGray8::rgb_to_int(int8u r, int8u g, int8u b)
{
	return map[int(r * (.33 / 255) + g * (.5 / 255) + b * (.17 / 255))];
}

void PixFmtGray8::hline_alpha(int8u* q, int x, int w, int alpha, Color c)
{
	q += x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	short r = (((c >> 16) & 0xff) * 11 + ((c >> 4) & 0xff0) + (c & 0xff) * 5) >> 5;
	while (--w >= 0)
	{
		*q = map[(r * u + (pen[*q] & 0xff) * v) >> 8];
		++q;
	}
}

void PixFmtGray8::hline_alpha_blit(int8u* p, const int8u* q, int x, int w, int alpha)
{
	p += x;
	q += x;
	short u = short(alpha) + 1;
	short v = short(256 - u);
	while (--w >= 0)
	{
		*p = map[((pen[*q] & 0xff) * u + (pen[*p] & 0xff) * v) >> 8];
		++p;
		++q;
	}
}


#if 0
//class ColorTable
//{
//public:
//
//	int8u find(Color c) { return 0; } //~
//	Color get_pen(int8u n) { return pens[n]; }
//
//private:
//	Color pens[256];
//};

class PixFmtLUT8 : public PixFmt
{
public:
	PixFmtLUT8(ColorTable *table) : PixFmt(lut8), color_table(*table)
	{
		bpp = 1;
		rectfill_func = ::rectfill<int8u>;
		blit_func = ::blit<int8u>;
		hline_func = ::hline<int8u>;
		hline_blit_func = ::hline_blit<int8u>;
		hline_dash_func = ::hline_dash<int8u>;
		vline_dash_func = ::vline_dash<int8u>;
		line_dash_func = ::line_dash<int8u>;
	}

	virtual Color int_to_color(int32u);
	virtual int32u color_to_int(Color);
	virtual int32u rgb_to_int(int8u r, int8u g, int8u b);
	virtual Color get_pixel(const int8u* p);
	virtual void put_pixel(int8u* p, Color);

	virtual void regionfill(int8u* q, int m, int x, int w, int h, ARegion::Row** p, Color c);
	virtual void blit(int8u* q, int m, const int8u* p, int m1, int w, int h, int dx, ARegion::Row** rows);

private:
	Color last_color;
	int8u last_pen;
	ColorTable& color_table;
};

int32u PixFmtLUT8::color_to_int(Color c)
{
	if (c == last_color)
	{
		return last_pen;
	}
	else
	{
		last_color = c;
		last_pen = color_table.find(c);
		return last_pen;
	}
}

Color PixFmtLUT8::int_to_color(int32u c)
{
	return color_table.get_pen(c);
}

int32u PixFmtLUT8::rgb_to_int(int8u r, int8u g, int8u b)
{
	return color_to_int((r << 16) | (g << 8) | b);
}

Color PixFmtLUT8::get_pixel(const int8u* p)
{
	return int_to_color(*p);
}

void PixFmtLUT8::put_pixel(int8u* p, Color c)
{
	*p = color_to_int(c);
}


void PixFmtLUT8::regionfill(int8u* q, int m, int x, int w, int h, ARegion::Row** p, Color c)
{
	int8u c1 = int8u(color_to_int(c));

	while (--h >= 0)
	{
		ARegion::Row* row = *p++;
		if (row)
		{
			int n = row->count;
			ARegion::Span *span = row->spans;

			while (--n >= 0)
			{
				int alpha = span->alpha;
				if (alpha > 127)
				{
					int x1 = span->x + x;
					int w1 = span->length;
					if (x1 < w && x1 + w1 > 0)
					{
						if (x1 < 0)
						{
							w1 += x1;
							x1 = 0;
						}
						if (x1 + w1 > w)
							w1 = w - x1;

						int8u* t = q + x1;
						for (int k = 0; k < w1; ++k)
							*t++ = c1;
					}
				}
				++span;
			}

		}
		q += m;
	}
}

void PixFmtLUT8::blit(int8u* q, int m, const int8u* p, int m1, int w, int h, int dx, ARegion::Row** rows)
{
	while (--h >= 0)
	{
		ARegion::Row* row = *rows++;
		if (row)
		{
			int n = row->count;
			ARegion::Span *span = row->spans;

			while (--n >= 0)
			{
				int alpha = span->alpha;
				if (alpha > 127)
				{
					int x = span->x + dx;
					int w1 = span->length;
					if (x + w1 > 0 && x < w)
					{
						if (x < 0)
						{
							w1 += x;
							x = 0;
						}
						if (x + w1 > w)
							w1 = w - x;

						int8u* d = q + x;
						const int8u* s = p + x;

						for (int k = 0; k < w1; ++k)
							*d++ = *s++;
					}
				}
				++span;
			}

		}
		q += m;
		p += m1;
	}
}
#endif


PixFmt* PixFmt::fmts[] =
{
	NULL,
	&pixfmt_rgb15,
	&pixfmt_bgr15,
	&pixfmt_rgb15pc,
	&pixfmt_bgr15pc,
	&pixfmt_rgb16,
	&pixfmt_bgr16,
	&pixfmt_rgb16pc,
	&pixfmt_bgr16pc,
	&pixfmt_rgb24,
	&pixfmt_bgr24,
	&pixfmt_argb32,
	&pixfmt_bgra32,
	&pixfmt_rgba32,
	&pixfmt_r8,
	NULL,
};

#if 0
PixFmt* PixFmt::get(ColorTable *table)
{
	return new PixFmtLUT8(table);
}
#endif

PixFmt* PixFmt::get(FmtId id, int8u* map)
{
	switch (id)
	{
		case gray8:
			return new PixFmtGray8(map);
	}
	return fmts[id];
}

void PixFmt::release(PixFmt *fmt)
{
	if (fmt->id == lut8)
		delete fmt;
}

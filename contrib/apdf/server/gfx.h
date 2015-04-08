#include <cstddef>

#define DEBUG 1

#undef BIG_ENDIAN
#if defined(__PPC__) || defined(mc68000)
#define BIG_ENDIAN	1
#elif defined(__i386__)
#define BIG_ENDIAN	0
#else
#error set the endian type
#endif

static const bool big_endian = BIG_ENDIAN;

typedef unsigned char int8u;
typedef unsigned short int16u;
typedef unsigned long int32u;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;

typedef int32u Color;

class Color24
{
public:

	Color24() {}
	Color24(int32u c) : r(c >> 16), g(c >> 8), b(c) {}

	operator int32u () const { return (r << 16) | (g << 8) | b; }

	int8u r;
	int8u g;
	int8u b;
};

static inline int8u swap(int8u x) { return x; }
static inline int16u swap(int16u x) { return int16u((x << 8) | (x >> 8)); }
static inline int32u swap(int32u x) { x = (x << 16) | (x >> 16); return ((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8); }

class DashState;
class ABitmap;

class ARegion
{
public:

	struct Span
	{
		int16 x;
		int8u length;
		int8u alpha;
	};

	struct Row
	{
		int count;
		Span spans[0];
	};

	ARegion();
	~ARegion();

	int x0;
	int y0;
	int width;
	int height;

	int xmin() const { return -x0; }
	int ymin() const { return -y0; }
	int xmax() const { return width - x0; }
	int ymax() const { return height - y0; }

	Row **rows;

	Row **alloc_rows(int num);
	Span* add_spans(int y, int num_spans);
	void clear();
};


struct Color32
{
	int32u r, g, b;
};

struct ColorEntry
{
	int index;
	Color32 color;
};

#if 0
class ColorAllocator
{
public:
	ColorAllocator() {}
	virtual ~ColorAllocator() {}
	virtual int8u allocate(Color) = 0;
	virtual void allocate(int, ColorEntry *) = 0;
private:
	ColorAllocator(const ColorAllocator&);
	ColorAllocator& operator = (const ColorAllocator&);
};
#endif
#if 0
class ColorTable
{
public:
	ColorTable(ColorAllocator *color_allocator1)
		: color_allocator(color_allocator1), num(0),
		  entries(NULL), max_entries(0) {}
	~ColorTable();
	int8u find(Color);
	Color get_pen(int8u c) { return pens[c]; }
	void allocate(int n, ColorEntry *p);
	void clear() { num = 0; }
private:
	ColorTable(const ColorTable&);
	ColorTable& operator = (const ColorTable&);

	unsigned num;
	struct entry
	{
		int8u pen;
		Color value;
	};
	entry* entries;
	unsigned max_entries;
	ColorAllocator* color_allocator;
	Color pens[256];
};
#elif 0
class ColorTable
{
public:
	ColorTable(ColorAllocator *color_allocator1)
		: color_allocator(color_allocator1) {}
	~ColorTable();
	int8u get_pen(int8u c) { return pens[c]; }
	Color get_pen_value(int8u pen) { return pen_values[pen]; }
	void allocate(int n, ColorEntry *p);
protected:
	ColorTable(const ColorTable&);
	ColorTable& operator = (const ColorTable&);

	ColorAllocator* color_allocator;
	int8u pens[256];
	Color pen_values[256];
};
#endif

class PixFmt
{
public:
	enum FmtId
	{
		lut8, rgb15, bgr15, rgb15pc, bgr15pc, rgb16, bgr16, rgb16pc, bgr16pc,
		rgb24, bgr24, argb32, bgra32, rgba32, red8, gray8, count
	};

	PixFmt(FmtId id1) : id(id1) {}
	virtual ~PixFmt() {}

	virtual int32u color_to_int(Color color) =0;
	virtual Color int_to_color(int32u color) =0;
	virtual int32u rgb_to_int(int8u r,  int8u g, int8u b) =0;
	virtual Color get_pixel(const int8u*) = 0;
	virtual void put_pixel(int8u*, Color) = 0;

	//virtual void fill(int x1, int y1, int x2, int y2, Color c);
	//virtual void fill(int x, int y, ARegion* region, Color c);

	//virtual void blit(int x1, int y1, ABitmap* bm, int x2, int y2, int w, int h);

	int bytes_per_pixel() const
	{
		return bpp;
	}

	bool is_truecolor() const { return id != lut8; }

	void rectfill(int8u* p, int add, int w, int h, int32u color)
	{
		rectfill_func(p, add, w, h, color);
	}

	virtual void regionfill(int8u* q, int m, int x, int w, int h, ARegion::Row** p, Color c) =0;
	virtual void blit(int8u* q, int m, const int8u* p, int m1, int w, int h, int dx, ARegion::Row** rows) =0;

	void blit(int8u* dst, int add1, const int8u* src, int add2, int w, int h)
	{
		blit_func(dst, add1, src, add2, w, h);
	}

	static PixFmt* get(FmtId id) { return fmts[id]; }
	static PixFmt* get(FmtId id, int8u*);
	//static PixFmt* get(ColorTable*);
	static void release(PixFmt*);

protected:
#if 0//!BIG_ENDIAN
	int32u (*color_to_swapped_int_func)(Color);
	Color (*swapped_int_to_color_func)(int32u);
#endif
	void (*rectfill_func)(int8u*, int, int, int, int32u);
	void (*blit_func)(int8u*, int, const int8u*, int, int, int);

	FmtId id;
	int bpp;
public:
	void hline(int8u* p, int x, int w, int32u c) { hline_func(p, x, w, c); }
	void (*hline_func)(int8u*, int x, int w, int32u c);
	//void (*hline_alpha_func)(int8u*, int x, int w, int a, Color c);
	void hline_blit(int8u* dst, const int8u* src, int x, int w) { hline_blit_func(dst, src, x, w); }
	void (*hline_blit_func)(int8u* dst, const int8u* src, int x, int w);
	//void (*hline_alpha_blit_func)(int8u* dst, int x1, const int8u* src, int x2, int w, int a);
	void (*hline_dash_func)(int8u*, int x, int w, int32u c, DashState&);
	void (*vline_dash_func)(int8u*, int x, int y, int h, int modulo, int32u c, DashState&);
	void (*line_dash_func)(int8u*, int modulo, int x1, int y1, int x2, int y2, int32u c, DashState&);

private:
	static PixFmt* fmts[count];
};


class APath;


class DashState
{
public:
	enum { fract_bits = 8 };

	DashState() : array(NULL), length(0) {}
	DashState(int dash_length, const int *dash_pattern, int dash_start)
	: array(NULL), length(0)
	{
		init(dash_length, dash_pattern, dash_start);
	}
	~DashState() { delete [] array; }
	void reset() { delete [] array; array = NULL; length = 0; }
	void init(int dash_length, const int* dash_pattern, int dash_start);
	bool is_solid() const { return array == NULL; }
	bool is_on() const { return on; }
	int count() const { return num + (1 << fract_bits - 1) >> fract_bits; }
	int raw_count() const { return num; }
	void advance(int step)
	{
		num -= step;
		while (num <= 0)
		{
			on = !on;
			if (++index == length)
				index = 0;
			num += array[index];
		}
	}
private:
	const int *array;	// dash pattern
	int length;		// array size
	int index;		// current index in array
	int num;		// number of remaining dots for this value of index
	bool on;		// current state is 'on' ?
};


class ABitmap
{
public:
	ABitmap() : data(NULL), pixfmt(NULL), width(0), height(0), owner(false) {}
	ABitmap(int w, int h, PixFmt* fmt, int xoffset = 0, int yoffset = 0);
	ABitmap(ABitmap& , int x, int y, int w, int h);
	~ABitmap();

	void init(int8u* dat, int x, int y, int w, int h, int mod, PixFmt* fmt);

	void fill(int x1, int y1, int x2, int y2, Color c);
	void fill(int x, int y, ARegion* region, Color c);

	void blit(int x1, int y1, ABitmap* bm, int x2, int y2, int w, int h);
	void blit(int x1, int y1, ABitmap* bm)
	{
		blit(x1, y1, bm, 0, 0, bm->width, bm->height);
	}
	void blit(int x1, int y1, ABitmap *bm, int x2, int y2, ARegion* mask);

	void line(int x1, int y1, int x2, int y2, Color c, DashState&);
	void bezier(int x1, int y1, int x2, int y2, int x3, int y3, Color c, DashState&);
	void bezier(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color c, DashState&);

	void stroke(int x, int y, APath* path, Color c);

	int8u* data;
	int width;
	int height;
	int bpp;
	int modulo;
	PixFmt* pixfmt;
	int xoffset;
	int yoffset;
	bool owner;
private:
	void int_line(int x1, int y1, int x2, int y2, int32u c, DashState&);
};


enum CapStyle
{
	cap_butt, cap_round, cap_projecting
};

enum JoinStyle
{
	join_miter, join_round, join_bevel
};


class APath
{
public:
	class Scanner;
	class CallBack;

	class Part
	{
		friend class APath;
		friend class Scanner;
		friend class CallBack;
	protected:
		enum Type
		{
			type_move, type_line, type_bezier2, type_bezier3
		};

		Part(Type t, double u, double v) : next(NULL), type(t), end(this), close(NULL), x(u), y(v) {}
		virtual ~Part() {}
		virtual Part* clone() const =0;
		virtual void callback(CallBack&) const =0;

	public:

#if DEBUG
		virtual void print() const =0;
#endif

		Part* join(Part* p)
		{
			if (p)
			{
				end->next = p;
				end = p->end;
				p->end = NULL;
			}
			return this;
		}

	protected:
		virtual Part* validate(Part*);
		Part* next;
		Part* end;
		Part* close;
		Type type;
	public:
		double x, y;
	};

	class Move : public Part
	{
	public:
		Move(double x1, double y1) : Part(type_move, x1, y1) {}
		virtual Move* clone() const { return move(x, y); }
		virtual void callback(CallBack&) const;
#if DEBUG
		virtual void print() const;
#endif
	private:
		virtual Part* validate(Part*);
	};

	class Line : public Part
	{
	public:
		Line(double x1, double y1) : Part(type_line, x1, y1) {}
		virtual Line* clone() const { return line(x, y); }
		virtual void callback(CallBack&) const;
#if DEBUG
		virtual void print() const;
#endif
	};

	class Bezier2 : public Part
	{
	public:
		Bezier2(double x1a, double y1a, double x2a, double y2a)
			: Part(type_bezier2, x2a, y2a), x1(x1a), y1(y1a) {}
		virtual Bezier2* clone() const { return bezier(x1, y1, x, y); }
		virtual void callback(CallBack&) const;
#if DEBUG
		virtual void print() const;
#endif
	private:
		double x1, y1;
	};

	class Bezier3 : public Part
	{
	public:
		Bezier3(double x1a, double y1a, double x2a, double y2a, double x3a, double y3a)
			: Part(type_bezier3, x3a, y3a), x1(x1a), y1(y1a), x2(x2a), y2(y2a) {}
		virtual Bezier3* clone() const { return bezier(x1, y1, x2, y2, x, y); }
		virtual void callback(CallBack&) const;
#if DEBUG
		virtual void print() const;
#endif
	private:
		double x1, y1, x2, y2;
	};

	class CallBack
	{
	public:
		virtual ~CallBack() {}
		virtual void move(double x, double y) {}
		virtual void line(double x, double y) {}
		virtual void bezier(double x1, double y1, double x2, double y2) {}
		virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3) {}
		void operator () (Part* p) { p->callback(*this); }
	};

	class Scanner : public CallBack
	{
	public:
		virtual ~Scanner() {}
		virtual void open(Part*) {}
		virtual void close() {}

		void scan(const APath*);
	};

	static Part* join(Part* p1, Part* p2);
	static Part* join(Part* p1, APath* p2);
	static Part* clone(Part* p);
	static Part* close(Part* p);
	static void destroy(Part* p);
	static void move(Part* p, double, double);
	static void get_delta(Part* p, double&, double&);

	static Move* move(double, double);
	static Line* line(double, double);
	static Bezier2* bezier(double, double, double, double);
	static Bezier3* bezier(double, double, double, double, double, double);

	APath(Part*);
	~APath();

	APath* make_dashes(DashState&) const;
	APath* thicken(double lw, CapStyle cap_style, JoinStyle join_style, double miter_limit) const;

	ARegion* make_region(bool eo = false) const;

	bool is_rect(int& x1, int& y1, int& x2, int& y2) const;

	void get_delta(double& dx, double& dy) const;

#if DEBUG
	void print() const;
	static void print(const Part*);
#endif

private:
	friend class Scanner;
	Part* parts;
};


class AGfxState
{
	class ClipState;

public:
	class Save
	{
		friend class AGfxState;
		Save *prev;
		ClipState *clip;
	};

	AGfxState(ABitmap* =NULL);
	~AGfxState();

	void reset();

	Save* push();
	void pop();
	void pop(Save *);

	void add_clip(int x1, int y1, int x2, int y2);
	void add_clip(int x, int y, ARegion *mask);

	void fill(int x1, int y1, int x2, int y2, Color c)
	{
		if (bitmap)
			bitmap->fill(x1, y1, x2, y2, c);
	}

	void fill(int x, int y, ARegion* region, Color c)
	{
		if (bitmap)
			bitmap->fill(x, y, region, c);
	}

	void line(int x1, int y1, int x2, int y2, Color c, DashState& ds)
	{
		if (bitmap)
			bitmap->line(x1, y1, x2, y2, c, ds);
	}

	void stroke(int x, int y, APath* path, Color c, double lw, CapStyle cap, JoinStyle join, double ml, DashState& ds);

	ABitmap *bitmap;

private:
	void pop_clip();

	class ClipState
	{
		friend class AGfxState;
		ClipState *prev;
		ABitmap *bitmap;
		ARegion *mask;
		int mask_x;
		int mask_y;
	};


	ClipState *clip;
	Save *save_list;
};


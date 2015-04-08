#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "gfx.h"

#define PATHDB(x)	;
#define DEBUG_PARTS(x)	;
#define DEBUG_JOIN(x)	;
#define DEBUG_APATH(x)	;
#define DEBUG_SCAN(x)	;
#define DEBUG_ISRECT(x)	;
#define DEBUG_LOOPS(x)
#define DEBUG_BEZIER(x) ;

#define dprintf printf

inline double length(double x, double y) { return sqrt(x * x + y * y); }

#if DEBUG

void APath::print(const Part* parts)
{
	for (const Part* p = parts; p; p = p->next)
	{
		printf("\t%p ", p);
		p->print();
		printf(" x %f y %f", p->x, p->y);
		if (p->close)
			printf(" closed %p", p->close);
		printf("\n");
	}
}

void APath::print() const
{
	printf("APath %p\n", this);
	print(parts);
}

void APath::Move::print() const
{
	printf("move");
}

void APath::Line::print() const
{
	printf("line");
}

void APath::Bezier2::print() const
{
	printf("bezier x1 %f y1 %f", x1, y1);
}

void APath::Bezier3::print() const
{
	printf("bezier x1 %f y1 %f x2 %f y2 %f", x1, y1, x2, y2);
}

#endif

static inline int _iround(double x)
{
	return int(x + 65536.5) - 65536;
}

bool APath::is_rect(int& x1, int& y1, int& x2, int& y2) const
{
	int x[4];
	int y[4];
	int i = 0;
	int j = 0;
	double x0, y0;

	DEBUG_ISRECT(printf("is_rect:\n"); print());

	Part *p = parts;

	if (!p || !p->close || p->type != Part::type_move)
		return false;

	x0 = p->x;
	y0 = p->y;
	x[0] = _iround(x0);
	y[0] = _iround(y0);
	p = p->next;
	DEBUG_ISRECT(printf("\t%f,%f\t%d,%d\n", x0, y0, x[0], y[0]));

	for (int k = 1; k < 4; ++k, p = p->next)
	{
		if (!p || p->type != Part::type_line)
			return false;

		x0 += p->x;
		y0 += p->y;
		x[k] = _iround(x0);
		y[k] = _iround(y0);
		DEBUG_ISRECT(printf("\t%f,%f\t%d,%d\n", x0, y0, x[k], y[k]));

		if (x[k] <= x[i] && y[k] <= y[i])
			i = k;

		if (x[k] >= x[j] && y[k] >= y[j])
			j = k;
	}

	if (p && (p->next || p->type != Part::type_line ||
				_iround(x0 + p->x) != x[0] || _iround(y0 + p->y) != y[0]))
	{
		DEBUG_ISRECT(printf("too many components\n"));
		return false;
	}

	DEBUG_ISRECT(printf("i %d j %d\n", i, j));

	if (i != j && x[i] != x[j] && y[i] != y[j])
	{
		if (j != ((i + 2) & 3))
		{
			return false;
		}

		int k = (i + 1) & 3;
		int l = (j + 1) & 3;

		if (x[k] == x[i])
		{
			int t = l; l = k; k = t;
		}

		DEBUG_ISRECT(printf("k %d l %d\n", k, l));

		if (x[k] != x[j] || x[l] != x[i] || y[k] != y[i] || y[l] != y[j])
			return false;
	}

	x1 = x[i];
	y1 = y[i];
	x2 = x[j];
	y2 = y[j];

	DEBUG_ISRECT(printf("-> ok: (%d,%d)-(%d,%d)\n", x1, y1, x2, y2));

	return true;
}

void APath::Scanner::scan(const APath* path)
{
	DEBUG_SCAN(dprintf("APath::Scanner::scan: path %p\n", path));
	Part* end = NULL;
	for (Part* p = path->parts; p; p = p->next)
	{
		if (p->close)
		{
			DEBUG_SCAN(dprintf("APath::Scanner::scan: open %p\n", p->close));
			open(p->close);
			end = p->close;
		}
		DEBUG_SCAN(dprintf("APath::Scanner::scan: %p\n", p));
		p->callback(*this);
		if (p == end)
		{
			DEBUG_SCAN(dprintf("APath::Scanner::scan: close\n"));
			close();
		}
	}
}

void APath::Move::callback(CallBack& s) const
{
	s.move(x, y);
}

void APath::Line::callback(CallBack& s) const
{
	s.line(x, y);
}

void APath::Bezier2::callback(CallBack& s) const
{
	s.bezier(x1, y1, x, y);
}

void APath::Bezier3::callback(CallBack& s) const
{
	s.bezier(x1, y1, x2, y2, x, y);
}

APath::Part* APath::Move::validate(Part* start)
{
	close = NULL;
	return this;
}

APath::Part* APath::Part::validate(Part* start)
{
	if (close && start)
	{
		start->close = this;
		start = NULL;
	}
	close = NULL;
	return start;
}

APath::Part* APath::join(Part* p1, Part *p2)
{
	DEBUG_JOIN(dprintf("APath::join: p1 %p p2 %p\n", p1, p2));
	if (p1 == NULL)
		return p2;
	else if (p2 == NULL)
		return p1;
	else
	{
		p1->end->next = p2;
		p1->end = p2->end;
		p2->end = NULL;
		return p1;
	}
}

APath::Part* APath::join(Part* p1, APath* p2)
{
	DEBUG_JOIN(dprintf("APath::join: p1 %p p2 %p\n", p1, p2));
	Part *r;
	if (p2 == NULL)
		return p1;
	else if (p1 == NULL)
		r = p2->parts;
	else
	{
		p1->end->next = p2->parts;
		p1->end = p2->parts->end;
		p2->parts->end = NULL;
		r = p1;
	}
	p2->parts = NULL;
	delete p2;
	return r;
}


APath::Part* APath::clone(Part* p)
{
	if (p)
	{
		Part* q = p->clone();
		while (p = p->next)
			q->join(p->clone());
		return q;
	}
	else
		return NULL;
}

void APath::destroy(Part* p)
{
	while (p)
	{
		Part* q = p->next;
		delete p;
		p = q;
	}
}

APath::Part* APath::close(Part* p)
{
	if (p)
	{
		double x = 0, y = 0;
		for (Part* q = p->next; q; q = q->next)
		{
			x += q->x;
			y += q->y;
		}
		if (x != 0 || y != 0)
		{
			Part* q = line(-x, -y);
			p->end->next = q;
			p->end = q;
		}
		p->end->close = p->end;
	}
	return p;
}

void APath::move(Part* p, double x, double y)
{
	p->x += x;
	p->y += y;
}

APath::Move* APath::move(double x, double y)
{
	DEBUG_APATH(printf("move(%f, %f)\n", x, y));
	return new Move(x, y);
}

APath::Line* APath::line(double x, double y)
{
	DEBUG_APATH(printf("line(%f, %f)\n", x, y));
	return new Line(x, y);
}

APath::Bezier2* APath::bezier(double x1, double y1, double x2, double y2)
{
	DEBUG_APATH(printf("bezier(%f, %f, %f, %f)\n", x1, y1, x2, y2));
	return new Bezier2(x1, y1, x2, y2);
}

APath::Bezier3* APath::bezier(double x1, double y1, double x2, double y2, double x3, double y3)
{
	DEBUG_APATH(printf("bezier(%f, %f, %f, %f, %f, %f)\n", x1, y1, x2, y2, x3, y3));
	return new Bezier3(x1, y1, x2, y2, x3, y3);
}

APath::APath(Part* p)
{
	DEBUG_APATH(dprintf("APath::ctor: parts %p\n", p));
	Part* start = NULL;
	for (Part* q = p; q; q = q->next)
	{
		start = q->validate(start);
	}

	parts = p;
}

APath::~APath()
{
	destroy(parts);
}

void APath::get_delta(Part* parts, double& dx, double& dy)
{
	double x = 0, y = 0;
	for (Part* p = parts; p; p = p->next)
	{
		x += p->x;
		y += p->y;
		if (p->close)
			p = p->close;
	}
	dx = x;
	dy = y;
}

void APath::get_delta(double& dx, double& dy) const
{
	get_delta(parts, dx, dy);
}

class StrokePathBuilder : public APath::Scanner
{
public:
	StrokePathBuilder(double lw, CapStyle c, JoinStyle j, double ml)
	: full_path(NULL), path1(NULL), path2(NULL), first(true),
	  x1(0), y1(0), totx(0), toty(0), line_width(lw / 2), is_closed(false),
	  cap_style(c), join_style(j), miter_limit(ml), xe(0), ye(0) {}

	virtual void move(double x, double y);
	virtual void line(double x, double y);
	virtual void bezier(double x1, double y1, double x2, double y2);
	virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3);
	virtual void open(APath::Part*);

	void set_pos(double x, double y) { totx += x; toty += y; }
	void add_pos(double x, double y) { x1 += x; y1 += y; }
	void add_err(double x, double y) { xe += x; ye += y; }
	void set_dir(double x, double y);
	void start();
	void do_close();
	void join();
	APath* get() { return new APath(full_path); }
	void add1(APath::Part* p) { path1 = APath::join(path1, p); }
	void add2(APath::Part* p) { path2 = APath::join(p, path2); }
	void add_cap();
	void add_correction(double& x, double& y)
	{
		x += xe; y += ye;
		xe = ye = 0;
	}
private:

	class LastDir : public APath::CallBack
	{
	public:
		virtual void line(double x, double y) { dx = x; dy = y; }
		virtual void bezier(double x1, double y1, double x2, double y2)
		{
			dx = x2 - x1; dy = y2 - y1;
		}
		virtual void bezier(double x1, double y1, double x2, double y2,
				double x3, double y3)
		{
			dx = x3 - x2; dy = y3 - y2;
		}

		double dx, dy;
	};

	APath::Part* full_path;
	APath::Part* path1;
	APath::Part* path2;
	double x1, y1;
	double xe, ye;
	double dx1, dy1;
	double dx2, dy2;
	double totx, toty;
	const double line_width;
	double miter_limit;
	CapStyle cap_style;
	JoinStyle join_style;
	bool is_closed;
public:
	bool first;
};

void StrokePathBuilder::set_dir(double x, double y)
{
	dx1 = dx2;
	dy1 = dy2;
	dx2 = x;
	dy2 = y;
}

void StrokePathBuilder::start()
{
	PATHDB(printf("starting (%f,%f), (%f,%f) closed %d\n", x1, y1, dx2, dy2, is_closed));
	if (is_closed)
	{
		path1 = APath::move(x1 + dy1, y1 - dx1);
		totx = dy1;
		toty = -dx1;
		join();
	}
	else
	{
		switch(cap_style)
		{
			case cap_round:
				path1 = APath::move(x1 - dx2, y1 - dy2);
				path1->join(APath::bezier(dy2, -dx2, dx2 + dy2, dy2 - dx2));
				path2 = APath::bezier(-dx2, -dy2, dy2 - dx2, -dx2 - dy2);
				totx = dx2;
				toty = dy2;
				break;
			case cap_projecting:
				path1 = APath::move(x1 + dy2 - dx2, y1 - dx2 - dy2);
				path1->join(APath::line(dx2, dy2));
				path2 = APath::line(-dx2, -dy2);
				totx = dx2 - dy2;
				toty = dx2 + dy2;
				break;
			default: /* cap_butt */
				path1 = APath::move(x1 + dy2, y1 - dx2);
				totx = -dy2;
				toty = dx2;
				break;
		}
	}
	x1 = 0;
	y1 = 0;
}

void StrokePathBuilder::join()
{
	double tdx1, tdy1, tdx2, tdy2;
	bool side = dx1 * dy2 - dy1 * dx2 < 0;
	tdx1 = dx1; tdy1 = dy1; tdx2 = dx2; tdy2 = dy2;
	PATHDB(printf("joining (%f,%f)-(%f,%f)\n", dx1, dy1, dx2, dy2));
	APath::Part* seg1 = NULL;
	APath::Part* seg2 = APath::line(tdy2 - tdy1, tdx1 - tdx2);
	switch(join_style)
	{
		case join_round:
			{
				double d = tdx2 * tdy1 - tdx1 * tdy2;
				if (fabs(d) > 0.0001)
				{
					double a = (tdx2 * (tdx1 - tdx2) - tdy2 * (tdy2 - tdy1)) / d;
					double x = a * tdx1;
					double y = a * tdy1;
					if (side)
						seg1 = APath::bezier(tdy2 - tdy1 - x, tdx1 - tdx2 - y, tdy2 - tdy1, tdx1 - tdx2);
					else
						seg1 = APath::bezier(x, y, tdy2 - tdy1, tdx1 - tdx2);
				}
				else
				{
					seg1 = APath::clone(seg2);
				}
			}
			break;
		case join_miter:
			{
				double d = tdx2 * tdy1 - tdx1 * tdy2;
				if (fabs(d * miter_limit) > line_width * line_width)
				{
					double a = (tdx2 * (tdx1 - tdx2) - tdy2 * (tdy2 - tdy1)) / d;
					double x = a * tdx1;
					double y = a * tdy1;
					APath::Part* p1 = APath::line(x, y);
					APath::Part* p2 = APath::line(tdy2 - tdy1 - x, tdx1 - tdx2 - y);
					if (side)
					{
						APath::Part *t = p1; p1 = p2; p2 = t;
					}
					seg1 = p1->join(p2);
					break;
				}
			} /*FALLTHROUGH*/
		default: /*join_bevel*/
			seg1 = APath::clone(seg2);
			break;
	}
	if(side)
	{
		APath::Part* t = seg1; seg1 = seg2; seg2 = t;
	}
	path1 = APath::join(path1, seg1);
	path2 = APath::join(seg2, path2);
}

void StrokePathBuilder::do_close()
{
	PATHDB(printf("closing (%f, %f) is_closed %d\n", dx2, dy2, is_closed));
	if (is_closed)
	{
		path1 = APath::close(path1);
		path2 = APath::close(APath::move(-2 * totx, -2 * toty)->join(path2));
		path1 = APath::join(path1, path2);
	}
	else
	{
		switch(cap_style)
		{
			case cap_round:
				path1 = APath::join(path1, APath::bezier(dx2, dy2, dx2 - dy2, dx2 + dy2));
				path1->join(APath::bezier(-dy2, dx2, -dx2 - dy2, dx2 - dy2));
				break;
			case cap_projecting:
				path1 = APath::join(path1, APath::line(dx2, dy2));
				path1->join(APath::line(-2 * dy2, 2 * dx2));
				path1->join(APath::line(-dx2, -dy2));
				break;
			default: /* cap_butt */
				path1 = APath::join(path1, APath::line(-2 * dy2, 2 * dx2));
				break;
		}
		path1 = APath::join(path1, path2);
		path1 = APath::close(path1);
	}
	full_path = APath::join(full_path, path1);
	path1 = path2 = NULL;
	x1 = totx;
	y1 = toty;
	is_closed = false;
}

void StrokePathBuilder::move(double x, double y)
{
	PATHDB(printf("move(%f, %f) first %d\n", x, y, first));
	if (!first)
		do_close();
	add_correction(x, y);
	add_pos(x, y);
	first = true;
}

void StrokePathBuilder::open(APath::Part* end)
{
	PATHDB(printf("open(%p)\n", end));
	if (!first)
		do_close();
	first = true;
	LastDir dir;
	dir(end);
	double x = dir.dx;
	double y = dir.dy;
	double d = length(x, y);
	if (d >= 0.0001)
	{
		d /= line_width;
		set_dir(x / d, y / d);
		is_closed = true;
	}
	else
		add_err(x, y);
}

void StrokePathBuilder::line(double x, double y)
{
	PATHDB(printf("line(%f, %f)\n", x, y));
	add_correction(x, y);
	double d = length(x, y);
	if (d >= 0.0001)
	{
		d /= line_width;
		set_dir(x / d, y / d);
		if (first)
			start();
		else
			join();
		add1(APath::line(x, y));
		add2(APath::line(-x, -y));
		set_pos(x, y);
		first = false;
	}
	else
		add_err(x, y);
}

void StrokePathBuilder::bezier(double x1, double y1, double x, double y)
{
	PATHDB(printf("bezier(%f, %f, %f, %f)\n", x1, y1, x, y));
	add_correction(x, y);
	double X1 = x1;
	double Y1 = y1;
	double X2 = x - x1;
	double Y2 = y - y1;
	double D1 = length(X1, Y1);
	double D2 = length(X2, Y2);
	if (D1 >= 0.0001 || D2 >= 0.0001)
	{
		double DX1, DY1, DX2, DY2;
		D1 /= line_width;
		D2 /= line_width;
		const double eps = 0.0001 / line_width;//1 / (line_width * 16);
		if (D1 < eps)
		{
			DX1 = X2 / D2;
			DY1 = Y2 / D2;
		}
		else
		{
			DX1 = X1 / D1;
			DY1 = Y1 / D1;
		}
		if (D2 < eps)
		{
			DX2 = DX1;
			DY2 = DY1;
		}
		else
		{
			DX2 = X2 / D2;
			DY2 = Y2 / D2;
		}
		set_dir(DX1, DY1);
		if (first)
			start();
		else
			join();
		set_dir(DX2, DY2);
		//printf("(%f,%f) (%f,%f) (%f,%f) lw=%f\n",X1,Y1,X2,Y2,X3,Y3,lw);
		//printf("dx1=%f dy1=%f dx2=%f dy2=%f dx3=%f dy3=%f\n",DX1,DY1,DX2,DY2,DX3,DY3);
		double DX4, DY4;
		double d = DX2 * DY1 - DY2 * DX1;
		if (fabs(d) < 0.0001)
		{
			DX4 = DY1;
			DY4 = -DX1;
		}
		else
		{
			double a = (DX2 * (Y1 + DX1 - DX2) - DY2 * (X1 + DY2 - DY1)) / d;
			DX4 = DY1 + a * DX1 - X1;
			DY4 = -DX1 + a * DY1 - Y1;
		}
		//printf("dx4=%f dy4=%f\n",DX4,DY4);
		//printf("dx5=%f dy5=%f\n",DX5,DY5);
		add1(APath::bezier(x1 + DX4 - DY1, y1 + DY4 + DX1,
					x + dy2 - dy1, y - dx2 + dx1));
		add2(APath::bezier(x1 - x - DX4 + DY2, y1 - y - DY4 - DX2,
					dy2 - dy1 - x, dx1 - dx2 - y));
		set_pos(x, y);
		first = false;
	}
	else
		add_err(x, y);
}

void StrokePathBuilder::bezier(double x1, double y1, double x2, double y2, double x, double y)
{
	const int max_levels = 13;
	struct
	{
		double x;
		double y;
	} *s, stack[max_levels];

	s = stack;

	for (;;)
	{
		PATHDB(printf("bezier(%f, %f, %f, %f, %f, %f)\n", x1, y1, x2, y2, x, y));

		double X1 = x1;
		double Y1 = y1;
		double X2 = x2 - x1;
		double Y2 = y2 - y1;
		double X3 = x - x2;
		double Y3 = y - y2;
		double D1 = length(X1, Y1);
		double D2 = length(X2, Y2);
		double D3 = length(X3, Y3);

		if (s < stack + max_levels - 3 &&
				//(D1 >= 0.01 || D2 >= 0.01 || D3 >= 0.01) &&
				(X1 * X2 + Y1 * Y2 < -0.5 * D1 * D2 - 0.00005 ||
				 X2 * X3 + Y2 * Y3 < -0.5 * D2 * D3 - 0.00005))
		{
			PATHDB(printf("split: (%f,%f) (%f,%f) (%f,%f)\n",
						X1, Y1, X2, Y2, X3, Y3));
			double xa = x1 / 2;
			double ya = y1 / 2;
			double xb = (x1 + x2) / 2;
			double yb = (y1 + y2) / 2;
			double xc = (x2 + x) / 2;
			double yc = (y2 + y) / 2;
			double xd = (xa + xb) / 2;
			double yd = (ya + yb) / 2;
			double xe = (xb + xc) / 2;
			double ye = (yb + yc) / 2;
			double xf = (xd + xe) / 2;
			double yf = (yd + ye) / 2;

			s[0].x = xe - xf;
			s[0].y = ye - yf;
			s[1].x = xc - xf;
			s[1].y = yc - yf;
			s[2].x = x - xf;
			s[2].y = y - yf;
			s += 3;

			x = xf;
			y = yf;
			x1 = xa;
			y1 = ya;
			x2 = xd;
			y2 = yd;

			continue;
		}

		add_correction(x, y);
		X3 = x - x2;
		Y3 = y - y2;
		D3 = length(X3, Y3);
		if (D1 >= 0.01 || D2 >= 0.01 || D3 >= 0.01)
		{
			double DX1, DY1, DX2, DY2, DX3, DY3;
			D1 /= line_width;
			D2 /= line_width;
			D3 /= line_width;
			const double eps = 0.005 / line_width;//1 / (line_width * 16);
			if (D1 < eps)
			{
				if (D2 < eps)
				{
					DX1 = X3 / D3;
					DY1 = Y3 / D3;
				}
				else
				{
					DX1 = X2 / D2;
					DY1 = Y2 / D2;
				}
			}
			else
			{
				DX1 = X1 / D1;
				DY1 = Y1 / D1;
			}
			if (D2 < eps)
			{
				DX2 = DX1;
				DY2 = DY1;
			}
			else
			{
				DX2 = X2 / D2;
				DY2 = Y2 / D2;
			}
			if (D3 < eps)
			{
				DX3 = DX2;
				DY3 = DY2;
			}
			else
			{
				DX3 = X3 / D3;
				DY3 = Y3 / D3;
			}
			set_dir(DX1, DY1);
			if (first)
				start();
			else
				join();
			set_dir(DX3, DY3);
			//printf("(%f,%f) (%f,%f) (%f,%f) lw=%f\n",X1,Y1,X2,Y2,X3,Y3,lw);
			//printf("dx1=%f dy1=%f dx2=%f dy2=%f dx3=%f dy3=%f\n",DX1,DY1,DX2,DY2,DX3,DY3);
			double DX4, DY4;
			double d = DX2 * DY1 - DY2 * DX1;
			if (fabs(d) < 0.01)
			{
				DX4 = DY1;
				DY4 = -DX1;
			}
			else
			{
				double a = (DX2 * (Y1 + DX1 - DX2) - DY2 * (X1 + DY2 - DY1)) / d;
				DX4 = DY1 + a * DX1 - X1;
				DY4 = -DX1 + a * DY1 - Y1;
			}
			//printf("dx4=%f dy4=%f\n",DX4,DY4);
			double DX5, DY5;
			d = DX3 * DY2 - DY3 * DX2;
			if (fabs(d) < 0.01)
			{
				DX5 = DY2;
				DY5 = -DX2;
			}
			else
			{
				double a = (DX3 * (Y2 + DX2 - DX3) - DY3 * (X2 - DY2 + DY3)) / d;
				DX5 = DY2 + a * DX2 - X2;
				DY5 = -DX2 + a * DY2 - Y2;
			}
			//printf("dx5=%f dy5=%f\n",DX5,DY5);
			add1(APath::bezier(x1 + DX4 - DY1, y1 + DY4 + DX1,
						x2 + DX5 - DY1, y2 + DY5 + DX1,
						x + dy2 - dy1, y - dx2 + dx1));
			add2(APath::bezier(x2 - x - DX5 + DY3, y2 - y - DY5 - DX3,
						x1 - x - DX4 + DY3, y1 - y - DY4 - DX3,
						dy2 - dy1 - x, dx1 - dx2 - y));
			set_pos(x, y);
			first = false;
		}
		else
			add_err(x, y);

		if (s == stack)
			break;

		s -= 3;
		x1 = s[0].x;
		y1 = s[0].y;
		x2 = s[1].x;
		y2 = s[1].y;
		x = s[2].x;
		y = s[2].y;
	}
	PATHDB(printf("~bezier\n"));
}

APath* APath::thicken(double lw, CapStyle cap_style, JoinStyle join_style, double miter_limit) const
{
#if 0//DEBUG
	printf("thicken(%f, %d, %d, %f)\n", lw, cap_style, join_style, miter_limit);
	print();
#endif
	StrokePathBuilder builder(lw, cap_style, join_style, miter_limit);

	builder.scan(this);

	if (!builder.first)
		builder.do_close();

	APath* result = builder.get();

#if 0//DEBUG
	printf("result:\n");
	result->print();
#endif
	return result;
}



class DashPathBuilder : public APath::Scanner
{
public:
	DashPathBuilder(DashState& d)
	: path(NULL), ds(d)
	{
	}

	virtual void move(double x, double y);
	virtual void line(double x, double y);
	virtual void bezier(double x1, double y1, double x2, double y2);
	virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3);
	//virtual void open(APath::Part*);
	virtual void close();

	APath* get() { return new APath(path); }

private:
	APath::Part* path;
	DashState& ds;
};

void DashPathBuilder::move(double x, double y)
{
	PATHDB(printf("move(%f, %f) first %d\n", x, y, first));
	path = APath::join(path, APath::move(x, y));
}

void DashPathBuilder::line(double x, double y)
{
	PATHDB(printf("line(%f, %f)\n", x, y));

	double l = sqrt(x * x + y * y);
	double ux = x / l;
	double uy = y / l;
	double totx = 0;
	double toty = 0;

	for (;;)
	{
		int cnt = ds.raw_count();
		double l1 = double(cnt) / (1 << DashState::fract_bits);

		if (l1 >= l)
			break;

		double x1 = ux * l1;
		double y1 = uy * l1;

		APath::Part* p = ds.is_on() ? APath::line(x1, y1) : APath::move(x1, y1);
		path = APath::join(path, p);

		totx += x1;
		toty += y1;

		ds.advance(cnt);
		l -= l1;
	}

	x -= totx;
	y -= toty;

	if (x || y)
	{
		APath::Part* p = ds.is_on() ? APath::line(x, y) : APath::move(x, y);
		path = APath::join(path, p);
		ds.advance(int(l * (1 << DashState::fract_bits)));
	}
}

void DashPathBuilder::bezier(double x1, double y1, double x, double y)
{
	PATHDB(printf("bezier(%f, %f, %f, %f)\n", x1, y1, x, y));

	const int max_levels = 15;

	struct
	{
		double x;
		double y;
	} *s, stack[max_levels];

	s = stack;

	DEBUG_LOOPS(int n = 0);

	for (;;)
	{
		DEBUG_LOOPS(++n);

		DEBUG_BEZIER(printf("bezier: %f,%f %f,%f level %d\n",
					x1, y1, x, y, (s - stack) / 2));

		DEBUG_LOOPS(if (n > 200) printf("too many iterations: %f,%f %f,%f\n", x1,y1,x,y); else)
		if (s < stack + max_levels - 2)
		{
			double dx = fabs(x - 2 * x1);
			double dy = fabs(y - 2 * y1);
			if (dx < dy) dx = dy;

			if (dx > 1)
			{
				DEBUG_BEZIER(printf("bezier: split\n"));
				double x2 = (x1 + x) / 2;
				double y2 = (y1 + y) / 2;
				x1 /= 2;
				y1 /= 2;
				double x3 = (x1 + x2) / 2;
				double y3 = (y1 + y2) / 2;
				s->x = x - x3;
				s->y = y - y3;
				++s;
				s->x = x2 - x3;
				s->y = y2 - y3;
				++s;
				x = x3;
				y = y3;
				continue;
			}

			double midx = (2 * x1 + x) / 4;
			double midy = (2 * y1 + y) / 4;

			DEBUG_BEZIER(printf("bezier: line %f,%f + line %f,%f\n", midx, midy, x - midx, y - midy));

			line(midx, midy);
			line(x - midx, y - midy);
		}

		if (s == stack)
			break;

		--s;
		x1 = s->x;
		y1 = s->y;
		--s;
		x = s->x;
		y = s->y;
	}

	DEBUG_BEZIER(printf("bezier: done\n"));
}

void DashPathBuilder::bezier(double x1, double y1, double x2, double y2, double x, double y)
{
	PATHDB(printf("bezier(%f, %f, %f, %f, %f, %f)\n", x1, y1, x2, y2, x, y));

	const int max_levels = 22;

	struct
	{
		double x;
		double y;
	} *s, stack[max_levels];

	s = stack;

	DEBUG_LOOPS(int n = 0);

	for (;;)
	{
		DEBUG_LOOPS(if (++n > 400) printf("too many iterations: %f,%f %f,%f %f,%f\n", x1,y1,x2,y2,x,y); else)
		if (s < stack + max_levels - 3)
		{
			double dx = fabs(x - x1 - x2);
			double dy = fabs(y - y1 - y2);
			if (dx < dy) dx = dy;

			if (dx > 1)
			{
				double xa = (x1 + x2) / 2;
				double ya = (y1 + y2) / 2;
				double xb = (x2 + x) / 2;
				double yb = (y2 + y) / 2;
				x1 /= 2;
				y1 /= 2;
				x2 = (xa + x1) / 2;
				y2 = (ya + y1) / 2;
				double xc = (xa + xb) / 2;
				double yc = (ya + yb) / 2;
				double xe = (x2 + xc) / 2;
				double ye = (y2 + yc) / 2;
				s->x = xc - xe;
				s->y = yc - ye;
				++s;
				s->x = xb - xe;
				s->y = yb - ye;
				++s;
				s->x = x - xe;
				s->y = y - ye;
				++s;
				x = xe;
				y = ye;
				continue;
			}
		}

		double midx = (3 * (x1 + x2) + x) / 8;
		double midy = (3 * (y1 + y2) + y) / 8;

		line(midx, midy);
		line(x - midx, y - midy);

		if (s == stack)
			break;

		--s;
		x = s->x;
		y = s->y;
		--s;
		x2 = s->x;
		y2 = s->y;
		--s;
		x1 = s->x;
		y1 = s->y;
	}

	DEBUG_BEZIER(printf("bezier: done\n"));
}

void DashPathBuilder::close()
{
	path = APath::close(path);
}

APath* APath::make_dashes(DashState& ds) const
{
#if 0//DEBUG
	printf("make_dashes\n");
	print();
#endif

	DashPathBuilder builder(ds);

	builder.scan(this);

	APath* result = builder.get();

#if 0//DEBUG
	printf("result:\n");
	result->print();
#endif
	return result;
}

#include <stdio.h>
#include "gfx.h"

#define TIME 0

#ifndef ENABLE_ALTIVEC
#define ENABLE_ALTIVEC 0
#endif

#if TIME

static void get_time(unsigned long long& time)
{
    register unsigned long hi,lo,hi2;

#if __GNUC_PATCHLEVEL__ > 2
    hi = lo = hi2 = 0;
#endif
  __asm(
".loop:\n"
"	mftbu %1\n"
"	mftb  %2\n"
"	mftbu %3\n"
"	cmpw %1, %3\n"
"	bne .loop\n"
"	stw %1, 0(%0)\n"
"	stw %2, 4(%0)\n"
        :
        : "b" (&time), "r" (hi), "r" (lo), "r" (hi2)
        : "cc");
}

unsigned long long rectfill_time;
unsigned int num_rectfills;
unsigned long rectfill_size;

unsigned long long blit_time;
unsigned int num_blits;
unsigned long blit_size;

//unsigned long long hline_alpha_time;
//unsigned int num_hline_alpha;
//unsigned long hline_alpha_size;
#endif

typedef __vector unsigned char vint8u;
typedef __vector unsigned short vint16u;
typedef __vector unsigned int vint32u;

template<class T> void rectfill(int8u* q, int add, int w, int h, int32u color);

template<> void rectfill<int16u>(int8u* q, int add, int w, int h, int32u color)
{
#if TIME
	unsigned long long t1, t2;
	get_time(t1);
	++num_rectfills;
	rectfill_size += w * h;
#endif
	int16u c = int16u(color);
	if (ENABLE_ALTIVEC && w > 32)
	{
		union
		{
			vint16u v;
			int16u s;
		} tmpc;
		vint16u cv;
		unsigned p = reinterpret_cast<unsigned>(q);

		tmpc.s = c/* & 0xffef*/;
		cv = vec_splat(vec_lde(0, &tmpc.s), 0);

		add += w;
		add *= sizeof(int16u);
		w *= sizeof(int16u);

		while (--h >= 0)
		{
			int16u* r = reinterpret_cast<int16u*>(p & ~15);

			int x1 = p & 15;
			int x2 = x1 + w;
			int x = x1;

			switch (x1)
			{
				case 2: r[1] = c;
				case 4: r[2] = c;
				case 6: r[3] = c;
				case 8: r[4] = c;
				case 10: r[5] = c;
				case 12: r[6] = c;
				case 14: r[7] = c;
				x = 16;
			}

			while (x < x2 - 64)
			{
				vec_stl(cv, x, r);
				x += 16;
				vec_stl(cv, x, r);
				x += 16;
				vec_stl(cv, x, r);
				x += 16;
				vec_stl(cv, x, r);
				x += 16;
			}

			while (x < (x2 & ~15))
			{
				vec_stl(cv, x, r);
				x += 16;
			}

			r = reinterpret_cast<int16u*>(reinterpret_cast<unsigned>(r) + x);

			switch (x2 & 15)
			{
				case 14: r[6] = c;
				case 12: r[5] = c;
				case 10: r[4] = c;
				case 8: r[3] = c;
				case 6: r[2] = c;
				case 4: r[1] = c;
				case 2: r[0] = c;
			}

			p += add;
		}
	}
	else
	{
		int16u* p = reinterpret_cast<int16u*>(q);

		while (--h >= 0)
		{
			for (int k = 0; k < w; ++k)
				*p++ = c;

			p += add;
		}
	}
#if TIME
	get_time(t2);
	rectfill_time += t2 - t1;
#endif
}


template<class T> void blit(int8u* dst, int add1, const int8u* src, int add2, int w, int h);

template<> void blit<int16u>(int8u* dst, int add1, const int8u* src, int add2, int w, int h)
{
#if TIME
	unsigned long long t1, t2;
	get_time(t1);
	++num_blits;
	blit_size += w * h;
#endif
	if (ENABLE_ALTIVEC && w > 32)
	{
		unsigned d = reinterpret_cast<unsigned>(dst);
		const int16u* s = reinterpret_cast<const int16u*>(src);

		add1 += w;
		add1 *= sizeof(int16u);
		w *= sizeof(int16u);

		while (--h >= 0)
		{
			int16u* d1 = reinterpret_cast<int16u*>(d & ~15);

			int x1 = d & 15;
			int x2 = x1 + w;
			int x = x1;

			switch (x1)
			{
				case 2:  d1[1] = *s++;
				case 4:  d1[2] = *s++;
				case 6:  d1[3] = *s++;
				case 8:  d1[4] = *s++;
				case 10: d1[5] = *s++;
				case 12: d1[6] = *s++;
				case 14: d1[7] = *s++;
				x = 16;
			}

			vint16u high = vec_ldl(0, s);
			vint8u perm = vec_lvsl(0, s);

			s += 8;
			while (x < x2 - 64)
			{
				vint16u low = vec_ldl(0, s);
				s += 8;
				vint16u w = vec_perm(low, high, perm);
				vec_stl(w, x, d1);
				x += 16;
				high = vec_ldl(0, s);
				s += 8;
				w = vec_perm(high, low, perm);
				vec_stl(w, x, d1);
				x += 16;
				low = vec_ldl(0, s);
				s += 8;
				w = vec_perm(low, high, perm);
				vec_stl(w, x, d1);
				x += 16;
				high = vec_ldl(0, s);
				s += 8;
				w = vec_perm(high, low, perm);
				vec_stl(w, x, d1);
				x += 16;
			}

			while (x < (x2 & ~15))
			{
				vint16u low = vec_ldl(0, s);
				s += 8;
				vint16u w = vec_perm(low, high, perm);
				high = low;
				vec_stl(w, x, d1);
				x += 16;
			}

			s -= 8;
			d1 = reinterpret_cast<int16u*>(reinterpret_cast<unsigned>(d1) + x);

			switch (x2 & 15)
			{
				case 14: d1[6] = *s++;
				case 12: d1[5] = *s++;
				case 10: d1[4] = *s++;
				case 8:  d1[3] = *s++;
				case 6:  d1[2] = *s++;
				case 4:  d1[1] = *s++;
				case 2:  d1[0] = *s++;
			}

			d += add1;
			s += add2;
		}
	}
	else
	{
		int16u* p = reinterpret_cast<int16u*>(dst);
		const int16u* q = reinterpret_cast<const int16u*>(src);

		while (--h >= 0)
		{
			for (int k = 0; k < w; ++k)
				*p++ = *q++;

			p += add1;
			q += add2;
		}
	}
#if TIME
	get_time(t2);
	blit_time += t2 - t1;
#endif
}

#if 0
#define FUNC_START(x) " .globl " #x "\n" #x ":\n"
#define FUNC_END(x)
#define ENTRY_POINT(x) " .globl " #x "\n" #x ":\n"
asm(
	FUNC_START(_savev20)
"	addi	12,0,-192\n"
"	stvx	20,12,0	# save v20\n"
	ENTRY_POINT(_savev21)
"	addi	12,0,-176\n"
"	stvx	21,12,0	# save v21\n"
	ENTRY_POINT(_savev22)
"	addi	12,0,-160\n"
"	stvx	22,12,0	# save v22\n"
	ENTRY_POINT(_savev23)
"	addi	12,0,-144\n"
"	stvx	23,12,0	# save v23\n"
	ENTRY_POINT(_savev24)
"	addi	12,0,-128\n"
"	stvx	24,12,0	# save v24\n"
	ENTRY_POINT(_savev25)
"	addi	12,0,-112\n"
"	stvx	25,12,0	# save v25\n"
	ENTRY_POINT(_savev26)
"	addi	12,0,-96\n"
"	stvx	26,12,0	# save v26\n"
	ENTRY_POINT(_savev27)
"	addi	12,0,-80\n"
"	stvx	27,12,0	# save v27\n"
	ENTRY_POINT(_savev28)
"	addi	12,0,-64\n"
"	stvx	28,12,0	# save v28\n"
	ENTRY_POINT(_savev29)
"	addi	12,0,-48\n"
"	stvx	29,12,0	# save v29\n"
	ENTRY_POINT(_savev30)
"	addi	12,0,-32\n"
"	stvx	30,12,0	# save v30\n"
	ENTRY_POINT(_savev31)
"	addi	12,0,-16\n"
"	stvx	31,12,0	# save v31\n"
"	blr			# return to prologue\n"
	FUNC_END(_savev20)

	FUNC_START(_restv20)
"	addi	12,0,-192\n"
"	lvx	20,12,0	# restore v20\n"
	ENTRY_POINT(_restv21)
"	addi	12,0,-176\n"
"	lvx	21,12,0	# restore v21\n"
	ENTRY_POINT(_restv22)
"	addi	12,0,-160\n"
"	lvx	22,12,0	# restore v22\n"
	ENTRY_POINT(_restv23)
"	addi	12,0,-144\n"
"	lvx	23,12,0	# restore v23\n"
	ENTRY_POINT(_restv24)
"	addi	12,0,-128\n"
"	lvx	24,12,0	# restore v24\n"
	ENTRY_POINT(_restv25)
"	addi	12,0,-112\n"
"	lvx	25,12,0	# restore v25\n"
	ENTRY_POINT(_restv26)
"	addi	12,0,-96\n"
"	lvx	26,12,0	# restore v26\n"
	ENTRY_POINT(_restv27)
"	addi	12,0,-80\n"
"	lvx	27,12,0	# restore v27\n"
	ENTRY_POINT(_restv28)
"	addi	12,0,-64\n"
"	lvx	28,12,0	# restore v28\n"
	ENTRY_POINT(_restv29)
"	addi	12,0,-48\n"
"	lvx	29,12,0	# restore v29\n"
	ENTRY_POINT(_restv30)
"	addi	12,0,-32\n"
"	lvx	30,12,0	# restore v30\n"
	ENTRY_POINT(_restv31)
"	addi	12,0,-16\n"
"	lvx	31,12,0	# restore v31\n"
"	blr		# return to prologue\n"
	FUNC_END(_restv20)
);
#endif

#if TIME

class ShowTime
{
	public:
		~ShowTime();
};

ShowTime showtime;

ShowTime::~ShowTime()
{
	printf("rectfill: num %u size %lu time 0x%lx%08lx avg size %f avg time %f\n",
			num_rectfills, rectfill_size, long(rectfill_time >> 32), long(rectfill_time),
			double(rectfill_size) / num_rectfills,
			double(rectfill_time) / double(rectfill_size));

	printf("blit: num %u size %lu time 0x%lx%08lx avg size %f avg time %f\n",
			num_blits, blit_size, long(blit_time >> 32), long(blit_time),
			double(blit_size) / num_blits,
			double(blit_time) / double(blit_size));

//	printf("hline_alpha: num %u size %lu time 0x%lx%08lx avg size %f avg time %f\n",
//			num_hline_alpha, hline_alpha_size, long(hline_alpha_time >> 32), long(hline_alpha_time),
//			double(hline_alpha_size) / num_hline_alpha,
//			double(hline_alpha_time) / double(hline_alpha_size));
}

//__asm__(".section \".ctdt\",\"a\"");
//__asm__(".long print_time");
//__asm__(".long -108");

#endif


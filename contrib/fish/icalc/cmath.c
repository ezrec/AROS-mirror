/*
*	Math routines for complex-number expression parser.
*	In the routines, rv is variable holding 'return value'.
*
*	MWS, March 17, 1991.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "complex.h"
/*#include <libraries/mathieeedp.h>*/

/*extern struct Library * MathIeeeDoubTransBase;*/

extern const Complex zero, eye;

const Complex	one = {1.0, 0.0},
		minuseye = {0.0, -1.0};

int precision = 12;		/* number of decimal places to print */
				/* (when they exist) */

void cprin(fp, prefix, suffix, z)	/* print a complex number to file fp */
	FILE *fp;
	char *prefix, *suffix;
	Complex z;
{
	fputs(prefix, fp);

	if (z.imag == 0.0)
		fprintf(fp, "%.*g", precision, z.real);
	else if (z.real == 0.0)
		fprintf(fp, "%.*g i", precision, z.imag);
	else
		fprintf(fp, "%.*g %c %.*g i",
			precision, z.real, sign(z.imag), precision, fabs(z.imag));

	fputs(suffix, fp);
}

double Re(z)		/* real part of complex number */
	Complex z;
{
	return z.real;
}

double Im(z)		/* imaginary part of complex number */
	Complex z;
{
	return z.imag;
}

#define marg(z) atan((z).imag / (z).real)	/* macro arg */

double arg(z)		/* argument of complex number, in range (-PI,PI] */
	Complex z;
{
	return atan(z.imag / z.real);
}

double norm(z)		/* norm of a complex number */
	Complex z;
{
	return sqr(z.real) + sqr(z.imag);
}

double _cabs(z)		/* absolute value of a complex number */
	Complex z;
{
	return sqrt(norm(z));
}

Complex cadd(w,z)	/* complex addition */
	Complex w,z;
{
	Complex rv;

	rv.real = w.real + z.real;
	rv.imag = w.imag + z.imag;

	return rv;
}

Complex csub(w,z)	/* complex subtraction */
	Complex w,z;
{
	Complex rv;

	rv.real = w.real - z.real;
	rv.imag = w.imag - z.imag;

	return rv;
}

Complex cmul(w,z)	/* complex multiplication */
	Complex w,z;
{
	Complex rv;

	rv.real = w.real*z.real - w.imag*z.imag;
	rv.imag = w.real*z.imag + w.imag*z.real;

	return rv;
}

Complex cdiv(w,z)	/* complex division */
	Complex w,z;
{
	Complex rv;
	double temp = sqr(z.real)+sqr(z.imag);

	if (temp == 0.0)
		execerror("division by zero", NULL);	

	rv.real = (w.real*z.real + w.imag*z.imag)/temp;
	rv.imag = (w.imag*z.real - w.real*z.imag)/temp;

	return rv;
}

Complex cneg(z)		/* complex negation */
	Complex z;
{
	z.real = -z.real;
	z.imag = -z.imag;

	return z;
}

Complex csqr(z)		/* complex square, w^2 */
	Complex z;
{
	Complex rv;

	if (z.imag == 0.0)
	{
		z.real *= z.real;
		return z;
	}
	rv.real = sqr(z.real) - sqr(z.imag);
	rv.imag = 2*z.real*z.imag;

	return rv;
}

Complex csqrt(z)	/* complex square-root */
	Complex z;
{
	Complex rv;
	double temp = _cabs(z);

	rv.real = Sqrt((temp + z.real)*0.5);
	rv.imag = Sqrt((temp - z.real)*0.5);

	return rv;
}

Complex conj(z)		/* conjugate of w */
	Complex z;
{
	z.imag = -z.imag;

	return z;
}

Complex cexp(z)		/* complex exponential function e^z */
	Complex z;
{
	double temp = exp(z.real);

	if (z.imag == 0.0)
		z.real = temp;
	else
	{
		z.real = temp*cos(z.imag);
		z.imag = temp*sin(z.imag);
	}
	return z;
}

Complex clog(z)		/* complex natural logarithm */
	Complex z;
{
	Complex rv;

	rv.real = Log(norm(z))*0.5;
	rv.imag = marg(z);

	return rv;
}

Complex cpow(w,z)	/* complex exponential, w^z */
	Complex w,z;
{
	return cexp(cmul(z,clog(w)));
}

Complex csin(z)		/* complex sine */
	Complex z;
{
	if (z.imag == 0.0)
	{
		z.real = sin(z.real);
		return z;
	}
	else
	{
		Complex rv;

		rv.real = sin(z.real)*cosh(z.imag);
		rv.imag = cos(z.real)*sinh(z.imag);

		return rv;
	}
}

Complex ccos(z)		/* complex cosine */
	Complex z;
{
	if (z.imag == 0.0)
	{
		z.real = cos(z.real);
		return z;
	}
	else
	{
		Complex rv;

		rv.real = cos(z.real)*cosh(z.imag);
		rv.imag = -(sin(z.real)*sinh(z.imag));

		return rv;
	}
}

Complex ctan(z)		/* complex tangent */
	Complex z;
{
	if (z.imag == 0.0)
	{
	        z.real = tan(z.real);
		return z;
	}
	else
		return cdiv(csin(z),ccos(z));
}

Complex casin(z)	/* complex arcsine - lazy version */
	Complex z;
{
	/* asin z = -ilog(iz + sqrt(1-z^2)) */

	if (abs(z.real) <= 1.0 && z.imag == 0.0)
	{
		z.real = Asin(z.real);
		return z;
	}
	else
		return cmul(minuseye,clog(cadd(cmul(eye,z),csqrt(csub(one,csqr(z))))));
}

Complex cacos(z)	/* complex arccosine - lazy version */
	Complex z;
{
	/* acos z = -ilog(z + sqrt(z^2-1)) */

	if (abs(z.real) <= 1.0 && z.imag == 0.0)
	{
		z.real = Acos(z.real);
		return z;
	}
	else
		return cmul(minuseye,clog(cadd(z,csqrt(csub(csqr(z),one)))));
}

Complex catan(z)	/* complex arctangent - not so lazy version */
	Complex z;
{
	if (z.imag == 0.0)
		z.real = atan(z.real);
	else
	{
		Complex ctemp;
		double temp = norm(z);

		ctemp.real = ctemp.imag = 1.0 / (1.0 + temp - 2.0 * z.imag);
		ctemp.real *= 1.0 - temp;
		ctemp.imag *= -2.0*z.real;
		ctemp = clog(ctemp);
		z.real = -0.5*ctemp.imag;
		z.imag = 0.5*ctemp.real;
	}

	return z;
}

Complex csinh(z)	/* complex hyperbolic sine */
	Complex z;
{
	Complex rv;

	rv.real = cos(z.imag)*sinh(z.real);
	rv.imag = sin(z.imag)*cosh(z.real);

	return rv;
}

Complex ccosh(z)		/* complex hyperbolic cosine */
	Complex z;
{
	Complex rv;

	rv.real = cos(z.imag)*cosh(z.real);
	rv.imag = sin(z.imag)*sinh(z.real);

	return rv;
}

Complex ctanh(z)		/* complex tangent */
	Complex z;
{
	return cdiv(csinh(z),ccosh(z));
}

/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 *
 * Vector library for BREXX
 * Each vector is defined as a tree number string  v=x y z,
 *  ie.  v=0 0 0, is the origin
 * Call first the Vec_Init to initialize some basic vectors
 * Array and vector index are ZERO based
 */
say "Vector library file."
exit

/* --- Initialise Vectors --- */
VecInit:
	VecO  = 0. 0. 0.
	VecXo = 1. 0. 0.
	VecYo = 0. 1. 0.
	VecZo = 0. 0. 1.
return

/* --- Return Vectors components --- */
Vec:	return word(arg(1),arg(2)+1)
VecX:	return word(arg(1),1)
VecY:	return word(arg(1),2)
VecZ:	return word(arg(1),3)

/* --- Useless function --- */
VecMake: return	arg(1) arg(2) arg(3)

/* --- Negate a Vector (-V) --- */
VecNeg: procedure
	parse arg x y z
return	(-x) (-y) (-z)

/* --- Dot product of 2 vectors (A.B) --- */
VecDot: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ax*Bx + Ay*By + Az*Bz

/* --- Length of a vector --- */
VecLen: procedure
	parse arg x y z
return	sqrt(x*x + y*y + z*z)

/* --- Normalise a vector --- */
VecNorm: procedure
	parse arg x y z
	invlen = 1/sqrt(x*x + y*y + z*z)
return	invlen*x   invlen*y   invlen*z

/* --- Random Vector --- */
VecRnd: procedure
	max = 32000
	z = 2*(random(0,max)/max-0.5)
	atr = sqrt(1-z*z)
	phi = 6.283185*random(0,max)/max
	x = atr * cos(phi)
	y = atr * sin(phi)
return x y z

/* --- Add two vectors (A+B) --- */
VecAdd: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ax+Bx    Ay+By    Az+Bz

/* --- Subtract two vectors (A-B) --- */
VecSub: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ax-Bx    Ay-By    Az-Bz

/* --- Combine two vectors (a*A-b*B) --- */
VecComb: procedure
	parse arg af, Ax Ay Az, bf, Bx By Bz
return	af*Ax+bf*Bx    af*Ay+bf*By    af*Az+bf*Bz

/* --- Scale a vector (s*A) --- */
VecS: procedure
	parse arg s, x y z
return	s*x   s*y   s*z

/* --- Scale and Add two vector (s*A+B) --- */
VecAddS: procedure
	parse arg s,  Ax Ay Az,  Bx By Bz
return	s*Ax+Bx    s*Ay+By    s*Az+Bz

/* --- Multiply two vector (A*B) --- */
VecMult: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ax*Bx    Ay*By    Az*Bz

/* --- Cross product of two vector (AxB) --- */
VecCross: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ay*Bz-Az*By    Ax*Bz-Az*Bx     Ax*By-Ay*Bx

/* --- Create a 4x4 transformation matrix (everything in a row) --- */
MatMake: return arg(1) 0 arg(2) 0 arg(3) 0 0 0 0 1

/* --- Return Matrix item --- */
Mat: return word(arg(1),arg(3)*4+arg(2)+1)

/* --- Set a Matrix item --- */
SetMat: procedure
	parse arg mat,j,i,val
	pos = i*4+j
	if pos>0	then before = subword(mat,1,pos)
			else before = ""
	after = subword(mat,pos+2)
return before val after

/* --- Unary Matrix --- */
MatUnary: procedure
	m = ""
	do j=0 to 3
		do i=0 to 3
			m = m (i==j)
		end
	end
return m

/* --- Zero Matrix --- */
MatZero: return copies("0 ",16)

/* --- Vectror multiplied with the transformation vatrix --- */
VecXMat: procedure
	parse arg x y z,m
	nx = x*Mat(m,0,0) + y*Mat(m,1,0) + z*Mat(m,2,0) + Mat(m,3,0)
	ny = x*Mat(m,0,1) + y*Mat(m,1,1) + z*Mat(m,2,1) + Mat(m,3,1)
	nz = x*Mat(m,0,2) + y*Mat(m,1,2) + z*Mat(m,2,2) + Mat(m,3,2)
return nx ny nz

/* --- Matrix Transform --- */
MatT: procedure
	parse arg m
	mt = ""
	do j=0 to 3
		do i=0 to 3
			mt = mt Mat(m,j,i)
		end
	end
return mt

/* --- Create a Rotation Matrix, axis can be "x y z" and angle in radian --- */
MatRot: procedure
	parse arg axis,angle
	upper axis
	ax = wordpos(axis,"X Y Z")
	if axis=0 then do
		say "ERROR MatRot: Invalid Axis" axis
		return MatUnary()
	end
 
	mat = MatUnary()
        m1 = (ax//3)+1
        m2 = m1//3
	m1 = m1 - 1
        c = cos(angle)
        s = sin(angle)

	mat = SetMat(mat,m1,m1,c)
	mat = SetMat(mat,m2,m2,c)
	mat = SetMat(mat,m1,m2,s)
	mat = SetMat(mat,m2,m1,-s)
return mat

/* --- MatMult --- */
MatMult: procedure
	parse arg a,b
	m = ""
	do i=0 to 3
		do j=0 to 3
			sum = 0
			do k=0 to 3
				sum = sum + Mat(a,i,k)*Mat(b,k,j)
			end
			m = m sum
		end
	end
return m

/* --- Print Matrix --- */
MatPrint: procedure
	parse arg m
	do j=0 to 3
		line = ""
		do i=0 to 3
			line = line Mat(m,i,j)
		end
		say line
	end
return

/* --- MatInv --- */
MatInv: procedure
	mat = _ludcmp(arg(1))	/* matrix lu decomposition */
 	y = ""
	do j=0 to 3		/* matrix inversion */
		do i=0 to 3
			col.i = (j=i)
		end
		call _lubksb mat
		do i=0 to 3
			y = y col.i
		end
	end
return y

/****************************************************************
 * ludcmp(Mat4 a, int *indx, Flt *d)
 * LU decomposition.
 * Parameteres
 *      Mat4    a               input matrix. gets thrashed
 *      int     *indx           row permutation record
 *      Flt     *d              +/- 1.0 (even or odd # of row interchanges
 *****************************************************************/
_ludcmp: procedure expose d indx.
	parse arg a

	d = 1.0
	do i=0 to 3
		big = 0	
		do j=0 to 3
			big = max(abs(Mat(a,i,j)),big)
		end
		if big = 0 then do
			say "ERROR _ludcmp(): singular matrix found..."
			exit
		end
		vv.i = 1.0/big
	end
	do j=0 to 3
		do i=0 to j-1
			sum = Mat(a,i,j)
			do k=0 to i-1
				sum = sum - Mat(a,i,k) * Mat(a,k,j)
			end
			a = SetMat(a,i,j,sum)
		end
		big = 0
		do i=j to 3
			sum = Mat(a,i,j)
			do k=0 to j-1
				sum = sum - Mat(a,i,k) * Mat(a,k,j)
			end
			a = SetMat(a,i,j,sum)
			dum = vv.i*abs(sum)
			if dum >= big then do
				big = dum
				imax = i
			end
		end
		if j ^= imax then do
			do k=0 to 3
				dum = Mat(a,imax,k)
				a = SetMat(a,imax,k, Mat(a,j,k))
				a = SetMat(a,j,k, dum)
			end
			d = -d
			vv.imax = vv.j
		end
		indx.j = imax
		if Mat(a,j,j) = 0.0 then a = SetMat(a,j,j, 1E-20)
		if j ^= 3 then do
			dum = 1/Mat(a,j,j)
			do i=j+1 to 3
				a = SetMat(a,i,j, Mat(a,i,j)*dum)
			end
		end
	end
return a

/****************************************************************
 * lubksb(Mat4 a, int *indx, Flt b[])
 * backward substitution
 *      Mat4    a       input matrix
 *      int     *indx   row permutation record
 *      Flt     col     right hand vector (?)
 *****************************************************************/
_lubksb: procedure expose col. indx.
	parse arg a

	ii = -1
 
	do i=0 to 3
		ip = indx.i
		sum = col.ip
		col.ip = col.i
		if ii >= 0 then
			do j=ii to i-1
				sum = sum - Mat(a,i,j) * col.j
			end
		else
		if sum ^= 0.0 then ii = i
		col.i = sum
	end
	do i=3 to 0 by -1
		sum = col.i
		do j=i+1 to 3
			sum = sum - Mat(a,i,j) * col.j
		end
		col.i = sum/Mat(a,i,i)
	end
return

/* */
if arg(1) = "?" then do 
	say "syntax: calc [<expr>]"
	say "desc:   Scientific calculator, command line or input mode"
	say "examples: sqrt(sin(0.2*pi)*pow10(5)+1)"
	say "        x = 5.1"
	say "        2*x/pi"
	say "The special variable '$' hold the result of the last expression"
	say "author: Bill N. Vlachoudis"
	say "date:   Apr 1996"
	exit
end

trace o
signal on error
signal on syntax
signal on halt

pi = 3.1415927
mp = 1.66054E-27
mn = 1.008664904*mp

if arg() > 0 then do
	interpret "say" arg(1)
	exit
end

rad=1

mainloop:
do forever
	pull line
	line = space(line,0)
	if line="END" | line="QUIT" then exit

	if word(line,1) = "SET" then do
		parse var line . opt value
		select
			when opt = "RAD" then rad=1
			when opt = "DEG" then deg=1
		end
		iterate
	end
	parse var line front "=" rest
	if rest <> "" then do
		if front="" then signal error
		if pos("(",front) = 0 then do
			interpret line
			$ = value(front)
			say front "=" $
		end; else do
			parse var front name "(" vars ")" .
			interpret name "= rest"
			say name"("vars") = "rest
		end
	end; else do
		if line <> "" then interpret "$ =" line
		say ' =' $
	end
end
exit

error:
	say "+++ Error in expression +++"
	signal mainloop

syntax:
	say "+++ Error in expression +++"
	signal mainloop

halt:
	say "Ending program..."
	exit

/* -------- solve -------- */
solve:
	parse arg _func, _low, _high, _eps
	if _eps="" then _eps=1E-10
	if _func="" | _low="" | _high="" then do
		say "solve(function,low,high[,eps])"
		return 0
	end

	x = _low
	interpret "_yl="_func
	_sl = sign(_yl)
	if ABS(_yl)<_eps then return _low

	x = _high
	interpret "_yh="_func
	_sh = sign(_yh)
	if ABS(_yh)<_eps then return _high

	do forever
		if _sl=_sh then do
			say  "NO SOLUTION: Both values are from the same side"
			return 0
		end
		x = (_low+_high)/2
		interpret "_ym="_func
		_sm = sign(_ym)
		if ABS(_ym)<_eps then return x

		if _sl=_sm then do
			_low = x; _yl = _ym
		end; else do
			_high = x; _yh = _ym
		end
	end
return

d2r: return arg(1)/180*PI
r2d: return arg(1)/PI*180

sind: return sin(d2r(arg(1)))
cosd: return cos(d2r(arg(1)))
tand: return tan(d2r(arg(1)))

asind: return r2d(asin(arg(1)))
acosd: return r2d(acos(arg(1)))
atand: return r2d(atan(arg(1)))

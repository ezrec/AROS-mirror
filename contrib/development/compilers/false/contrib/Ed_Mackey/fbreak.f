{ fbreak.f -- FALSE Breakout by Ed Mackey. }

{ This program needs RAW i/o.  Under Workbench 2.0 or higher, type:
  fbreak <>RAW:0/0/680/400/FBreak
  Modify the dimentions as needed to fit the game in the window.

  Workbench 1.3 users should make their CLI window as big as
  possible (prefferebly with a bit of overscan; this game needs
  a full 80 columns!), and then type the following:
  fbreak <RAW:
  This will cause the output of the game to show in your CLI
  window, but the user input will come from a separate window.
  Make the separate window small and get it out of your way, but
  keep it the "active" window so that fbreak can read from it.
}

{ keys during the game: 
	SPACE        - release ball
	LEFT (or D)  - move paddle left
	RIGHT (or C) - move paddle right
	SHIFT-Q      - abort game.
}

ß

{ variables }
{ s = Score, p = PaddleX, x = BallX, y = BallY, h = horizontalC, v = verticalC }
{ b = brick array, j = horizDir, k = vertDir, m = hcReset, n = vcReset, r = bricksRemaining }
{ Reserved for functions: w, c, g. }
{ Temp variable local to CheckBrick(): t }

{ BOOL GotOne = WaitForChar(int timeout) }
[8709`9245`20142`65332`11008`]w:

{ void CheckPaddle(int newx, int newy) }
[
	{ if } 22= { then }
	[
		$+ p;$+9+ -	{x relative to paddle}
		{ if } $$10_> \9>~ & { then }
		[
			1_k:	{ball bounces up}
			1j:	{and to the right}
			{ if } $0>~ { then }
			[
				1_j:	{er, make that the left}
				_	{and make rel-x positive}
			]? {end if}
			1$h:v:		{get moving}
			5+$n:15\-m:	{new speeds}
			0
		]? {end if}
		%
	]? {end if}
]g:

{ BOOL CheckBrick(int newx, int newy)   Returns TRUE for collision. }
[
	{ if } $1= 2ø0=| 2ø81=|~ { then }	{if ball is in bounds,}
	$[
		@@
		0t:
		{ if } $2> 1ø7>~& { then }
		[
			$3- $+$+ b;+$;	{ Read row of bricks & pointer }

			{ stack: truth, x, y, pointer, brickmap }

			3ø1-5/@@2ø	{ brick x }
			1
			{ while }[1ø][$+\1-\]#\% {end while}

			{ stack: truth, x, y, brick x, pointer, brickmap, mask }

			{ if } $2ø& { then }  {collision here?}
			[
				1t:
				s;1+$s: 27,91,"1;12H".	{ Print new score }
				27,91,4ø.";"3ø5*1+."H     "	{ Erase brick from screen }
				r;1-r:		{ Decrease onscreen brick counter }
			]? {end if}

			~&\:%	{ delete brick's bit if needed }
		]? {end if}
		%%t;
		\
	]? { else } ~[
		%%1_
	]? {end if}
]c:

{ MAIN PROGRAM }

{ clear screen & print header }
27,"[H"27,"[J    Score: 0                  FBreak by Ed Mackey."

1_$$$$  { init five rows of bricks, 1 bit each.  Only low 16 bits used. }
11021`b:  { move.l a5,-(a5).  Gets a pointer to the rows. }

0s:	{ Score starts at 0 }
0r:	{ No bricks on screen yet }
36p:	{ Paddle starts in middle }
1h:1v:  { horiz & vert counters of ball }
1j:1_k:	{ horiz & vert directions of ball }
10$m:n:	{ horiz & vert speed of ball (counter's underflow reset values) }

{ Print the blocks on the screen }
0
{ while }[$ 4 >~]
[
	27,91, $3+. ";1H"	{ Move cursor to start of row }

	$ $+$+ b;+;	{ Read row of bricks }

	1
	{ while }[$ 65535 >~]
	[
		{ if } $ 2ø & {then}0\[~]?
		$[
			"<"27,91,"7m==="27,91,"0m>"
			r;1+r:	{ Increase number of onscreen bricks }
		]? {else} ~[
			"     "
		]? {end if}

		$+	{ rotate left 1 bit }
	]#% {end while}

	%  {Don't need row of bricks}

	1+
]#% {end while}

{ Render the paddle }
27,91,"22;"p;."H<======>"
27,91,"21;"p;4+."H*"
27,91,"H"ß

{ Initial aiming phase }
{ while }[^$$$1_=~@'Q=~&@32=~&]    {char is not EOF, Shift-Q, or space}
[
	{ if } $'D= p;1> & { then }
	[
		p;1-p:	{move paddle left}
		27,91,"22;"p;."H<======> "
		27,91,"21;"p;4+."H* "
		27,91,"H"ß
	]? {end if}

	{ if } 'C= 73p;> & { then }
	[
		27,91,"22;"p;."H <======>"
		27,91,"21;"p;4+."H *"
		27,91,"H"ß
		p;1+p:	{move paddle right}
	]? {end if}
]# {end while}

21y:p;4+x:	{ ball's initial X and Y }

{ Main game, ball in flight }
{ while }[0y;22>~r;0>&[%25000w;!$[^$$1_=~@'Q=~&@]?~[0ß1_]?]?]
[
	{ if } $155= {then} [%^]? {end if}

	0	{ don't send cursor home }

	{ if } 1ø'D= p;1> & { then }
	[
		p;1-p:	{move paddle left}
		27,91,"22;"p;."H<======> "
		{ if } y;22= x;p;1-= & { then }
		[
			1_k:	{ball bounces up}
			1$h:v:	{get moving}
			2m:15n:	{new speeds}
		]? {end if}
		%1	{send cursor home}
	]? {end if}

	{ if } \'C= 73p;> & { then }
	[
		27,91,"22;"p;."H <======>"
		p;1+p:	{move paddle right}
		{ if } y;22= x;p;8+= & { then }
		[
			1_k:	{ball bounces up}
			1$h:v:	{get moving}
			2m:15n:	{new speeds}
		]? {end if}
		%1	{send cursor home}
	]? {end if}

	{ if } 1h;1-$h:> { then }	{ if (1 > --h) ball moves horizontally }
	[
		m;h:

		x;j;+ y; g;!	{check if it would hit the paddle}

		{ while } [x;j;+ y; c;!]
		[
			j;_j:
		]# {end while}

		{ if } j;0> { then }
		$[
			27,91,y;.";"x;."H *"
			x;1+ x:
		]? { else } ~[
			x;1- x:
			27,91,y;.";"x;."H* "
		]? {end if}
		%1	{send cursor home}
	]? {end if}

	{ if } 1v;1-$v:> { then }	{ if (1 > --v) ball moves vertically }
	[
		n;v:

		x; y;k;+ g;!	{check if it would hit the paddle}

		{ while } [x; y;k;+ c;!]
		[
			k;_k:
		]# {end while}

		{ if } k;0> { then }
		$[
			y;1+ y:
			27,91,y;.";"x;."H*"8,11," "
		]? { else } ~[
			27,91,y;.";"x;."H "8,11,"*"
			y;1- y:
		]? {end if}
		%1	{send cursor home}
	]? {end if}

	{ if cursor needs to be sent home, then }
	[
		27,91,"H"
	]? {end if}
]# {end while}

{ Game Over message }
27,91,"10;30H ******************* "
{ if } r;0>$ { then }
[
	27,91,"11;30H **   GAME OVER   ** "
]? { else } ~[
	27,91,"11;30H **   YOU WIN!!   ** "
]? {end if}
27,91,"12;30H ** Press Shift-Q ** "
27,91,"13;30H ******************* "
27,91,"H"ß[^$1_=~\'Q=~&][]#ß27,"[H"27,"[J"ß

{ END of fbreak.f }

/*
*	header for complex-number expression parser.
*	MWS, March 17, 1991.
*/

#ifndef NULL
#define NULL (0L)
#endif

typedef struct complex {	/* our complex-number representation */
	double	real,
		imag;
} Complex;

typedef struct symlist {	/* parameter-list for a user-function */
	struct symbol	*sym;
	struct symlist	*next;
} SymList;

typedef struct userfunction {
	struct remember	*remkey;/* stores memory allocation list of tree */
	struct symbol *param;	/* parameter to function */
	struct node *tree;	/* the 'body' of the function */
} UserFunc;

typedef struct symbol {		/* general symbol - numerous types */
	char	*name;
	short	type;		/* VAR, CONST, BLTIN */
	union {
		Complex	val;		/* if VAR or CONST */
		Complex	(*cptr)();	/* C_BLTIN (function) */
		double	(*rptr)();	/* R_BLTIN (function) */
		void	(*vptr)();	/* COMMAND */
		UserFunc ufunc;		/* USER FUNCTION */
	} u;
	struct Symbol	*left, *right;	/* children */
} Symbol;


typedef union {
	Complex	val;		/* a complex number */
	Symbol	*sym;		/* a symbol */
} Contents;

typedef struct node {
	int	type;			/* type of node */
	Contents contents;		/* contents of node */
	struct node *left, *right;	/* children */
} Node;

/*
*	Convention: if node type is a builtin or unary operator,
*	the left child will hold the expression that is the
*	argument to the function.
*/

#define	sqr(x)	(x)*(x)
#define sign(x)	((x) >= 0.0 ? '+' : '-')

/* Lattice-generated prototypes (some of them) */
/* Prototypes for functions defined in cmath.c */
Complex cadd(Complex w, Complex z);
Complex csub(Complex w, Complex z);
Complex cmul(Complex w, Complex z);
Complex cdiv(Complex w, Complex z);
Complex cneg(Complex z);
Complex csqr(Complex z);
Complex csqrt(Complex z);
Complex conj(Complex z);
Complex cexp(Complex z);
Complex clog(Complex z);
Complex cpow(Complex w, Complex z);
Complex csin(Complex z);
Complex ccos(Complex z);
Complex ctan(Complex z);
Complex casin(Complex z);
Complex cacos(Complex z);
Complex catan(Complex z);
Complex csinh(Complex z);
Complex ccosh(Complex z);
Complex ctanh(Complex z);

double Re(Complex z), Im(Complex z);
double arg(Complex z);
double norm(Complex z);
double _cabs(Complex z);

/* Prototypes for functions defined in math.c */
double	Sqrt(double),
	Log(double),
	Asin(double),
	Acos(double),
	errcheck(double, char *);

/* Prototypes for functions defined in complex.y */
int	yylex(void);
void	warning(char *, char *),
	yyerror(char *),
	execerror(char *, char *),
	welcome(void),
	prompt(void),
	main(int, char **);

/* Prototypes for functions defined in symbol.c */
Symbol *lookup(char *s);
Symbol *allocsym(char *s, int t);
Symbol *install(char *s, int t, Complex cval);
void printlist(int type);

/* Prototypes for functions defined in init.c */
void init(void);

/* Prototypes for functions defined in command.c */
void	besilent(),
	beverbose(),
	quiticalc(),
	builtins(void),
	userfuncs(void),
	consts(void),
	vars(void),
	help(void);

/* Prototypes for functions defined in tree.c */
Node *n_asgn(Symbol *sym, Node *arg);
Node *n_binop(int op, Node *left, Node *right);
Node *n_unop(int op, Node *arg); 
Node *n_func(int type, Symbol *sym, Node *arg);
Node *n_symbol(int type, Symbol *sym);
Node *n_number(double real, double imag);
Complex eval_tree(Node *n);
void delete_tree(Node *n);

/* Prototypes for functions defined in function.c */
void clear_ufunc(UserFunc *func);

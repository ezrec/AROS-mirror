
/********************************************************************

	Lowlevel Amiga dos.library module.

-----------------------------------------------
	©Irmen de Jong.

	History:

	 9-jun-96   Created.
	13-jun-96   Added WaitSignal().
	17-nov-96   Fixed empty template handling.
	31-dec-96   Changed CheckSignal to look like WaitSignal.
	18-jan-98   Updated for Python 1.5
	27-sep-98	Added some extra functions.
	12-nov-98	Renamed `doslib' to `Doslib'
	1-oct-2000	Added raw console functions

Module members:

	error       -- Exeption string object.  ('Doslib.error')
	ReadArgs    -- dos.library/ReadArgs function.
				   result=Doslib.ReadArgs(template,args,types)

	WaitSignal  -- generic Wait() function, like select. Waits for certain
				   signals to occur.
				   (sigs,objlist) = WaitSignal(args)
				   args = integer, object, or list of ints/objects.
				   integers are sigmask values. Objects must have 'signal'
				   attribute to get this value.

	CheckSignal -- Like Wait() but DOES NOT WAIT FOR THE SIGNALS:
				   merely checks if certain signals are set.

	CompareDates, DateStamp, DateToStr, StrToDate, Fault,
	IoErr, SetIoErr, IsFileSystem, Relabel, SetProtection,
	SetComment, Examine, Inhibit, SetFileDate, SetOwner, Info,
	GetProgramDir, GetProgramName: 
		just like the original dos.library calls.

	DS2time     -- convert DateStamp tuple to time() value (see time module)
	time2DS     -- convert time() value to DateStamp tuple (see time module)


**************************************************************************/

#include <stdlib.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/datetime.h>
#include <dos/rdargs.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include "Python.h"

#ifdef AROS
#define __aligned 
#endif

static PyObject *error;    // Exception


/* Convenience; sets error with IoError() string, return NULL */
/* If ok, return Py_NONE */
static PyObject *check_ok(BOOL ok)
{
	char buf[100];
	if(ok)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	Fault(IoErr(),NULL,buf,100);
	PyErr_SetString(error,buf);
	return NULL;
}


/************** MODULE FUNCTIONS *******************/

static PyObject *
Doslib_ReadArgs(PyObject *self, PyObject *arg)
{
	char *template;
	char *args;
	PyObject *types;        // the keywords & their types (tuple of tuples)

	PyObject *result=NULL;

	LONG *argarray;
	int num_args;
	BOOL free_args=FALSE;
	
	if (!PyArg_ParseTuple(arg, "ssO!", &template, &args,
		&PyTuple_Type,&types))
		return NULL;

	if(PyTuple_Size(types)>0 || strlen(template)>0)
	{
		/* check argument count */
		char *t=template;
		num_args=1;
		while(*t)
		{
			if(*t++ == ',') num_args++;
		}
		if(num_args!=PyTuple_Size(types))
		{
			PyErr_SetString(PyExc_ValueError,"types length does not match template");
			return NULL;
		}
	}
	else
	{
		/* template is empty string; no arguments expected */
		if(strlen(stpblk(args))>0) 
		{
			SetIoErr(ERROR_TOO_MANY_ARGS);
			return check_ok(FALSE);
		}
		return PyDict_New();        // empty result dictionary
	}

	if(argarray=calloc(sizeof(LONG),num_args))
	{
		struct RDArgs *rdargs;
		int arglen=strlen(args);

		if((arglen==0) || (args[arglen-1]!='\n'))
		{
			char *t = malloc(arglen+2);
			if(t)
			{
				strcpy(t,args); t[arglen++]='\n'; t[arglen]=0;  // add \n !!!
				args=t; free_args=TRUE;
			}
			else
			{
				free(argarray);
				return PyErr_NoMemory();
			}
		}

		if(rdargs = AllocDosObject(DOS_RDARGS, NULL))
		{
			rdargs->RDA_Flags = RDAF_NOPROMPT;
			rdargs->RDA_Source.CS_Buffer = args;
			rdargs->RDA_Source.CS_Length = arglen;
			rdargs->RDA_Source.CS_CurChr = 0;
			rdargs->RDA_DAList = NULL;
			rdargs->RDA_Buffer = NULL;

			if( !ReadArgs(template, argarray, rdargs) )
			{
				(void)check_ok(FALSE);
			}
			else
			{
				if(result=PyDict_New())
				{
					/************
					Traverse the types tuple and for each argument,
					extract it's value. types consists of tuples (k,t) where:
					k=keyword;
					t=the keyword's type:
						'X'     - string
						'S','T' - bool (integer)   !!! T not yet supported 
						'N'     - integer
						'A'     - array of strings
						'I'     - array of ints
					***********/

					PyObject *tup, *val;
					BOOL result_ok=TRUE;
					int cnt=0;
					while (result_ok && (tup=PyTuple_GetItem(types, cnt)))
					{
						PyObject *keyword, *type;
						char *type_c;

						if(PyTuple_Size(tup)!=2)
						{
							PyErr_SetString(PyExc_ValueError,"types tuple invalid");
							result_ok=FALSE; break;
						}

						keyword=PyTuple_GetItem(tup,0);
						type=PyTuple_GetItem(tup,1);

						result_ok=FALSE;
						type_c=PyString_AsString(type);
						if(type_c==NULL)
						{
							result_ok=FALSE; break; /* no string!? */
						}

						switch (type_c[0]) {
						case 'S':       // boolean switch
							if(val=PyInt_FromLong(argarray[cnt]))
							{
								result_ok=(0==PyDict_SetItem(result,keyword,val));
								Py_DECREF(val);
							}
							break;
						case 'N':       // number
							if(argarray[cnt])
							{
								if(val=PyInt_FromLong(*(LONG*)argarray[cnt]))
								{
									result_ok=(0==PyDict_SetItem(result,keyword,val));
									Py_DECREF(val);
								}
							}
							else result_ok=(0==PyDict_SetItem(result,keyword,Py_None));
							break;
						case 'X':       // string
							if(argarray[cnt])
							{
								if(val=PyString_FromString((STRPTR)argarray[cnt]))
								{
									result_ok=(0==PyDict_SetItem(result,keyword,val));
									Py_DECREF(val);
								}
							}
							else result_ok=(0==PyDict_SetItem(result,keyword,Py_None));
							break;
						case 'A':       // array of strings
						case 'I':       // array of numbers
							if(val=PyList_New(0))
							{
								if(argarray[cnt])
								{
									if(type_c[0]=='A')
									{
										char **str = (char**)argarray[cnt];
										PyObject *so;
										/* build the string list */
										while(*str)
										{
											if(so=PyString_FromString(*str))
											{
												if(!(result_ok=(0==PyList_Append(val,so))))
													break;
												Py_DECREF(so);
											}
											else break;
											str++;
										}
									}
									else /* array of numbers */
									{
										int **ia = (int**)argarray[cnt];
										PyObject *io;
										/* build the integer list */
										while(*ia)
										{
											if(io=PyInt_FromLong(**ia))
											{
												if(!(result_ok=(0==PyList_Append(val,io))))
													break;
												Py_DECREF(io);
											}
											else break;
											ia++;
										}
									}
									result_ok=result_ok&&(0==PyDict_SetItem(result,keyword,val));
								}
								else
								{
									// insert the empty list.
									result_ok=(0==PyDict_SetItem(result,keyword,val));
								}
								Py_DECREF(val);
							}
							break;
						default:
							PyErr_SetString(PyExc_ValueError,"illegal arg type");
							break;
						}
						cnt++;
					}
					if(!result_ok)
					{
						Py_DECREF(result);
						result=0;
					}
				}
			}

			FreeArgs(rdargs);
			FreeDosObject( DOS_RDARGS, rdargs );
		}
		else (void)PyErr_NoMemory();

		if(free_args) free(args);
		free(argarray);
	}
	else (void)PyErr_NoMemory();

	return (PyObject*)result;
}


static PyObject *
CheckOrWaitSignal(PyObject *self, PyObject *arg, BOOL doWait)
{
	struct {
		PyObject *ob;       // the object, NULL means: only sigmask value
		ULONG sigmask;
	} objs[32];
	int i,listsize = 0;
	PyObject *argo, *result;
	ULONG signal;

	if(!PyArg_ParseTuple(arg,"O",&argo)) return NULL;

	if(PyInt_Check(argo))
	{
		/* 1 int arg */
		signal=PyInt_AsLong(argo);
		objs[0].ob=NULL;
		objs[0].sigmask=signal;
		listsize++;
	}
	else if(PyList_Check(argo))
	{
		/* 1 list arg */
		int len=PyList_Size(argo);

		if(len==0 || len>32)
		{
			PyErr_SetString(PyExc_ValueError,"list size must be 1..32");
			return NULL;
		}

		signal = 0;

		for(i=0;i<len;i++)
		{
			PyObject *attr;
			PyObject *item = PyList_GetItem(argo,i);
			ULONG s;

			if(PyInt_Check(item))
			{
				s = PyInt_AsLong(item);
				objs[listsize].ob = NULL;
				objs[listsize++].sigmask=s;
				signal |= s;
			}
			else if(attr = PyObject_GetAttrString(item,"signal"))
			{
				ULONG s = PyInt_AsLong(attr);

				if((s==-1) && PyErr_Occurred())
				{
					Py_DECREF(attr);
					return NULL;
				}
				
				signal |= s;
				objs[listsize].ob=item;
				objs[listsize++].sigmask=s;
			}
			else return (PyObject*)PyErr_BadArgument();
		}
	}
	else
	{
		/* other, try signal attribute */
		PyObject *attr;
		if(attr=PyObject_GetAttrString(argo,"signal"))
		{
			/* Found! */
			signal = PyInt_AsLong(attr);

			if((signal==-1) && PyErr_Occurred())
			{
				Py_DECREF(attr);
				return NULL;
			}

			objs[0].ob = attr;
			objs[0].sigmask = signal;
			listsize++;
		}
		else return NULL;
	}
	
	if(doWait)
		signal = Wait(signal);
	else
		signal = CheckSignal(signal);

	if(result=PyList_New(0))
	{
		PyObject *tup;

		for(i=0;i<listsize;i++)
		{
			if(objs[i].ob && (signal&objs[i].sigmask))
			{
				if(0!=PyList_Append(result,objs[i].ob))
				{
					Py_DECREF(result);
					return NULL;
				}
			}
		}

		tup=Py_BuildValue("(iO)",signal,result);
		Py_DECREF(result);
		return tup;
	}
	else return NULL;
}

static PyObject *
Doslib_WaitSignal(PyObject *self, PyObject *arg)
{
	return CheckOrWaitSignal(self,arg,TRUE);
}

static PyObject *
Doslib_CheckSignal(PyObject *self, PyObject *arg)
{
	return CheckOrWaitSignal(self,arg,FALSE);
}


static PyObject *
Doslib_DateToStr(PyObject *self, PyObject *arg)
{
	// (datestamp[,format=FORMAT_DOS,flags=0]) -> string

	struct DateTime dt;
	PyObject *ds_t;
	int flags=0,format=FORMAT_DOS;
	char day[32];
	char date[32];
	char time[32];

	if(!PyArg_ParseTuple(arg,"O!|ii",&PyTuple_Type,&ds_t,&format,&flags)) return NULL;

	if(!PyArg_ParseTuple(ds_t,"iii;invalid datestamp tuple",
		&dt.dat_Stamp.ds_Days,&dt.dat_Stamp.ds_Minute,&dt.dat_Stamp.ds_Tick)) return NULL;

	dt.dat_StrDay=day;
	dt.dat_StrDate=date;
	dt.dat_StrTime=time;
	dt.dat_Flags=flags;
	dt.dat_Format=format;
	DateToStr(&dt);
	return Py_BuildValue("(sss)",dt.dat_StrDay,dt.dat_StrDate,dt.dat_StrTime);  
}

static PyObject *
Doslib_StrToDate(PyObject *self, PyObject *arg)
{
	// (datestring[,timestring,format=FORMAT_DOS,flags=0]) -> datestamp

	struct DateTime dt;
	
	char *date;
	char *time=NULL;
	int flags=0,format=FORMAT_DOS;

	if(!PyArg_ParseTuple(arg,"s|sii",&date,&time,&format,&flags)) return NULL;

	memset(&dt,0,sizeof(dt));
	dt.dat_StrDate=date;
	dt.dat_StrTime=time;
	dt.dat_Flags=flags;
	dt.dat_Format=format;
	StrToDate(&dt);
	return Py_BuildValue("(iii)",dt.dat_Stamp.ds_Days,dt.dat_Stamp.ds_Minute,dt.dat_Stamp.ds_Tick);
}

static PyObject *
Doslib_CompareDates(PyObject *self, PyObject *arg)      // (ds1, ds2) -> <0,0,>0
{
	PyObject *ds_t1, *ds_t2;
	struct DateStamp ds1,ds2;

	if(!PyArg_ParseTuple(arg,"O!O!",
		&PyTuple_Type,&ds_t1,&PyTuple_Type,&ds_t2)) return NULL;

	if(!PyArg_ParseTuple(ds_t1,"iii;invalid datestamp tuple",&ds1.ds_Days,&ds1.ds_Minute,&ds1.ds_Tick)) return NULL;
	if(!PyArg_ParseTuple(ds_t2,"iii;invalid datestamp tuple",&ds2.ds_Days,&ds2.ds_Minute,&ds2.ds_Tick)) return NULL;

	return PyInt_FromLong(CompareDates(&ds1,&ds2)); 
}

static PyObject *
Doslib_DateStamp(PyObject *self, PyObject *arg)     // () -> datestamp
{
	struct DateStamp ds;
	if(!PyArg_NoArgs(arg)) return NULL;
	
	DateStamp(&ds);
	return Py_BuildValue("(iii)",ds.ds_Days,ds.ds_Minute,ds.ds_Tick);
}

static PyObject *
Doslib_Info(PyObject *self, PyObject *arg)     // (name) -> infoblock
{
	struct InfoData *_id;
	char *name;

	if(!PyArg_ParseTuple(arg,"s",&name)) return NULL;
	if(_id=AllocVec(sizeof(struct InfoData),MEMF_PUBLIC))
	{
		BPTR lock;
		if(lock=Lock(name, SHARED_LOCK))
		{
			if(Info(lock,_id))
			{
				struct InfoData id;
				id = *_id;
				FreeVec(_id);
				UnLock(lock);
				return Py_BuildValue("(iiiiiiii)", id.id_NumSoftErrors, id.id_UnitNumber,
					id.id_DiskState, id.id_NumBlocks, id.id_NumBlocksUsed, id.id_BytesPerBlock,
					id.id_DiskType, id.id_InUse);
			}
			else
			{
				(void) check_ok(FALSE);
			}
			FreeVec(_id);
			UnLock(lock);
		}
		else
		{
			(void) check_ok(FALSE);
		}
		return NULL;
	}
	return PyErr_NoMemory();
}

static PyObject *
Doslib_Fault(PyObject *self, PyObject *arg)         // (errnum[, header]) -> string
{
	int errnum;
	char *header=NULL;
	char buf[100];

	if(!PyArg_ParseTuple(arg,"i|s",&errnum,&header)) return NULL;
	Fault(errnum,header,buf,100);
	return PyString_FromString(buf);
}

static PyObject *
Doslib_GetProgramDir(PyObject *self, PyObject *arg) // () -> string
{
	BPTR lock;
	if(!PyArg_NoArgs(arg)) return NULL;
	
	if(lock=GetProgramDir())
	{
		char name[256];
		if(NameFromLock(lock, name, 256))
		{
			return PyString_FromString(name);
		}
		return check_ok(FALSE);
	}
	return PyString_FromString("");
}

static PyObject *
Doslib_GetProgramName(PyObject *self, PyObject *arg) // () -> string
{
	char name[256];
	if(!PyArg_NoArgs(arg)) return NULL;
	
	if(GetProgramName(name, 256))
	{
		return PyString_FromString(name);
	}
	return check_ok(FALSE);
}

static PyObject *
Doslib_IoErr(PyObject *self, PyObject *arg)         // () -> int
{
	if(!PyArg_NoArgs(arg)) return NULL;
	return PyInt_FromLong(IoErr());
}

static PyObject *
Doslib_Inhibit(PyObject *self, PyObject *arg) // (drive [,switch]) -> bool
{
	char *name;
	int swtch=1;

	if(!PyArg_ParseTuple(arg,"s|i",&name,&swtch)) return NULL;

	return check_ok(Inhibit(name,swtch));
}

static PyObject *
Doslib_SetIoErr(PyObject *self, PyObject *arg)      // (int) -> int
{
	int errnum;

	if(!PyArg_ParseTuple(arg,"i",&errnum)) return NULL;
	return PyInt_FromLong(SetIoErr(errnum));
}

static PyObject *
Doslib_IsFileSystem(PyObject *self, PyObject *arg)      // (string) -> bool
{
	char *name;

	if(!PyArg_ParseTuple(arg,"s",&name)) return NULL;
	return PyInt_FromLong(IsFileSystem(name));
}

static PyObject *
Doslib_Relabel(PyObject *self, PyObject *arg)   // (oldvolname, newvolname)
{
	char *old, *new;

	if(!PyArg_ParseTuple(arg,"ss",&old,&new)) return NULL;
	if(old[strlen(old)-1]!=':')
	{
		PyErr_SetString(PyExc_ValueError,"volume name must end with :");
		return NULL;
	}
	if(new[strlen(new)-1]==':')
	{
		PyErr_SetString(PyExc_ValueError,"new name must not end with :");
		return NULL;
	}
	return check_ok(Relabel(old,new));
}

static PyObject *
Doslib_SetProtection(PyObject *self, PyObject *arg) // (file,protbits) -> bool
{
	char *name;
	int pb;

	if(!PyArg_ParseTuple(arg,"si",&name,&pb)) return NULL;

	return check_ok(SetProtection(name,pb));
}

static PyObject *
Doslib_SetFileDate(PyObject *self, PyObject *arg) // (file,datestamp) -> bool
{
	struct DateStamp ds;
	PyObject *ds_t;
	char *name;

	if(!PyArg_ParseTuple(arg,"sO!",&name,&PyTuple_Type,&ds_t)) return NULL;

	if(!PyArg_ParseTuple(ds_t,"iii;invalid datestamp tuple",
		&ds.ds_Days,&ds.ds_Minute,&ds.ds_Tick)) return NULL;

	return check_ok(SetFileDate(name,&ds));
}

static PyObject *
Doslib_Examine(PyObject *self, PyObject *arg)       // (filename) -> fib structure
{
	char *name;
	BPTR lock;
	struct FileInfoBlock __aligned fib;

	if(!PyArg_ParseTuple(arg,"s",&name)) return NULL;
	if(lock=Lock(name,ACCESS_READ))
	{
		if(Examine(lock,&fib))
		{
			UnLock(lock);
			return Py_BuildValue("(siiiii(iii)sii)",
				fib.fib_FileName,
				fib.fib_Size,
				fib.fib_DirEntryType,
				fib.fib_Protection,
				fib.fib_DiskKey,
				fib.fib_NumBlocks,
				fib.fib_Date.ds_Days, fib.fib_Date.ds_Minute, fib.fib_Date.ds_Tick,
				fib.fib_Comment,
				fib.fib_OwnerUID,
				fib.fib_OwnerGID);
/******************
			return Py_BuildValue("(iisiiii(iii)sii)",
				fib.fib_DiskKey,
				fib.fib_DirEntryType,
				fib.fib_FileName,
				fib.fib_Protection,
				fib.fib_EntryType,
				fib.fib_Size,
				fib.fib_NumBlocks,
				fib.fib_Date.ds_Days, fib.fib_Date.ds_Minute, fib.fib_Date.ds_Tick,
				fib.fib_Comment,
				fib.fib_OwnerUID,
				fib.fib_OwnerGID);
******************/
		}
		UnLock(lock);
	}
	return check_ok(FALSE);
}

static PyObject *
Doslib_SetComment(PyObject *self, PyObject *args)	// (file,comment) -> bool
{
	char *path, *note;

	if (!PyArg_ParseTuple(args, "ss", &path,&note)) return NULL;
	return check_ok(SetComment(path,note));
}

static PyObject *
Doslib_SetOwner(PyObject *self, PyObject *args)		// (file, uid, gid) -> bool
{
	char *file;
	int uid, gid;
	LONG guid;

	if (!PyArg_ParseTuple(args, "sii", &file,&uid,&gid)) return NULL;
	guid = ((uid&0xFFFF)<<16) | (gid&0xFFFF);
	return check_ok(SetOwner(file,guid));
}


/* the offset between DateStamp() and time() epoch: */
#define TIME_OFFSET 252457200.0

static PyObject *
Doslib_DS2time(PyObject *self, PyObject *args)
{
	struct DateStamp ds;

	if(!PyArg_ParseTuple(args,"(iii)",&ds.ds_Days,&ds.ds_Minute,&ds.ds_Tick)) return NULL;

	return PyFloat_FromDouble(
		86400.0*ds.ds_Days + 60.0*ds.ds_Minute + ((double)ds.ds_Tick/50.0) + TIME_OFFSET
	);
}

static PyObject *
Doslib_time2DS(PyObject *self, PyObject *args)
{
	double t;
	int days,minutes,ticks;

	if(!PyArg_ParseTuple(args,"d",&t)) return NULL;

	t -= TIME_OFFSET;
	days =  t / 86400.0;
	t -= days*86400.0;
	minutes = t / 60.0;
	t -= minutes*60.0;
	ticks = t*50.0;

	return Py_BuildValue("(iii)",days,minutes,ticks);
}


static PyObject *
Doslib_SetMode( PyObject *self, PyObject *args )
{
        LONG mode;
        BOOL result;

        if( !PyArg_ParseTuple( args , "l" , &mode ) ) return NULL;

        result = SetMode( Input() , mode );

        return Py_BuildValue( "l" , result );
}

static PyObject *
Doslib_WaitForChar( PyObject *self, PyObject *args )
{
        LONG timeout;
        BOOL result;

        if( !PyArg_ParseTuple( args , "l" , &timeout ) ) return NULL;

        result = WaitForChar( Input() , timeout );

        if( result == -1 )      /* Just to be safe, as I don't know if Python           */
        {                       /* considers -1 as true (like AmigaOS does apparently). */
                result = 1;
        }

        return Py_BuildValue( "l" , result );
}

static PyObject *
Doslib_GetChar( PyObject *self, PyObject *args )
{
        char buffer;
        LONG result;

        if( !PyArg_ParseTuple( args , "" ) ) return NULL;

        result = Read( Input() , &buffer , 1L );

        if( result == 0 )                       /* Did we get an EOF?      */
        {
            return Py_BuildValue( "s" , "" );
        }
        else if( result == -1 )                 /* Or maybe even an error? */
        {
            return NULL;
        }
        else
        {
            return Py_BuildValue( "c" , buffer );
        }
}


static PyObject *
Doslib_PutChar( PyObject *self, PyObject *args )
{
    char buffer;

    if( !PyArg_ParseTuple( args , "c" , &buffer ) ) return NULL;

    if( Write( Output() , &buffer , 1L ) == 1 )
    {
        return Py_BuildValue( "i" , 1 );
    }
    else                                        /* An error occurred. */
    {
        return NULL;
    }
}

static PyObject *
Doslib_PutString( PyObject *self, PyObject *args )
{
    char *buffer;
    LONG bufferlen;

    if( !PyArg_ParseTuple( args , "s" , &buffer ) ) return NULL;

    bufferlen = strlen( buffer );


    if( Write( Output() , buffer , bufferlen ) == bufferlen )
    {
        return Py_BuildValue( "i" , 1 );
    }
    else                                        /* An error occurred. */
    {
        return NULL;
    }
}


/*** FUNCTIONS FROM THE MODULE ***/

static struct PyMethodDef Doslib_global_methods[] = {
	{"ReadArgs", Doslib_ReadArgs, 1},
	{"WaitSignal", Doslib_WaitSignal, 1},
	{"CheckSignal", Doslib_CheckSignal, 1},
	{"CompareDates", Doslib_CompareDates, 1},
	{"DateStamp", Doslib_DateStamp, 0},
	{"DateToStr", Doslib_DateToStr, 1},
	{"StrToDate", Doslib_StrToDate, 1},
	{"Fault", Doslib_Fault, 1},
	{"GetProgramDir", Doslib_GetProgramDir, 0},
	{"GetProgramName", Doslib_GetProgramName, 0},
	{"Inhibit", Doslib_Inhibit, 1},
	{"Info", Doslib_Info, 1},
	{"IoErr", Doslib_IoErr, 0},
	{"SetIoErr", Doslib_SetIoErr, 1},
	{"IsFileSystem", Doslib_IsFileSystem, 1},
	{"Relabel", Doslib_Relabel, 1},
	{"SetProtection", Doslib_SetProtection, 1},
	{"SetComment", Doslib_SetComment, 1},
	{"SetFileDate", Doslib_SetFileDate, 1},
	{"SetOwner", Doslib_SetOwner, 1},
	{"Examine", Doslib_Examine, 1},
	{"DS2time", Doslib_DS2time, 1},
	{"time2DS", Doslib_time2DS, 1},
	{"SetMode", Doslib_SetMode, 1},
	{"WaitForChar", Doslib_WaitForChar, 1},
	{"GetChar", Doslib_GetChar, 1},
	{"PutChar", Doslib_PutChar, 1},
	{"PutString", Doslib_PutString, 1},
	{NULL,      NULL}       /* sentinel */
};

void
initDoslib Py_PROTO((void))
{
	PyObject *m, *d;

	m = Py_InitModule("Doslib", Doslib_global_methods);
	d = PyModule_GetDict(m);

	/* Initialize error exception */
	error = PyErr_NewException("Doslib.error", NULL, NULL);
	if (error != NULL)
		PyDict_SetItemString(d, "error", error);
}

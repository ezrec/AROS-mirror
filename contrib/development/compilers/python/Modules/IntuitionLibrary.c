/*
    Low-level intuition.library module for AROS and AmigaOS.
*/

#include <exec/types.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include "Python.h"

/**** WindowObject definition ************************************************/

typedef struct {
    PyObject_HEAD
    struct Window  *window;
    PyStringObject *title;
} WindowObject;

static PyObject *error;    // Exception

/**** WindowObject support functions *****************************************/

static BOOL WindowObject_IsOpen( WindowObject *self )
{
    if( self->window == NULL )
    {
    	PyErr_SetString( error, "operation on closed Window" );
	return FALSE;
    }
    
    return TRUE;
}

static void FixGZZ(const WindowObject *w, long *x, long *y)
{
    *x += w->window->BorderLeft;
    *y += w->window->BorderTop;
}

/**** WindowObject methods ***************************************************/

static PyObject *WindowObject_close( WindowObject *self, PyObject *args ) 
{
    if( !WindowObject_IsOpen( self ) ) return NULL;
    if( !PyArg_NoArgs( args ) ) return NULL;
    
    CloseWindow( self->window ); self->window = NULL;
    Py_DECREF( self->title );    self->title  = NULL;

    Py_INCREF( Py_None );
    return Py_None;
}

static PyObject *WindowObject_hide( WindowObject *self, PyObject *args )
{
    if( !WindowObject_IsOpen( self ) ) return NULL;
    
    HideWindow( self->window );

    Py_INCREF( Py_None );
    return Py_None;
}

static PyObject *WindowObject_show( WindowObject *self, PyObject *args ) 
{
    if( !WindowObject_IsOpen( self ) ) return NULL;
    
    ShowWindow( self->window );

    Py_INCREF( Py_None );
    return Py_None;
}

/* For settings the window's title. Or maybe make it an attribute??? */

static PyObject *WindowObject_setTitle( WindowObject *self, PyObject *args )
{
    PyStringObject *newTitle;
    
    if( !WindowObject_IsOpen( self ) ) return NULL;
    if( !PyArg_ParseTuple( args, "S", &newTitle ) ) return NULL;
    
    Py_INCREF( newTitle );
    Py_DECREF( self->title );
    self->title = newTitle;
        
    SetWindowTitles( self->window, PyString_AsString( newTitle ), (STRPTR) ~0L );

    Py_INCREF( Py_None );
    return Py_None;
}

static PyObject *WindowObject_getTitle( WindowObject *self, PyObject *args ) 
{
    if( !WindowObject_IsOpen( self ) ) return NULL;
    if( !PyArg_NoArgs( args ) ) return NULL;
    
    Py_INCREF( self->title );  // Do I need to do this, or is done automatically?
    return self->title;   
}

static PyObject *WindowObject_setPen( WindowObject *self, PyObject *args )
{
    int pen;
    
    if( !WindowObject_IsOpen( self ) ) return NULL;
    if( !PyArg_ParseTuple( args, "i", &pen ) ) return NULL;

    SetAPen( self->window->RPort, pen );

    Py_INCREF( Py_None );
    return Py_None;
}

static PyObject *WindowObject_writePixel( WindowObject *self, PyObject *args )
{
    int x, y;

    if( !WindowObject_IsOpen( self ) ) return NULL;
    if( !PyArg_ParseTuple( args, "ii", &x, &y ) ) return NULL;
    
    WritePixel( self->window->RPort, x, y );
    
    Py_INCREF( Py_None );
    return Py_None;   
}

/**** WindowObject method table **********************************************/

static PyMethodDef WindowObject_Methods[] = 
{
    { "close",      (PyCFunction) WindowObject_close,      METH_VARARGS },
    { "hide",       (PyCFunction) WindowObject_hide,       METH_VARARGS },
    { "show",       (PyCFunction) WindowObject_show,       METH_VARARGS },
    { "setTitle",   (PyCFunction) WindowObject_setTitle,   METH_VARARGS },
    { "getTitle",   (PyCFunction) WindowObject_getTitle,   METH_VARARGS },
    { "setPen",     (PyCFunction) WindowObject_setPen,     METH_VARARGS },
    { "writePixel", (PyCFunction) WindowObject_writePixel, METH_VARARGS },

    {NULL,      NULL}
};

/**** WindowType methods *****************************************************/

static void WindowType_dealloc( WindowObject *self ) 
{
    if( self->window != NULL ) CloseWindow( self->window );
    Py_DECREF( self->title );
    PyMem_DEL( self );
}

static PyObject *WindowType_getattr( WindowObject *self, char *name )
{
    return Py_FindMethod( WindowObject_Methods, (PyObject *) self, name );
}

static PyObject *WindowType_repr( WindowObject *self )
{
    char buffer[50];
    
    sprintf( buffer, "<Window at %lx>", (LONG) self );
    return PyString_FromString( buffer );
}

/**** WindowType initialization table ****************************************/

static PyTypeObject WindowType = 
{
    PyObject_HEAD_INIT( &PyType_Type )
    0,                                /* ob_size     */
    "Window",                         /* tp_name     */
    sizeof( WindowObject ),           /* tp_size     */
    0,                                /* tp_itemsize */
    (destructor)  WindowType_dealloc, /* tp_dealloc  */
                  NULL,               /* tp_print    */
    (getattrfunc) WindowType_getattr, /* tp_getattr  */
    (setattrfunc) NULL,               /* tp_setattr  */
                  NULL,               /* tp_compare  */
    (reprfunc)    WindowType_repr,    /* tp_repr     */
};

/**** Module methods ********************************************************/

static PyObject *IntuitionLibrary_Window(PyObject *self, PyObject *args)
{
    WindowObject   *wo    = NULL; 
    PyStringObject *title = NULL;
    long x=40,y=40,w=400,h=200;

    if (!PyArg_ParseTuple(args, "|Siiii", &title,&x,&y,&w,&h)) return NULL;

    if( title == NULL ) 
    {
    	title = PyString_FromString( "Python Window" );
    }

    if( wo = PyObject_NEW( WindowObject, &WindowType ) ) 
    {
    	if
	( 
	    wo->window = OpenWindowTags
	    (
	    	NULL,
		WA_Title,       PyString_AsString( title ),
		WA_Left,        x,
		WA_Top,         y,
		WA_InnerWidth,  w,
		WA_InnerHeight, h,
		WA_Flags,       WFLG_DRAGBAR | 
		                WFLG_DEPTHGADGET | 
				WFLG_ACTIVATE | 
				WFLG_SIMPLE_REFRESH | 
				WFLG_NOCAREREFRESH,
		TAG_DONE
	    ) 
	)
	{
	    Py_INCREF( title );
	    wo->title = title;
	    
	    return (PyObject *) wo;
	}
	else
	{
	    PyErr_SetString( error, "can't open window" );
	    PyMem_DEL( wo );
	}
    }
    
    return NULL;
}

/**** Module method table ****************************************************/

static PyMethodDef IntuitionLibrary_Methods[] = 
{
    {"Window",  IntuitionLibrary_Window, METH_VARARGS},
    {NULL,      NULL}
};

/**** Module initialization routine ******************************************/

void initIntuitionLibrary Py_PROTO( (void) )
{
    PyObject *module, *dictionary;

    module     = Py_InitModule( "IntuitionLibrary", IntuitionLibrary_Methods );
    dictionary = PyModule_GetDict( module );

    /* Initialize error exception */
    error = PyString_FromString( "IntuitionLibrary.error");
    if( error == NULL || PyDict_SetItemString( dictionary, "error", error ) != 0 )
    {
    	Py_FatalError( "Can't define IntuitionLibrary.error" );
    }
}

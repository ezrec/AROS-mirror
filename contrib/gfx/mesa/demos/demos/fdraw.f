C fdraw.f

C Fortran subroutine to test Fortran Mesa bindings.
C This is called from ftest.c



      program fdrawf
      call cmain
      end



      subroutine fdraw( width, height )
      implicit none
      integer*4 width, height
      real*4 mat(16)
      character*256 str
      integer*4 err
      logical*1 n
      include "../include/GL/fgl.h"

C matrix for scaling by 3.0
      mat(1) = 3.0
      mat(2) = 0.0
      mat(3) = 0.0
      mat(4) = 0.0
      mat(5) = 0.0
      mat(6) = 3.0
      mat(7) = 0.0
      mat(8) = 0.0
      mat(9) = 0.0
      mat(10) = 0.0
      mat(11) = 3.0
      mat(12) = 0.0
      mat(13) = 0.0
      mat(14) = 0.0
      mat(15) = 0.0
      mat(16) = 1.0

      str = fglGetString( GL_RENDERER )
      print *, str

      err = fglGetError()
      print *, "Error code", err

      n = fglIsEnabled( GL_DITHER )
      print *, "dither=", n

      n = fglIsEnabled( GL_LIGHTING )
      print *, "lighting=", n

      call fglViewport( 0, 0, width, height )

      call fglMatrixMode( GL_PROJECTION )
      call fglLoadIdentity

      call fglMatrixMode( GL_MODELVIEW )
      call fglLoadIdentity
      call fglRotatef( 45.0, 0.0, 0.0, 1.0 )
      call fglTranslatef( -0.6, -0.0, 0.0 )
      call fglMultMatrixf( mat )
C      call fglScalef( 5.0, 5.0, 0.0 )

      call fglClear( GL_COLOR_BUFFER_BIT )

      call fglBegin( GL_LINE_LOOP )
      call fglVertex2i( 0, 0 )
      call fglVertex2f( 0.0, 0.1 )
      call fglVertex4f( 0.1, 0.1, 0.0, 1.0 )
      call fglVertex3f( 0.1, 0.0, 0.0 )
      call fglEnd()

      return
      end

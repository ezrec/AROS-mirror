
#include <qapplication.h>
#include <qpushbutton.h>
#include <qcanvas.h>
#include <qpixmap.h>

#define GFX_DONTSHRINK
#include "../common/nsmtracker.h"
#include "../common/windows_proc.h"

#include "../common/visual_proc.h"


class MyWidget : public QWidget
{
public:
    MyWidget(struct Tracker_Windows *window, QWidget *parent=0, const char *name=0 );
   ~MyWidget();

    QColor     colors[8];				// color array
    QPixmap    *qpixmap;
    QPixmap    *cursorpixmap;

    void timerEvent(QTimerEvent *);

protected:
    void	paintEvent( QPaintEvent * );
    void        keyPressEvent(QKeyEvent *);
    void        keyReleaseEvent(QKeyEvent *qkeyevent);
    void	mousePressEvent( QMouseEvent *);
    void	mouseReleaseEvent( QMouseEvent *);
    void	mouseMoveEvent( QMouseEvent *);
    void        resizeEvent( QResizeEvent *);
    void        closeEvent(QCloseEvent *);
private:
    QPoint     *points;				// point array

    int		count;				// count = number of points
    bool	down;				// TRUE if mouse down

    struct Tracker_Windows *window;
};



#include "MyWidget.h"

#include "../common/resizewindow_proc.h"
#include "../common/blts_proc.h"
#include "../common/eventreciever_proc.h"

#include "../common/player_proc.h"

extern QApplication *qapplication;
extern "C" void WBLOCKS_bltBlt(struct Tracker_Windows *window);

void MyWidget::timerEvent(QTimerEvent *){
  PlayerTask(20000);
}

void MyWidget::paintEvent( QPaintEvent *e ){

//  setBackgroundColor( this->colors[0] );		/* white background */

  Resize_resized(this->window,this->width()-100,this->height()-30,false);
  //    UpdateTrackerWindow(this->window);

    //    QPainter paint(this);

    //    bitBlt(this,0,0,this->qpixmap);
//  QPainter paint( this );
//  paint.drawLine(0,0,50,50);

}

struct TEvent tevent={{0}};

const unsigned int Qt2SubId[0x2000]={
  EVENT_A,
  EVENT_B,
  EVENT_C,
  EVENT_D,
  EVENT_E,
  EVENT_F,
  EVENT_G,
  EVENT_H,
  EVENT_I,
  EVENT_J,
  EVENT_K,
  EVENT_L,
  EVENT_M,
  EVENT_N,
  EVENT_O,
  EVENT_P,
  EVENT_Q,
  EVENT_R,
  EVENT_S,
  EVENT_T,
  EVENT_U,
  EVENT_V,
  EVENT_W,
  EVENT_X,
  EVENT_Y,
  EVENT_Z
};

void MyWidget::keyPressEvent(QKeyEvent *qkeyevent){
  //  printf("key press: %d\n",qkeyevent->ascii());
  //  printf("key press: %d\n",qkeyevent->key());
  printf("key press: %d,%d\n",qkeyevent->state(),Qt2SubId[max(0,qkeyevent->key()-0x41)]);

  tevent.ID=TR_KEYBOARD;

  ButtonState buttonstate=qkeyevent->state();

  tevent.keyswitch=0;

  if(buttonstate&ShiftButton) tevent.keyswitch=EVENT_LEFTSHIFT;
  if(buttonstate&ControlButton) tevent.keyswitch|=EVENT_LEFTCTRL;
  if(buttonstate&AltButton) tevent.keyswitch|=EVENT_RIGHTALT;

  //  printf("%d\n",qkeyevent->key());


  if(qkeyevent->key()==4117){
      tevent.SubID=EVENT_DOWNARROW;
      //          for(int lokke=0;lokke<50000;lokke++)
      //EventReciever(&tevent,this->window);

  }else{
    if(qkeyevent->key()==4115){
      tevent.SubID=EVENT_UPARROW;      
    }else{
      if(qkeyevent->key()==4114){ //ventre
	tevent.SubID=EVENT_LEFTARROW;
      }else{
	if(qkeyevent->key()==4116){
	  tevent.SubID=EVENT_RIGHTARROW;
	}else{
	  tevent.SubID=Qt2SubId[max(0,qkeyevent->key()-0x41)];
	}
      }
    }
  }

  if(qkeyevent->key()==32){
    tevent.SubID=EVENT_SPACE;
  }

  EventReciever(&tevent,this->window);

  //  WBLOCKS_bltBlt(this->window);

  //this->repaint();
  //  bitBlt(this,0,0,this->qpixmap);
}

void MyWidget::keyReleaseEvent(QKeyEvent *qkeyevent){
  //  printf("key release: %d\n",qkeyevent->ascii());
  //  printf("key release: %d\n",qkeyevent->key());
}

void MyWidget::mousePressEvent( QMouseEvent *qmouseevent){

  if(qmouseevent->button()==LeftButton){
    tevent.ID=TR_LEFTMOUSEDOWN;
  }else{
    if(qmouseevent->button()==RightButton){
      tevent.ID=TR_RIGHTMOUSEDOWN;
      exit(2);
    }else{
      tevent.ID=TR_MIDDLEMOUSEDOWN;
    }
  }
  tevent.x=qmouseevent->x();
  tevent.y=qmouseevent->y();

  EventReciever(&tevent,this->window);

  //  WBLOCKS_bltBlt(this->window);
  //bitBlt(this,0,0,this->qpixmap);
}

void MyWidget::mouseReleaseEvent( QMouseEvent *qmouseevent){
  if(qmouseevent->button()==LeftButton){
    tevent.ID=TR_LEFTMOUSEUP;
  }else{
    if(qmouseevent->button()==RightButton){
      tevent.ID=TR_RIGHTMOUSEUP;
    }else{
      tevent.ID=TR_MIDDLEMOUSEUP;
    }
  }
  tevent.x=qmouseevent->x();
  tevent.y=qmouseevent->y();


  EventReciever(&tevent,this->window);

  //  WBLOCKS_bltBlt(this->window);
  //bitBlt(this,0,0,this->qpixmap);
}

void MyWidget::mouseMoveEvent( QMouseEvent *qmouseevent){
  tevent.ID=TR_MOUSEMOVE;
  tevent.x=qmouseevent->x();
  tevent.y=qmouseevent->y();
  EventReciever(&tevent,this->window);

  //  WBLOCKS_bltBlt(this->window);
  //  bitBlt(this,0,0,this->qpixmap);
}

void MyWidget::resizeEvent( QResizeEvent *qresizeevent){
  //  this->window->width=this->width()-100;
  //  this->window->height=this->height();

  //  this->qpixmap=new QPixmap(this->width(),this->height(),-1,QPixmap::BestOptim);
  this->qpixmap=new QPixmap(this->width(),this->height(),-1,QPixmap::BestOptim);
  this->qpixmap->fill( this->colors[0] );		/* grey background */

  this->cursorpixmap=new QPixmap(this->width(),this->window->fontheight);
  this->cursorpixmap->fill( this->colors[7] );		/* the xored background color for the cursor.*/

  Resize_resized(this->window,this->width()-100,this->height()-30,false);

  QPainter paint( this );
  paint.setPen(this->colors[6]);
  paint.drawLine(this->width()-99,0,this->width()-99,this->height());

}

void MyWidget::closeEvent(QCloseEvent *ce){
  ce->accept();
}

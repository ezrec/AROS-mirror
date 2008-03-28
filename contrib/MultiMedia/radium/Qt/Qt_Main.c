
#include <qapplication.h>

#include "../common/nsmtracker.h"
#include "../common/eventreciever_proc.h"
#include "../common/control_proc.h"


QApplication *qapplication;

extern struct Root *root;

int main(int argc,char **argv){

  qapplication=new QApplication(argc,argv);

  printf("starting\n");
  InitProgram();
  printf("ending\n");

  qapplication->setMainWidget((QWidget *)root->song->tracker_windows->os_visual->widget);
  return qapplication->exec();

}


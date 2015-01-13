#include <QApplication>
#include "gui/visiongui.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mrvision::VisionGui vVisionGui;
	if( argc >= 2 ){
		//vVisionGui.loadConfig( argv[1] );
	}
    vVisionGui.show();

    return a.exec();
}

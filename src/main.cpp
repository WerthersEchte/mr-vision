#define WIN32_LEAN_AND_MEAN 1

#include <QApplication>
#include "gui/visiongui.h"
#include <iostream>
#include "opencv2/opencv.hpp"

#include "Windows.h"

int main(int argc, char *argv[])
{

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

    QApplication a(argc, argv);
    mrvision::VisionGui vVisionGui;
	if( argc >= 2 ){
		//vVisionGui.loadConfig( argv[1] );
	}
    vVisionGui.show();

    return a.exec();
}

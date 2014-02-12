#include <QApplication>
#include "gui/mainGUI.h"

int main( int argc, char *argv[] ){

	QApplication app(argc, argv);
	mainGUI mainWindow;
	mainWindow.show();
	return app.exec();

}

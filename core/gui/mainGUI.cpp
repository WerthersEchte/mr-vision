#include <iostream>
#include "mainGUI.h"

mainGUI::mainGUI(QWidget *parent)
: QWidget(parent), Ui::mainUI()
{
  setupUi(this);

  connect( cmdAddFile, SIGNAL(clicked()), this, SLOT( sayHello() ) );

}

mainGUI::~mainGUI()
{
}

void mainGUI::sayHello(){

	std::cout << "Hello";

}

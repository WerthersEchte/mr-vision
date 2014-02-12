#ifndef MAINGUI_H_
#define MAINGUI_H_

#include <QWidget>
#include <QScopedPointer>
#include "ui_mainGUI.h"

class mainGUI: public QWidget, private Ui::mainUI {
  Q_OBJECT

public:
  mainGUI(QWidget *parent = 0);
  virtual ~mainGUI();

public slots:
  void sayHello();

};


#endif /* MAINGUI_H_ */

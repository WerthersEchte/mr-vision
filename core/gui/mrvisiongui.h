#ifndef _mrvisiongui_h
#define _mrvisiongui_h

#include <QMainWindow>

namespace Ui{
    class GUIMRVision;
};

namespace mrvision {

class MRVisionGui : public QMainWindow
{
    Q_OBJECT

    Ui::GUIMRVision* ui;

public:
	MRVisionGui(QWidget *parent = 0);
    ~MRVisionGui();
private:

};


};

#endif // _mrvisiongui_h

#include "mrvisiongui.h"
#include "ui_mrvisiongui.h"

namespace mrvision {

MRVisionGui::MRVisionGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUIMRVision)
{
    ui->setupUi(this);
}

MRVisionGui::~MRVisionGui()
{
    delete ui;
}

};

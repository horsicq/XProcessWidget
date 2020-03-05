#include "xprocessdialoghex.h"
#include "ui_xprocessdialoghex.h"

XProcessDialogHex::XProcessDialogHex(QWidget *parent, XBinary *pBinary) :
    QDialog(parent),
    ui(new Ui::XProcessDialogHex)
{
    ui->setupUi(this);

    options={};
    options.memoryMap=pBinary->getMemoryMap();

    ui->widgetHex->setData(pBinary->getDevice(),&options);
}

XProcessDialogHex::~XProcessDialogHex()
{
    delete ui;
}

void XProcessDialogHex::on_pushButtonOK_clicked()
{
    this->close();
}

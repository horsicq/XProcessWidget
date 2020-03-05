#ifndef XPROCESSDIALOGHEX_H
#define XPROCESSDIALOGHEX_H

#include <QDialog>
#include "qhexview.h"

namespace Ui {
class XProcessDialogHex;
}

class XProcessDialogHex : public QDialog
{
    Q_OBJECT

public:
    explicit XProcessDialogHex(QWidget *parent,XBinary *pBinary);
    ~XProcessDialogHex();

private slots:
    void on_pushButtonOK_clicked();

private:
    Ui::XProcessDialogHex *ui;

    QHexView::OPTIONS options;
};

#endif // XPROCESSDIALOGHEX_H

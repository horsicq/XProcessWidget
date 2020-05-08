// copyright (c) 2020 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef XPROCESSWIDGET_H
#define XPROCESSWIDGET_H

#include <QWidget>
#include <QMenu>
#include "xprocess.h"
#include "xprocessdevice.h"
#include "xprocessdialoghex.h"
#include "xprocessdialogsystemstructs.h"
#include "xpe.h"

namespace Ui {
class XProcessWidget;
}

class XProcessWidget : public QWidget
{
    Q_OBJECT

    enum CN
    {
        CN_ID=0,
        CN_NAME,
        CN_FILEPATH,
        CN_size
    };

public:
    explicit XProcessWidget(QWidget *parent = nullptr);
    ~XProcessWidget();

private slots:
    void on_pushButtonReload_clicked();
    void reload();

    void on_tableWidgetProcesses_customContextMenuRequested(const QPoint &pos);
    void _hexFile();
    void _hexMemory();
    void _systemStructs();
    void _strings();

private:
    Ui::XProcessWidget *ui;
};

#endif // XPROCESSWIDGET_H

/* Copyright (c) 2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef XPROCESSWIDGETADVANCED_H
#define XPROCESSWIDGETADVANCED_H

#include "dialogdumpprocessmemory.h"
#include "dialoghandleinfoprocess.h"
#include "xoptions.h"
#include "xshortcutswidget.h"

namespace Ui {
class XProcessWidgetAdvanced;
}

class XProcessWidgetAdvanced : public XShortcutsWidget {
    Q_OBJECT

    enum COLUMN_ALL {
        COLUMN_ALL_PID = 0,
        COLUMN_ALL_NAME,
        COLUMN_ALL_PROTECTION,
        COLUMN_ALL_FILENAME,
        __COLUMN_ALL_SIZE
    };

    enum COLUMN_NET {
        COLUMN_NET_PID = 0,
        COLUMN_NET_NAME,
        COLUMN_NET_VERSION,
        COLUMN_NET_PROTECTION,
        COLUMN_NET_FILENAME,
        __COLUMN_NET_SIZE
    };

    enum USERROLE {
        USERROLE_PID = 0,
        USERROLE_ADDRESS,
        USERROLE_SIZE,
        USERROLE_FILENAME
    };

public:
    explicit XProcessWidgetAdvanced(QWidget *pParent = nullptr);
    ~XProcessWidgetAdvanced();

    void reload();
    void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);

protected:
    virtual void registerShortcuts(bool bState);

private slots:
    void on_pushButtonReloadProcesses_clicked();
    void on_pushButtonSaveProcesses_clicked();
    void on_tableViewProcesses_customContextMenuRequested(const QPoint &pos);
    void _dumpProcess();
    void _showInFolderSlot();

private:
    Ui::XProcessWidgetAdvanced *ui;
};

#endif  // XPROCESSWIDGETADVANCED_H

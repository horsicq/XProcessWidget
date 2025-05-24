/* Copyright (c) 2020-2025 hors<horsicq@gmail.com>
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
#ifndef XPROCESSWIDGET_H
#define XPROCESSWIDGET_H

#include <QDesktopServices>
#include "xoptions.h"
#include "xprocess.h"
#include "xpe.h"
#include "xshortcutswidget.h"
#ifdef X_WINIODRIVER
#include "xwiniodriver.h"
#endif
#include "dialoghexview.h"
#include "dialogsearchstrings.h"
#ifdef XDYNSTRUCTS
#include "dialogxdynstructs.h"
#endif
#include "dialogsearchsignatures.h"
#include "dialogxprocessmemorymap.h"
#include "dialogxprocessmodules.h"

#ifdef Q_OS_WIN
#include "dialogpe.h"
#endif
#ifdef Q_OS_LINUX
#include "dialogelf.h"
#endif
#ifdef Q_OS_OSX
#include "dialogmach.h"
#endif

namespace Ui {
class XProcessWidget;
}

class XProcessWidget : public XShortcutsWidget {
    Q_OBJECT

    enum COLUMN {
        COLUMN_ID = 0,
        COLUMN_NAME,
        COLUMN_FILENAME,
        COLUMN_size
    };

    enum CBDATA {
        CBDATA_PID = 0,
        CBDATA_NAME,
        CBDATA_FILEPATH,
        CBDATA_IMAGEADDRESS,
        CBDATA_IMAGESIZE
    };

    enum SC {
        SC_PROCESSSCTRUCT = 0,
        SC_PROCESSDUMPTOFILE,
        SC_PROCESSMEMORYHEX,
        SC_PROCESSMEMORYSTRINGS,
        SC_PROCESSMEMORYSIGNATURES,
        SC_PROCESSMEMORYMAP,
        SC_PROCESSMODULES,
        SC_PROCESSFILEVIEWER,
        SC_PROCESSFILEFOLDER,
        SC_PROCESSFILECOPYFILENAME,
        SC_PROCESSFILECOPYNAME,
        SC_PROCESSFILECOPYPID,
        __SC_SIZE
    };

public:
    explicit XProcessWidget(QWidget *pParent = nullptr);
    ~XProcessWidget();

    void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);

    virtual void adjustView()
    {
    }

private slots:
    void reload();

    void on_tableWidgetProcesses_customContextMenuRequested(const QPoint &pos);
    void _memoryHex();
    void _memoryStrings();
    void _memorySignatures();
    void _memoryMap();
    void _modules();
    void _showInViewer();
    void _showInFolder();
    void _copyPID();
    void _copyName();
    void _copyFilename();
    void _structs();
    void _dumpToFile();
    void on_pushButtonProcessesReload_clicked();
    void on_pushButtonProcessesSave_clicked();
    void errorMessageSlot(QString sErrorMessage);
    void on_comboBoxMode_currentIndexChanged(int nIndex);
#ifdef XDYNSTRUCTS
    XDynStructsEngine::IOMODE getCurrentIOMode();
#endif
    void on_checkBoxShowAll_toggled(bool bChecked);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XProcessWidget *ui;
#ifdef XDYNSTRUCTS
    XDynStructsEngine g_dynStructsEngine;
#endif
    QShortcut *g_shortCuts[__SC_SIZE];
#ifdef X_WINIODRIVER
    bool g_bIsDriverLoaded;
    QString g_sServiceName;
#endif
};

#endif  // XPROCESSWIDGET_H

/* Copyright (c) 2020-2022 hors<horsicq@gmail.com>
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

#include "xoptions.h"
#include "xprocess.h"
#include "xprocessdevice.h"
#include "xpe.h"
#include "xshortcutswidget.h"
#include "dialoghexview.h"
#include "dialogsearchstrings.h"
#include "dialogxdynstructs.h"
#include "dialogsearchsignatures.h"

#ifdef Q_OS_WIN
#include "dialogpe.h"
#endif
#ifdef Q_OS_LINUX
#include "dialogelf.h"
#endif

namespace Ui {
class XProcessWidget;
}

class XProcessWidget : public XShortcutsWidget
{
    Q_OBJECT

    enum COLUMN
    {
        COLUMN_ID=0,
        COLUMN_NAME,
        COLUMN_FILENAME,
        COLUMN_size
    };

    enum CBDATA
    {
        CBDATA_PID=0,
        CBDATA_NAME,
        CBDATA_FILEPATH,
        CBDATA_IMAGEADDRESS,
        CBDATA_IMAGESIZE
    };

public:
//    struct OPTIONS
//    {
//        QString sStructsPath;
//        QString sSearchSignaturesPath;
//    };

    explicit XProcessWidget(QWidget *pParent=nullptr);
    ~XProcessWidget();
//    void setOptions(OPTIONS options);
    void setGlobal(XShortcuts *pShortcuts,XOptions *pXOptions);

private slots:
    void reload();

    void on_tableWidgetProcesses_customContextMenuRequested(const QPoint &pos);
    void _memoryHex();
    void _memoryStrings();
    void _memorySignatures();
    void _fileViewer();
    void _fileCopyFileName();
    void _structs();
    void _dumpToFile();
    void on_pushButtonProcessesReload_clicked();
    void on_pushButtonProcessStructs_clicked();
    void on_pushButtonProcessesSave_clicked();
    void on_pushButtonProcessHex_clicked();
    void on_pushButtonProcessStrings_clicked();
    void on_pushButtonSignatures_clicked();
    void on_pushButtonProcessesFileViewer_clicked();
    void errorMessageSlot(QString sErrorMessage);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XProcessWidget *ui;

//    OPTIONS g_options;
    XDynStructsEngine dynStructsEngine;
    QShortcut *g_scProcessSctruct;
    QShortcut *g_scProcessDumpToFile;
    QShortcut *g_scProcessMemoryHex;
    QShortcut *g_scProcessMemoryStrings;
    QShortcut *g_scProcessMemorySignatures;
    QShortcut *g_scProcessFileViewer;
    QShortcut *g_scProcessFileCopyFileName;
};

#endif // XPROCESSWIDGET_H

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
#ifndef DIALOGDUMPPROCESSMEMORY_H
#define DIALOGDUMPPROCESSMEMORY_H

#include "dialogdumpprocess.h"

namespace Ui {
class DialogDumpProcessMemory;
}

class DialogDumpProcessMemory : public QDialog {
    Q_OBJECT

    enum MODE {
        NODE_UNKNOWN = 0,
#ifdef Q_OS_WIN
        MODE_USER_READPROCESSMEMORY
#endif
    };

public:
    enum METHOD {
        METHOD_RAWDUMP = 0,
        METHOD_REBUILDIMAGE
    };

    explicit DialogDumpProcessMemory(QWidget *parent = nullptr);
    ~DialogDumpProcessMemory();

    void setData(X_ID nProcessID, XADDR nImageBase, qint64 nImageSize, QString sFileName, METHOD method);

private slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonDump_clicked();
    void on_comboBoxMethod_currentIndexChanged(int nIndex);
    void on_pushButtonCodeDisasm_clicked();
    void on_pushButtonImportScan_clicked();
    void on_checkBoxSetFileAlignment_toggled(bool bChecked);
    void on_checkBoxSetSectionAlignment_clicked(bool bChecked);
    void on_checkBoxSetEntryPoint_clicked(bool bChecked);
    void on_checkBoxSetImageBase_clicked(bool bChecked);
    void on_checkBoxAddImportSection_clicked(bool bChecked);

private:
    Ui::DialogDumpProcessMemory *ui;
    X_ID g_nProcessID;
    XADDR g_nImageBase;
    qint64 g_nImageSize;
    QString g_sFileName;
#ifdef Q_OS_WIN
    XPE::FIXDUMP_OPTIONS g_fixDumpOptions;
#endif
};

#endif  // DIALOGDUMPPROCESSMEMORY_H

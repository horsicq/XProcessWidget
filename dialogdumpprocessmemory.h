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

#include <QDialog>
#include "xprocess.h"

namespace Ui {
class DialogDumpProcessMemory;
}

class DialogDumpProcessMemory : public QDialog
{
    Q_OBJECT

    enum MODE {
        MODE_USER_READPROCESSMEMORY
    };

public:
    explicit DialogDumpProcessMemory(QWidget *parent = nullptr);
    ~DialogDumpProcessMemory();

    void setData(X_ID nProcessID);

private slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonDump_clicked();

private:
    Ui::DialogDumpProcessMemory *ui;
    X_ID g_nProcessID;
};

#endif // DIALOGDUMPPROCESSMEMORY_H

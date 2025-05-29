/* Copyright (c) 2024-2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef DIALOGHANDLEINFOPROCESS_H
#define DIALOGHANDLEINFOPROCESS_H

#include "xdialogprocess.h"
#include "xhandleinfo.h"

class DialogHandleInfoProcess : public XDialogProcess {
    Q_OBJECT
public:
    explicit DialogHandleInfoProcess(QWidget *pParent = nullptr);
    ~DialogHandleInfoProcess();

    void setData(XhandleInfo::HANDLE_INFO_OPTIONS hiOptions, QList<XProcess::PROCESS_INFO> *pListProcesses);

private:
    XhandleInfo *g_pXhandleInfo;
    QThread *g_pThread;
};

#endif  // DIALOGHANDLEINFOPROCESS_H

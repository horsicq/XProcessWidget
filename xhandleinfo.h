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
#ifndef XHANDLEINFO_H
#define XHANDLEINFO_H

#include "xprocess.h"
#include "staticscan.h"
#include "die_script.h"

class XhandleInfo : public QObject {
    Q_OBJECT
public:
    enum INFOCLASS {
        INFOCLASS_PROCESSES = 0
    };

    enum PIO {
        PIO_ALL = 0,
        PIO_VALID,
        PIO_NET
    };

    struct HANDLE_INFO_OPTIONS {
        INFOCLASS infoClass;
        PIO pio;
        QString sScanEngine;
        bool bIsScanAll;
        QString sDieDatabase;
        QString sDieDatabaseCustom;
    };

    explicit XhandleInfo(QObject *pParent = nullptr);
    void setData(HANDLE_INFO_OPTIONS hiOptions, QList<XProcess::PROCESS_INFO> *pListProcesses, XBinary::PDSTRUCT *pPdStruct);
    QString scanFile(const QString &sFileName);

signals:
    void errorMessage(const QString &sText);
    void completed(qint64 nElapsed);

public slots:
    void process();

private:
    HANDLE_INFO_OPTIONS g_hiOptions;
    QList<XProcess::PROCESS_INFO> *g_pListProcesses;
    XBinary::PDSTRUCT *g_pPdStruct;
    QMap<QString, QString> g_mapDetects;
    DiE_Script *g_pDieScript;
};

#endif  // XHANDLEINFO_H

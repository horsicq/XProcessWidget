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
#include "dialogdumpprocessmemory.h"
#include "ui_dialogdumpprocessmemory.h"

DialogDumpProcessMemory::DialogDumpProcessMemory(QWidget *parent) : QDialog(parent), ui(new Ui::DialogDumpProcessMemory)
{
    ui->setupUi(this);

    g_nProcessID = 0;
    g_processInfo = {};

    {
        ui->comboBoxMode->blockSignals(true);
#ifdef Q_OS_WIN
        ui->comboBoxMode->addItem(QString("User/ReadProcessMemory"), MODE_USER_READPROCESSMEMORY);
#endif
        ui->comboBoxMode->blockSignals(false);
    }

    {
        ui->comboBoxMethod->blockSignals(true);

        ui->comboBoxMethod->addItem(tr("Raw dump"), METHOD_RAWDUMP);
#ifdef Q_OS_WIN
        ui->comboBoxMethod->addItem(tr("Rebuild image"), METHOD_REBUILDIMAGE);
#endif
        ui->comboBoxMethod->blockSignals(false);
    }
}

DialogDumpProcessMemory::~DialogDumpProcessMemory()
{
    delete ui;
}

void DialogDumpProcessMemory::setData(X_ID nProcessID, METHOD method)
{
    g_nProcessID = nProcessID;

    g_processInfo = XProcess::getInfoByProcessID(nProcessID);

    ui->lineEditImageBase->setValue32_64(g_processInfo.nImageAddress);
    ui->lineEditImageSize->setValue32_64(g_processInfo.nImageSize);

    if (METHOD_RAWDUMP) {
        ui->stackedWidgetDump->setCurrentWidget(ui->pageRaw);
    } else if (method == METHOD_REBUILDIMAGE) {
        ui->stackedWidgetDump->setCurrentWidget(ui->pageRebuild);
    } else {
        ui->stackedWidgetDump->setCurrentWidget(ui->pageRaw);
    }
}

void DialogDumpProcessMemory::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogDumpProcessMemory::on_pushButtonDump_clicked()
{
    MODE mode = (MODE)(ui->comboBoxMode->currentData().toULongLong());
    METHOD method = (METHOD)(ui->comboBoxMethod->currentData().toULongLong());

    QString sSaveFileName = XBinary::getResultFileName(g_processInfo.sFilePath, QString("dmp"));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.dmp)").arg(tr("Raw data")));

    if (!sFileName.isEmpty()) {

        if (mode == MODE_USER_READPROCESSMEMORY) {
            if (method == METHOD_RAWDUMP) {
                DialogDumpProcess dialogDumpProcess(this);
                dialogDumpProcess.setData(g_nProcessID, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_RAWDUMP, sFileName);
                dialogDumpProcess.exec();
            } else if (method == METHOD_REBUILDIMAGE) {
                DialogDumpProcess dialogDumpProcess(this);
#ifdef Q_OS_WIN
                const XPE::FIXDUMP_OPTIONS &fixDumpOptions = {};

                dialogDumpProcess.setData(g_nProcessID, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_REBUILD, sFileName, fixDumpOptions);
#endif
                dialogDumpProcess.exec();
            }
        }
    }
}

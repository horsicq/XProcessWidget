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
    g_nImageBase = 0;
    g_nImageSize = 0;

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

#ifdef Q_OS_WIN
    g_fixDumpOptions = {};
#endif

    ui->stackedWidgetDump->setCurrentWidget(ui->pageRaw);
}

DialogDumpProcessMemory::~DialogDumpProcessMemory()
{
    delete ui;
}

void DialogDumpProcessMemory::setData(X_ID nProcessID, XADDR nImageBase, qint64 nImageSize, QString sFileName, METHOD method)
{
    g_nProcessID = nProcessID;
    g_nImageBase = nImageBase;
    g_nImageSize = nImageSize;
    g_sFileName = sFileName;

    ui->lineEditProcessImageBase->setValue32_64(g_nImageBase);
    ui->lineEditProcessImageSize->setValue32_64(g_nImageSize);

    qint32 nNumberOfRecords = ui->comboBoxMethod->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((METHOD)(ui->comboBoxMethod->itemData(i).toULongLong()) == method) {
            ui->comboBoxMethod->setCurrentIndex(i);
            break;
        }
    }

    if (g_nImageSize) {
        XProcess xprocess(g_nProcessID, g_nImageBase, g_nImageSize);

        if (xprocess.open(QIODevice::ReadOnly)) {
    #ifdef Q_OS_WIN
            XPE pe(&xprocess, true, g_processInfo.nImageAddress);

            if (pe.isValid()) {
                g_fixDumpOptions = pe.getFixDumpOptions();
            }
    #endif
            xprocess.close();
        }
    }

#ifdef Q_OS_WIN
    ui->checkBoxOptimizeSize->setChecked(g_fixDumpOptions.bOptimizeSize);
    ui->checkBoxCleanHeader->setChecked(g_fixDumpOptions.bCleanHeader);
    ui->checkBoxFixSizeOfHeaders->setChecked(g_fixDumpOptions.bFixSizeOfSections);
    ui->checkBoxSetFileAlignment->setChecked(g_fixDumpOptions.bSetFileAlignment);
    ui->lineEditFileAlignment->setEnabled(g_fixDumpOptions.bSetFileAlignment);
    ui->lineEditFileAlignment->setValue_uint32(g_fixDumpOptions.nFileAlignment);
    ui->checkBoxSetSectionAlignment->setChecked(g_fixDumpOptions.bSetSectionAlignment);
    ui->lineEditSectionAlignment->setEnabled(g_fixDumpOptions.bSetSectionAlignment);
    ui->lineEditSectionAlignment->setValue_uint32(g_fixDumpOptions.nSectionAlignment);
    ui->checkBoxSetEntryPoint->setChecked(g_fixDumpOptions.bSetEntryPoint);
    ui->lineEditEntryPoint->setEnabled(g_fixDumpOptions.bSetEntryPoint);
    ui->lineEditEntryPoint->setValue_uint32(g_fixDumpOptions.nEntryPoint);
    ui->checkBoxSetImageBase->setChecked(g_fixDumpOptions.bSetImageBase);
    ui->lineEditImageBase->setEnabled(g_fixDumpOptions.bSetImageBase);
#ifndef Q_OS_WIN64
    ui->lineEditImageBase->setValue_uint32(g_fixDumpOptions.nImageBase);
#else
    ui->lineEditImageBase->setValue_uint64(g_fixDumpOptions.nImageBase);
#endif
#endif
}

void DialogDumpProcessMemory::on_pushButtonClose_clicked()
{
    this->close();
}

void DialogDumpProcessMemory::on_pushButtonDump_clicked()
{
    MODE mode = (MODE)(ui->comboBoxMode->currentData().toULongLong());
    METHOD method = (METHOD)(ui->comboBoxMethod->currentData().toULongLong());

    QString sSaveFileName = XBinary::getResultFileName(g_sFileName, QString("dmp"));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.dmp)").arg(tr("Raw data")));

    if (!sFileName.isEmpty()) {
#ifdef Q_OS_WIN
        if (mode == MODE_USER_READPROCESSMEMORY) {
            if (method == METHOD_RAWDUMP) {
                DialogDumpProcess dialogDumpProcess(this);
                dialogDumpProcess.setData(g_nProcessID, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_RAWDUMP, sFileName);
                dialogDumpProcess.exec();
            } else if (method == METHOD_REBUILDIMAGE) {
                DialogDumpProcess dialogDumpProcess(this);

                dialogDumpProcess.setData(g_nProcessID, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_REBUILD, sFileName, g_fixDumpOptions);

                dialogDumpProcess.exec();
            }
        }
#endif
    }
}

void DialogDumpProcessMemory::on_comboBoxMethod_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    METHOD method = (METHOD)(ui->comboBoxMethod->currentData().toULongLong());

    if (method == METHOD_RAWDUMP) {
        ui->stackedWidgetDump->setCurrentWidget(ui->pageRaw);
    } else if (method == METHOD_REBUILDIMAGE) {
        ui->stackedWidgetDump->setCurrentWidget(ui->pageRebuild);
    }
}

void DialogDumpProcessMemory::on_pushButtonCodeDisasm_clicked()
{
    // TODO
}

void DialogDumpProcessMemory::on_pushButtonImportScan_clicked()
{
    // TODO
}

void DialogDumpProcessMemory::on_checkBoxSetFileAlignment_toggled(bool bChecked)
{
    ui->lineEditFileAlignment->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxSetSectionAlignment_clicked(bool bChecked)
{
    ui->lineEditSectionAlignment->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxSetEntryPoint_clicked(bool bChecked)
{
    ui->lineEditEntryPoint->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxSetImageBase_clicked(bool bChecked)
{
    ui->lineEditImageBase->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxAddImportSection_clicked(bool bChecked)
{
    ui->groupBoxImport->setEnabled(bChecked);
}


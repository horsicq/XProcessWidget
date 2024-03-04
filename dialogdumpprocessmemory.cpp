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

// TODO Save / Load settings
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
#ifdef Q_OS_LINUX
        ui->comboBoxMode->addItem(QString("User//proc/<pid>/mem"), MODE_USER_PROCPIDMEM);
        ui->comboBoxMode->addItem(QString("User/ptrace"), MODE_USER_PTRACE);
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

    ui->lineEditProcessImageBase->setReadOnly(true);
    ui->lineEditProcessImageSize->setReadOnly(true);

    ui->stackedWidgetDump->setCurrentWidget(ui->pageRaw);
}

DialogDumpProcessMemory::~DialogDumpProcessMemory()
{
    delete ui;
}

void DialogDumpProcessMemory::setData(X_ID nProcessID, QString sFileName, METHOD method)
{
    g_nProcessID = nProcessID;

    {
        ui->comboBoxModule->blockSignals(true);

        QList<XProcess::MODULE> listModules = XProcess::getModulesList(nProcessID);

        qint32 nNumberOfRecords = listModules.count();
        qint32 nIndex = -1;

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            QString sData = QString("%1|%2|%3").arg(listModules.at(i).sFileName, QString::number(listModules.at(i).nAddress), QString::number(listModules.at(i).nSize));
            ui->comboBoxModule->addItem(listModules.at(i).sFileName, sData);

            if (listModules.at(i).sFileName.toUpper() == sFileName.toUpper()) {
                nIndex = i;
            }
        }

        if (nIndex != -1) {
            ui->comboBoxModule->setCurrentIndex(nIndex);
        }

        ui->comboBoxModule->blockSignals(false);
    }
    {
        ui->comboBoxUseHeaders->blockSignals(true);

        ui->comboBoxUseHeaders->addItem(tr("Use header from file"), USEHEADER_FILE);
        ui->comboBoxUseHeaders->addItem(tr("Use header from memory"), USEHEADER_MEMORY);

        ui->comboBoxUseHeaders->blockSignals(false);
    }

    {
        ui->comboBoxMethod->blockSignals(true);

        qint32 nNumberOfRecords = ui->comboBoxMethod->count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            if ((METHOD)(ui->comboBoxMethod->itemData(i).toULongLong()) == method) {
                ui->comboBoxMethod->setCurrentIndex(i);
                break;
            }
        }

        ui->comboBoxMethod->blockSignals(true);
    }

    reload();
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
                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_RAWDUMP, sFileName);
                dialogDumpProcess.exec();
            } else if (method == METHOD_REBUILDIMAGE) {
                DialogDumpProcess dialogDumpProcess(this);

                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_REBUILD, sFileName, g_fixDumpOptions, g_baHeaders);

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
#ifdef Q_OS_WIN
        ui->stackedWidgetDump->setCurrentWidget(ui->pagePE);
#endif
        reload();
    }
}

void DialogDumpProcessMemory::on_pushButtonCodeDisasm_clicked()
{
    XProcess xprocess(g_nProcessID, g_nImageBase, g_nImageSize);

    if (xprocess.open(QIODevice::ReadOnly)) {
        XMultiDisasmWidget::OPTIONS options = {};
        options.bMenu_Hex = false;
        options.nStartAddress = g_nImageBase;
        options.nInitAddress = ui->lineEditImportAddressOfCode->getValue_uint64();

        DialogMultiDisasm dialogMultiDisasm(this);
        // TODO global options
        dialogMultiDisasm.setData(&xprocess, options);
        dialogMultiDisasm.exec();

        xprocess.close();
    }
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
    ui->lineEditImportSectionName->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_pushButtonScanForIAT_clicked()
{
    // TODO
}

void DialogDumpProcessMemory::on_comboBoxModule_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

void DialogDumpProcessMemory::on_pushButtonGetImports_clicked()
{
    // TODO
}

void DialogDumpProcessMemory::reload()
{
    QString sData = ui->comboBoxModule->currentData().toString();

    g_sFileName = sData.section("|", 0, 0);
    g_nImageBase = sData.section("|", 1, 1).toULongLong();
    g_nImageSize = sData.section("|", 2, 2).toLongLong();

    ui->lineEditProcessImageBase->setValue32_64(g_nImageBase);
    ui->lineEditProcessImageSize->setValue32_64(g_nImageSize);

    if (g_nImageSize) {
        USEHEADERS useHeaders = (USEHEADERS)(ui->comboBoxUseHeaders->currentData().toULongLong());

        if (useHeaders == USEHEADER_FILE) {
            QFile file;
            file.setFileName(g_sFileName);

            if (file.open(QIODevice::ReadOnly)) {
        #ifdef Q_OS_WIN
                XPE pe(&file);

                if (pe.isValid()) {
                    g_fixDumpOptions = pe.getFixDumpOptions();
                    g_baHeaders = pe.getHeaders();
                }
        #endif
                file.close();
            }

        } else if (useHeaders == USEHEADER_MEMORY) {
            XProcess xprocess(g_nProcessID, g_nImageBase, g_nImageSize);

            if (xprocess.open(QIODevice::ReadOnly)) {
        #ifdef Q_OS_WIN
                XPE pe(&xprocess, true, g_nImageBase);

                if (pe.isValid()) {
                    g_fixDumpOptions = pe.getFixDumpOptions();
                    g_baHeaders = pe.getHeaders();
                }
        #endif
                xprocess.close();
            }
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

void DialogDumpProcessMemory::on_comboBoxUseHeaders_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}


void DialogDumpProcessMemory::on_lineEditEntryPoint_textChanged(const QString &sArg)
{
    Q_UNUSED(sArg)

    quint32 nEntryPoint = ui->lineEditEntryPoint->getValue_uint32();

    if (nEntryPoint < g_nImageSize) {
    #ifndef Q_OS_WIN64
        ui->lineEditImportAddressOfCode->setValue_uint32(nEntryPoint + g_nImageBase);
    #else
        ui->lineEditImportAddressOfCode->setValue_uint64(nEntryPoint + g_nImageBase);
    #endif
    }
}

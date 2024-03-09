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
DialogDumpProcessMemory::DialogDumpProcessMemory(QWidget *parent) : XShortcutsDialog(parent), ui(new Ui::DialogDumpProcessMemory)
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
#ifdef Q_OS_LINUX
        ui->comboBoxMethod->addItem(tr("Rebuild image"), METHOD_REBUILDIMAGE);
#endif
        ui->comboBoxMethod->blockSignals(false);
    }

#ifdef Q_OS_WIN
    g_PEfixDumpOptions = {};
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
#ifdef Q_OS_WIN
        ui->comboBoxPEUseHeaders->blockSignals(true);

        ui->comboBoxPEUseHeaders->addItem(tr("Use header from file"), USEHEADER_FILE);
        ui->comboBoxPEUseHeaders->addItem(tr("Use header from memory"), USEHEADER_MEMORY);

        ui->comboBoxPEUseHeaders->blockSignals(false);
#endif
#ifdef Q_OS_LINUX
        ui->comboBoxELFUseHeaders->blockSignals(true);

        ui->comboBoxELFUseHeaders->addItem(tr("Use header from file"), USEHEADER_FILE);
        ui->comboBoxELFUseHeaders->addItem(tr("Use header from memory"), USEHEADER_MEMORY);

        ui->comboBoxELFUseHeaders->blockSignals(false);
#endif
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

        ui->comboBoxMethod->blockSignals(false);
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

                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_READPROCESSMEMORY_REBUILD, sFileName,
                                          g_PEfixDumpOptions, g_baHeaders);

                dialogDumpProcess.exec();
            }
        }
#endif
#ifdef Q_OS_LINUX
        if (method == METHOD_RAWDUMP) {
            DialogDumpProcess dialogDumpProcess(this);
            if (mode == MODE_USER_PROCPIDMEM) {
                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_PROCPIDMEM_RAWDUMP, sFileName);
            } else if (mode == MODE_USER_PTRACE) {
                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_PTRACE_RAWDUMP, sFileName);
            }
            dialogDumpProcess.exec();
        } else if (method == METHOD_REBUILDIMAGE) {
            DialogDumpProcess dialogDumpProcess(this);
            if (mode == MODE_USER_PROCPIDMEM) {
                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_PROCPIDMEM_REBUILD, sFileName,
                                          g_ELFfixDumpOptions, g_baHeaders);
            } else if (mode == MODE_USER_PTRACE) {
                dialogDumpProcess.setData(g_nProcessID, g_nImageBase, g_nImageSize, DumpProcess::DT_DUMP_PROCESS_USER_PTRACE_REBUILD, sFileName,
                                          g_ELFfixDumpOptions, g_baHeaders);
            }

            dialogDumpProcess.exec();
        }
#endif
    }
}

void DialogDumpProcessMemory::on_comboBoxMethod_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

void DialogDumpProcessMemory::on_pushButtonPECodeDisasm_clicked()
{
    XADDR nAddress = ui->lineEditPEImportAddressOfCode->getValue_uint64();

    viewDisasm(nAddress);
}

void DialogDumpProcessMemory::on_checkBoxPESetFileAlignment_toggled(bool bChecked)
{
    ui->lineEditPEFileAlignment->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxPESetSectionAlignment_clicked(bool bChecked)
{
    ui->lineEditPESectionAlignment->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxPESetEntryPoint_clicked(bool bChecked)
{
    ui->lineEditPEEntryPoint->setEnabled(bChecked);
    ui->pushButtonPEEntryPointDisasm->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxPESetImageBase_clicked(bool bChecked)
{
    ui->lineEditPEImageBase->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_checkBoxPEAddImportSection_clicked(bool bChecked)
{
    ui->lineEditPEImportSectionName->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_pushButtonPEScanForIAT_clicked()
{
    // TODO
}

void DialogDumpProcessMemory::on_comboBoxModule_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

void DialogDumpProcessMemory::on_pushButtonPEGetImports_clicked()
{
    QList<XPE::IMPORT_RECORD> listImportRecords;

    XInfoDBTransfer::OPTIONS options = {};
    options.nProcessID = g_nProcessID;
    options.nAddress = ui->lineEditPEIATAddress->getValue_uint64();
    options.nSize = ui->lineEditPEIATSize->getValue_uint64();
#ifdef Q_OS_WIN
    DialogXInfoDBTransferProcess dialogDataTransfer(this);

    dialogDataTransfer.setData(XInfoDBTransfer::COMMAND_GETIAT, options, &listImportRecords);

    dialogDataTransfer.showDialogDelay();
#endif
}

void DialogDumpProcessMemory::reload()
{
    METHOD method = (METHOD)(ui->comboBoxMethod->currentData().toULongLong());

    if (method == METHOD_RAWDUMP) {
        ui->stackedWidgetDump->setCurrentWidget(ui->pageRaw);
    } else if (method == METHOD_REBUILDIMAGE) {
#ifdef Q_OS_WIN
        ui->stackedWidgetDump->setCurrentWidget(ui->pagePE);
#endif
#ifdef Q_OS_LINUX
        ui->stackedWidgetDump->setCurrentWidget(ui->pageELF);
#endif
    }

    QString sData = ui->comboBoxModule->currentData().toString();

    g_sFileName = sData.section("|", 0, 0);
    g_nImageBase = sData.section("|", 1, 1).toULongLong();
    g_nImageSize = sData.section("|", 2, 2).toLongLong();

    ui->lineEditProcessImageBase->setValue32_64(g_nImageBase);
    ui->lineEditProcessImageSize->setValue32_64(g_nImageSize);

    if (g_nImageSize) {
#ifdef Q_OS_WIN
        USEHEADERS useHeaders = (USEHEADERS)(ui->comboBoxPEUseHeaders->currentData().toULongLong());

        if (useHeaders == USEHEADER_FILE) {
            QFile file;
            file.setFileName(g_sFileName);

            if (file.open(QIODevice::ReadOnly)) {
                XPE pe(&file);

                if (pe.isValid()) {
                    g_PEfixDumpOptions = pe.getFixDumpOptions();
                    g_baHeaders = pe.getHeaders();
                }
                file.close();
            }
        } else if (useHeaders == USEHEADER_MEMORY) {
            XProcess xprocess(g_nProcessID, g_nImageBase, g_nImageSize);

            if (xprocess.open(QIODevice::ReadOnly)) {
                XPE pe(&xprocess, true, g_nImageBase);

                if (pe.isValid()) {
                    g_PEfixDumpOptions = pe.getFixDumpOptions();
                    g_baHeaders = pe.getHeaders();
                }
                xprocess.close();
            }
        }

        ui->checkBoxPEOptimizeSize->setChecked(g_PEfixDumpOptions.bOptimizeSize);
        ui->checkBoxPECleanHeader->setChecked(g_PEfixDumpOptions.bCleanHeader);
        ui->checkBoxPEFixSections->setChecked(g_PEfixDumpOptions.bFixSections);
        ui->checkBoxPESetFileAlignment->setChecked(g_PEfixDumpOptions.bSetFileAlignment);
        ui->lineEditPEFileAlignment->setEnabled(g_PEfixDumpOptions.bSetFileAlignment);
        ui->lineEditPEFileAlignment->setValue_uint32(g_PEfixDumpOptions.nFileAlignment);
        ui->checkBoxPESetSectionAlignment->setChecked(g_PEfixDumpOptions.bSetSectionAlignment);
        ui->lineEditPESectionAlignment->setEnabled(g_PEfixDumpOptions.bSetSectionAlignment);
        ui->lineEditPESectionAlignment->setValue_uint32(g_PEfixDumpOptions.nSectionAlignment);
        ui->checkBoxPESetEntryPoint->setChecked(g_PEfixDumpOptions.bSetEntryPoint);
        ui->lineEditPEEntryPoint->setEnabled(g_PEfixDumpOptions.bSetEntryPoint);
        ui->pushButtonPEEntryPointDisasm->setEnabled(g_PEfixDumpOptions.bSetEntryPoint);
        ui->lineEditPEEntryPoint->setValue_uint32(g_PEfixDumpOptions.nEntryPoint);
        ui->checkBoxPESetImageBase->setChecked(g_PEfixDumpOptions.bSetImageBase);
        ui->lineEditPEImageBase->setEnabled(g_PEfixDumpOptions.bSetImageBase);
    #ifndef Q_OS_WIN64
        ui->lineEditPEImageBase->setValue_uint32(g_PEfixDumpOptions.nImageBase);
        ui->lineEditPEIATAddress->setValue_uint32(g_PEfixDumpOptions.ddIAT.VirtualAddress + ui->lineEditProcessImageBase->getValue_uint32());
    #else
        ui->lineEditPEImageBase->setValue_uint64(g_PEfixDumpOptions.nImageBase);
        ui->lineEditPEIATAddress->setValue_uint64(g_PEfixDumpOptions.ddIAT.VirtualAddress + ui->lineEditProcessImageBase->getValue_uint64());
    #endif
        ui->lineEditPEIATSize->setValue_uint32(g_PEfixDumpOptions.ddIAT.Size);
#endif
#ifdef Q_OS_LINUX
        USEHEADERS useHeaders = (USEHEADERS)(ui->comboBoxPEUseHeaders->currentData().toULongLong());

        if (useHeaders == USEHEADER_FILE) {
            QFile file;
            file.setFileName(g_sFileName);

            if (file.open(QIODevice::ReadOnly)) {
                XELF elf(&file);

                if (elf.isValid()) {
                    g_ELFfixDumpOptions = elf.getFixDumpOptions();
                    g_baHeaders = elf.getHeaders();
                }
                file.close();
            }
        } else if (useHeaders == USEHEADER_MEMORY) {
            XProcess xprocess(g_nProcessID, g_nImageBase, g_nImageSize);

            if (xprocess.open(QIODevice::ReadOnly)) {
                XELF elf(&xprocess, true, g_nImageBase);

                if (elf.isValid()) {
                    g_ELFfixDumpOptions = elf.getFixDumpOptions();
                    g_baHeaders = elf.getHeaders();
                }
                xprocess.close();
            }
        }

        ui->checkBoxELFOptimizeSize->setChecked(g_ELFfixDumpOptions.bOptimizeSize);
        ui->checkBoxELFFixSegments->setChecked(g_ELFfixDumpOptions.bFixSegments);
        ui->checkBoxELFSetEntryPoint->setChecked(g_ELFfixDumpOptions.bSetEntryPoint);
        ui->lineEditELFEntryPoint->setEnabled(g_ELFfixDumpOptions.bSetEntryPoint);
        ui->pushButtonELFEntryPointDisasm->setEnabled(g_ELFfixDumpOptions.bSetEntryPoint);
        ui->lineEditELFEntryPoint->setValue_uint64(g_ELFfixDumpOptions.nEntryPoint); // TODO 32/64
#endif
    }
}

void DialogDumpProcessMemory::on_comboBoxPEUseHeaders_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

void DialogDumpProcessMemory::on_lineEditPEEntryPoint_textChanged(const QString &sArg)
{
    Q_UNUSED(sArg)

    quint32 nEntryPoint = ui->lineEditPEEntryPoint->getValue_uint32();

    if (nEntryPoint < g_nImageSize) {
#ifndef Q_OS_WIN64
        ui->lineEditPEImportAddressOfCode->setValue_uint32(nEntryPoint + g_nImageBase);
#else
        ui->lineEditPEImportAddressOfCode->setValue_uint64(nEntryPoint + g_nImageBase);
#endif
    }
}

void DialogDumpProcessMemory::on_pushButtonPEImportLoad_clicked()
{

}

void DialogDumpProcessMemory::on_pushButtonPEImportSave_clicked()
{

}

void DialogDumpProcessMemory::on_pushButtonPEEntryPointDisasm_clicked()
{
    XADDR nAddress = ui->lineEditPEEntryPoint->getValue_uint64() + ui->lineEditProcessImageBase->getValue_uint64();

    viewDisasm(nAddress);
}

void DialogDumpProcessMemory::viewDisasm(XADDR nAddress)
{
    XProcess::MEMORY_REGION memoryRegion = XProcess::getMemoryRegionByAddress(g_nProcessID, nAddress);

    if (memoryRegion.nSize) {
        XProcess xprocess(g_nProcessID, memoryRegion.nAddress, memoryRegion.nSize);

        if (xprocess.open(QIODevice::ReadOnly)) {
            XMultiDisasmWidget::OPTIONS options = {};
            options.bMenu_Hex = false;
            options.nStartAddress = memoryRegion.nAddress;
            options.nInitAddress = nAddress;
            options.bModeFixed = true;
            options.bHideReadOnly = true;
    #ifdef Q_PROCESSOR_X86_32
            options.sArch = "X86";
    #endif
    #ifdef Q_PROCESSOR_X86_64
            options.sArch = "X64";
    #endif
            options.fileType = XBinary::FT_REGION;

            DialogMultiDisasm dialogMultiDisasm(this);
            dialogMultiDisasm.setGlobal(getShortcuts(), getGlobalOptions());
            // TODO global options
            dialogMultiDisasm.setData(&xprocess, options);
            dialogMultiDisasm.exec();

            xprocess.close();
        }
    }
}

void DialogDumpProcessMemory::on_checkBoxELFSetEntryPoint_clicked(bool bChecked)
{
    ui->lineEditELFEntryPoint->setEnabled(bChecked);
    ui->pushButtonELFEntryPointDisasm->setEnabled(bChecked);
}

void DialogDumpProcessMemory::on_pushButtonELFEntryPointDisasm_clicked()
{
    XADDR nAddress = ui->lineEditELFEntryPoint->getValue_uint64();

    viewDisasm(nAddress);
}


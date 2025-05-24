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
#include "xprocesswidget.h"
#include "ui_xprocesswidget.h"

XProcessWidget::XProcessWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XProcessWidget)
{
    ui->setupUi(this);
#ifdef X_WINIODRIVER
    g_bIsDriverLoaded = false;
#endif

    memset(g_shortCuts, 0, sizeof g_shortCuts);
#ifdef XDYNSTRUCTS
    connect(&g_dynStructsEngine, SIGNAL(errorMessage(QString)), this, SLOT(errorMessageSlot(QString)));
#endif
    // mb TODO auto refresh

    ui->tableWidgetProcesses->setColumnCount(COLUMN_size);
    ui->tableWidgetProcesses->setRowCount(0);

    ui->tableWidgetProcesses->setHorizontalHeaderItem(COLUMN_ID, new QTableWidgetItem(QString("PID")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(COLUMN_NAME, new QTableWidgetItem(tr("Name")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(COLUMN_FILENAME, new QTableWidgetItem(tr("File name")));

    ui->tableWidgetProcesses->setColumnWidth(COLUMN_ID, 50);
    ui->tableWidgetProcesses->setColumnWidth(COLUMN_NAME, 100);
    ui->tableWidgetProcesses->setColumnWidth(COLUMN_FILENAME, 300);

    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_ID, QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_NAME, QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_FILENAME, QHeaderView::Stretch);

    // ui->labelBuild->setText(XProcess::getOsInfo().sBuild);

    ui->comboBoxMode->addItem(tr("User mode"));
#ifdef X_WINIODRIVER
    ui->comboBoxMode->addItem(tr("Kernel mode"));
#endif
    //    ui->comboBoxMode->addItem(QString("kldbgdrv"));

    reload();
}

XProcessWidget::~XProcessWidget()
{
#ifdef X_WINIODRIVER
    if (g_bIsDriverLoaded) {
        XWinIODriver().unloadDriver(g_sServiceName);
    }
#endif

    delete ui;
}

void XProcessWidget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    // TODO
    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
    // g_dynStructsEngine.setOptions(pXOptions);
}

void XProcessWidget::reload()
{
    // TODO TableView

    qint64 nPID = -1;

    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();
    }

    // TODO ProgressDialog
    QList<XProcess::PROCESS_INFO> listProcesses = XProcess::getProcessesList(ui->checkBoxShowAll->isChecked());

    qint32 nCount = listProcesses.count();

    ui->tableWidgetProcesses->setSortingEnabled(false);

    ui->tableWidgetProcesses->setRowCount(0);
    ui->tableWidgetProcesses->setRowCount(nCount);

    for (qint32 i = 0; i < nCount; i++) {
        QTableWidgetItem *pItemID = new QTableWidgetItem;
        pItemID->setData(Qt::DisplayRole, (quint32)(listProcesses.at(i).nID));
        pItemID->setData(Qt::UserRole + CBDATA_PID, (quint32)(listProcesses.at(i).nID));
        pItemID->setData(Qt::UserRole + CBDATA_NAME, listProcesses.at(i).sName);
        pItemID->setData(Qt::UserRole + CBDATA_FILEPATH, listProcesses.at(i).sFilePath);
        pItemID->setData(Qt::UserRole + CBDATA_IMAGEADDRESS, listProcesses.at(i).nImageAddress);
        pItemID->setData(Qt::UserRole + CBDATA_IMAGESIZE, listProcesses.at(i).nImageSize);
        pItemID->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidgetProcesses->setItem(i, COLUMN_ID, pItemID);

        QTableWidgetItem *pItemName = new QTableWidgetItem;
        pItemName->setText(listProcesses.at(i).sName);
        ui->tableWidgetProcesses->setItem(i, COLUMN_NAME, pItemName);

        QTableWidgetItem *pItemPath = new QTableWidgetItem;
        pItemPath->setText(listProcesses.at(i).sFilePath);
        ui->tableWidgetProcesses->setItem(i, COLUMN_FILENAME, pItemPath);
    }

    ui->tableWidgetProcesses->setSortingEnabled(true);

    // Restore row if selected
    if (nPID != -1) {
        for (qint32 i = 0; i < nCount; i++) {
            qint64 _nPID = ui->tableWidgetProcesses->item(i, COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();

            if (nPID == _nPID) {
                ui->tableWidgetProcesses->setCurrentCell(i, COLUMN_ID);

                break;
            }
        }
    }
}

void XProcessWidget::on_tableWidgetProcesses_customContextMenuRequested(const QPoint &pos)
{
    // TODO check isReadable
    // TODO Shortcuts
    // TODO File -> Copy Filename
    if (ui->tableWidgetProcesses->selectedItems().count()) {
        QMenu menuContext(this);

        QAction actionStructs(tr("Structs"), this);
        actionStructs.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_STRUCTS));
        connect(&actionStructs, SIGNAL(triggered()), this, SLOT(_structs()));
        menuContext.addAction(&actionStructs);

        QMenu menuMemory(tr("Memory"), this);

        QAction actionMemoryHex(tr("Hex"), this);
        actionMemoryHex.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_HEX));
        connect(&actionMemoryHex, SIGNAL(triggered()), this, SLOT(_memoryHex()));
        menuMemory.addAction(&actionMemoryHex);

        QAction actionMemoryStrings(tr("Strings"), this);
        actionMemoryStrings.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_STRINGS));
        connect(&actionMemoryStrings, SIGNAL(triggered()), this, SLOT(_memoryStrings()));
        menuMemory.addAction(&actionMemoryStrings);

        QAction actionMemorySignatures(tr("Signatures"), this);
        actionMemorySignatures.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_SIGNATURES));
        connect(&actionMemorySignatures, SIGNAL(triggered()), this, SLOT(_memorySignatures()));
        menuMemory.addAction(&actionMemorySignatures);

        QAction actionMemoryMap(tr("Memory map"), this);
        actionMemoryMap.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_MEMORYMAP));
        connect(&actionMemoryMap, SIGNAL(triggered()), this, SLOT(_memoryMap()));
        menuMemory.addAction(&actionMemoryMap);

        QAction actionModules(tr("Modules"), this);
        actionModules.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_MODULES));
        connect(&actionModules, SIGNAL(triggered()), this, SLOT(_modules()));
        menuMemory.addAction(&actionModules);

        menuContext.addMenu(&menuMemory);

        QMenu menuShowIn(tr("Show in"), this);

        QString sFileViewer = tr("Viewer");

#ifdef Q_OS_WIN
        sFileViewer = QString("PE %1").arg(tr("Viewer"));
#endif
#ifdef Q_OS_LINUX
        sFileViewer = QString("ELF %1").arg(tr("Viewer"));
#endif
#ifdef Q_OS_OSX
        sFileViewer = QString("MACH-O %1").arg(tr("Viewer"));
#endif

        QAction actionShowInViewer(sFileViewer, this);
        actionShowInViewer.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_SHOWIN_VIEWER));
        connect(&actionShowInViewer, SIGNAL(triggered()), this, SLOT(_showInViewer()));
        menuShowIn.addAction(&actionShowInViewer);

        QAction actionShowInFolder(tr("Folder"), this);
        actionShowInFolder.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_SHOWIN_FOLDER));
        connect(&actionShowInFolder, SIGNAL(triggered()), this, SLOT(_showInFolder()));
        menuShowIn.addAction(&actionShowInFolder);

        menuContext.addMenu(&menuShowIn);

        QMenu menuCopy(tr("Copy"), this);

        QAction actionCopyPID(QString("PID"), this);
        actionCopyPID.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_COPY_PID));
        connect(&actionCopyPID, SIGNAL(triggered()), this, SLOT(_copyPID()));
        menuCopy.addAction(&actionCopyPID);

        QAction actionCopyName(tr("Name"), this);
        actionCopyName.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_COPY_NAME));
        connect(&actionCopyName, SIGNAL(triggered()), this, SLOT(_copyName()));
        menuCopy.addAction(&actionCopyName);

        QAction actionCopyFileName(tr("Filename"), this);
        actionCopyFileName.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_COPY_FILENAME));
        connect(&actionCopyFileName, SIGNAL(triggered()), this, SLOT(_copyFilename()));
        menuCopy.addAction(&actionCopyFileName);

        menuContext.addMenu(&menuCopy);

        QAction actionDumpToFile(tr("Dump to file"), this);
        actionDumpToFile.setShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_DUMPTOFILE));
        connect(&actionDumpToFile, SIGNAL(triggered()), this, SLOT(_dumpToFile()));
        menuContext.addAction(&actionDumpToFile);

        menuContext.exec(ui->tableWidgetProcesses->viewport()->mapToGlobal(pos));
    }
}

void XProcessWidget::_memoryHex()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        // qint64 nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();
        // qint64 nImageAddress = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGEADDRESS).toLongLong();
        // qint64 nImageSize = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGESIZE).toLongLong();
        // QString sName = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_NAME).toString();

        // XDynStructsEngine dynStructsEngine;
        // dynStructsEngine.setOptions(getGlobalOptions());
        // dynStructsEngine.setProcessId(nPID, getCurrentIOMode());

        // XIODevice *pIODevice = dynStructsEngine.createIODevice(nImageAddress, nImageSize);

        // if (pIODevice && (pIODevice->open(QIODevice::ReadOnly))) {
        //     XHexView::OPTIONS options = {};

        //     options.sTitle = sName;
        //     options.nStartAddress = nImageAddress;

        //     DialogHexView dialogHexView(this, pIODevice, options, nullptr);  // TODO XInfoDB!

        //     dialogHexView.setGlobal(getShortcuts(), getGlobalOptions());

        //     dialogHexView.exec();

        //     pIODevice->close();
        // }

        // delete pIODevice;
    }
}

void XProcessWidget::_memoryStrings()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        // qint64 nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();
        // qint64 nImageAddress = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGEADDRESS).toLongLong();
        // qint64 nImageSize = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGESIZE).toLongLong();
        // QString sName = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_NAME).toString();

        // XDynStructsEngine dynStructsEngine;
        // dynStructsEngine.setOptions(getGlobalOptions());
        // dynStructsEngine.setProcessId(nPID, getCurrentIOMode());

        // XIODevice *pIODevice = dynStructsEngine.createIODevice(nImageAddress, nImageSize);

        // if (pIODevice && (pIODevice->open(QIODevice::ReadOnly))) {
        //     SearchStringsWidget::OPTIONS options = {};
        //     options.bAnsi = true;
        //     options.bUnicode = true;
        //     options.bCStrings = false;
        //     options.sTitle = sName;
        //     options.nBaseAddress = nImageAddress;

        //     DialogSearchStrings dialogSearchStrings(this);

        //     dialogSearchStrings.setData(pIODevice, XBinary::FT_REGION, options, true);
        //     dialogSearchStrings.setGlobal(getShortcuts(), getGlobalOptions());

        //     dialogSearchStrings.exec();

        //     pIODevice->close();
        // }

        // delete pIODevice;
    }
}

void XProcessWidget::_memorySignatures()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        // qint64 nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();
        // qint64 nImageAddress = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGEADDRESS).toLongLong();
        // qint64 nImageSize = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGESIZE).toLongLong();
        //        QString sName=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_NAME).toString();

        // XDynStructsEngine dynStructsEngine;
        // dynStructsEngine.setOptions(getGlobalOptions());
        // dynStructsEngine.setProcessId(nPID, getCurrentIOMode());

        // XIODevice *pIODevice = dynStructsEngine.createIODevice(nImageAddress, nImageSize);

        // if (pIODevice && (pIODevice->open(QIODevice::ReadOnly))) {
        //     DialogSearchSignatures dialogSearchSignatures(this);

        //     dialogSearchSignatures.setGlobal(getShortcuts(), getGlobalOptions());

        //     SearchSignaturesWidget::OPTIONS options = {};

        //     dialogSearchSignatures.setData(pIODevice, XBinary::FT_REGION, options, false);

        //     dialogSearchSignatures.exec();

        //     pIODevice->close();
        // }

        // delete pIODevice;
    }
}

void XProcessWidget::_memoryMap()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        qint64 nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();

        DialogXProcessMemoryMap dialogMemoryMap(this);

        dialogMemoryMap.setData(nPID);

        dialogMemoryMap.exec();
    }
}

void XProcessWidget::_modules()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        qint64 nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();

        DialogXProcessModules dialogModules(this);

        dialogModules.setData(nPID);

        dialogModules.exec();
    }
}

void XProcessWidget::_showInViewer()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        QString sFilePath = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_FILEPATH).toString();

        QFile file;

        file.setFileName(sFilePath);

        if (file.open(QIODevice::ReadOnly)) {
#ifdef Q_OS_WIN
            FW_DEF::OPTIONS options = {};

            options.sTitle = sFilePath;
            options.nStartType = SPE::TYPE_NFDSCAN;
            options.nImageBase = -1;

            DialogPE dialogPE(this);
            dialogPE.setData(&file, options);
            dialogPE.setGlobal(getShortcuts(), getGlobalOptions());

            dialogPE.exec();
#endif
#ifdef Q_OS_LINUX
            FW_DEF::OPTIONS options = {};

            options.sTitle = sFilePath;
            options.nStartType = SELF::TYPE_NFDSCAN;
            options.nImageBase = -1;

            DialogELF dialogELF(this);
            dialogELF.setData(&file, options);
            dialogELF.setGlobal(getShortcuts(), getGlobalOptions());

            dialogELF.exec();
#endif
#ifdef Q_OS_OSX
            FW_DEF::OPTIONS options = {};

            options.sTitle = sFilePath;
            options.nStartType = SMACH::TYPE_NFDSCAN;
            options.nImageBase = -1;

            DialogMACH dialogMach(this);
            dialogMach.setData(&file, options);
            dialogMach.setGlobal(getShortcuts(), getGlobalOptions());

            dialogMach.exec();
#endif

            file.close();
        }
    }
}

void XProcessWidget::_showInFolder()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        QString sFilePath = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_FILEPATH).toString();

        XOptions::showInFolder(sFilePath);
    }
}

void XProcessWidget::_copyPID()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        QString sFilePath = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toString();

        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(sFilePath);
    }
}

void XProcessWidget::_copyName()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        QString sFilePath = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_NAME).toString();

        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(sFilePath);
    }
}

void XProcessWidget::_copyFilename()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        QString sFilePath = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_FILEPATH).toString();

        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(sFilePath);
    }
}

void XProcessWidget::_structs()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        // qint64 nProcessId = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();
        // qint64 nImageAddress = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGEADDRESS).toLongLong();

        // g_dynStructsEngine.setProcessId(nProcessId, getCurrentIOMode());

        // DialogXDynStructs dialogXDynStructs(this);

        // dialogXDynStructs.setData(&g_dynStructsEngine, nImageAddress);
        // dialogXDynStructs.setGlobal(getShortcuts(), getGlobalOptions());

        // dialogXDynStructs.exec();
    }
}

void XProcessWidget::_dumpToFile()
{
    QList<QTableWidgetItem *> listSelected = ui->tableWidgetProcesses->selectedItems();

    if (listSelected.count()) {
        // qint64 nPID = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_PID).toLongLong();
        // qint64 nImageAddress = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGEADDRESS).toLongLong();
        // qint64 nImageSize = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_IMAGESIZE).toLongLong();
        // QString sName = listSelected.at(COLUMN_ID)->data(Qt::UserRole + CBDATA_NAME).toString();

        // XDynStructsEngine dynStructsEngine;
        // dynStructsEngine.setOptions(getGlobalOptions());
        // dynStructsEngine.setProcessId(nPID, getCurrentIOMode());

        // XIODevice *pIODevice = dynStructsEngine.createIODevice(nImageAddress, nImageSize);

        // if (pIODevice && (pIODevice->open(QIODevice::ReadOnly))) {
        //     QString sSaveFileName = QString("%1.%2.bin").arg(sName, tr("Dump"));
        //     QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.bin)").arg(tr("Raw data")));

        //     if (!sFileName.isEmpty()) {
        //         DialogDumpProcess dialogDumpProcess(this, pIODevice, 0, nImageSize, sFileName, DumpProcess::DT_OFFSET);

        //         dialogDumpProcess.exec();

        //         pIODevice->close();
        //     }
        // }

        // delete pIODevice;
    }
}

void XProcessWidget::on_pushButtonProcessesReload_clicked()
{
    reload();
}

void XProcessWidget::on_pushButtonProcessesSave_clicked()
{
    QString sFileName = QString("%1.txt").arg(tr("Processes"));
    sFileName = QFileDialog::getSaveFileName(this, tr("Save file"), sFileName, QString("%1 (*.txt);;%2 (*)").arg(tr("Text files"), tr("All files")));

    if (!sFileName.isEmpty()) {
        XOptions::saveTableWidget(ui->tableWidgetProcesses, sFileName);
    }
}

void XProcessWidget::registerShortcuts(bool bState)
{
    if (bState) {
        if (!g_shortCuts[SC_PROCESSSCTRUCT]) g_shortCuts[SC_PROCESSSCTRUCT] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_STRUCTS), this, SLOT(_structs()));
        if (!g_shortCuts[SC_PROCESSDUMPTOFILE])
            g_shortCuts[SC_PROCESSDUMPTOFILE] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_DUMPTOFILE), this, SLOT(_dumpToFile()));
        if (!g_shortCuts[SC_PROCESSMEMORYHEX])
            g_shortCuts[SC_PROCESSMEMORYHEX] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_HEX), this, SLOT(_memoryHex()));
        if (!g_shortCuts[SC_PROCESSMEMORYSTRINGS])
            g_shortCuts[SC_PROCESSMEMORYSTRINGS] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_STRINGS), this, SLOT(_memoryStrings()));
        if (!g_shortCuts[SC_PROCESSMEMORYSIGNATURES])
            g_shortCuts[SC_PROCESSMEMORYSIGNATURES] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_SIGNATURES), this, SLOT(_memorySignatures()));
        if (!g_shortCuts[SC_PROCESSFILEVIEWER])
            g_shortCuts[SC_PROCESSFILEVIEWER] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_SHOWIN_VIEWER), this, SLOT(_showInViewer()));
        if (!g_shortCuts[SC_PROCESSFILEFOLDER])
            g_shortCuts[SC_PROCESSFILEFOLDER] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_SHOWIN_FOLDER), this, SLOT(_showInFolder()));
        if (!g_shortCuts[SC_PROCESSFILECOPYFILENAME])
            g_shortCuts[SC_PROCESSFILECOPYFILENAME] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_COPY_FILENAME), this, SLOT(_copyFilename()));
        if (!g_shortCuts[SC_PROCESSFILECOPYNAME])
            g_shortCuts[SC_PROCESSFILECOPYFILENAME] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_COPY_NAME), this, SLOT(_copyName()));
        if (!g_shortCuts[SC_PROCESSFILECOPYPID])
            g_shortCuts[SC_PROCESSFILECOPYFILENAME] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_COPY_PID), this, SLOT(_copyPID()));
        if (!g_shortCuts[SC_PROCESSMEMORYMAP])
            g_shortCuts[SC_PROCESSMEMORYMAP] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_MEMORYMAP), this, SLOT(_memoryMap()));
        if (!g_shortCuts[SC_PROCESSMODULES])
            g_shortCuts[SC_PROCESSMODULES] = new QShortcut(getShortcuts()->getShortcut(X_ID_PROCESS_MEMORY_MODULES), this, SLOT(_modules()));
    } else {
        for (qint32 i = 0; i < __SC_SIZE; i++) {
            if (g_shortCuts[i]) {
                delete g_shortCuts[i];
                g_shortCuts[i] = nullptr;
            }
        }
    }
}

void XProcessWidget::errorMessageSlot(QString sErrorMessage)
{
    QMessageBox::critical(XOptions::getMainWidget(this), tr("Error"), sErrorMessage);
}

void XProcessWidget::on_comboBoxMode_currentIndexChanged(int nIndex)
{
#ifdef Q_OS_WIN
    // if (nIndex == 1)  // Kernel mode
    // {
    //     if (!g_bIsDriverLoaded) {
    //         QString sDriverFileName = getGlobalOptions()->getValue(XOptions::ID_IODRIVER_FILENAME).toString();
    //         QString sServiceName = getGlobalOptions()->getValue(XOptions::ID_IODRIVER_SERVICENAME).toString();

    //         sDriverFileName = XBinary::convertPathName(sDriverFileName);

    //         g_bIsDriverLoaded = XWinIODriver().loadDriver(sDriverFileName, sServiceName);

    //         if (g_bIsDriverLoaded) {
    //             g_sServiceName = sServiceName;
    //         }
    //     }

    //     if (!g_bIsDriverLoaded) {
    //         QMessageBox::critical(this, tr("Error"), QString("%1").arg(tr("Cannot load driver")));

    //         ui->comboBoxMode->setCurrentIndex(0);
    //     }
    // }
#else
    Q_UNUSED(nIndex)
#endif
}
#ifdef XDYNSTRUCTS
XDynStructsEngine::IOMODE XProcessWidget::getCurrentIOMode()
{
    XDynStructsEngine::IOMODE result = XDynStructsEngine::IOMODE_PROCESS_USER;

#ifdef Q_OS_WIN
    if (ui->comboBoxMode->currentIndex() == 1) {
        result = XDynStructsEngine::IOMODE_PROCESS_KERNEL;
    }
#endif

    return result;
}
#endif
void XProcessWidget::on_checkBoxShowAll_toggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    reload();
}

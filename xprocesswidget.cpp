// copyright (c) 2020-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "xprocesswidget.h"
#include "ui_xprocesswidget.h"

XProcessWidget::XProcessWidget(QWidget *pParent) :
    XShortcutsWidget(pParent),
    ui(new Ui::XProcessWidget)
{
    ui->setupUi(this);

    g_scProcessSctruct=nullptr;
    g_scProcessSctruct=nullptr;
    g_scProcessDumpToFile=nullptr;
    g_scProcessMemoryHex=nullptr;
    g_scProcessMemoryStrings=nullptr;
    g_scProcessMemorySignatures=nullptr;
    g_scProcessFileViewer=nullptr;
    g_scProcessFileCopyFileName=nullptr;

    // mb TODO auto refresh

    ui->tableWidgetProcesses->setColumnCount(COLUMN_size);
    ui->tableWidgetProcesses->setRowCount(0);

    ui->tableWidgetProcesses->setHorizontalHeaderItem(COLUMN_ID,new QTableWidgetItem(QString("PID")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(COLUMN_NAME,new QTableWidgetItem(tr("Name")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(COLUMN_FILENAME,new QTableWidgetItem(tr("File name")));

    ui->tableWidgetProcesses->setColumnWidth(COLUMN_ID,50);
    ui->tableWidgetProcesses->setColumnWidth(COLUMN_NAME,100);
    ui->tableWidgetProcesses->setColumnWidth(COLUMN_FILENAME,300);

    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_ID,QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_NAME,QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_FILENAME,QHeaderView::Stretch);

    reload();
}

XProcessWidget::~XProcessWidget()
{
    delete ui;
}

void XProcessWidget::setOptions(OPTIONS options)
{
    g_options=options;
}

void XProcessWidget::setShortcuts(XShortcuts *pShortcuts)
{
    XShortcutsWidget::setShortcuts(pShortcuts);
}

void XProcessWidget::reload()
{
    // TODO TableView

    qint64 nPID=-1;

    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        nPID=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();
    }

    QList<XProcess::PROCESS_INFO> listProcesses=XProcess::getProcessesList();

    int nCount=listProcesses.count();

    ui->tableWidgetProcesses->setSortingEnabled(false);

    ui->tableWidgetProcesses->setRowCount(0);
    ui->tableWidgetProcesses->setRowCount(nCount);

    for(int i=0;i<nCount;i++)
    {
        QTableWidgetItem *pItemID=new QTableWidgetItem;
        pItemID->setData(Qt::DisplayRole,listProcesses.at(i).nID);
        pItemID->setData(Qt::UserRole+CBDATA_PID,listProcesses.at(i).nID);
        pItemID->setData(Qt::UserRole+CBDATA_NAME,listProcesses.at(i).sName);
        pItemID->setData(Qt::UserRole+CBDATA_FILEPATH,listProcesses.at(i).sFilePath);
        pItemID->setData(Qt::UserRole+CBDATA_IMAGEADDRESS,listProcesses.at(i).nImageAddress);
        pItemID->setData(Qt::UserRole+CBDATA_IMAGESIZE,listProcesses.at(i).nImageSize);
        ui->tableWidgetProcesses->setItem(i,COLUMN_ID,pItemID);

        QTableWidgetItem *pItemName=new QTableWidgetItem;
        pItemName->setText(listProcesses.at(i).sName);
        ui->tableWidgetProcesses->setItem(i,COLUMN_NAME,pItemName);

        QTableWidgetItem *pItemPath=new QTableWidgetItem;
        pItemPath->setText(listProcesses.at(i).sFilePath);
        ui->tableWidgetProcesses->setItem(i,COLUMN_FILENAME,pItemPath);
    }

    ui->tableWidgetProcesses->setSortingEnabled(true);

    // Restore row
    if(nPID!=-1)
    {
        for(int i=0;i<nCount;i++)
        {
            qint64 _nPID=ui->tableWidgetProcesses->item(i,COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();

            if(nPID==_nPID)
            {
                ui->tableWidgetProcesses->setCurrentCell(i,COLUMN_ID);

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
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        QMenu menuContext(this);

        QAction actionStructs(tr("Structs"),this);
        actionStructs.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_STRUCTS));
        connect(&actionStructs,SIGNAL(triggered()),this,SLOT(_structs()));
        menuContext.addAction(&actionStructs);

        QMenu menuMemory(tr("Memory"),this);

        QAction actionMemoryHex(tr("Hex"),this);
        actionMemoryHex.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_MEMORY_HEX));
        connect(&actionMemoryHex,SIGNAL(triggered()),this,SLOT(_memoryHex()));
        menuMemory.addAction(&actionMemoryHex);

        QAction actionMemoryStrings(tr("Strings"),this);
        actionMemoryStrings.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_MEMORY_STRINGS));
        connect(&actionMemoryStrings,SIGNAL(triggered()),this,SLOT(_memoryStrings()));
        menuMemory.addAction(&actionMemoryStrings);

        QAction actionMemorySignatures(tr("Signatures"),this);
        actionMemorySignatures.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_MEMORY_SIGNATURES));
        connect(&actionMemorySignatures,SIGNAL(triggered()),this,SLOT(_memorySignatures()));
        menuMemory.addAction(&actionMemorySignatures);

        menuContext.addMenu(&menuMemory);

        QMenu menuFile(tr("File"),this);

        QAction actionFileViewer(QString("PE %1").arg(tr("Viewer")),this); // TODO Windows/Linux/OSX
        actionFileViewer.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_FILE_VIEWER));
        connect(&actionFileViewer,SIGNAL(triggered()),this,SLOT(_fileViewer()));
        menuFile.addAction(&actionFileViewer);

        QAction actionFileCopyFileName(tr("Copy filename"),this);
        actionFileCopyFileName.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_FILE_COPYFILENAME));
        connect(&actionFileCopyFileName,SIGNAL(triggered()),this,SLOT(_fileCopyFileName()));
        menuFile.addAction(&actionFileCopyFileName);

        menuContext.addMenu(&menuFile);

        QAction actionDumpToFile(tr("Dump to file"),this);
        actionDumpToFile.setShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_DUMPTOFILE));
        connect(&actionDumpToFile,SIGNAL(triggered()),this,SLOT(_dumpToFile()));
        menuContext.addAction(&actionDumpToFile);

        menuContext.exec(ui->tableWidgetProcesses->viewport()->mapToGlobal(pos));
    }
}

void XProcessWidget::_memoryHex()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        qint64 nPID=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();
        qint64 nImageAddress=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGEADDRESS).toLongLong();
        qint64 nImageSize=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGESIZE).toLongLong();
        QString sName=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_NAME).toString();

        XProcessDevice pd;
        if(pd.openPID(nPID,nImageAddress,nImageSize,QIODevice::ReadOnly))
        {
            XHexView::OPTIONS options={};

            options.sTitle=sName;
            options.nStartAddress=nImageAddress;

            DialogHexView dialogHexView(this,&pd,options);
            // TODO setShortcuts
            dialogHexView.exec();

            pd.close();
        }
    }
}

void XProcessWidget::_memoryStrings()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        qint64 nPID=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();
        qint64 nImageAddress=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGEADDRESS).toLongLong();
        qint64 nImageSize=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGESIZE).toLongLong();
        QString sName=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_NAME).toString();

        XProcessDevice pd;
        if(pd.openPID(nPID,nImageAddress,nImageSize,QIODevice::ReadOnly))
        {
            SearchStringsWidget::OPTIONS options={};
            options.bAnsi=true;
            options.bUnicode=true;
            options.bCStrings=true;
            options.sTitle=sName;
            options.nBaseAddress=nImageAddress;

            DialogSearchStrings dialogSearchStrings(this);

            dialogSearchStrings.setData(&pd,options,true);
            dialogSearchStrings.setShortcuts(getShortcuts());

            dialogSearchStrings.exec();

            pd.close();
        }
    }
}

void XProcessWidget::_memorySignatures()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        qint64 nPID=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();
        qint64 nImageAddress=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGEADDRESS).toLongLong();
        qint64 nImageSize=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGESIZE).toLongLong();
//        QString sName=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_NAME).toString();

        XProcessDevice pd;
        if(pd.openPID(nPID,nImageAddress,nImageSize,QIODevice::ReadOnly))
        {
            DialogSearchSignatures dialogSearchSignatures(this);

            SearchSignaturesWidget::OPTIONS options={};
            options.sSignaturesPath=g_options.sSearchSignaturesPath;

            dialogSearchSignatures.setData(&pd,XBinary::FT_REGION,options,false);
            dialogSearchSignatures.setShortcuts(getShortcuts());

            dialogSearchSignatures.exec();

            pd.close();
        }
    }
}

void XProcessWidget::_fileViewer()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        QString sFilePath=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_FILEPATH).toString();

        QFile file;

        file.setFileName(sFilePath);

        if(file.open(QIODevice::ReadOnly))
        {
            FW_DEF::OPTIONS options={};

            options.sTitle=sFilePath;
            options.nStartType=SPE::TYPE_HEURISTICSCAN;
            options.sSearchSignaturesPath=g_options.sSearchSignaturesPath;

            // TODO Windows/Linux/OSX
            DialogPE dialogPE(this);
            dialogPE.setData(&file,options);
            dialogPE.setShortcuts(getShortcuts());

            dialogPE.exec();

            file.close();
        }
    }
}

void XProcessWidget::_fileCopyFileName()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        QString sFilePath=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_FILEPATH).toString();

        QClipboard *clipboard=QApplication::clipboard();
        clipboard->setText(sFilePath);
    }
}

void XProcessWidget::_structs()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        qint64 nPID=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();
        qint64 nImageAddress=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGEADDRESS).toLongLong();

        DialogXDynStructs dialogXDynStructs(this);

        XDynStructsEngine::OPTIONS options={};

        options.nProcessId=nPID;
        options.nAddress=nImageAddress;
        options.sStructsPath=g_options.sStructsPath;
        options.bSystem=true;
        options.bGeneral=true;
        options.bCustom=true;

        dialogXDynStructs.setData(options);
        dialogXDynStructs.setShortcuts(getShortcuts());

        dialogXDynStructs.exec();
    }
}

void XProcessWidget::_dumpToFile()
{
    QList<QTableWidgetItem*> listSelected=ui->tableWidgetProcesses->selectedItems();

    if(listSelected.count())
    {
        qint64 nPID=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_PID).toLongLong();
        qint64 nImageAddress=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGEADDRESS).toLongLong();
        qint64 nImageSize=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_IMAGESIZE).toLongLong();
        QString sName=listSelected.at(COLUMN_ID)->data(Qt::UserRole+CBDATA_NAME).toString();

        XProcessDevice pd;
        if(pd.openPID(nPID,nImageAddress,nImageSize,QIODevice::ReadOnly))
        {
            QString sSaveFileName=QString("%1.%2.bin").arg(sName,tr("Dump"));
            QString sFileName=QFileDialog::getSaveFileName(this,tr("Save dump"),sSaveFileName,QString("%1 (*.bin)").arg(tr("Raw data")));

            if(!sFileName.isEmpty())
            {
                DialogDumpProcess dialogDumpProcess(this,&pd,0,nImageSize,sFileName,DumpProcess::DT_OFFSET);

                dialogDumpProcess.exec();

                pd.close();
            }
        }
    }
}

void XProcessWidget::on_pushButtonProcessesReload_clicked()
{
    reload();
}

void XProcessWidget::on_pushButtonProcessStructs_clicked()
{
    _structs();
}

void XProcessWidget::on_pushButtonProcessesSave_clicked()
{
    QString sFileName=tr("Processes");
    sFileName=QFileDialog::getSaveFileName(this, tr("Save file"),sFileName, QString("%1 (*.txt);;%2 (*)").arg(tr("Text files"),tr("All files")));

    if(!sFileName.isEmpty())
    {
        XOptions::saveTableWidget(ui->tableWidgetProcesses,sFileName);
    }
}

void XProcessWidget::on_pushButtonProcessHex_clicked()
{
    _memoryHex();
}

void XProcessWidget::on_pushButtonProcessStrings_clicked()
{
    _memoryStrings();
}

void XProcessWidget::on_pushButtonSignatures_clicked()
{
    _memorySignatures();
}

void XProcessWidget::registerShortcuts(bool bState)
{
    if(bState)
    {
        if(!g_scProcessSctruct)             g_scProcessSctruct                  =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_STRUCTS),             this,SLOT(_struct()));
        if(!g_scProcessDumpToFile)          g_scProcessDumpToFile               =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_DUMPTOFILE),          this,SLOT(_dumpToFile()));
        if(!g_scProcessMemoryHex)           g_scProcessMemoryHex                =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_MEMORY_HEX),          this,SLOT(_memoryHex()));
        if(!g_scProcessMemoryStrings)       g_scProcessMemoryStrings            =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_MEMORY_STRINGS),      this,SLOT(_memoryStrings()));
        if(!g_scProcessMemorySignatures)    g_scProcessMemorySignatures         =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_MEMORY_SIGNATURES),   this,SLOT(_memorySignatures()));
        if(!g_scProcessFileViewer)          g_scProcessFileViewer               =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_FILE_VIEWER),         this,SLOT(_fileViewer()));
        if(!g_scProcessFileCopyFileName)    g_scProcessFileCopyFileName         =new QShortcut(getShortcuts()->getShortcut(XShortcuts::ID_PROCESS_FILE_COPYFILENAME),   this,SLOT(_fileCopyFileName()));
    }
    else
    {
        if(g_scProcessSctruct)              {delete g_scProcessSctruct;                 g_scProcessSctruct=nullptr;}
        if(g_scProcessDumpToFile)           {delete g_scProcessDumpToFile;              g_scProcessDumpToFile=nullptr;}
        if(g_scProcessMemoryHex)            {delete g_scProcessMemoryHex;               g_scProcessMemoryHex=nullptr;}
        if(g_scProcessMemoryStrings)        {delete g_scProcessMemoryStrings;           g_scProcessMemoryStrings=nullptr;}
        if(g_scProcessMemorySignatures)     {delete g_scProcessMemorySignatures;        g_scProcessMemorySignatures=nullptr;}
        if(g_scProcessFileViewer)           {delete g_scProcessFileViewer;              g_scProcessFileViewer=nullptr;}
        if(g_scProcessFileCopyFileName)     {delete g_scProcessFileCopyFileName;        g_scProcessFileCopyFileName=nullptr;}
    }
}

void XProcessWidget::on_pushButtonProcessesFileViewer_clicked()
{
    _fileViewer();
}

// copyright (c) 2020 hors<horsicq@gmail.com>
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

void XProcessWidget::reload()
{
    // TODO TableView
    QList<XProcess::PROCESS_INFO> listProcesses=XProcess::getProcessesList();

    int nCount=listProcesses.count();

    ui->tableWidgetProcesses->setSortingEnabled(false);

    ui->tableWidgetProcesses->setRowCount(0);
    ui->tableWidgetProcesses->setRowCount(nCount);

    for(int i=0;i<nCount;i++)
    {
        QTableWidgetItem *pItemID=new QTableWidgetItem;
        pItemID->setData(Qt::DisplayRole,listProcesses.at(i).nID);
        ui->tableWidgetProcesses->setItem(i,COLUMN_ID,pItemID);

        QTableWidgetItem *pItemName=new QTableWidgetItem;
        pItemName->setText(listProcesses.at(i).sName);
        ui->tableWidgetProcesses->setItem(i,COLUMN_NAME,pItemName);

        QTableWidgetItem *pItemPath=new QTableWidgetItem;
        pItemPath->setText(listProcesses.at(i).sFilePath);
        ui->tableWidgetProcesses->setItem(i,COLUMN_FILENAME,pItemPath);
    }

    ui->tableWidgetProcesses->setSortingEnabled(true);
}

void XProcessWidget::on_tableWidgetProcesses_customContextMenuRequested(const QPoint &pos)
{
    // TODO Shortcuts
    // TODO Dump
    // TODO File -> Copy Filename
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        QMenu menuContext(this);

        QAction actionStructs(tr("Structs"),this);
        connect(&actionStructs,SIGNAL(triggered()),this,SLOT(_structs()));
        menuContext.addAction(&actionStructs);

        QMenu menuMemory(tr("Memory"),this);

        QAction actionMemoryHex(tr("Hex"),this);
        connect(&actionMemoryHex,SIGNAL(triggered()),this,SLOT(_memoryHex()));
        menuMemory.addAction(&actionMemoryHex);

        menuContext.addMenu(&menuMemory);

        QAction actionDumpToFile(tr("Dump to file"),this);
        connect(&actionDumpToFile,SIGNAL(triggered()),this,SLOT(_dumpToFile()));
        menuContext.addAction(&actionDumpToFile);

        menuContext.exec(ui->tableWidgetProcesses->viewport()->mapToGlobal(pos));
    }
}

void XProcessWidget::_memoryHex()
{
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        QString sFilePath=ui->tableWidgetProcesses->selectedItems().at(COLUMN_FILENAME)->data(Qt::DisplayRole).toString();

        QFile file;
        file.setFileName(sFilePath);

        if(file.open(QIODevice::ReadOnly))
        {
            XBinary binary(&file);

//            XProcessDialogHex xpdh(this,&binary);
//            xpdh.exec();

            file.close();
        }
    }
}

void XProcessWidget::_hexMemory()
{
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        qint64 nPID=ui->tableWidgetProcesses->selectedItems().at(COLUMN_ID)->data(Qt::DisplayRole).toLongLong();

        XProcess::PROCESS_INFO processInfo=XProcess::getInfoByProcessID(nPID);

        XProcessDevice pd;
        if(pd.openPID(nPID,processInfo.nImageAddress,processInfo.nImageSize,QIODevice::ReadOnly))
        {
            XBinary binary(&pd,true,processInfo.nImageAddress);

//            XProcessDialogHex xpdh(this,&binary);
//            xpdh.exec();

            pd.close();
        }
    }
}

void XProcessWidget::_structs()
{
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        qint64 nPID=ui->tableWidgetProcesses->selectedItems().at(COLUMN_ID)->data(Qt::DisplayRole).toLongLong();

//        XProcessDialogSystemStructs xpdss(this);

//        xpdss.exec();
    }
}

void XProcessWidget::_dumpToFile()
{
//    QString sFilter;
//    sFilter+=QString("%1 (*.bin)").arg(tr("Raw data"));
//    QString sSaveFileName=XBinary::getResultFileName(getDevice(),QString("%1.bin").arg(tr("Dump")));
//    QString sFileName=QFileDialog::getSaveFileName(this,tr("Save dump"),sSaveFileName,sFilter);

//    if(!sFileName.isEmpty())
//    {
//        STATE state=getState();

//        DialogDumpProcess dd(this,getDevice(),state.nSelectionOffset,state.nSelectionSize,sFileName,DumpProcess::DT_OFFSET);

//        dd.exec();
//    }
}

void XProcessWidget::_strings()
{
    // TODO
    qDebug("Strings");
}

void XProcessWidget::on_pushButtonProcessesReload_clicked()
{
    reload();
}

void XProcessWidget::on_pushButtonProcessStructs_clicked()
{

}


void XProcessWidget::on_pushButtonProcessesSave_clicked()
{

}

void XProcessWidget::on_pushButtonProcessHex_clicked()
{

}

void XProcessWidget::on_pushButtonProcessStrings_clicked()
{

}

void XProcessWidget::on_pushButtonSignatures_clicked()
{

}

void XProcessWidget::registerShortcuts(bool bState)
{

}

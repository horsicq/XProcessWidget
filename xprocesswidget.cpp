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

XProcessWidget::XProcessWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XProcessWidget)
{
    ui->setupUi(this);

    ui->tableWidgetProcesses->setColumnCount(CN_size);
    ui->tableWidgetProcesses->setRowCount(0);

    ui->tableWidgetProcesses->setHorizontalHeaderItem(CN_ID,new QTableWidgetItem(QString("PID")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(CN_NAME,new QTableWidgetItem(tr("Name")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(CN_FILENAME,new QTableWidgetItem(tr("File name")));

    ui->tableWidgetProcesses->setColumnWidth(CN_ID,50);
    ui->tableWidgetProcesses->setColumnWidth(CN_NAME,100);
    ui->tableWidgetProcesses->setColumnWidth(CN_FILENAME,300);

    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(CN_ID,QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(CN_NAME,QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(CN_FILENAME,QHeaderView::Stretch);

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
        ui->tableWidgetProcesses->setItem(i,CN_ID,pItemID);

        QTableWidgetItem *pItemName=new QTableWidgetItem;
        pItemName->setText(listProcesses.at(i).sName);
        ui->tableWidgetProcesses->setItem(i,CN_NAME,pItemName);

        QTableWidgetItem *pItemPath=new QTableWidgetItem;
        pItemPath->setText(listProcesses.at(i).sFilePath);
        ui->tableWidgetProcesses->setItem(i,CN_FILENAME,pItemPath);
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
        QMenu contextMenu(this);

        QAction actionStructs(tr("Structs"),this);
        connect(&actionStructs,SIGNAL(triggered()),this,SLOT(_structs()));
        contextMenu.addAction(&actionStructs);

        QAction actionHexFile(QString("%1(%2)").arg(tr("Hex")).arg(tr("File")),this);
        connect(&actionHexFile,SIGNAL(triggered()),this,SLOT(_hexFile()));
        contextMenu.addAction(&actionHexFile);

        QAction actionHexMemory(QString("%1(%2)").arg(tr("Hex")).arg(tr("Memory")),this);
        connect(&actionHexMemory,SIGNAL(triggered()),this,SLOT(_hexMemory()));
        contextMenu.addAction(&actionHexMemory);

        QAction actionStrings(tr("Strings"),this);
        connect(&actionStrings,SIGNAL(triggered()),this,SLOT(_strings()));
        contextMenu.addAction(&actionStrings);

        // Add Strings
        // PE Editor

        contextMenu.exec(ui->tableWidgetProcesses->viewport()->mapToGlobal(pos));
    }
}

void XProcessWidget::_hexFile()
{
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        QString sFilePath=ui->tableWidgetProcesses->selectedItems().at(CN_FILENAME)->data(Qt::DisplayRole).toString();

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
        qint64 nPID=ui->tableWidgetProcesses->selectedItems().at(CN_ID)->data(Qt::DisplayRole).toLongLong();

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
        qint64 nPID=ui->tableWidgetProcesses->selectedItems().at(CN_ID)->data(Qt::DisplayRole).toLongLong();

//        XProcessDialogSystemStructs xpdss(this);

//        xpdss.exec();
    }
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


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

    ui->tableWidgetProcesses->setHorizontalHeaderItem(CN_ID,new QTableWidgetItem(tr("ID")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(CN_NAME,new QTableWidgetItem(tr("Name")));
    ui->tableWidgetProcesses->setHorizontalHeaderItem(CN_FILEPATH,new QTableWidgetItem(tr("File path")));

    ui->tableWidgetProcesses->setColumnWidth(CN_ID,50);
    ui->tableWidgetProcesses->setColumnWidth(CN_NAME,100);
    ui->tableWidgetProcesses->setColumnWidth(CN_FILEPATH,300);

    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(CN_ID,QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(CN_NAME,QHeaderView::Interactive);
    ui->tableWidgetProcesses->horizontalHeader()->setSectionResizeMode(CN_FILEPATH,QHeaderView::Stretch);

    reload();
}

XProcessWidget::~XProcessWidget()
{
    delete ui;
}

void XProcessWidget::on_pushButtonReload_clicked()
{
    reload();
}

void XProcessWidget::reload()
{
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
        ui->tableWidgetProcesses->setItem(i,CN_FILEPATH,pItemPath);
    }

    ui->tableWidgetProcesses->setSortingEnabled(true);
}

void XProcessWidget::on_tableWidgetProcesses_customContextMenuRequested(const QPoint &pos)
{
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        QMenu contextMenu(this);

        QAction actionHex(tr("Hex"),this);
        connect(&actionHex,SIGNAL(triggered()),this,SLOT(_hex()));

        contextMenu.addAction(&actionHex);

        contextMenu.exec(ui->tableWidgetProcesses->viewport()->mapToGlobal(pos));
    }
}

void XProcessWidget::_hex()
{
    if(ui->tableWidgetProcesses->selectedItems().count())
    {
        qint64 nPID=ui->tableWidgetProcesses->selectedItems().at(0)->data(Qt::DisplayRole).toLongLong();

        XProcess::PROCESS_INFO processInfo=XProcess::getInfoByProcessID(nPID);

        XProcessDevice pd;
        if(pd.openPID(nPID,processInfo.nImageAddress,processInfo.nImageSize,QIODevice::ReadOnly))
        {
            XPE pe(&pd,true,processInfo.nImageAddress);

            XProcessDialogHex xpdh(this,&pe);
            xpdh.exec();
        }
    }
}

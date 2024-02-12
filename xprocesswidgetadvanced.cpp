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
#include "xprocesswidgetadvanced.h"
#include "ui_xprocesswidgetadvanced.h"

XProcessWidgetAdvanced::XProcessWidgetAdvanced(QWidget *pParent) :
    XShortcutsWidget(pParent),
    ui(new Ui::XProcessWidgetAdvanced)
{
    ui->setupUi(this);
}

XProcessWidgetAdvanced::~XProcessWidgetAdvanced()
{
    delete ui;
}

void XProcessWidgetAdvanced::reload()
{
    QList<XProcess::PROCESS_INFO> listProcessInfo;
    // TODO
    DialogHandleInfoProcess dip(XOptions::getMainWidget(this));

    dip.setData(&listProcessInfo);

    dip.showDialogDelay();

    if (dip.isSuccess()) {
        QAbstractItemModel *pOldModel = ui->tableViewProcesses->model();

        qint32 nNumberOfRecords = listProcessInfo.count();

        QStandardItemModel *pModel = new QStandardItemModel(nNumberOfRecords, COLUMN_SIZE);

        pModel->setHeaderData(COLUMN_PID, Qt::Horizontal, QString("PID"));
        pModel->setHeaderData(COLUMN_NAME, Qt::Horizontal, tr("Name"));
        pModel->setHeaderData(COLUMN_FILENAME, Qt::Horizontal, tr("File name"));

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setData((qint32)(listProcessInfo.at(i).nID), Qt::DisplayRole);

                // pItem->setData(extractor_data.listRecords.at(i).nOffset, Qt::UserRole + 0);
                // pItem->setData(extractor_data.listRecords.at(i).nSize, Qt::UserRole + 1);
                // pItem->setData(extractor_data.listRecords.at(i).sExt, Qt::UserRole + 2);

                pModel->setItem(i, COLUMN_PID, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(listProcessInfo.at(i).sName);
                pModel->setItem(i, COLUMN_NAME, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(listProcessInfo.at(i).sFilePath);
                pModel->setItem(i, COLUMN_FILENAME, pItem);
            }
        }

        XOptions::setModelTextAlignment(pModel, COLUMN_PID, Qt::AlignRight | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, COLUMN_NAME, Qt::AlignLeft | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, COLUMN_FILENAME, Qt::AlignLeft | Qt::AlignVCenter);

        ui->tableViewProcesses->setModel(pModel);

        deleteOldAbstractModel(&pOldModel);

        ui->tableViewProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_FILENAME, QHeaderView::Stretch);
        ui->tableViewProcesses->sortByColumn(COLUMN_PID, Qt::AscendingOrder);
    }
}

void XProcessWidgetAdvanced::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
    // TODO
}

void XProcessWidgetAdvanced::on_pushButtonReloadProcesses_clicked()
{
    reload();
}

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

    ui->comboBoxProcessesMode->blockSignals(true);

    ui->comboBoxProcessesMode->addItem(tr("All"), XProcess::PIO_VALID);
    ui->comboBoxProcessesMode->addItem(QString(".NET"), XProcess::PIO_NET);

    ui->comboBoxProcessesMode->blockSignals(false);
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

    XProcess::PROCESS_INFO_OPTIONS piOptions = {};
    piOptions.pio = (XProcess::PIO)(ui->comboBoxProcessesMode->currentData().toInt());

    dip.setData(piOptions, &listProcessInfo);

    dip.showDialogDelay();

    if (dip.isSuccess()) {
        QAbstractItemModel *pOldModel = ui->tableViewProcesses->model();

        qint32 nNumberOfRecords = listProcessInfo.count();
        qint32 nNumberOfRows = 0;

        if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
            nNumberOfRows = __COLUMN_ALL_SIZE;
        } else if (piOptions.pio == XProcess::PIO_NET) {
            nNumberOfRows = __COLUMN_NET_SIZE;
        }

        QStandardItemModel *pModel = new QStandardItemModel(nNumberOfRecords, nNumberOfRows);

        if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
            pModel->setHeaderData(COLUMN_ALL_PID, Qt::Horizontal, QString("PID"));
            pModel->setHeaderData(COLUMN_ALL_NAME, Qt::Horizontal, tr("Name"));
            pModel->setHeaderData(COLUMN_ALL_INFO, Qt::Horizontal, tr("Info"));
            pModel->setHeaderData(COLUMN_ALL_FILENAME, Qt::Horizontal, tr("File name"));
        } else if (piOptions.pio == XProcess::PIO_NET) {
            pModel->setHeaderData(COLUMN_NET_PID, Qt::Horizontal, QString("PID"));
            pModel->setHeaderData(COLUMN_NET_NAME, Qt::Horizontal, tr("Name"));
            pModel->setHeaderData(COLUMN_NET_VERSION, Qt::Horizontal, tr("Version"));
            pModel->setHeaderData(COLUMN_NET_INFO, Qt::Horizontal, tr("Info"));
            pModel->setHeaderData(COLUMN_NET_FILENAME, Qt::Horizontal, tr("File name"));
        }

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setData((qint32)(listProcessInfo.at(i).nID), Qt::DisplayRole);

                // pItem->setData(extractor_data.listRecords.at(i).nOffset, Qt::UserRole + 0);
                // pItem->setData(extractor_data.listRecords.at(i).nSize, Qt::UserRole + 1);
                // pItem->setData(extractor_data.listRecords.at(i).sExt, Qt::UserRole + 2);

                if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
                    pModel->setItem(i, COLUMN_ALL_PID, pItem);
                } else if (piOptions.pio == XProcess::PIO_NET) {
                    pModel->setItem(i, COLUMN_NET_PID, pItem);
                }
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(listProcessInfo.at(i).sName);

                if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
                    pModel->setItem(i, COLUMN_ALL_NAME, pItem);
                } else if (piOptions.pio == XProcess::PIO_NET) {
                    pModel->setItem(i, COLUMN_NET_NAME, pItem);
                }
            }
            if (piOptions.pio == XProcess::PIO_NET) {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(listProcessInfo.at(i).sInfoExtra);
                pModel->setItem(i, COLUMN_NET_INFO, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(listProcessInfo.at(i).sInfo);

                if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
                    pModel->setItem(i, COLUMN_ALL_INFO, pItem);
                } else if (piOptions.pio == XProcess::PIO_NET) {
                    pModel->setItem(i, COLUMN_NET_INFO, pItem);
                }
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(listProcessInfo.at(i).sFilePath);

                if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
                    pModel->setItem(i, COLUMN_ALL_FILENAME, pItem);
                } else if (piOptions.pio == XProcess::PIO_NET) {
                    pModel->setItem(i, COLUMN_NET_FILENAME, pItem);
                }
            }
        }

        if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
            XOptions::setModelTextAlignment(pModel, COLUMN_ALL_PID, Qt::AlignRight | Qt::AlignVCenter);
            XOptions::setModelTextAlignment(pModel, COLUMN_ALL_NAME, Qt::AlignLeft | Qt::AlignVCenter);
            XOptions::setModelTextAlignment(pModel, COLUMN_ALL_FILENAME, Qt::AlignLeft | Qt::AlignVCenter);
        } else if (piOptions.pio == XProcess::PIO_NET) {
            XOptions::setModelTextAlignment(pModel, COLUMN_NET_PID, Qt::AlignRight | Qt::AlignVCenter);
            XOptions::setModelTextAlignment(pModel, COLUMN_NET_NAME, Qt::AlignLeft | Qt::AlignVCenter);
            XOptions::setModelTextAlignment(pModel, COLUMN_NET_FILENAME, Qt::AlignLeft | Qt::AlignVCenter);
        }

        ui->tableViewProcesses->setModel(pModel);

        deleteOldAbstractModel(&pOldModel);

        if ((piOptions.pio == XProcess::PIO_ALL) || (piOptions.pio == XProcess::PIO_VALID)) {
            ui->tableViewProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_ALL_FILENAME, QHeaderView::Stretch);
        } else if (piOptions.pio == XProcess::PIO_NET) {
            ui->tableViewProcesses->horizontalHeader()->setSectionResizeMode(COLUMN_NET_FILENAME, QHeaderView::Stretch);
        }

        ui->tableViewProcesses->sortByColumn(0, Qt::AscendingOrder);
    }
}

void XProcessWidgetAdvanced::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    pXOptions->setComboBox(ui->comboBoxProcessesScan, XOptions::ID_SCAN_ENGINE);

    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
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

void XProcessWidgetAdvanced::on_pushButtonSaveProcesses_clicked()
{
    QString sResultFileName = QString("%1.txt").arg(tr("Processes"));

    QAbstractItemModel *pModel = ui->tableViewProcesses->model();

    XShortcutsWidget::saveTableModel(pModel, sResultFileName);
}

void XProcessWidgetAdvanced::on_comboBoxMode_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

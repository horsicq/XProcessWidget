/* Copyright (c) 2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "xhandleinfo.h"

XhandleInfo::XhandleInfo(QObject *pParent) : QObject(pParent)
{
    g_pListProcesses = nullptr;
    g_pPdStruct = nullptr;
    g_pDieScript = nullptr;
}

void XhandleInfo::setData(HANDLE_INFO_OPTIONS hiOptions, QList<XProcess::PROCESS_INFO> *pListProcesses, XBinary::PDSTRUCT *pPdStruct)
{
    g_hiOptions = hiOptions;
    g_pListProcesses = pListProcesses;
    g_pPdStruct = pPdStruct;
}

QString XhandleInfo::scanFile(const QString &sFileName)
{
    QString sResult;

    if (g_mapDetects.contains(sFileName)) {
        sResult = g_mapDetects.value(sFileName);
    } else {
        if (g_hiOptions.sScanEngine == "die") {
            DiE_Script::OPTIONS scanOptions = {};
            scanOptions.bIsDeepScan = true;
            scanOptions.bIsHeuristicScan = true;
            scanOptions.bIsVerbose = true;

            DiE_Script::SCAN_RESULT scanResult = g_pDieScript->scanFile(sFileName, &scanOptions, g_pPdStruct);
            sResult = XFormats::getProtection(&(scanResult.listRecords));
        } else if (g_hiOptions.sScanEngine == "nfd") {
            SpecAbstract::SCAN_OPTIONS scanOptions = {};
            scanOptions.bIsDeepScan = true;
            // scanOptions.bIsHeuristicScan = true;
            // scanOptions.bIsVerbose = true;

            SpecAbstract::SCAN_RESULT scanResult = StaticScan::processFile(sFileName, &scanOptions, g_pPdStruct);
            QList<XBinary::SCANSTRUCT> listScanResults = SpecAbstract::convert(&(scanResult.listRecords));
            sResult = XFormats::getProtection(&listScanResults);
        }

        g_mapDetects.insert(sFileName, sResult);
    }

    return sResult;
}

void XhandleInfo::process()
{
    g_mapDetects.clear();

    if (g_hiOptions.sScanEngine == "die") {
        g_pDieScript = new DiE_Script;
        g_pDieScript->loadDatabase(g_hiOptions.sDieDatabase, true);
        g_pDieScript->loadDatabase(g_hiOptions.sDieDatabaseCustom, false);
    }

    if (g_hiOptions.infoClass == INFOCLASS_PROCESSES) {
        qint32 nFreeIndex = XBinary::getFreeIndex(g_pPdStruct);

        g_pListProcesses->clear();

        QList<XProcess::PROCESS_INFO> listRecords = XProcess::getProcessesList(true, g_pPdStruct);

        qint32 nNumberOfRecords = listRecords.count();

        XBinary::setPdStructInit(g_pPdStruct, nFreeIndex, nNumberOfRecords);

        for (qint32 i = 0; (i < nNumberOfRecords) && (!g_pPdStruct->bIsStop); i++) {
            bool bAdd = false;

            XProcess::PROCESS_INFO record = listRecords.at(i);

            XBinary::setPdStructStatus(g_pPdStruct, nFreeIndex, listRecords.at(i).sName);

            QList<XProcess::MODULE> listModules = XProcess::getModulesList(record.nID, g_pPdStruct);

            if (g_hiOptions.pio == PIO_ALL) {
                bAdd = true;
            }

            if (!bAdd) {
                if ((record.sFilePath != "") && (XBinary::isFileExists(record.sFilePath))) {
                    if (g_hiOptions.pio == PIO_VALID) {
                        bAdd = true;
                    }

                    if (!bAdd) {
                        if (g_hiOptions.pio == PIO_NET) {
#ifdef Q_OS_WIN
                            if (XProcess::isModulePesent("clr.dll", &listModules, g_pPdStruct)) {
                                record.sInfoExtra += "clr.dll";
                                bAdd = true;
                            }

                            if (XProcess::isModulePesent("mscorwks.dll", &listModules, g_pPdStruct)) {
                                if (record.sInfoExtra != "") {
                                    record.sInfoExtra += ", ";
                                    record.sInfoExtra += "mscorwks.dll";
                                }
                                bAdd = true;
                            }

                            // TODO Check version
#endif
                        }
                    }
                }
            }

            if (bAdd) {
                if (g_hiOptions.pio == PIO_NET) {
                    QFile file;
                    file.setFileName(record.sFilePath);
                    if (file.open(QIODevice::ReadOnly)) {
#ifdef Q_OS_WIN
                        XPE pe(&file);
                        if (pe.isValid(g_pPdStruct)) {
                            XPE::CLI_INFO cliInfo = pe.getCliInfo(true);

                            if (cliInfo.bValid) {
                                record.sInfoExtra = cliInfo.metaData.header.sVersion;

                                if (cliInfo.bHidden) {
                                    record.sInfoExtra += QString("(Hidden)");
                                }
                            }
                        }
#endif
                        file.close();
                    }
                }

                record.sInfo = scanFile(record.sFilePath);

                if (g_hiOptions.bIsScanAll) {
                    if (record.sInfo == "") {
                        qint32 nNumberOfModules = listModules.count();

                        qint32 nFreeIndexModules = XBinary::getFreeIndex(g_pPdStruct);
                        XBinary::setPdStructInit(g_pPdStruct, nFreeIndexModules, nNumberOfModules);

                        for (qint32 j = 0; j < nNumberOfModules; j++) {
                            XBinary::setPdStructStatus(g_pPdStruct, nFreeIndexModules, listModules.at(j).sName);

                            QString sModuleDetect = scanFile(listModules.at(j).sFileName);

                            if (sModuleDetect != "") {
                                record.sInfo = listModules.at(j).sName;
                                break;
                            }

                            XBinary::setPdStructCurrent(g_pPdStruct, nFreeIndexModules, j);
                        }

                        XBinary::setPdStructFinished(g_pPdStruct, nFreeIndexModules);
                    }
                }

                g_pListProcesses->append(record);
            }

            XBinary::setPdStructCurrent(g_pPdStruct, nFreeIndex, i);
        }

        XBinary::setPdStructFinished(g_pPdStruct, nFreeIndex);

        emit completed(0);
    }

    if (g_hiOptions.sScanEngine == "die") {
        delete g_pDieScript;
    }
}

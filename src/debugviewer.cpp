//  This file is part of Qt Bitcoin Trader
//      https://github.com/JulyIGHOR/QtBitcoinTrader
//  Copyright (C) 2013-2017 July IGHOR <julyighor@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  In addition, as a special exception, the copyright holders give
//  permission to link the code of portions of this program with the
//  OpenSSL library under certain conditions as described in each
//  individual source file, and distribute linked combinations including
//  the two.
//
//  You must obey the GNU General Public License in all respects for all
//  of the code used other than OpenSSL. If you modify file(s) with this
//  exception, you may extend this exception to your version of the
//  file(s), but you are not obligated to do so. If you do not wish to do
//  so, delete this exception statement from your version. If you delete
//  this exception statement from all source files in the program, then
//  also delete it here.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "debugviewer.h"
#include <QScrollBar>
#include "main.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>
#include "timesync.h"

DebugViewer::DebugViewer()
	: QWidget()
{
	savingFile=false;
	ui.setupUi(this);
	ui.checkEnabled->setChecked(true);

	setWindowFlags(Qt::Window);
	setAttribute(Qt::WA_DeleteOnClose,true);

	if(baseValues.logThread_)
	{
		baseValues.logThread_->terminate();
		baseValues.logThread_->deleteLater();
		baseValues.logThread_=0;
	}

	logThread=new LogThread(false);
    connect(logThread,SIGNAL(sendLogSignal(QByteArray)),this,SLOT(sendLogSlot(QByteArray)));
    debugLevel=2;
	show();
}

DebugViewer::~DebugViewer()
{
	debugLevel=0;
	if(logThread)
	{
		logThread->terminate();
		logThread->deleteLater();
		logThread=0;
	}
}

void DebugViewer::on_buttonSaveAs_clicked()
{
	savingFile=true;
    QString fileName=QFileDialog::getSaveFileName(this, "Save Debug Information",QDateTime::fromTime_t(TimeSync::getTimeT()).toUTC().toString("yyyy-MM-dd HH.mm.ss")+".log","Log file (*.log)");
	if(fileName.isEmpty()){savingFile=false;return;}
	
	QFile writeLog(fileName);
	if(writeLog.open(QIODevice::WriteOnly))
	{
		writeLog.write("Qt Bitcoin Trader "+baseValues.appVerStr+"\r\n");

		QByteArray osLine;
#ifdef Q_OS_WIN
		osLine="OS: Windows "+QByteArray::number(QSysInfo::windowsVersion())+"\r\n";
#endif

#ifdef Q_OS_MAC
		osLine="OS: Mac OS "+QByteArray::number(QSysInfo::MacintoshVersion)+"\r\n";
#endif
		if(osLine.isEmpty())
			osLine="OS: Some Linux\r\n";

		writeLog.write(osLine);
		writeLog.write(ui.debugText->toPlainText().toLatin1());
		writeLog.close();
	}
	else
		QMessageBox::critical(this,windowTitle(),"Cannot save log file");

	savingFile=false;
}

void DebugViewer::sendLogSlot(QByteArray text)
{
	QStringList filterData(QString(text).split("\r\n"));
	for(int n=0;n<filterData.count();n++)
		if(filterData.at(n).startsWith("Cookie",Qt::CaseInsensitive))
			filterData[n]="Cookie: THERE_WAS_A_COOKIE";

    if(savingFile==false&&ui.checkEnabled->isChecked())
        ui.debugText->appendPlainText(filterData.join("\n"));
}

void DebugViewer::on_radioDebug_toggled(bool debugEnabled)
{
	if(debugEnabled)debugLevel=1;
	else debugLevel=2;
    ui.debugText->setPlainText("");
}

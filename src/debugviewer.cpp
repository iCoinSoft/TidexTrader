//  This file is part of TidexTrader
//      https://github.com/JulyIGHOR/QtBitcoinTrader
//  Copyright (C) 2013-2017 July IGHOR <julyighor@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

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
        writeLog.write("TidexTrader "+baseValues.appVerStr+"\r\n");

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

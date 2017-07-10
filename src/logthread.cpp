

#include "logthread.h"
#include <QDateTime>
#include <QFile>
#include <QApplication>
#include "main.h"
#include "timesync.h"

LogThread::LogThread(bool wrf)
	: QThread()
{
	writeFile=wrf;
	moveToThread(this);
	start();
}

LogThread::~LogThread()
{

}

void LogThread::run()
{
	connect(this,SIGNAL(writeLogSignal(QByteArray,int)),this,SLOT(writeLogSlot(QByteArray,int)));
	exec();
}

void LogThread::writeLog(QByteArray data, int dbLvl)
{
	if(debugLevel==0)return;

	if(debugLevel==2&&dbLvl!=2)return;//0: Disabled; 1: Debug; 2: Log

	emit writeLogSignal(data,dbLvl);
}

void LogThread::writeLogSlot(QByteArray data, int dbLvl)
{
    data="------------------\r\n"+QDateTime::fromTime_t(TimeSync::getTimeT()).toString("yyyy-MM-dd HH:mm:ss LVL:").toLatin1()+QByteArray::number(dbLvl)+"\r\n"+data+"\r\n------------------\r\n\r\n";
	if(writeFile)
	{
		QFile logFile(baseValues.logFileName);
		if(logFile.open(QIODevice::Append))
		{
			logFile.write(data);
			logFile.close();
		}
	}
	else
	emit sendLogSignal(data);
}

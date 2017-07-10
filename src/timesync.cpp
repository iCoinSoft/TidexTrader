

#include "timesync.h"
#include "main.h"
#include <QUdpSocket>
#include <QtEndian>
#include <QStandardPaths>
#include <QMessageBox>

TimeSync::TimeSync() : QObject()
{
    started=0;
    startTime=QDateTime::currentDateTime().toTime_t();
    timeShift=0;
    additionalTimer=0;
    getNTPTimeRetryCount=0;

    dateUpdateThread=new QThread;
    connect(dateUpdateThread,SIGNAL(started()),this,SLOT(runThread()));
    connect(this,SIGNAL(finishThread()),dateUpdateThread,SLOT(quit()));
    connect(this,SIGNAL(startSync()),this,SLOT(getNTPTime()));
    this->moveToThread(dateUpdateThread);
    dateUpdateThread->start();
}

TimeSync::~TimeSync()
{
    emit finishThread();
}

TimeSync* TimeSync::global()
{
    static TimeSync *instance=0;

    static QAtomicInt created=0;
    if(created)return instance;
    static QMutex mut;
    QMutexLocker lock(&mut);

    if(instance==0)
    {
        instance=new TimeSync;
        while(instance->started==0)QThread::msleep(100);
        created=1;
    }

    return instance;
}

quint32 TimeSync::getTimeT()
{
    TimeSync* timeSync=TimeSync::global();
    if(timeSync->additionalTimer==0)return QDateTime::currentDateTime().toTime_t();

    qint64 additionalBuffer=0;
    timeSync->mutex.lock();
    additionalBuffer=timeSync->additionalTimer->elapsed();
    timeSync->mutex.unlock();

    return timeSync->startTime+timeSync->timeShift+quint32(additionalBuffer/1000);
}

void TimeSync::syncNow()
{
    QString appDataDir=QStandardPaths::standardLocations(QStandardPaths::DataLocation).first().replace('\\','/')+"/";
    QSettings mainSettings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
    if(mainSettings.value("TimeSynchronization",true).toBool()){
        emit TimeSync::global()->startSync();
    }
}

void TimeSync::runThread()
{
    startTime=QDateTime::currentDateTime().toTime_t();

    additionalTimer=new QElapsedTimer();
    additionalTimer->start();

    started=1;
}

void TimeSync::getNTPTime()
{
    QUdpSocket sock;
    sock.connectToHost("0.pool.ntp.org",123);
    if(!sock.waitForConnected(1000))return;
    QByteArray data(48,char(0)); *(reinterpret_cast<qint32 *>(&data.data()[0]))=4194959577;
    if(sock.write(data)<0||!sock.waitForReadyRead(3000)||sock.bytesAvailable()!=48)return;
    data=sock.readAll();
    quint32 seconds=qToBigEndian(*(reinterpret_cast<quint32 *>(&data.data()[40])));
    quint32 fraction=qToBigEndian(*(reinterpret_cast<quint32 *>(&data.data()[44])));
    quint32 newTime=QDateTime::fromMSecsSinceEpoch(seconds*1000ll+fraction*1000ll/0x100000000ll-2208988800000ll).toTime_t();

    if(newTime<1451606400 || newTime>4000000000){
        QThread::msleep(500);
        emit startSync();
        return;
    }
    qint32 tempTimeShift=qint64(newTime)-qint64(QDateTime::currentDateTime().toTime_t());

    if(timeShift!=0)tempTimeShift=qint32((qint64(timeShift)+qint64(tempTimeShift))/2);

    if(tempTimeShift>3600 || tempTimeShift<-3600){
        static bool showMessage=true;
        if(showMessage)emit warningMessage(julyTr("TIME_SYNC_ERROR","Your clock is not set. Please close the TidexTrader and set the clock. Changing time at TidexTrader enabled can cause errors and damage the keys."));
        showMessage=false;
    }
    else
        timeShift.fetchAndStoreOrdered(tempTimeShift);

    getNTPTimeRetryCount++;
    if(getNTPTimeRetryCount<3)emit startSync();
}

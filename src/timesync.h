

#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <QThread>
#include <QElapsedTimer>
#include <QMutex>

class TimeSync : public QObject
{
    Q_OBJECT

public:
    TimeSync();
    ~TimeSync();

    static TimeSync* global();
    static quint32 getTimeT();
    static void syncNow();

    QAtomicInt started;

private:
    QThread *dateUpdateThread;
    quint32 startTime;
    QAtomicInt timeShift;
    QElapsedTimer *additionalTimer;
    QMutex mutex;
    int getNTPTimeRetryCount;

signals:
    void finishThread();
    void warningMessage(QString);
    void startSync();

private slots:
    void runThread();
    void getNTPTime();
};

#endif // TIMESYNC_H



#ifndef INDICATORENGINE_H
#define INDICATORENGINE_H

#include <QObject>
#include <QMutex>
#include <QHash>

class IndicatorEngine : public QObject
{
    Q_OBJECT
public:
    IndicatorEngine();
    ~IndicatorEngine();

    static IndicatorEngine* global();
    static void setValue(QString,QString,QString,double);
    static double getValue(QString);

private:
    QMutex locker;
    QHash<QByteArray,double> indicators;

private slots:
    void setValueSlot(QString,QString,QString,double);

signals:
    void indicatorChanged(QString,QString,QString,double);

    void indicatorHighChanged(QString,double);
    void indicatorLowChanged(QString,double);
    void indicatorSellChanged(QString,double);
    void indicatorBuyChanged(QString,double);
    void indicatorLastChanged(QString,double);
    void indicatorVolumeChanged(QString,double);

    void finishThread();
};

#endif // INDICATORENGINE_H

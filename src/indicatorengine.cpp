

#include <QThread>
#include "indicatorengine.h"
#include "main.h"

IndicatorEngine::IndicatorEngine() : QObject()
{
    connect(this,SIGNAL(indicatorChanged(QString,QString,QString,double)),this,SLOT(setValueSlot(QString,QString,QString,double)));

    connect(this,SIGNAL(indicatorHighChanged(QString, double)),baseValues_->mainWindow_,SLOT(indicatorHighChanged(QString, double)));
    connect(this,SIGNAL(indicatorLowChanged(QString, double)),baseValues_->mainWindow_,SLOT(indicatorLowChanged(QString, double)));
    connect(this,SIGNAL(indicatorSellChanged(QString, double)),baseValues_->mainWindow_,SLOT(indicatorSellChanged(QString, double)));
    connect(this,SIGNAL(indicatorBuyChanged(QString, double)),baseValues_->mainWindow_,SLOT(indicatorBuyChanged(QString, double)));
    connect(this,SIGNAL(indicatorLastChanged(QString, double)),baseValues_->mainWindow_,SLOT(indicatorLastChanged(QString, double)));
    connect(this,SIGNAL(indicatorVolumeChanged(QString, double)),baseValues_->mainWindow_,SLOT(indicatorVolumeChanged(QString, double)));

    QThread *indicatorEngineThread=new QThread;
    connect(this,SIGNAL(finishThread()),indicatorEngineThread,SLOT(quit()));
    this->moveToThread(indicatorEngineThread);
    indicatorEngineThread->start();
}

IndicatorEngine::~IndicatorEngine()
{
    emit finishThread();
}

//---------------------------------------- Static ----------------------------------------
IndicatorEngine* IndicatorEngine::global()
{
    static IndicatorEngine instance;
    return &instance;
}

void IndicatorEngine::setValue(QString exchange,QString symbol,QString indicator,double value)
{
    emit IndicatorEngine::global()->indicatorChanged(exchange,symbol,indicator,value);
}

double IndicatorEngine::getValue(QString index)
{
    IndicatorEngine::global()->locker.lock();
    double value=IndicatorEngine::global()->indicators[index.toLatin1()];
    IndicatorEngine::global()->locker.unlock();

    return value;
}

//---------------------------------------- Private ----------------------------------------
void IndicatorEngine::setValueSlot(QString exchange,QString symbol,QString name,double value)
{
    if(exchange==baseValues.exchangeName&&symbol==baseValues.currentPair.symbol)
    {
        QByteArray index=(exchange+'_'+symbol+'_'+name).toLatin1();
        if(indicators.contains(index)){
            if(indicators[index]==value)
                return;
            else {
                locker.lock();
                indicators[index]=value;
                locker.unlock();
            }
        }
        else {
            locker.lock();
            indicators.insert(index,value);
            locker.unlock();
        }

        if(name=="High")emit indicatorHighChanged(symbol,value);
        else if(name=="Low")emit indicatorLowChanged(symbol,value);
        else if(name=="Sell")emit indicatorSellChanged(symbol,value);
        else if(name=="Buy")emit indicatorBuyChanged(symbol,value);
        else if(name=="Last")emit indicatorLastChanged(symbol,value);
        else if(name=="Volume")emit indicatorVolumeChanged(symbol,value);
    }
}

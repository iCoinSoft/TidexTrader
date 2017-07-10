

#include <QThread>
#include "news/newsmodel.h"

NewsModel::NewsModel()
    : QObject(),
      downloadThread(new QThread),
      runningJulyHttp(false)
{
    connect(downloadThread, &QThread::started, this, &NewsModel::runThread);
    moveToThread(downloadThread);
    downloadThread->start();
}

NewsModel::~NewsModel()
{
    julyHttp->destroyClass = true;
    julyHttp->deleteLater();
    downloadThread->deleteLater();
}

void NewsModel::destroyedJulyHttp()
{
    runningJulyHttp = false;
}

void NewsModel::runThread()
{
    julyHttp = new JulyHttp("centrabit.com", "", this);
    connect(julyHttp, &JulyHttp::dataReceived, this, &NewsModel::dataReceived);
    connect(julyHttp, &JulyHttp::destroyed,    this, &NewsModel::destroyedJulyHttp);
    runningJulyHttp = true;
}

void NewsModel::loadData()
{
    julyHttp->sendData(100, "GET /TraderNews/");
}

void NewsModel::dataReceived(QByteArray data, int reqType)
{
    if (reqType == 100)
        emit setHtmlData(data);
}

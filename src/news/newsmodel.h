

#ifndef NEWSMODEL_H
#define NEWSMODEL_H

#include "julyhttp.h"

class QThread;

class NewsModel : public QObject
{
    Q_OBJECT

public:
    NewsModel();
    ~NewsModel();

public slots:
    void loadData();

signals:
    void setHtmlData(QByteArray);

private slots:
    void runThread();
    void dataReceived(QByteArray, int);
    void destroyedJulyHttp();

private:
    QThread*  downloadThread;
    bool      runningJulyHttp;
    JulyHttp* julyHttp;
};

#endif // NEWSMODEL_H

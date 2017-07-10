

#include "news/newsview.h"
#include <QFile>
#include <QDesktopServices>
#include <QTimer>
#include "timesync.h"

NewsView::NewsView():QWidget()
{
    ui.setupUi(this);
    loadingFinished=true;
    lastUpdatedTime=0;

    newsModel=new NewsModel;
    connect(this,SIGNAL(loadData()),newsModel,SLOT(loadData()));
    connect(newsModel,SIGNAL(setHtmlData(QByteArray)),this,SLOT(setHtmlData(QByteArray)));
}

NewsView::~NewsView()
{
    newsModel->deleteLater();
}

void NewsView::visibilityChanged(bool visible)
{
    if(visible && loadingFinished)
    {
        quint32 nowTime=TimeSync::getTimeT();
        if(nowTime-lastUpdatedTime>600)
        {
            loadingFinished=false;
            emit loadData();
        }
    }
}

void NewsView::setHtmlData(QByteArray data)
{
    ui.newsBrowser->setHtml(data);
    lastUpdatedTime=TimeSync::getTimeT();
    loadingFinished=true;
}

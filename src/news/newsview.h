

#ifndef NEWSVIEW_H
#define NEWSVIEW_H

#include <QWidget>
#include "ui_newsview.h"
#include "news/newsmodel.h"

class NewsView : public QWidget
{
    Q_OBJECT

public:
    NewsView();
    ~NewsView();

private:
    Ui::NewsView ui;
    NewsModel* newsModel;
    bool loadingFinished;
    quint32 lastUpdatedTime;

signals:
    void loadData();

public slots:
    void visibilityChanged(bool);

private slots:
    void setHtmlData(QByteArray);
};

#endif // NEWSVIEW_H

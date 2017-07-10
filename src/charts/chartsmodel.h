
#ifndef CHARTSMODEL_H
#define CHARTSMODEL_H

#include <QObject>

struct TradesItem;
class QFontMetrics;

class ChartsModel : public QObject
{
    Q_OBJECT
public:
    qint16 chartsWidth;
    qint16 chartsHeight;
    qint32 widthAmountYAxis;
    qint32 widthPriceYAxis;
    qint16 perfomanceStep;

    QList<qint16>  graphTradesX;
    QList<qint16>  graphTradesY;
    QList<qint16>  graphTradesType;
    QList<qint16>  graphBoundsSellX;
    QList<qint16>  graphBoundsSellY;
    QList<qint16>  graphBoundsBuyX;
    QList<qint16>  graphBoundsBuyY;
    QList<qint16>  graphAmountX;
    QList<qint16>  graphAmountY;

    QList<QString> graphDateText;
    QList<qint16>  graphDateTextX;
    QList<QString> graphPriceText;
    QList<qint16>  graphPriceTextY;
    QList<QString> graphAmountText;
    QList<qint16>  graphAmountTextY;

    ChartsModel();
    ~ChartsModel();

    bool prepareChartsData(qint16,qint16);

public slots:
    void addLastTrades(QList<TradesItem>*);
    void addBound(double, bool);
    void clearCharts();

private:
    qint32 intervalDate;
    qint32 intervalCount;
    QFontMetrics* fontMetrics;

    QList<quint32> tradesDate;
    QList<double>  tradesPrice;
    QList<qint16>  tradesType;
    QList<quint32> boundsSellDate;
    QList<double>  boundsSellPrice;
    QList<quint32> boundsBuyDate;
    QList<double>  boundsBuyPrice;
    QList<quint32> amountDate;
    QList<double>  amountPrice;

    quint32 nowTime;
    quint32 graphLastDate;
    quint32 graphFirstDate;
    double  graphXScale;
    double  amountYScale;
    double  priceYScale;
    qint32  iAmountFirst;
    qint32  iTradesFirst;
    qint32  iBoundsSellFirst;
    qint32  iBoundsBuyFirst;
    double  amountMax;
    bool    priceInit;
    double  priceMin;
    double  priceMax;
    double  priceStepY;

    double stepRound(double);
    double axisRound(double, double);
    void prepareInit();
    void prepareXAxis();
    void prepareAmountYAxis();
    void prepareAmount();
    void preparePriceMinMax();
    void preparePriceYAxis();
    void preparePrice();
    void prepareBound();
};

#endif // CHARTSMODEL_H

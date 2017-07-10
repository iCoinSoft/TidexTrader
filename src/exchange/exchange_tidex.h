
#ifndef EXCHANGE_BTCE_H
#define EXCHANGE_BTCE_H

#include "exchange.h"

class Exchange_BTCe : public Exchange
{
	Q_OBJECT

public:
	Exchange_BTCe(QByteArray pRestSign, QByteArray pRestKey);
	~Exchange_BTCe();

private:
	bool isApiDown;
	bool isFirstAccInfo;
	bool isReplayPending(int);
    bool tickerOnly;

	int apiDownCounter;
	int lastOpenedOrders;

	JulyHttp *julyHttp;

	qint64 lastFetchTid;

	QList<DepthItem> *depthAsks;
	QList<DepthItem> *depthBids;

	QMap<double,double> lastDepthAsksMap;
	QMap<double,double> lastDepthBidsMap;

	QTime authRequestTime;

	quint32 lastPriceDate;
	quint32 lastTickerDate;
	quint32 privateNonce;
    quint32 lastHistoryId;

	void clearVariables();
	void depthSubmitOrder(QString,QMap<double,double> *currentMap ,double priceDouble, double amount, bool isAsk);
    void depthUpdateOrder(QString, double,double,bool);
	void sendToApi(int reqType, QByteArray method, bool auth=false, bool sendNow=true, QByteArray commands=0);
private slots:
	void reloadDepth();
	void sslErrors(const QList<QSslError> &);
	void dataReceivedAuth(QByteArray,int);
	void secondSlot();
public slots:
	void clearValues();
	void getHistory(bool);
	void buy(QString, double, double);
	void sell(QString, double, double);
	void cancelOrder(QString, QByteArray);
};

#endif // EXCHANGE_BTCE_H

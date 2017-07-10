
#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <QThread>
#include <QTimer>
#include <QTime>
#include <openssl/hmac.h>
#include "main.h"
#include <QtCore/qmath.h>
#include "qtbitcointrader.h"
#include "julyhttp.h"
#include "orderitem.h"
#include "tradesitem.h"
#include "julymath.h"
#include "timesync.h"
#include "indicatorengine.h"

struct DepthItem;

class Exchange : public QThread
{
	Q_OBJECT

public:
	bool exchangeDisplayOnlyCurrentPairOpenOrders;
	bool clearHistoryOnCurrencyChanged;
	bool exchangeTickerSupportsHiLowPrices;
	bool isDepthEnabled();
	bool depthEnabledFlag;
    virtual void filterAvailableUSDAmountValue(double *amount);

	CurrencyPairItem defaultCurrencyParams;
	bool balanceDisplayAvailableAmount;
	int minimumRequestIntervalAllowed;
	int minimumRequestTimeoutAllowed;
    double decAmountFromOpenOrder;
	int calculatingFeeMode;//0: direct multiply; 1: rounded by decimals
	bool buySellAmountExcludedFee;

	CurrencyPairItem currencyPairInfo;
    double lastTickerLast;
    double lastTickerHigh;
    double lastTickerLow;
    double lastTickerSell;
    double lastTickerBuy;
    double lastTickerVolume;

    double lastBtcBalance;
    double lastUsdBalance;
    double lastAvUsdBalance;
    double lastVolume;
    double lastFee;

	QByteArray lastDepthData;
	QByteArray lastHistory;
	QByteArray lastOrders;

	QString currencyMapFile;
    bool multiCurrencyTradeSupport;
	bool isLastTradesTypeSupported;
	bool exchangeSupportsAvailableAmount;
	bool checkDuplicatedOID;
	bool forceDepthLoad;
	bool tickerOnly;
	bool supportsLoginIndicator;
    bool supportsAccountVolume;
	bool supportsExchangeFee;
	bool supportsExchangeVolume;

	bool orderBookItemIsDedicatedOrder;

	QTimer *secondTimer;

	void setApiKeySecret(QByteArray key, QByteArray secret);

	QByteArray &getApiKey();
    QByteArray getApiSign();

	virtual void clearVariables();
	void translateUnicodeStr(QString *str);
	void translateUnicodeOne(QByteArray *str);
	QByteArray getMidData(QString a, QString b,QByteArray *data);
	void setupApi(QtBitcoinTrader *, bool tickerOnly=false);
	Exchange();
	~Exchange();

private:
	QByteArray privateKey;

	QList<char*> apiKeyChars;
	QList<char*> apiSignChars;

	void run();

signals:
	void setCurrencyPairsList(QList<CurrencyPairItem>*);

    void availableAmountChanged(QString,double);
	void depthRequested();
	void depthRequestReceived();
    void depthFirstOrder(QString,double,double,bool);

    void depthSubmitOrders(QString,QList<DepthItem> *asks, QList<DepthItem> *bids);

    void addLastTrades(QString,QList<TradesItem> *trades);

    void orderBookChanged(QString,QList<OrderItem> *orders);
	void showErrorMessage(QString);

	void historyChanged(QList<HistoryItem>*);

	void ordersIsEmpty();
    void orderCanceled(QString,QByteArray);

    void accVolumeChanged(double);
    void accFeeChanged(QString,double);
    void accBtcBalanceChanged(QString,double);
    void accUsdBalanceChanged(QString,double);
	void loginChanged(QString);
    void apiDownChanged(bool);
	void softLagChanged(int);
private slots:
	void sslErrors(const QList<QSslError> &);
public slots:
	virtual void secondSlot();
	virtual void dataReceivedAuth(QByteArray,int);
	virtual void reloadDepth();
	virtual void clearValues();
	virtual void getHistory(bool);
    virtual void buy(QString, double, double);
    virtual void sell(QString, double, double);
    virtual void cancelOrder(QString, QByteArray);
};

#endif // EXCHANGE_H

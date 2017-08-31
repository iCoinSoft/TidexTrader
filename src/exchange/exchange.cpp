
#include "exchange.h"
#include <QDateTime>
#include "main.h"
#include "depthitem.h"
#include <QFile>
#include "currencypairitem.h"

Exchange::Exchange()
	: QThread()
{
    multiCurrencyTradeSupport=false;
	exchangeDisplayOnlyCurrentPairOpenOrders=false;
	orderBookItemIsDedicatedOrder=false;
	supportsExchangeFee=true;
	supportsExchangeVolume=true;
	clearHistoryOnCurrencyChanged=false;
	exchangeTickerSupportsHiLowPrices=true;
	depthEnabledFlag=true;
	balanceDisplayAvailableAmount=true;
	minimumRequestIntervalAllowed=100;
	minimumRequestTimeoutAllowed=2000;
	decAmountFromOpenOrder=0.0;
	buySellAmountExcludedFee=false;
	calculatingFeeMode=0;
	supportsLoginIndicator=true;
    supportsAccountVolume=true;
	exchangeSupportsAvailableAmount=false;
	checkDuplicatedOID=false;
    isLastTradesTypeSupported=true;
	forceDepthLoad=false;

	clearVariables();
	moveToThread(this);
}

Exchange::~Exchange()
{
	if(debugLevel)logThread->writeLogB(baseValues.exchangeName+" API Thread Deleted",2);
}

bool Exchange::isDepthEnabled()
{
	return depthEnabledFlag||baseValues.scriptsThatUseOrderBookCount;
}

QByteArray Exchange::getMidData(QString a, QString b,QByteArray *data)
{
	QByteArray rez;
	if(b.isEmpty())b="\",";
	int startPos=data->indexOf(a,0);
	if(startPos>-1)
	{
		int endPos=data->indexOf(b,startPos+a.length());
		if(endPos>-1)rez=data->mid(startPos+a.length(),endPos-startPos-a.length());
	}
	return rez;
}

void Exchange::translateUnicodeStr(QString *str)
{
	const QRegExp rx("(\\\\u[0-9a-fA-F]{4})");
	int pos=0;
	while((pos=rx.indexIn(*str,pos))!=-1)str->replace(pos++, 6, QChar(rx.cap(1).right(4).toUShort(0, 16)));
}

void Exchange::translateUnicodeOne(QByteArray *str)
{
	if(!str->contains("\\u"))return;
	QStringList bytesList=QString(*str).split("\\u");
	if(bytesList.count())bytesList.removeFirst();
	else return;
	QString strToReturn;
	for(int n=0;n<bytesList.count();n++)
		if(bytesList.at(n).length()>3)
			strToReturn+="\\u"+bytesList.at(n).left(4);
	translateUnicodeStr(&strToReturn);
	*str=strToReturn.toLatin1();
}

void Exchange::run()
{
	if(debugLevel)logThread->writeLogB(baseValues.exchangeName+" API Thread Started",2);
	clearVariables();

	secondTimer=new QTimer;
	secondTimer->setSingleShot(true);
	connect(secondTimer,SIGNAL(timeout()),this,SLOT(secondSlot()));
	secondSlot();
	exec();
}

void Exchange::secondSlot()
{
	if(secondTimer)secondTimer->start(baseValues.httpRequestInterval);
}

void Exchange::dataReceivedAuth(QByteArray, int)
{
}

void Exchange::reloadDepth()
{
	forceDepthLoad=true;
}

void Exchange::clearVariables()
{
	lastTickerLast=0.0;
	lastTickerHigh=0.0;
	lastTickerLow=0.0;
	lastTickerSell=0.0;
	lastTickerBuy=0.0;
	lastTickerVolume=0.0;

	lastBtcBalance=0.0;
	lastUsdBalance=0.0;
	lastAvUsdBalance=0.0;
	lastVolume=0.0;
	lastFee=0.0;
}

void Exchange::filterAvailableUSDAmountValue(double *)
{

}

void Exchange::setupApi(QtBitcoinTrader *mainClass, bool tickOnly)//Execute only once
{
	QSettings settingsParams(":/Resources/Exchanges/"+currencyMapFile+".ini",QSettings::IniFormat);
	QStringList symbolList=settingsParams.childGroups();
	QList<CurrencyPairItem> *newCurrPairs=new QList<CurrencyPairItem>;

	for(int n=0;n<symbolList.count();n++)
	{
		CurrencyPairItem currentPair=defaultCurrencyParams;
		currentPair.name=settingsParams.value(symbolList.at(n)+"/Symbol","").toByteArray();
        //if(currentPair.name.length()!=6)continue;
		currentPair.setSymbol(currentPair.name.toLatin1());
        //currentPair.name.insert(3,"/");
		currentPair.currRequestSecond=settingsParams.value(symbolList.at(n)+"/RequestSecond","").toByteArray();
		if(!currentPair.currRequestSecond.isEmpty())
			currentPair.name.append(" ["+currentPair.currRequestSecond+"]");
		currentPair.currRequestPair=settingsParams.value(symbolList.at(n)+"/Request","").toByteArray();
		if(currentPair.currRequestPair.isEmpty())continue;
		currentPair.priceDecimals=settingsParams.value(symbolList.at(n)+"/PriceDecimals","").toInt();
		currentPair.priceMin=settingsParams.value(symbolList.at(n)+"/PriceMin","").toDouble();
		currentPair.tradeVolumeMin=settingsParams.value(symbolList.at(n)+"/TradeVolumeMin","").toDouble();
        currentPair.tradePriceMin=settingsParams.value(symbolList.at(n)+"/TradePriceMin","").toDouble();
        currentPair.currADecimals=settingsParams.value(symbolList.at(n)+"/ItemDecimals").toInt();
        currentPair.currBDecimals=settingsParams.value(symbolList.at(n)+"/ValueDecimals").toInt();
		(*newCurrPairs)<<currentPair;
	}

	connect(this,SIGNAL(setCurrencyPairsList(QList<CurrencyPairItem>*)),mainClass,SLOT(setCurrencyPairsList(QList<CurrencyPairItem>*)));

	emit setCurrencyPairsList(newCurrPairs);

	tickerOnly=tickOnly;
	if(!tickerOnly)
	{
        connect(mainClass,SIGNAL(apiBuy(QString, double, double)),this,SLOT(buy(QString, double, double)));
        connect(mainClass,SIGNAL(apiSell(QString, double, double)),this,SLOT(sell(QString, double, double)));
        connect(mainClass,SIGNAL(cancelOrderByOid(QString, QByteArray)),this,SLOT(cancelOrder(QString, QByteArray)));
        connect(mainClass,SIGNAL(getHistory(bool)),this,SLOT(getHistory(bool)));

        connect(this,SIGNAL(orderBookChanged(QString, QList<OrderItem> *)),mainClass,SLOT(orderBookChanged(QString, QList<OrderItem> *)));
		connect(this,SIGNAL(historyChanged(QList<HistoryItem>*)),mainClass,SLOT(historyChanged(QList<HistoryItem>*)));
        connect(this,SIGNAL(orderCanceled(QString, QByteArray)),mainClass,SLOT(orderCanceled(QString, QByteArray)));
		connect(this,SIGNAL(ordersIsEmpty()),mainClass,SLOT(ordersIsEmpty()));
	}

	connect(this,SIGNAL(depthRequested()),mainClass,SLOT(depthRequested()));
	connect(this,SIGNAL(depthRequestReceived()),mainClass,SLOT(depthRequestReceived()));
    connect(this,SIGNAL(depthSubmitOrders(QString, QList<DepthItem> *, QList<DepthItem> *)),mainClass,SLOT(depthSubmitOrders(QString, QList<DepthItem> *, QList<DepthItem> *)));
    connect(this,SIGNAL(depthFirstOrder(QString, double,double,bool)),mainClass,SLOT(depthFirstOrder(QString, double,double,bool)));
	connect(this,SIGNAL(showErrorMessage(QString)),mainClass,SLOT(showErrorMessage(QString)));

    connect(this,SIGNAL(availableAmountChanged(QString, double)),mainClass,SLOT(availableAmountChanged(QString, double)));
	connect(mainClass,SIGNAL(clearValues()),this,SLOT(clearValues()));
	connect(mainClass,SIGNAL(reloadDepth()),this,SLOT(reloadDepth()));

    connect(this,SIGNAL(accVolumeChanged(double)),mainClass->ui.accountVolume,SLOT(setValue(double)));
    connect(this,SIGNAL(accFeeChanged(QString, double)),mainClass,SLOT(accFeeChanged(QString,double)));
    connect(this,SIGNAL(accBtcBalanceChanged(QString, double)),mainClass,SLOT(accBtcBalanceChanged(QString, double)));
    connect(this,SIGNAL(accUsdBalanceChanged(QString, double)),mainClass,SLOT(accUsdBalanceChanged(QString, double)));

    connect(this,SIGNAL(loginChanged(QString)),mainClass,SLOT(loginChanged(QString)));

    connect(this,SIGNAL(addLastTrades(QString, QList<TradesItem> *)),mainClass,SLOT(addLastTrades(QString, QList<TradesItem> *)));

	start();
}

void Exchange::setApiKeySecret(QByteArray key, QByteArray secret)
{
    if(!apiKeyChars.isEmpty())return;

	privateKey=key;

	for(int n=secret.size()-1;n>=0;n--)
		apiSignChars<<new char(secret[n]);
}

QByteArray &Exchange::getApiKey()
{
	return privateKey;
}

QByteArray Exchange::getApiSign()
{
	QByteArray result;
	for(int n=apiSignChars.size()-1;n>=0;n--)
		result+=*(apiSignChars[n]);
	return result;
}

void Exchange::clearValues()
{

}

void Exchange::getHistory(bool)
{
}

void Exchange::buy(QString, double, double)
{
}

void Exchange::sell(QString, double, double)
{
}

void Exchange::cancelOrder(QString, QByteArray)
{
}

void Exchange::sslErrors(const QList<QSslError> &errors)
{
	QStringList errorList;
	for(int n=0;n<errors.count();n++)errorList<<errors.at(n).errorString();
	if(debugLevel)logThread->writeLog(errorList.join(" ").toLatin1(),2);
	emit showErrorMessage("SSL Error: "+errorList.join(" "));
}

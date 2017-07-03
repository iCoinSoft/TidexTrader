//  This file is part of Qt Bitcoin Trader
//      https://github.com/JulyIGHOR/QtBitcoinTrader
//  Copyright (C) 2013-2017 July IGHOR <julyighor@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  In addition, as a special exception, the copyright holders give
//  permission to link the code of portions of this program with the
//  OpenSSL library under certain conditions as described in each
//  individual source file, and distribute linked combinations including
//  the two.
//
//  You must obey the GNU General Public License in all respects for all
//  of the code used other than OpenSSL. If you modify file(s) with this
//  exception, you may extend this exception to your version of the
//  file(s), but you are not obligated to do so. If you do not wish to do
//  so, delete this exception statement from your version. If you delete
//  this exception statement from all source files in the program, then
//  also delete it here.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "exchange_bitstamp.h"
#include <openssl/hmac.h>

Exchange_Bitstamp::Exchange_Bitstamp(QByteArray pRestSign, QByteArray pRestKey)
	: Exchange()
{
	checkDuplicatedOID=true;
    accountFee=0.0;
	minimumRequestIntervalAllowed=1200;
	calculatingFeeMode=1;
	isLastTradesTypeSupported=false;
	lastBidAskTimestamp=0;
	baseValues.exchangeName="Bitstamp";
	baseValues.currentPair.name="BTC/USD";
	baseValues.currentPair.setSymbol("BTCUSD");
	baseValues.currentPair.currRequestPair="BTCUSD";
	baseValues.currentPair.priceDecimals=2;
	baseValues.currentPair.priceMin=qPow(0.1,baseValues.currentPair.priceDecimals);
	baseValues.currentPair.tradeVolumeMin=0.01;
	baseValues.currentPair.tradePriceMin=0.1;
	depthAsks=0;
	depthBids=0;
	forceDepthLoad=false;
	julyHttp=0;
	tickerOnly=false;
	setApiKeySecret(pRestKey.split(':').last(),pRestSign);
	privateClientId=pRestKey.split(':').first();

	currencyMapFile="Bitstamp";
	defaultCurrencyParams.currADecimals=8;
	defaultCurrencyParams.currBDecimals=5;
	defaultCurrencyParams.currABalanceDecimals=8;
	defaultCurrencyParams.currBBalanceDecimals=5;
	defaultCurrencyParams.priceDecimals=2;
	defaultCurrencyParams.priceMin=qPow(0.1,baseValues.currentPair.priceDecimals);

	supportsLoginIndicator=true;
    supportsAccountVolume=false;

	moveToThread(this);
	authRequestTime.restart();
    privateNonce=(QDateTime::currentDateTime().toTime_t()-1371854884)*10;
}

Exchange_Bitstamp::~Exchange_Bitstamp()
{
}

void Exchange_Bitstamp::filterAvailableUSDAmountValue(double *amount)
{
    double decValue=cutDoubleDecimalsCopy((*amount)*mainWindow.floatFee,baseValues.currentPair.priceDecimals,false);
	decValue+=qPow(0.1,qMax(baseValues.currentPair.priceDecimals,1));
    *amount=cutDoubleDecimalsCopy((*amount)-decValue,baseValues.currentPair.currBDecimals,false);
}

void Exchange_Bitstamp::clearVariables()
{
	cancelingOrderIDs.clear();
	Exchange::clearVariables();
	secondPart=0;
	apiDownCounter=0;
	lastHistory.clear();
	lastOrders.clear();
	reloadDepth();
	lastInfoReceived=false;
	lastBidAskTimestamp=0;
    lastTradesDate=QDateTime::currentDateTime().toTime_t()-600;
	lastTickerDate=0;
}

void Exchange_Bitstamp::clearValues()
{
	clearVariables();
	if(julyHttp)julyHttp->clearPendingData();
}

void Exchange_Bitstamp::secondSlot()
{
    static int sendCounter=0;
    switch(sendCounter)
	{
    case 0:
        if(!isReplayPending(103))sendToApi(103,"v2/ticker/"+baseValues.currentPair.currRequestPair.toLower()+"/",false,true);
        break;
    case 1:
        if(!isReplayPending(202))sendToApi(202,"balance/",true,true);
        break;
    case 2:
        if(!isReplayPending(109))sendToApi(109,"v2/transactions/"+baseValues.currentPair.currRequestPair.toLower()+"/",false,true);
        break;
    case 3:
        if(!tickerOnly&&!isReplayPending(204))sendToApi(204,"v2/open_orders/"+baseValues.currentPair.currRequestPair.toLower()+"/",true,true);
        break;
    case 4:
        if(isDepthEnabled()&&(forceDepthLoad||!isReplayPending(111)))
        {
            emit depthRequested();
            sendToApi(111,"v2/order_book/"+baseValues.currentPair.currRequestPair.toLower()+"/",false,true);
            forceDepthLoad=false;
        }
        break;
    case 5:
        if(lastHistory.isEmpty()&&!isReplayPending(208))sendToApi(208,"v2/user_transactions/",true,true);
        break;
	default: break;
	}
    if(sendCounter++>=5)sendCounter=0;

	Exchange::secondSlot();
}

bool Exchange_Bitstamp::isReplayPending(int reqType)
{
	if(julyHttp==0)return false;
	return julyHttp->isReqTypePending(reqType);
}

void Exchange_Bitstamp::getHistory(bool force)
{
	if(tickerOnly)return;
	if(force)lastHistory.clear();
    if(!isReplayPending(208))sendToApi(208,"v2/user_transactions/",true,true);
}

void Exchange_Bitstamp::buy(QString symbol, double apiBtcToBuy, double apiPriceToBuy)
{
	if(tickerOnly)return;

    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol,pairItem);
    if(pairItem.symbol.isEmpty())return;

    QByteArray params="amount="+byteArrayFromDouble(apiBtcToBuy,pairItem.currADecimals,0)+"&price="+byteArrayFromDouble(apiPriceToBuy,pairItem.priceDecimals,0);
	if(debugLevel)logThread->writeLog("Buy: "+params,2);
    sendToApi(306,"v2/buy/"+baseValues.currentPair.currRequestPair.toLower()+"/",true,true,params);
}

void Exchange_Bitstamp::sell(QString symbol, double apiBtcToSell, double apiPriceToSell)
{
	if(tickerOnly)return;

    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol,pairItem);
    if(pairItem.symbol.isEmpty())return;

    QByteArray params="amount="+byteArrayFromDouble(apiBtcToSell,pairItem.currADecimals,0)+"&price="+byteArrayFromDouble(apiPriceToSell,pairItem.priceDecimals,0);
	if(debugLevel)logThread->writeLog("Sell: "+params,2);
    sendToApi(307,"v2/sell/"+baseValues.currentPair.currRequestPair.toLower()+"/",true,true,params);
}

void Exchange_Bitstamp::cancelOrder(QString, QByteArray order)
{
	if(tickerOnly)return;
	cancelingOrderIDs<<order;
	if(debugLevel)logThread->writeLog("Cancel order: "+order,2);
	sendToApi(305,"cancel_order/",true,true,"id="+order);
}

void Exchange_Bitstamp::sendToApi(int reqType, QByteArray method, bool auth, bool sendNow, QByteArray commands)
{
	if(julyHttp==0)
	{ 
		julyHttp=new JulyHttp("www.bitstamp.net","",this);
		connect(julyHttp,SIGNAL(anyDataReceived()),baseValues_->mainWindow_,SLOT(anyDataReceived()));
		connect(julyHttp,SIGNAL(setDataPending(bool)),baseValues_->mainWindow_,SLOT(setDataPending(bool)));
		connect(julyHttp,SIGNAL(apiDown(bool)),baseValues_->mainWindow_,SLOT(setApiDown(bool)));
		connect(julyHttp,SIGNAL(errorSignal(QString)),baseValues_->mainWindow_,SLOT(showErrorMessage(QString)));
		connect(julyHttp,SIGNAL(sslErrorSignal(const QList<QSslError> &)),this,SLOT(sslErrors(const QList<QSslError> &)));
		connect(julyHttp,SIGNAL(dataReceived(QByteArray,int)),this,SLOT(dataReceivedAuth(QByteArray,int)));
	}

	if(auth)
	{
		QByteArray postData=QByteArray::number(++privateNonce);
		postData="key="+getApiKey()+"&signature="+hmacSha256(getApiSign(),QByteArray(postData+privateClientId+getApiKey())).toHex().toUpper()+"&nonce="+postData;
		if(!commands.isEmpty())postData.append("&"+commands);
		if(sendNow)
		julyHttp->sendData(reqType, "POST /api/"+method,postData);
		else
		julyHttp->prepareData(reqType, "POST /api/"+method, postData);

	}
	else
	{
		if(sendNow)
			julyHttp->sendData(reqType, "GET /api/"+method);
		else 
			julyHttp->prepareData(reqType, "GET /api/"+method);
	}
}

void Exchange_Bitstamp::depthUpdateOrder(QString symbol, double price, double amount, bool isAsk)
{
    if(symbol!=baseValues.currentPair.symbol)return;

	if(isAsk)
	{
		if(depthAsks==0)return;
		DepthItem newItem;
		newItem.price=price;
		newItem.volume=amount;
		if(newItem.isValid())
			(*depthAsks)<<newItem;
	}
	else
	{
		if(depthBids==0)return;
		DepthItem newItem;
		newItem.price=price;
		newItem.volume=amount;
		if(newItem.isValid())
			(*depthBids)<<newItem;
	}
}

void Exchange_Bitstamp::depthSubmitOrder(QString symbol, QMap<double,double> *currentMap ,double priceDouble, double amount, bool isAsk)
{
    if(symbol!=baseValues.currentPair.symbol)return;

	if(priceDouble==0.0||amount==0.0)return;

	if(isAsk)
	{
		(*currentMap)[priceDouble]=amount;
		if(lastDepthAsksMap.value(priceDouble,0.0)!=amount)
            depthUpdateOrder(symbol,priceDouble,amount,true);
	}
	else
	{
		(*currentMap)[priceDouble]=amount;
		if(lastDepthBidsMap.value(priceDouble,0.0)!=amount)
            depthUpdateOrder(symbol,priceDouble,amount,false);
	}
}

void Exchange_Bitstamp::reloadDepth()
{
	lastDepthBidsMap.clear();
	lastDepthAsksMap.clear();
	lastDepthData.clear();
	Exchange::reloadDepth();
}

void Exchange_Bitstamp::dataReceivedAuth(QByteArray data, int reqType)
{
	if(debugLevel)logThread->writeLog("RCV: "+data);
    if(data.size()&&data.at(0)==QLatin1Char('<'))return;

    bool success=((!data.startsWith("{\"error\"")&&(data.startsWith("{")))||data.startsWith("["))||data=="true"||data=="false";
	switch(reqType)
	{
	case 103: //ticker
		if(!success)break;
			if(data.startsWith("{\"high\":"))
			{
				quint32 tickerTimestamp=getMidData("\"timestamp\": \"","\"",&data).toUInt();
				QByteArray tickerHigh=getMidData("\"high\": \"","\"",&data);
				if(!tickerHigh.isEmpty())
				{
                    double newTickerHigh=tickerHigh.toDouble();
                    if(newTickerHigh!=lastTickerHigh)
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"High",newTickerHigh);
					lastTickerHigh=newTickerHigh;
				}

				QByteArray tickerLow=getMidData("\"low\": \"","\"",&data);
				if(!tickerLow.isEmpty())
				{
                    double newTickerLow=tickerLow.toDouble();
                    if(newTickerLow!=lastTickerLow)
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Low",newTickerLow);
					lastTickerLow=newTickerLow;
				}

				QByteArray tickerVolume=getMidData("\"volume\": \"","\"",&data);
				if(!tickerVolume.isEmpty())
				{
                    double newTickerVolume=tickerVolume.toDouble();
                    if(newTickerVolume!=lastTickerVolume)
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Volume",newTickerVolume);
					lastTickerVolume=newTickerVolume;
				}

				QByteArray tickerLast=getMidData("\"last\": \"","\"",&data);
				if(!tickerLast.isEmpty()&&lastTickerDate<tickerTimestamp)
				{
                    double newTickerLast=tickerLast.toDouble();
					if(newTickerLast>0.0)
					{
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Last",newTickerLast);
						lastTickerDate=tickerTimestamp;
					}
				}
				if(tickerTimestamp>lastBidAskTimestamp)
				{
					QByteArray tickerSell=getMidData("\"bid\": \"","\"",&data);
					if(!tickerSell.isEmpty())
					{
                        double newTickerSell=tickerSell.toDouble();
                        if(newTickerSell!=lastTickerSell)
                            IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Sell",newTickerSell);
						lastTickerSell=newTickerSell;
					}

					QByteArray tickerBuy=getMidData("\"ask\": \"","\"",&data);
					if(!tickerBuy.isEmpty())
					{
                        double newTickerBuy=tickerBuy.toDouble();
                        if(newTickerBuy!=lastTickerBuy)
                            IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Buy",newTickerBuy);
						lastTickerBuy=newTickerBuy;
					}
					lastBidAskTimestamp=tickerTimestamp;
				}
			}
			else if(debugLevel)logThread->writeLog("Invalid ticker data:"+data,2);
		break;//ticker
	case 109: //api/transactions
		if(success&&data.size()>32)
		{
			if(data.startsWith("[{\"date\":"))
			{
				QStringList tradeList=QString(data).split("}, {");
				QList<TradesItem> *newTradesItems=new QList<TradesItem>;

				for(int n=tradeList.count()-1;n>=0;n--)
				{
					QByteArray tradeData=tradeList.at(n).toLatin1();
					TradesItem newItem;
					newItem.date=getMidData("\"date\": \"","\"",&tradeData).toUInt();
					if(newItem.date<=lastTradesDate)continue;
					newItem.amount=getMidData("\"amount\": \"","\"",&tradeData).toDouble();
					QByteArray priceBytes=getMidData("\"price\": \"","\"",&tradeData);
					newItem.price=priceBytes.toDouble();
					if(n==0&&newItem.price>0.0)
					{
						lastTradesDate=newItem.date;
						if(lastTickerDate<newItem.date)
						{
						lastTickerDate=newItem.date;
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Last",newItem.price);
						}
					}
					newItem.symbol=baseValues.currentPair.symbol;
					//newItem.type=0;

					if(newItem.isValid())(*newTradesItems)<<newItem;
					else if(debugLevel)logThread->writeLog("Invalid trades fetch data line:"+tradeData,2);
				}
				if(newTradesItems->count())emit addLastTrades(baseValues.currentPair.symbol,newTradesItems);
				else delete newTradesItems;
			}
			else if(debugLevel)logThread->writeLog("Invalid trades fetch data:"+data,2);
		}
		break;
	case 111: //api/order_book
		if(data.startsWith("{\"timestamp\":"))
		{
			emit depthRequestReceived();

			if(lastDepthData!=data)
			{
				lastDepthData=data;
				depthAsks=new QList<DepthItem>;
				depthBids=new QList<DepthItem>;

				quint32 tickerTimestamp=getMidData("\"timestamp\": \"","\"",&data).toUInt();
                QMap<double,double> currentAsksMap;
				QStringList asksList=QString(getMidData("\"asks\": [[","]]",&data)).split("], [");
                double groupedPrice=0.0;
                double groupedVolume=0.0;
				int rowCounter=0;
				bool updateTicker=tickerTimestamp>lastBidAskTimestamp;
				if(updateTicker)lastBidAskTimestamp=tickerTimestamp;

				for(int n=0;n<asksList.count();n++)
				{
					if(baseValues.depthCountLimit&&rowCounter>=baseValues.depthCountLimit)break;
					QByteArray currentRow=asksList.at(n).toLatin1();
                    double priceDouble=getMidData("\"","\"",&currentRow).toDouble();
                    double amount=getMidData(", \"","\"",&currentRow).toDouble();

                    if(n==0&&updateTicker)
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Buy",priceDouble);

					if(baseValues.groupPriceValue>0.0)
					{
						if(n==0)
						{
							emit depthFirstOrder(baseValues.currentPair.symbol,priceDouble,amount,true);
							groupedPrice=baseValues.groupPriceValue*(int)(priceDouble/baseValues.groupPriceValue);
							groupedVolume=amount;
						}
						else
						{
							bool matchCurrentGroup=priceDouble<groupedPrice+baseValues.groupPriceValue;
							if(matchCurrentGroup)groupedVolume+=amount;
							if(!matchCurrentGroup||n==asksList.count()-1)
							{
                                depthSubmitOrder(baseValues.currentPair.symbol,
                                            &currentAsksMap,groupedPrice+baseValues.groupPriceValue,groupedVolume,true);
								rowCounter++;
								groupedVolume=amount;
								groupedPrice+=baseValues.groupPriceValue;
							}
						}
					}
					else
					{
                    depthSubmitOrder(baseValues.currentPair.symbol,
                                     &currentAsksMap,priceDouble,amount,true);
					rowCounter++;
					}
				}
                QList<double> currentAsksList=lastDepthAsksMap.keys();
				for(int n=0;n<currentAsksList.count();n++)
                    if(currentAsksMap.value(currentAsksList.at(n),0)==0)depthUpdateOrder(baseValues.currentPair.symbol,
                                                                                         currentAsksList.at(n),0.0,true);//Remove price
				lastDepthAsksMap=currentAsksMap;

                QMap<double,double> currentBidsMap;
				QStringList bidsList=QString(getMidData("\"bids\": [[","]]",&data)).split("], [");
				groupedPrice=0.0;
				groupedVolume=0.0;
				rowCounter=0;
				for(int n=0;n<bidsList.count();n++)
				{
					if(baseValues.depthCountLimit&&rowCounter>=baseValues.depthCountLimit)break;
					QByteArray currentRow=bidsList.at(n).toLatin1();
                    double priceDouble=getMidData("\"","\"",&currentRow).toDouble();
                    double amount=getMidData(", \"","\"",&currentRow).toDouble();

                    if(n==0&&updateTicker)
                        IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Sell",priceDouble);

					if(baseValues.groupPriceValue>0.0)
					{
						if(n==0)
						{
							emit depthFirstOrder(baseValues.currentPair.symbol,priceDouble,amount,false);
							groupedPrice=baseValues.groupPriceValue*(int)(priceDouble/baseValues.groupPriceValue);
							groupedVolume=amount;
						}
						else
						{
							bool matchCurrentGroup=priceDouble>groupedPrice-baseValues.groupPriceValue;
							if(matchCurrentGroup)groupedVolume+=amount;
							if(!matchCurrentGroup||n==bidsList.count()-1)
							{
                                depthSubmitOrder(baseValues.currentPair.symbol,
                                                 &currentBidsMap,groupedPrice-baseValues.groupPriceValue,groupedVolume,false);
								rowCounter++;
								groupedVolume=amount;
								groupedPrice-=baseValues.groupPriceValue;
							}
						}
					}
					else
					{
                        depthSubmitOrder(baseValues.currentPair.symbol,
                                         &currentBidsMap,priceDouble,amount,false);
						rowCounter++;
					}
				}
                QList<double> currentBidsList=lastDepthBidsMap.keys();
				for(int n=0;n<currentBidsList.count();n++)
                    if(currentBidsMap.value(currentBidsList.at(n),0)==0)depthUpdateOrder(baseValues.currentPair.symbol,
                                                                                         currentBidsList.at(n),0.0,false);//Remove price
				lastDepthBidsMap=currentBidsMap;

				emit depthSubmitOrders(baseValues.currentPair.symbol,depthAsks, depthBids);
				depthAsks=0;
				depthBids=0;
			}
		}
		else if(debugLevel)logThread->writeLog("Invalid depth data:"+data,2);
		break;
	case 202: //balance
        {
			if(!success)break;
            if(data.startsWith("{\""))
			{
				lastInfoReceived=true;
				if(debugLevel)logThread->writeLog("Info: "+data);

                double accFee=getMidData("\"fee\": \"","\"",&data).toDouble();
				if(accFee>0.0)
				{
                    emit accFeeChanged(baseValues.currentPair.symbol,accFee);
                    accountFee=accFee;
				}

                QByteArray btcBalance=getMidData("\""+baseValues.currentPair.currAStrLow+"_available\": \"","\"",&data);
                if(!btcBalance.isEmpty())
                {
                    double newBtcBalance=btcBalance.toDouble();
                    if(lastBtcBalance!=newBtcBalance)emit accBtcBalanceChanged(baseValues.currentPair.symbol,newBtcBalance);
                    lastBtcBalance=newBtcBalance;
                }

                QByteArray usdBalance=getMidData("\""+baseValues.currentPair.currBStrLow+"_available\": \"","\"",&data);
                if(!usdBalance.isEmpty())
                {
                    double newUsdBalance=usdBalance.toDouble();
                    if(newUsdBalance!=lastUsdBalance)emit accUsdBalanceChanged(baseValues.currentPair.symbol,newUsdBalance);
                    lastUsdBalance=newUsdBalance;
                }

				static bool balanceSent=false;
				if(!balanceSent)
				{
					balanceSent=true;
					emit loginChanged(privateClientId);
				}
			}
			else if(debugLevel)logThread->writeLog("Invalid Info data:"+data,2);
		}
		break;//balance
	case 204://open_orders
        if(!success)break;
		if(data=="[]"){lastOrders.clear();emit ordersIsEmpty();break;}
		if(data.startsWith("[")&&data.endsWith("]"))
		{
			if(lastOrders!=data)
			{
				lastOrders=data;
				if(data.size()>3)
				{
				data.remove(0,2);
				data.remove(data.size()-2,2);
				}
				QStringList ordersList=QString(data).split("}, {");
				QList<OrderItem> *orders=new QList<OrderItem>;
				for(int n=0;n<ordersList.count();n++)
				{	
					OrderItem currentOrder;
					QByteArray currentOrderData=ordersList.at(n).toLatin1();
                    currentOrder.oid=getMidData("\"id\": \"","\",",&currentOrderData);

					QByteArray dateTimeData=getMidData("\"datetime\": \"","\"",&currentOrderData);
					QDateTime orderDateTime=QDateTime::fromString(dateTimeData,"yyyy-MM-dd HH:mm:ss");
					orderDateTime.setTimeSpec(Qt::UTC);
                    currentOrder.date=orderDateTime.toTime_t();
                    currentOrder.type=getMidData("\"type\": \"","\",",&currentOrderData)=="1";
					currentOrder.status=1;
					currentOrder.amount=getMidData("\"amount\": \"","\"",&currentOrderData).toDouble();
					currentOrder.price=getMidData("\"price\": \"","\"",&currentOrderData).toDouble();
					currentOrder.symbol=baseValues.currentPair.symbol;
					if(currentOrder.isValid())(*orders)<<currentOrder;
				}
                emit orderBookChanged(baseValues.currentPair.symbol,orders);
				lastInfoReceived=false;
			}
		}
		else if(debugLevel)logThread->writeLog("Invalid Orders data:"+data,2);
		break;//open_orders
	case 305: //cancel_order
		{
			if(!success)break;
			if(!cancelingOrderIDs.isEmpty())
			{
			if(data=="true")emit orderCanceled(baseValues.currentPair.symbol,cancelingOrderIDs.first());
			if(debugLevel)logThread->writeLog("Order canceled:"+cancelingOrderIDs.first(),2);
			cancelingOrderIDs.removeFirst();
			}
		}
		break;//cancel_order
	case 306: //order/buy
		if(!success||!debugLevel)break;
			  if(data.startsWith("{\"result\":\"success\",\"data\":\""))logThread->writeLog("Buy OK: "+data);
			  else logThread->writeLog("Invalid Order Buy Data:"+data);
		break;//order/buy
	case 307: //order/sell
		if(!success||!debugLevel)break;
			  if(data.startsWith("{\"result\":\"success\",\"data\":\""))logThread->writeLog("Sell OK: "+data);
			  else logThread->writeLog("Invalid Order Sell Data:"+data);
		break;//order/sell
	case 208: //user_transactions
        if(!success)break;
		if(data.startsWith("["))
		{
			if(lastHistory!=data)
			{
				lastHistory=data;
				if(data=="[]")break;

				QList<HistoryItem> *historyItems=new QList<HistoryItem>;
				QString newLog(data);
				QStringList dataList=newLog.split("}, {");
				newLog.clear();
				for(int n=0;n<dataList.count();n++)
				{
					HistoryItem currentHistoryItem;

					QByteArray curLog(dataList.at(n).toLatin1());

                    QString firstCurrency;
                    if(curLog.indexOf("\"btc_eur\"")>=0)
                    {
                        currentHistoryItem.price=getMidData("btc_eur\": ",",",&curLog).toDouble();
                        currentHistoryItem.symbol="BTCEUR";
                        firstCurrency="btc";
                    }
                    else
                    {
                        if(curLog.indexOf("\"btc_usd\"")>=0)
                        {
                            currentHistoryItem.price=getMidData("btc_usd\": ",",",&curLog).toDouble();
                            currentHistoryItem.symbol="BTCUSD";
                            firstCurrency="btc";
                        }
                        else
                        {
                            if(curLog.indexOf("\"eur_usd\"")>=0)
                            {
                                currentHistoryItem.price=getMidData("eur_usd\": ",",",&curLog).toDouble();
                                currentHistoryItem.symbol="EURUSD";
                                firstCurrency="eur";
                            }
                            else continue;
                        }
                    }

                    int logTypeInt=getMidData("\"type\": \"","\"",&curLog).toInt();
                    QByteArray btcAmount=getMidData("\""+firstCurrency+"\": \"","\"",&curLog);
					bool negativeAmount=btcAmount.startsWith("-");
					if(negativeAmount)btcAmount.remove(0,1);
					QDateTime orderDateTime=QDateTime::fromString(getMidData("\"datetime\": \"","\"",&curLog),"yyyy-MM-dd HH:mm:ss");
					orderDateTime.setTimeSpec(Qt::UTC);

					currentHistoryItem.volume=btcAmount.toDouble();

					if(logTypeInt==0)currentHistoryItem.type=4;//Deposit
					else
					if(logTypeInt==1)currentHistoryItem.type=5;//Withdrawal
					else
					if(logTypeInt==2)//Market Trade
					{
						if(negativeAmount)currentHistoryItem.type=1;//Sell
						else currentHistoryItem.type=2;//Buy
					}
					currentHistoryItem.dateTimeInt=orderDateTime.toTime_t();
					if(currentHistoryItem.isValid())(*historyItems)<<currentHistoryItem;
				}
				emit historyChanged(historyItems);
			}
		}
		else if(debugLevel)logThread->writeLog("Invalid History data:"+data.left(200),2);
		break;//user_transactions
	default: break;
	}

    static int authErrorCount=0;
    if(reqType>=200 && reqType<300)
    {
        if(!success)
        {
            authErrorCount++;
            if(authErrorCount>2)
            {
                QString authErrorString=getMidData("error\": \"","\"",&data);
                if(debugLevel)logThread->writeLog("API error: "+authErrorString.toLatin1()+" ReqType: "+QByteArray::number(reqType),2);

                if(authErrorString=="API key not found")authErrorString=julyTr("TRUNAUTHORIZED","Invalid API key.");
                else if(authErrorString=="Invalid nonce")authErrorString=julyTr("THIS_PROFILE_ALREADY_USED","Invalid nonce parameter.");
                if(!authErrorString.isEmpty())emit showErrorMessage(authErrorString);
            }
        }
        else authErrorCount=0;
    }

	static int errorCount=0;
	if(!success&&reqType!=305)
	{
		errorCount++;
		QString errorString;
		bool invalidMessage=!data.startsWith("{");
		if(!invalidMessage)
		{
			errorString=getMidData("[\"","\"]",&data);
			if(errorString.isEmpty())
			{
				QByteArray nErrorString=getMidData("{\"error\":","}",&data);
				errorString=getMidData("\"","\"",&nErrorString);
			}
		}
		else errorString=data;
		if(debugLevel)logThread->writeLog("API Error: "+errorString.toLatin1()+" ReqType:"+QByteArray::number(reqType),2);
		if(errorCount<3&&reqType<300&&errorString!="Invalid username and/or password")return;
		if(errorString.isEmpty())return;
		errorString.append("<br>"+QString::number(reqType));
		if(invalidMessage||reqType<300)emit showErrorMessage("I:>"+errorString);
	}
	else errorCount=0;
}

void Exchange_Bitstamp::sslErrors(const QList<QSslError> &errors)
{
	QStringList errorList;
	for(int n=0;n<errors.count();n++)errorList<<errors.at(n).errorString();
	if(debugLevel)logThread->writeLog(errorList.join(" ").toLatin1(),2);
	emit showErrorMessage("SSL Error: "+errorList.join(" "));
}

//  This file is part of TidexTrader
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

#include "exchange_btce.h"
#include <openssl/hmac.h>
#include "main.h"

Exchange_BTCe::Exchange_BTCe(QByteArray pRestSign, QByteArray pRestKey) 
	: Exchange()
{
	calculatingFeeMode=1;
    baseValues.exchangeName="Tidex";
	baseValues.currentPair.name="BTC/USD";
	baseValues.currentPair.setSymbol("BTCUSD");
	baseValues.currentPair.currRequestPair="btc_usd";
	baseValues.currentPair.priceDecimals=3;
    minimumRequestIntervalAllowed=500;
	baseValues.currentPair.priceMin=qPow(0.1,baseValues.currentPair.priceDecimals);
	baseValues.currentPair.tradeVolumeMin=0.01;
	baseValues.currentPair.tradePriceMin=0.1;
	depthAsks=0;
	depthBids=0;
	forceDepthLoad=false;
	julyHttp=0;
	isApiDown=false;
	tickerOnly=false;
	setApiKeySecret(pRestKey,pRestSign);

    moveToThread(this);

	currencyMapFile="BTCe";
	defaultCurrencyParams.currADecimals=8;
	defaultCurrencyParams.currBDecimals=8;
	defaultCurrencyParams.currABalanceDecimals=8;
	defaultCurrencyParams.currBBalanceDecimals=8;
	defaultCurrencyParams.priceDecimals=3;
	defaultCurrencyParams.priceMin=qPow(0.1,baseValues.currentPair.priceDecimals);

	supportsLoginIndicator=false;
    supportsAccountVolume=false;

	authRequestTime.restart();
    privateNonce=(QDateTime::currentDateTime().toTime_t()-1371854884)*10;
    lastHistoryId=0;

    QSettings networkSettings(baseValues.iniFileName,QSettings::IniFormat);
    baseValues.alternativeDomainBTCe=networkSettings.value("Network/AlternativeDomainBTCe",false).toBool();
}

Exchange_BTCe::~Exchange_BTCe()
{
}

void Exchange_BTCe::clearVariables()
{
	isFirstAccInfo=true;
	Exchange::clearVariables();
	lastOpenedOrders=-1;
	apiDownCounter=0;
	lastHistory.clear();
	lastOrders.clear();
	reloadDepth();
    lastFetchTid=QDateTime::currentDateTime().toTime_t()-600;
	lastFetchTid=-lastFetchTid;
	lastTickerDate=0;
}

void Exchange_BTCe::clearValues()
{
	clearVariables();
	if(julyHttp)julyHttp->clearPendingData();
}

void Exchange_BTCe::reloadDepth()
{
	lastDepthBidsMap.clear();
	lastDepthAsksMap.clear();
	lastDepthData.clear();
	Exchange::reloadDepth();
}

void Exchange_BTCe::dataReceivedAuth(QByteArray data, int reqType)
{
    if(debugLevel)logThread->writeLog("RCV: "+data);
    if(data.size()&&data.at(0)==QLatin1Char('<'))return;

	bool success=!data.startsWith("{\"success\":0");
	QString errorString;
	if(!success)errorString=getMidData("error\":\"","\"",&data);


	switch(reqType)
	{
	case 103: //ticker
		{
			QByteArray tickerHigh=getMidData("high\":",",\"",&data);
			if(!tickerHigh.isEmpty())
            {
                double newTickerHigh=tickerHigh.toDouble();
                if(newTickerHigh!=lastTickerHigh)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"High",newTickerHigh);
				lastTickerHigh=newTickerHigh;
			}

			QByteArray tickerLow=getMidData("\"low\":",",\"",&data);
			if(!tickerLow.isEmpty())
            {
                double newTickerLow=tickerLow.toDouble();
                if(newTickerLow!=lastTickerLow)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Low",newTickerLow);
				lastTickerLow=newTickerLow;
			}

			QByteArray tickerSell=getMidData("\"sell\":",",\"",&data);
			if(!tickerSell.isEmpty())
            {
                double newTickerSell=tickerSell.toDouble();
                if(newTickerSell!=lastTickerSell)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Sell",newTickerSell);
				lastTickerSell=newTickerSell;
			}

			QByteArray tickerBuy=getMidData("\"buy\":",",\"",&data);
			if(!tickerBuy.isEmpty())
            {
                double newTickerBuy=tickerBuy.toDouble();
                if(newTickerBuy!=lastTickerBuy)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Buy",newTickerBuy);
				lastTickerBuy=newTickerBuy;
			}

			QByteArray tickerVolume=getMidData("\"vol_cur\":",",\"",&data);
			if(!tickerVolume.isEmpty())
			{
                double newTickerVolume=tickerVolume.toDouble();
                if(newTickerVolume!=lastTickerVolume)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Volume",newTickerVolume);
				lastTickerVolume=newTickerVolume;
			}

			quint32 newTickerDate=getMidData("\"updated\":","}",&data).toUInt();
			if(lastTickerDate<newTickerDate)
			{
				lastTickerDate=newTickerDate;
				QByteArray tickerLast=getMidData("\"last\":",",\"",&data);
                double tickerLastDouble=tickerLast.toDouble();
                if(tickerLastDouble>0.0)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Last",tickerLastDouble);
			}
		}
		break;//ticker
	case 109: //trades
		{
			if(data.size()<10)break;
			QByteArray currentRequestSymbol=getMidData("\"","\":[{",&data).toUpper().replace("_","");
			QStringList tradeList=QString(data).split("},{");
			QList<TradesItem> *newTradesItems=new QList<TradesItem>;

			for(int n=tradeList.count()-1;n>=0;n--)
			{
				QByteArray tradeData=tradeList.at(n).toLatin1()+"}";
				TradesItem newItem;
				newItem.date=getMidData("timestamp\":","}",&tradeData).toUInt();
				newItem.price=getMidData("\"price\":",",\"",&tradeData).toDouble();
				if(lastFetchTid<0&&newItem.date<-lastFetchTid)continue;
				quint32 currentTid=getMidData("\"tid\":",",\"",&tradeData).toUInt();
				if(currentTid<1000||lastFetchTid>=currentTid)continue;
				lastFetchTid=currentTid;
				if(n==0&&lastTickerDate<newItem.date)
				{
					lastTickerDate=newItem.date;
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Last",newItem.price);
				}
				newItem.amount=getMidData("\"amount\":",",\"",&tradeData).toDouble();
				newItem.symbol=currentRequestSymbol;
				newItem.orderType=getMidData("\"type\":\"","\"",&tradeData)=="ask"?1:-1;

				if(newItem.isValid())(*newTradesItems)<<newItem;
				else if(debugLevel)logThread->writeLog("Invalid trades fetch data line:"+tradeData,2);
			}
            if(newTradesItems->count())emit addLastTrades(baseValues.currentPair.symbol,newTradesItems);
			else delete newTradesItems;
		}
		break;//trades
	case 110: //Fee
		{
			QStringList feeList=QString(getMidData("pairs\":{\"","}}}",&data)).split("},\"");
			for(int n=0;n<feeList.count();n++)
			{
				if(!feeList.at(n).startsWith(baseValues.currentPair.currRequestPair))continue;
				QByteArray currentFeeData=feeList.at(n).toLatin1()+",";
                double newFee=getMidData("fee\":",",",&currentFeeData).toDouble();
                if(newFee!=lastFee)emit accFeeChanged(baseValues.currentPair.symbol,newFee);
				lastFee=newFee;
			}
		}
		break;// Fee
	case 111: //depth
		if(data.startsWith("{\""+baseValues.currentPair.currRequestPair+"\":{\"asks"))
		{
			emit depthRequestReceived();

			if(lastDepthData!=data)
			{
				lastDepthData=data;
				depthAsks=new QList<DepthItem>;
				depthBids=new QList<DepthItem>;

                QMap<double,double> currentAsksMap;
				QStringList asksList=QString(getMidData("asks\":[[","]]",&data)).split("],[");
                double groupedPrice=0.0;
                double groupedVolume=0.0;
				int rowCounter=0;

				for(int n=0;n<asksList.count();n++)
				{
					if(baseValues.depthCountLimit&&rowCounter>=baseValues.depthCountLimit)break;
					QStringList currentPair=asksList.at(n).split(",");
					if(currentPair.count()!=2)continue;
                    double priceDouble=currentPair.first().toDouble();
                    double amount=currentPair.last().toDouble();

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
                                                                                         currentAsksList.at(n),0.0,true);
				lastDepthAsksMap=currentAsksMap;

                QMap<double,double> currentBidsMap;
				QStringList bidsList=QString(getMidData("bids\":[[","]]",&data)).split("],[");
				groupedPrice=0.0;
				groupedVolume=0.0;
				rowCounter=0;

				for(int n=0;n<bidsList.count();n++)
				{
					if(baseValues.depthCountLimit&&rowCounter>=baseValues.depthCountLimit)break;
					QStringList currentPair=bidsList.at(n).split(",");
					if(currentPair.count()!=2)continue;
                    double priceDouble=currentPair.first().toDouble();
                    double amount=currentPair.last().toDouble();
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
							if(!matchCurrentGroup||n==asksList.count()-1)
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
                                                                                         currentBidsList.at(n),0.0,false);
				lastDepthBidsMap=currentBidsMap;

                emit depthSubmitOrders(baseValues.currentPair.symbol,depthAsks, depthBids);
				depthAsks=0;
				depthBids=0;
			}
		}
		else if(debugLevel)logThread->writeLog("Invalid depth data:"+data,2);
		break;
	case 202: //info
		{
			if(!success)break;
			QByteArray fundsData=getMidData("funds\":{","}",&data)+",";
			QByteArray btcBalance=getMidData(baseValues.currentPair.currAStrLow+"\":",",",&fundsData);
			if(!btcBalance.isEmpty())
			{
                double newBtcBalance=btcBalance.toDouble();
                if(lastBtcBalance!=newBtcBalance)emit accBtcBalanceChanged(baseValues.currentPair.symbol,newBtcBalance);
				lastBtcBalance=newBtcBalance;
			}

			QByteArray usdBalance=getMidData("\""+baseValues.currentPair.currBStrLow+"\":",",",&fundsData);
			if(!usdBalance.isEmpty())
			{
                double newUsdBalance=usdBalance.toDouble();
                if(newUsdBalance!=lastUsdBalance)emit accUsdBalanceChanged(baseValues.currentPair.symbol,newUsdBalance);
				lastUsdBalance=newUsdBalance;
			}

			int openedOrders=getMidData("open_orders\":",",\"",&data).toInt();
			if(openedOrders==0&&lastOpenedOrders){lastOrders.clear(); emit ordersIsEmpty();}
			lastOpenedOrders=openedOrders;

			if(isFirstAccInfo)
			{
				QByteArray rights=getMidData("rights\":{","}",&data);
				if(!rights.isEmpty())
				{
                    bool isRightsGood=rights.contains("info\":true")&&rights.contains("trade\":true");
					if(!isRightsGood)emit showErrorMessage("I:>invalid_rights");
					isFirstAccInfo=false;
				}
			}
		}
		break;//info
	case 204://orders
		{
		if(data.size()<=30)break;
		bool isEmptyOrders=!success&&errorString==QLatin1String("no orders");if(isEmptyOrders)success=true;
		if(lastOrders!=data)
		{
			lastOrders=data;
			if(isEmptyOrders)
			{
				emit ordersIsEmpty();
				break;
			}
			data.replace("return\":{\"","},\"");
			QString rezultData;
			QStringList ordersList=QString(data).split("},\"");
			if(ordersList.count())ordersList.removeFirst();
            if(ordersList.count()==0)return;

			QList<OrderItem> *orders=new QList<OrderItem>;
			for(int n=0;n<ordersList.count();n++)
			{
				OrderItem currentOrder;
				QByteArray currentOrderData="{"+ordersList.at(n).toLatin1()+"}";

				currentOrder.oid=getMidData("{","\":{",&currentOrderData);
				currentOrder.date=getMidData("timestamp_created\":",",\"",&currentOrderData).toUInt();
				currentOrder.type=getMidData("type\":\"","\",\"",&currentOrderData)=="sell";
				currentOrder.status=getMidData("status\":","}",&currentOrderData).toInt()+1;
				currentOrder.amount=getMidData("amount\":",",\"",&currentOrderData).toDouble();
				currentOrder.price=getMidData("rate\":",",\"",&currentOrderData).toDouble();
				currentOrder.symbol=getMidData("pair\":\"","\",\"",&currentOrderData).toUpper().replace("_","");
				if(currentOrder.isValid())(*orders)<<currentOrder;
			}
            emit orderBookChanged(baseValues.currentPair.symbol,orders);
		}
		break;//orders
		}
	case 305: //order/cancel
		if(success)
		{
			QByteArray oid=getMidData("order_id\":",",\"",&data);
            if(!oid.isEmpty())emit orderCanceled(baseValues.currentPair.symbol,oid);
		}
		break;//order/cancel
	case 306: if(debugLevel)logThread->writeLog("Buy OK: "+data,2);break;//order/buy
	case 307: if(debugLevel)logThread->writeLog("Sell OK: "+data,2);break;//order/sell
    case 208: ///history
        {
		bool isEmptyOrders=!success&&errorString==QLatin1String("no trades");if(isEmptyOrders)success=true;
        if(lastHistory!=data)
		{
			lastHistory=data;
			if(!success)break;
			QList<HistoryItem> *historyItems=new QList<HistoryItem>;

            data=getMidData("return\":{","}}}",&data);
			QString newLog(data);
            QStringList dataList=newLog.split("},");
            if(dataList.count()==0)return;
            newLog.clear();
            quint32 currentId;
            quint32 maxId=0;
			for(int n=0;n<dataList.count();n++)
			{
                QByteArray curLog("~"+dataList.at(n).toLatin1()+"~");

                currentId=getMidData("~\"","\":{",&curLog).toUInt();
                if(currentId<=lastHistoryId)break;
                if(n==0)maxId=currentId;

				HistoryItem currentHistoryItem;
				QByteArray logType=getMidData("type\":\"","\",\"",&curLog);
				if(logType=="sell")currentHistoryItem.type=1;
				else 
				if(logType=="buy")currentHistoryItem.type=2;
				
				if(currentHistoryItem.type)
				{
					if(currentHistoryItem.type==1||currentHistoryItem.type==2)
						currentHistoryItem.symbol=getMidData("pair\":\"","\",\"",&curLog).toUpper().replace("_","");
                    currentHistoryItem.dateTimeInt=getMidData("timestamp\":","~",&curLog).toUInt();
					currentHistoryItem.price=getMidData("rate\":",",\"",&curLog).toDouble();
					currentHistoryItem.volume=getMidData("amount\":",",\"",&curLog).toDouble();
					if(currentHistoryItem.isValid())(*historyItems)<<currentHistoryItem;
				}
			}
            if(maxId>lastHistoryId)lastHistoryId=maxId;
			emit historyChanged(historyItems);
		}
		break;//money/wallet/history
		}
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
                QString authErrorString=getMidData("error\":\"","\"",&data);
                if(debugLevel)logThread->writeLog("API error: "+authErrorString.toLatin1()+" ReqType: "+QByteArray::number(reqType),2);

                if(authErrorString=="invalid api key")authErrorString=julyTr("TRUNAUTHORIZED","Invalid API key.");
                else if(authErrorString.startsWith("invalid nonce parameter"))authErrorString=julyTr("THIS_PROFILE_ALREADY_USED","Invalid nonce parameter.");
                if(!authErrorString.isEmpty())emit showErrorMessage(authErrorString);
            }
        }
        else authErrorCount=0;
    }

    static int errorCount=0;
    if(!success)
    {
        errorCount++;
        if(errorCount<3)return;
        if(debugLevel)logThread->writeLog("API error: "+errorString.toLatin1()+" ReqType: "+QByteArray::number(reqType),2);
        if(errorString.isEmpty())return;
        if(errorString==QLatin1String("no orders"))return;
        if(reqType<300)emit showErrorMessage("I:>"+errorString);
    }
    else errorCount=0;
}

void Exchange_BTCe::depthUpdateOrder(QString symbol, double price, double amount, bool isAsk)
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

void Exchange_BTCe::depthSubmitOrder(QString symbol, QMap<double,double> *currentMap ,double priceDouble, double amount, bool isAsk)
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

bool Exchange_BTCe::isReplayPending(int reqType)
{
	if(julyHttp==0)return false;
	return julyHttp->isReqTypePending(reqType);
}

void Exchange_BTCe::secondSlot()
{
    static int sendCounter=0;
    switch(sendCounter)
    {
    case 0:
        if(!isReplayPending(103))sendToApi(103,"ticker/"+baseValues.currentPair.currRequestPair,false,true);
        break;
    case 1:
        if(!isReplayPending(202))sendToApi(202,"",true,true,"method=getInfo&");
        break;
    case 2:
        if(!isReplayPending(109))sendToApi(109,"trades/"+baseValues.currentPair.currRequestPair,false,true);
        break;
    case 3:
        if(!tickerOnly&&!isReplayPending(204))sendToApi(204,"",true,true,"method=ActiveOrders&");
        break;
    case 4:
        if(isDepthEnabled()&&(forceDepthLoad||!isReplayPending(111)))
        {
            emit depthRequested();
            sendToApi(111,"depth/"+baseValues.currentPair.currRequestPair+"?limit="+baseValues.depthCountLimitStr,false,true);
            forceDepthLoad=false;
        }
        break;
    case 5:
        if(lastHistory.isEmpty())getHistory(false);
        break;
    default: break;
    }
    if(sendCounter++>=5)sendCounter=0;

	Exchange::secondSlot();
}

void Exchange_BTCe::getHistory(bool force)
{
	if(tickerOnly)return;
    if(force)lastHistory.clear();
    if(!isReplayPending(208))sendToApi(208,"",true,true,"method=TradeHistory&");
    if(!isReplayPending(110))sendToApi(110,"info",false,true);
}

void Exchange_BTCe::buy(QString symbol, double apiBtcToBuy, double apiPriceToBuy)
{
    if(tickerOnly)return;

    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol,pairItem);
    if(pairItem.symbol.isEmpty())return;

    QByteArray data="method=Trade&pair="+pairItem.currRequestPair+"&type=buy&rate="+byteArrayFromDouble(apiPriceToBuy,pairItem.priceDecimals,0)+"&amount="+byteArrayFromDouble(apiBtcToBuy,pairItem.currADecimals,0)+"&";
    if(debugLevel)logThread->writeLog("Buy: "+data,2);
	sendToApi(306,"",true,true,data);
}

void Exchange_BTCe::sell(QString symbol, double apiBtcToSell, double apiPriceToSell)
{
	if(tickerOnly)return;

    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol,pairItem);
    if(pairItem.symbol.isEmpty())return;

    QByteArray data="method=Trade&pair="+pairItem.currRequestPair+"&type=sell&rate="+byteArrayFromDouble(apiPriceToSell,pairItem.priceDecimals,0)+"&amount="+byteArrayFromDouble(apiBtcToSell,pairItem.currADecimals,0)+"&";
    if(debugLevel)logThread->writeLog("Sell: "+data,2);
	sendToApi(307,"",true,true,data);
}

void Exchange_BTCe::cancelOrder(QString,QByteArray order)
{
	if(tickerOnly)return;

	order.prepend("method=CancelOrder&order_id=");
	order.append("&");
	if(debugLevel)logThread->writeLog("Cancel order: "+order,2);
	sendToApi(305,"",true,true,order);
}

void Exchange_BTCe::sendToApi(int reqType, QByteArray method, bool auth, bool sendNow, QByteArray commands)
{
	if(julyHttp==0)
	{ 
        if(baseValues.alternativeDomainBTCe){julyHttp=new JulyHttp("api.tidex.com","Key: "+getApiKey()+"\r\n",this);}
        else julyHttp=new JulyHttp("api.tidex.com","Key: "+getApiKey()+"\r\n",this);

		connect(julyHttp,SIGNAL(anyDataReceived()),baseValues_->mainWindow_,SLOT(anyDataReceived()));
		connect(julyHttp,SIGNAL(apiDown(bool)),baseValues_->mainWindow_,SLOT(setApiDown(bool)));
		connect(julyHttp,SIGNAL(setDataPending(bool)),baseValues_->mainWindow_,SLOT(setDataPending(bool)));
		connect(julyHttp,SIGNAL(errorSignal(QString)),baseValues_->mainWindow_,SLOT(showErrorMessage(QString)));
		connect(julyHttp,SIGNAL(sslErrorSignal(const QList<QSslError> &)),this,SLOT(sslErrors(const QList<QSslError> &)));
		connect(julyHttp,SIGNAL(dataReceived(QByteArray,int)),this,SLOT(dataReceivedAuth(QByteArray,int)));
	}

	if(auth)
    {
        QByteArray postData=commands+"nonce="+QByteArray::number(++privateNonce);
		if(sendNow)
			julyHttp->sendData(reqType, "POST /tapi/"+method, postData, "Sign: "+hmacSha512(getApiSign(),postData).toHex()+"\r\n");
		else
			julyHttp->prepareData(reqType, "POST /tapi/"+method, postData, "Sign: "+hmacSha512(getApiSign(),postData).toHex()+"\r\n");
	}
	else
	{
		if(commands.isEmpty())
		{
			if(sendNow)
				julyHttp->sendData(reqType, "GET /api/3/"+method);
			else 
				julyHttp->prepareData(reqType, "GET /api/3/"+method);
		}
		else
		{
			if(sendNow)
				julyHttp->sendData(reqType, "POST /api/3/"+method, commands);
			else 
				julyHttp->prepareData(reqType, "POST /api/3/"+method, commands);
		}
	}
}

void Exchange_BTCe::sslErrors(const QList<QSslError> &errors)
{
	QStringList errorList;
	for(int n=0;n<errors.count();n++)errorList<<errors.at(n).errorString();
	if(debugLevel)logThread->writeLog(errorList.join(" ").toLatin1(),2);
	emit showErrorMessage("SSL Error: "+errorList.join(" "));
}

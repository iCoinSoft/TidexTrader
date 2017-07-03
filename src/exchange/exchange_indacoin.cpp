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

#include "exchange_indacoin.h"
#include <openssl/hmac.h>
#include <openssl/ecdsa.h>

Exchange_Indacoin::Exchange_Indacoin(QByteArray pRestSign, QByteArray pRestKey)
    : Exchange()
{
    calculatingFeeMode=1;
    baseValues.exchangeName="Indacoin";
    baseValues.currentPair.name="BTC/USD";
    baseValues.currentPair.setSymbol("BTCUSD");
    baseValues.currentPair.currRequestPair="btc_usd";
    baseValues.currentPair.priceDecimals=3;
    minimumRequestIntervalAllowed=700;
	minimumRequestTimeoutAllowed=10000;
    baseValues.currentPair.priceMin=qPow(0.1,baseValues.currentPair.priceDecimals);
    baseValues.currentPair.tradeVolumeMin=0.01;
    baseValues.currentPair.tradePriceMin=0.1;
    depthAsks=0;
    depthBids=0;
    forceDepthLoad=false;
    julyHttp=0;
    isApiDown=false;
    tickerOnly=false;

	setApiKeySecret(pRestKey,QByteArray::fromBase64(pRestSign));

    moveToThread(this);

    currencyMapFile="Indacoin";
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
    lastHistoryTs=0;
}

Exchange_Indacoin::~Exchange_Indacoin()
{
}

void Exchange_Indacoin::clearVariables()
{
    isFirstAccInfo=true;
    cancelingOrderIDs.clear();
    Exchange::clearVariables();
    lastOpenedOrders=-1;
    apiDownCounter=0;
    lastHistory.clear();
    lastOrders.clear();
    reloadDepth();
    lastFetchTid=0;
    lastFetchDate=QDateTime::currentDateTime().toTime_t()-600;
    lastTickerDate=0;
}

void Exchange_Indacoin::clearValues()
{
    clearVariables();
    if(julyHttp)julyHttp->clearPendingData();
}

void Exchange_Indacoin::reloadDepth()
{
    lastDepthBidsMap.clear();
    lastDepthAsksMap.clear();
    lastDepthData.clear();
    Exchange::reloadDepth();
}

void Exchange_Indacoin::dataReceivedAuth(QByteArray data, int reqType)
{
    if(debugLevel)logThread->writeLog("RCV: "+data);
    if(data.size()==0)return;
    //if(data.at(0)==QLatin1Char('<'))return;
    bool success=data!=QByteArray("\"unauthorized\"");//!data.startsWith("{\"success\":0");
    QString errorString;
    if(!success)errorString=data;


    switch(reqType)
    {
    case 103: //ticker
        {
            data=getMidData("\""+baseValues.currentPair.currRequestPair.toUpper()+"\":{","}",&data);
            if(data.size()<10)break;
            QByteArray tickerHigh=getMidData("max_price\":\"","\"",&data);
            if(!tickerHigh.isEmpty())
            {
                double newTickerHigh=tickerHigh.toDouble();
                if(newTickerHigh!=lastTickerHigh)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"High",newTickerHigh);
                lastTickerHigh=newTickerHigh;
            }

            QByteArray tickerLow=getMidData("\"min_price\":\"","\"",&data);
            if(!tickerLow.isEmpty())
            {
                double newTickerLow=tickerLow.toDouble();
                if(newTickerLow!=lastTickerLow)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Low",newTickerLow);
                lastTickerLow=newTickerLow;
            }

            QByteArray tickerLast=getMidData("\"last_price\":\"","\"",&data);
            if(!tickerLast.isEmpty())
            {
                double newTickerLast=tickerLast.toDouble();
                if(newTickerLast!=lastTickerLast)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Last",newTickerLast);
                lastTickerLast=newTickerLast;
            }

            QByteArray tickerVolume=getMidData("\"volume_base\":\"","\"",&data);
            if(!tickerVolume.isEmpty())
            {
                double newTickerVolume=tickerVolume.toDouble();
                if(newTickerVolume!=lastTickerVolume)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Volume",newTickerVolume);
                lastTickerVolume=newTickerVolume;
            }
        }
        break;//ticker
    case 109: //trades
        {
            if(data.size()<10)break;
            QStringList tradeList=QString(data).split("},{");
            QList<TradesItem> *newTradesItems=new QList<TradesItem>;

            TradesItem newItem;
            for(int n=0;n<tradeList.count();n++)
            {
                QByteArray tradeData=tradeList.at(n).toLatin1()+"}";

                quint32 currentTid=getMidData("\"tid\":",",",&tradeData).toUInt();
                if(currentTid<1000||currentTid<=lastFetchTid)continue;
                lastFetchTid=currentTid;

                newItem.date=getMidData("date\":","}",&tradeData).toUInt();
                if(newItem.date>lastFetchDate)lastFetchDate=newItem.date;

                newItem.price=getMidData("\"price\":",",",&tradeData).toDouble();
                newItem.amount=getMidData("\"amount\":",",",&tradeData).toDouble();
                newItem.orderType=getMidData("\"oper_type\":",",",&tradeData)!="1"?1:-1;
                newItem.symbol=baseValues.currentPair.symbol;

                if(newItem.isValid())(*newTradesItems)<<newItem;
                else if(debugLevel)logThread->writeLog("Invalid trades fetch data line:"+tradeData,2);
            }

            if(lastTickerDate<newItem.date)
            {
                lastTickerDate=newItem.date;
                IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Last",newItem.price);
            }
            if(newTradesItems->count())emit addLastTrades(baseValues.currentPair.symbol,newTradesItems);
            else delete newTradesItems;
        }
        break;//trades
    case 111: //depth
        if(data.startsWith("{\"bids\":[["))
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

                if(asksList.count()==0)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Buy",0);

                for(int n=0;n<asksList.count();n++)
                {
                    if(baseValues.depthCountLimit&&rowCounter>=baseValues.depthCountLimit)break;
                    QStringList currentPair=asksList.at(n).split(",");
                    if(currentPair.count()!=2)continue;
                    double priceDouble=currentPair.first().toDouble();
                    double amount=currentPair.last().toDouble();

                    if(n==0){
                        if(priceDouble!=lastTickerBuy)
                            IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Buy",priceDouble);
                        lastTickerBuy=priceDouble;
                    }

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

                if(bidsList.count()==0)
                    IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Sell",0);

                for(int n=0;n<bidsList.count();n++)
                {
                    if(baseValues.depthCountLimit&&rowCounter>=baseValues.depthCountLimit)break;
                    QStringList currentPair=bidsList.at(n).split(",");
                    if(currentPair.count()!=2)continue;
                    double priceDouble=currentPair.first().toDouble();
                    double amount=currentPair.last().toDouble();

                    if(n==0){
                        if(priceDouble!=lastTickerSell)
                            IndicatorEngine::setValue(baseValues.exchangeName,baseValues.currentPair.symbol,"Sell",priceDouble);
                        lastTickerSell=priceDouble;
                    }

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
            //QByteArray fundsData=getMidData("funds\":{","}",&data)+",";
            QByteArray btcBalance=getMidData(baseValues.currentPair.currAStr+"\",\"","\"",&data);//fundsData);
            if(!btcBalance.isEmpty())
            {
                double newBtcBalance=btcBalance.toDouble();
                if(lastBtcBalance!=newBtcBalance)emit accBtcBalanceChanged(baseValues.currentPair.symbol,newBtcBalance);
                lastBtcBalance=newBtcBalance;
            }

            QByteArray usdBalance=getMidData("\""+baseValues.currentPair.currBStr+"\",\"","\"",&data);//fundsData);
            if(!usdBalance.isEmpty())
            {
                double newUsdBalance=usdBalance.toDouble();
                if(newUsdBalance!=lastUsdBalance)emit accUsdBalanceChanged(baseValues.currentPair.symbol,newUsdBalance);
                lastUsdBalance=newUsdBalance;
            }

            emit accFeeChanged(baseValues.currentPair.symbol,0.15);
        }
        break;//info
    case 204://orders
        if(data.size()&&(data[0]=='['||data[0]=='{'))
        {
        if(data.size()<2)break;
        if(lastOrders!=data)
        {
            lastOrders=data;
            bool isEmptyOrders=(data=="[]");if(isEmptyOrders)success=true;
            if(isEmptyOrders)
            {
                emit ordersIsEmpty();
                break;
            }
            QStringList ordersList=QString(getMidData("[[","]]",&data)).split("],[");
            if(ordersList.count()==0)return;

            QList<OrderItem> *orders=new QList<OrderItem>;
            for(int n=0;n<ordersList.count();n++)
            {
                QStringList currentOrderList=ordersList.at(n).split("\",\"");
                OrderItem currentOrder;
                currentOrder.oid=currentOrderList.at(0).toLatin1();
                currentOrder.oid.remove(0,1);
                currentOrder.symbol=currentOrderList.at(1).toUpper().replace("_","");
                currentOrder.type=currentOrderList.at(2)=="SELL";
                currentOrder.price=currentOrderList.at(3).toDouble();
                currentOrder.amount=currentOrderList.at(5).toDouble();
                currentOrder.date=currentOrderList.at(6).toUInt();
                currentOrder.status=1;
                if(currentOrder.isValid())(*orders)<<currentOrder;
            }
            emit orderBookChanged(baseValues.currentPair.symbol,orders);
        }
        break;//orders
        }
    case 305: //order/cancel
        if(!success)break;
        if(!cancelingOrderIDs.isEmpty())
        {
            if(data=="\"success\"")
            {
                emit orderCanceled(baseValues.currentPair.symbol,cancelingOrderIDs.first());
                if(debugLevel)logThread->writeLog("Order canceled:"+cancelingOrderIDs.first(),2);
                cancelingOrderIDs.removeFirst();
            }
        }
        break;//order/cancel
    case 306:
        if(debugLevel)logThread->writeLog("Buy OK: "+data,2);break;//order/buy
    case 307:
        if(debugLevel)logThread->writeLog("Sell OK: "+data,2);break;//order/sell
    case 208: ///history
        {
        bool isEmptyOrders=!success&&errorString==QLatin1String("no trades");if(isEmptyOrders)success=true;
        if(lastHistory!=data)
        {
            lastHistory=data;
            if(!success)break;
            QList<HistoryItem> *historyItems=new QList<HistoryItem>;

            QString newLog(getMidData("[[","]]",&data));
            QStringList dataList=newLog.split("],[");
            if(dataList.count()==0)return;
            quint32 maxTs=0;
            for(int n=0;n<dataList.count();n++)
            {
                QString curLog(dataList.at(n));
                curLog.remove(0,1);
                curLog.chop(1);
                QStringList curLogList=curLog.split("\",\"");

                HistoryItem currentHistoryItem;
                if     (curLogList.at(0)=="SELL")currentHistoryItem.type=1;
                else if(curLogList.at(0)=="BUY" )currentHistoryItem.type=2;
                else if(curLogList.at(0)=="IN"  )currentHistoryItem.type=4;
                else if(curLogList.at(0)=="OUT" )currentHistoryItem.type=5;
                else continue;

                if(currentHistoryItem.type==1||currentHistoryItem.type==2){
                    currentHistoryItem.symbol=curLogList.at(1).toUpper().replace("_","");
                    currentHistoryItem.price=curLogList.at(2).toDouble();
                    currentHistoryItem.volume=curLogList.at(3).toDouble();
                    currentHistoryItem.dateTimeInt=curLogList.at(6).toUInt();
                }

                if(currentHistoryItem.type==4||currentHistoryItem.type==5){
                    currentHistoryItem.symbol=curLogList.at(2).toUpper()+"   ";
                    currentHistoryItem.volume=curLogList.at(3).toDouble();
                    currentHistoryItem.dateTimeInt=curLogList.at(4).toUInt();
                }

                if(currentHistoryItem.dateTimeInt<=lastHistoryTs)break;
                if(n==0)maxTs=currentHistoryItem.dateTimeInt;

                if(currentHistoryItem.isValid())(*historyItems)<<currentHistoryItem;
            }
            if(maxTs>lastHistoryTs)lastHistoryTs=maxTs;
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
                QString authErrorString=data;
                if(data=="\"unauthorized\"")authErrorString="unauthorized";
                if(debugLevel)logThread->writeLog("API error: "+authErrorString.toLatin1()+" ReqType: "+QByteArray::number(reqType),2);

                if(authErrorString=="unauthorized")authErrorString=julyTr("TRUNAUTHORIZED","Invalid API key.")+"<br><br>"+julyTr("THIS_PROFILE_ALREADY_USED","Invalid nonce parameter.");
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
        if(debugLevel)logThread->writeLog("API error: "+errorString.toLatin1()+" ReqType:"+QByteArray::number(reqType),2);
        if(errorString.isEmpty())return;
        if(errorString==QLatin1String("no orders"))return;
        if(reqType<300)emit showErrorMessage("I:>"+errorString);
    }
    else errorCount=0;
}

void Exchange_Indacoin::depthUpdateOrder(QString symbol, double price, double amount, bool isAsk)
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

void Exchange_Indacoin::depthSubmitOrder(QString symbol, QMap<double,double> *currentMap ,double priceDouble, double amount, bool isAsk)
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

bool Exchange_Indacoin::isReplayPending(int reqType)
{
    if(julyHttp==0)return false;
    return julyHttp->isReqTypePending(reqType);
}

void Exchange_Indacoin::secondSlot()
{privateNonce=1;
    static int sendCounter=0;
    switch(sendCounter)
    {
    case 0:
        if(!isReplayPending(103))sendToApi(103,"ticker",false,true);
        break;
    case 1:
        if(!isReplayPending(202))sendToApi(202,"getbalance",true,true,"");
        break;
    case 2:
        if(!isReplayPending(109))sendToApi(109,"2/trades/"+baseValues.currentPair.currRequestPair+"/0/"+QByteArray::number(lastFetchDate),false,true);
        break;
    case 3:
        if(!tickerOnly&&!isReplayPending(204))sendToApi(204,"openorders",true,true,"");
        break;
    case 4:
        if(isDepthEnabled()&&(forceDepthLoad||!isReplayPending(111)))
        {
            emit depthRequested();
            sendToApi(111,"orderbook?pair="+baseValues.currentPair.currRequestPair/*+"?limit="+baseValues.depthCountLimitStr*/,false,true);
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

void Exchange_Indacoin::getHistory(bool force)
{
    if(tickerOnly)return;
    if(force)lastHistory.clear();
    if(!isReplayPending(208))sendToApi(208,"gethistory",true,true,"");
}

void Exchange_Indacoin::buy(QString symbol, double apiBtcToBuy, double apiPriceToBuy)
{
    if(tickerOnly)return;

    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol,pairItem);
    if(pairItem.symbol.isEmpty())return;

    QByteArray data="pair:'"+pairItem.currRequestPair.toUpper()+"',price:'"+byteArrayFromDouble(apiPriceToBuy,pairItem.priceDecimals,0)+"',amount:'"+byteArrayFromDouble(apiBtcToBuy,pairItem.currADecimals,0)+"'";
    if(debugLevel)logThread->writeLog("Buy: "+data,2);
    sendToApi(306,"buyorder",true,true,data);
}

void Exchange_Indacoin::sell(QString symbol, double apiBtcToSell, double apiPriceToSell)
{
    if(tickerOnly)return;

    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol,pairItem);
    if(pairItem.symbol.isEmpty())return;

    QByteArray data="pair:'"+pairItem.currRequestPair.toUpper()+"',price:'"+byteArrayFromDouble(apiPriceToSell,pairItem.priceDecimals,0)+"',amount:'"+byteArrayFromDouble(apiBtcToSell,pairItem.currADecimals,0)+"'";
    if(debugLevel)logThread->writeLog("Sell: "+data,2);
    sendToApi(307,"sellorder",true,true,data);
}

void Exchange_Indacoin::cancelOrder(QString,QByteArray order)
{
    if(tickerOnly)return;
    cancelingOrderIDs<<order;
    if(debugLevel)logThread->writeLog("Cancel order: "+order,2);
    sendToApi(305,"cancelorder",true,true,"i:'"+order+"'");
}


QByteArray Exchange_Indacoin::ecdsaSha1(QByteArray shaKey, QByteArray &data)
{
    EC_KEY *eckey=EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(eckey);
    BIGNUM *tempPrivateKey=BN_new();

    BN_bin2bn((unsigned char *)shaKey.data(), shaKey.length(), tempPrivateKey);
    EC_KEY_set_private_key(eckey,tempPrivateKey);

    QByteArray rezult; rezult.resize(ECDSA_size(eckey));
    quint32 len=rezult.size();
    ECDSA_sign(0, (unsigned char *)QCryptographicHash::hash(data,QCryptographicHash::Sha1).data(), 20, (unsigned char *)rezult.data(), &len, eckey);

    BN_free(tempPrivateKey);
    return rezult;
}

void Exchange_Indacoin::sendToApi(int reqType, QByteArray method, bool auth, bool sendNow, QByteArray commands)
{
    if(julyHttp==0)
    {
        julyHttp=new JulyHttp("indacoin.com","",this,true,true,"application/json; charset=UTF-8");
        connect(julyHttp,SIGNAL(anyDataReceived()),baseValues_->mainWindow_,SLOT(anyDataReceived()));
        connect(julyHttp,SIGNAL(apiDown(bool)),baseValues_->mainWindow_,SLOT(setApiDown(bool)));
        connect(julyHttp,SIGNAL(setDataPending(bool)),baseValues_->mainWindow_,SLOT(setDataPending(bool)));
        connect(julyHttp,SIGNAL(errorSignal(QString)),baseValues_->mainWindow_,SLOT(showErrorMessage(QString)));
        connect(julyHttp,SIGNAL(sslErrorSignal(const QList<QSslError> &)),this,SLOT(sslErrors(const QList<QSslError> &)));
        connect(julyHttp,SIGNAL(dataReceived(QByteArray,int)),this,SLOT(dataReceivedAuth(QByteArray,int)));
    }

    if(auth)
    {
        QByteArray nonceStr=QByteArray::number(++privateNonce);
        QByteArray postData='{'+commands+'}';

        QByteArray appendHeaders= "API-Key: "+getApiKey()+"\r\n"
								  "API-Nonce: "+nonceStr+"\r\n";
        nonceStr.prepend(method); nonceStr.append(getApiKey());
                   appendHeaders+="API-Sign: "+ecdsaSha1(getApiSign(),nonceStr).toBase64()+"\r\n";
        if(sendNow)
            julyHttp->sendData(reqType, "POST /api/"+method, postData, appendHeaders);
        else
            julyHttp->prepareData(reqType, "POST /api/"+method, postData, appendHeaders);
    }
    else
    {
        if(commands.isEmpty())
        {
            if(sendNow)
                julyHttp->sendData(reqType, "GET /api/"+method);
            else
                julyHttp->prepareData(reqType, "GET /api/"+method);
        }
        else
        {
            if(sendNow)
                julyHttp->sendData(reqType, "POST /api/"+method, commands);
            else
                julyHttp->prepareData(reqType, "POST /api/"+method, commands);
        }
    }
}

void Exchange_Indacoin::sslErrors(const QList<QSslError> &errors)
{
    QStringList errorList;
    for(int n=0;n<errors.count();n++)errorList<<errors.at(n).errorString();
    if(debugLevel)logThread->writeLog(errorList.join(" ").toLatin1(),2);
    emit showErrorMessage("SSL Error: "+errorList.join(" "));
}

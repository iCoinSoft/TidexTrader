

#include "tradesitem.h"
#include "main.h"
#include "julymath.h"

TradesItem::TradesItem()
{
	backGray=false;
	displayFullDate=false;
	date=0;
	amount=0.0;
	price=0.0;
	total=0.0;
	orderType=0;
	direction=0;
}

void TradesItem::cacheStrings()
{
	QDateTime itemDate=QDateTime::fromTime_t(date);

    if(baseValues_->use24HourTimeFormat){
        timeStr=itemDate.toString(baseValues.timeFormat);
        dateStr=itemDate.toString(baseValues.dateTimeFormat);
    } else {
        QString mmssTemp=itemDate.toString("mm:ss");
        QString hTemp=itemDate.toString("H");
        qint16 hTempInt=hTemp.toInt();
        if(hTempInt<=12)timeStr=hTemp+':'+mmssTemp+" am";
        else timeStr=QString::number(hTempInt-12)+':'+mmssTemp+" pm";
        dateStr=itemDate.toString("dd.MM.yyyy")+' '+timeStr;
    }

    if(price>0.0)priceStr=textFromDouble(price,baseValues.decimalsPriceLastTrades);
    if(amount>0.0)amountStr=textFromDouble(amount,baseValues.decimalsAmountLastTrades);
    if(amount>0.0&&price>0.0)totalStr=textFromDouble(price*amount,qMin(baseValues.currentPair.currBDecimals,baseValues.decimalsTotalLastTrades));
}

bool TradesItem::isValid()
{
	bool valid=date>0&&price>0.0&&amount>0.0;
	if(valid)cacheStrings();
	return valid;
}

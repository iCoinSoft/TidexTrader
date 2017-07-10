
#include "historyitem.h"
#include "main.h"
#include "julymath.h"

HistoryItem::HistoryItem()
{
	displayFullDate=false;
	dateTimeInt=0;
	volume=0.0;
	price=0.0;
	total=0.0;
	type=0;
}

void HistoryItem::cacheStrings()
{
	QDateTime cachedDateTime=QDateTime::fromTime_t(dateTimeInt);

    if(baseValues_->use24HourTimeFormat){
        timeStr=cachedDateTime.toString(baseValues.timeFormat);
        dateTimeStr=cachedDateTime.toString(baseValues.dateTimeFormat);
    } else {
        QString mmssTemp=cachedDateTime.toString("mm:ss");
        QString hTemp=cachedDateTime.toString("H");
        qint16 hTempInt=hTemp.toInt();
        if(hTempInt<=12)timeStr=hTemp+':'+mmssTemp+" am";
        else timeStr=QString::number(hTempInt-12)+':'+mmssTemp+" pm";
        dateTimeStr=cachedDateTime.toString("dd.MM.yyyy")+' '+timeStr;
    }

    cachedDateTime.setTime(QTime(0,0,0,0));
	dateInt=cachedDateTime.toTime_t();

	QString usdSign=baseValues.currencyMap.value(symbol.right(3),CurencyInfo("$")).sign;
    if(price>0.0)priceStr=usdSign+textFromDouble(price,baseValues.decimalsPriceMyTransactions);
	if(volume>0.0)
	{
        volumeStr=baseValues.currencyMap.value(symbol.left(3),CurencyInfo("BTC")).sign+textFromDouble(volume,baseValues.decimalsAmountMyTransactions);
	}
	if(volume>0.0&&price>0.0)
    {
        totalStr=textFromDouble(price*volume,baseValues.decimalsTotalMyTransactions);

		if(!baseValues.forceDotInSpinBoxes)
		{
			priceStr.replace(".",",");
			volumeStr.replace(".",",");
			totalStr.replace(".",",");
		}
	}
}

bool HistoryItem::isValid()
{
	bool valid=dateTimeInt>0&&symbol.size()==6;
	if(valid)cacheStrings();
	return valid;
}

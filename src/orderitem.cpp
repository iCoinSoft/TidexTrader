

#include "orderitem.h"
#include "main.h"
#include "julymath.h"

bool OrderItem::isValid()
{
	bool isVal=date>0&&price>0.0&&symbol.size()==6;
	if(isVal)
	{
        QDateTime itemDate=QDateTime::fromTime_t(date);
        if(baseValues_->use24HourTimeFormat){
            dateStr=itemDate.toString(baseValues.dateTimeFormat);
        } else {
            QString mmssTemp=itemDate.toString("mm:ss");
            QString hTemp=itemDate.toString("H");
            qint16 hTempInt=hTemp.toInt();
            QString timeStr;
            if(hTempInt<=12)timeStr=hTemp+':'+mmssTemp+" am";
            else timeStr=QString::number(hTempInt-12)+':'+mmssTemp+" pm";
            dateStr=itemDate.toString("dd.MM.yyyy")+' '+timeStr;
        }

		QString priceSign=baseValues.currencyMap.value(symbol.right(3),CurencyInfo("$")).sign;
        amountStr=baseValues.currencyMap.value(symbol.left(3),CurencyInfo("$")).sign+textFromDouble(amount);
        priceStr=priceSign+textFromDouble(price);
		total=price*amount;
        totalStr=priceSign+textFromDouble(total,baseValues.currentPair.currBDecimals);
	}
	return isVal;
}


#include "depthitem.h"
#include "main.h"
#include "julymath.h"

bool DepthItem::isValid()
{
	bool valid=price>=0.0&&volume>=0.0;
	if(valid)
	{
        priceStr=textFromDouble(price,qMin(baseValues.currentPair.priceDecimals,baseValues.decimalsPriceOrderBook));

        int decimals=qMin(baseValues.currentPair.currADecimals,baseValues.decimalsAmountOrderBook);
        volumeStr=textFromDouble(volume,decimals,decimals);
	}
	return valid;
}

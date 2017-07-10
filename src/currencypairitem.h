
#ifndef CURRENCYPAIRITEM_H
#define CURRENCYPAIRITEM_H

#include "currencyinfo.h"

struct CurrencyPairItem
{
	CurrencyPairItem();

	void setSymbol(QByteArray symb);

	QString currAName;
	QString name;

	CurencyInfo currAInfo;
	CurencyInfo currBInfo;

    QString symbol;
    QString symbolSecond() const {return symbol+currRequestSecond;}
	QByteArray currRequestPair;
	QByteArray currRequestSecond;

    QString currAStr;
    QString currAStrLow;
    QString currASign;

    QString currBStr;
    QString currBStrLow;
    QString currBSign;

    double priceMin;//qPow(0.1,priceDecimals);
    //double priceMax;
    double tradePriceMin;
    double tradeVolumeMin;

	int priceDecimals;
	int currADecimals;
	int currBDecimals;
	int currABalanceDecimals;
	int currBBalanceDecimals;
};

#endif // CURRENCYPAIRITEM_H

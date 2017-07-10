

#ifndef TRADESITEM_H
#define TRADESITEM_H

#include <QObject>

struct TradesItem
{
	TradesItem();
	bool backGray;

	bool displayFullDate;
	quint32 date;
	QString dateStr;
	QString timeStr;

    double amount;
	QString amountStr;

    double price;
	QString priceStr;

    double total;
	QString totalStr;

    QString symbol;//Like a "BTCUSD" 6 symbols only

	int orderType;//-1:Bid; 0:None; 1:Ask

	int direction;//-1:Down; 0: None; 1:Up

	void cacheStrings();

    bool isValid();
};

#endif // TRADESITEM_H

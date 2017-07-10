
#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <QObject>

class HistoryItem
{
public:
	HistoryItem();

	bool displayFullDate;
	quint32 dateTimeInt;
	quint32 dateInt;
	QString dateTimeStr;
	QString timeStr;
	QString description;

    double volume;
	QString volumeStr;

    double price;
	QString priceStr;

    double total;
	QString totalStr;

    QString symbol;

	int type; //0=General, 1=Sell, 2=Buy, 3=Fee, 4=Deposit, 5=Withdraw

	void cacheStrings();

	bool isValid();
};

#endif // HISTORYITEM_H

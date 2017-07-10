

#ifndef ORDERITEM_H
#define ORDERITEM_H
#include <QByteArray>
#include <QString>

struct OrderItem 
{
	QByteArray oid;
	quint32 date;
	QString dateStr;
	bool type;//true=Ask, false=Bid
	int status;//0=Canceled, 1=Open, 2=Pending, 3=Post-Pending
    double amount;
	QString amountStr;
    double price;
	QString priceStr;
    double total;
	QString totalStr;
    QString symbol;
	bool isValid();
};

#endif // ORDERITEM_H

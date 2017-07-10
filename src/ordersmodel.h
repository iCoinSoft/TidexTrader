

#ifndef ORDERSMODEL_H
#define ORDERSMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "orderitem.h"

class OrdersModel : public QAbstractItemModel
{
	Q_OBJECT

public:
    int getAsksCount();
	int getRowNum(int row);
	QByteArray getRowOid(int row);
	quint32 getRowDate(int row);
	int getRowType(int row);
	int getRowStatus(int row);
    double getRowPrice(int row);
    double getRowVolume(int row);
    double getRowTotal(int row);

    QMap<double,bool> currentAsksPrices;
    QMap<double,bool> currentBidsPrices;

	bool checkDuplicatedOID;
    void ordersCancelAll(QString pair=0);
    void ordersCancelBids(QString pair=0);
    void ordersCancelAsks(QString pair=0);
	void setOrderCanceled(QByteArray);

	void clear();

	OrdersModel();
	~OrdersModel();

	void orderBookChanged(QList<OrderItem> *ordersRcv);

	void setHorizontalHeaderLabels(QStringList list);

	QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QList<OrderItem> orders;

private:
    void ordersCountChanged();
    void ordersAsksCountChanged();
    void ordersBidsCountChanged();

    int lastOrdersCount;
    int lastAsksCount;
    int lastBidsCount;

    int asksCount;

	QHash<QByteArray,quint32> oidMapForCheckingDuplicates;
	QStringList textStatusList;
	QString textAsk;
	QString textBid;

	bool haveOrders;

	int countWidth;
	int columnsCount;
	int dateWidth;
	int typeWidth;
	int statusWidth;

	QStringList headerLabels;

	QList<QByteArray> oidList;

	QList<quint32> dateList;
	QStringList dateStrList;

	QList<bool> typesList;

	QList<int> statusList;

    QList<double> amountList;
	QStringList amountStrList;

    QList<double> priceList;
	QStringList priceStrList;

    QList<double> totalList;
	QStringList totalStrList;

	QStringList symbolList;

signals:
	void cancelOrder(QString, QByteArray);
	void ordersIsAvailable();
    void volumeAmountChanged(double, double);
};

#endif // ORDERSMODEL_H

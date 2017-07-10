

#ifndef TRADESMODEL_H
#define TRADESMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "tradesitem.h"

class TradesModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TradesModel();
	~TradesModel();
	void updateTotalBTC();
	double getRowPrice(int);
	double getRowVolume(int);
	int getRowType(int);
	void clear();
	void removeDataOlderThen(quint32);
	void addNewTrades(QList<TradesItem> *);

	void setHorizontalHeaderLabels(QStringList list);

	QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
	double lastPrecentBids;
	quint32 lastRemoveDate;
	void removeFirst();
	QString textBid;
	QString textAsk;
	
	double lastPrice;
	int columnsCount;
	int dateWidth;
	int typeWidth;

	QStringList headerLabels;

	QList<TradesItem> itemsList;
signals:
	void precentBidsChanged(double);
	void trades10MinVolumeChanged(double);
	void addChartsTrades(QList<TradesItem> *);
    void addChartsData(QList<TradesItem> *);
};

#endif // TRADESMODEL_H


#ifndef DEPTHMODEL_H
#define DEPTHMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "depthitem.h"

class DepthModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	void reloadVisibleItems();
	void fixTitleWidths();
	int itemsCount(){return volumeList.count();}
	void calculateSize();
	void clear();
	void setHorizontalHeaderLabels(QStringList list);
	void setAsk(bool on){isAsk=on;};
	DepthModel(bool isAskData=true);
	~DepthModel();
	double rowPrice(int row);
	double rowVolume(int row);
	double rowSize(int row);

	QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	void depthUpdateOrders(QList<DepthItem> *items);
	void depthFirstOrder(double price, double volume);

    double getVolumeByPrice(double);
    double getPriceByVolume(double);

private slots:
	void delayedReloadVisibleItems();
private:
    void depthUpdateOrder(DepthItem item);
	bool originalIsAsk;
	bool somethingChanged;
	double groupedPrice;
	double groupedVolume;

	int widthPrice;
	int widthVolume;
	int widthSize;

	int widthPriceTitle;
	int widthVolumeTitle;
	int widthSizeTitle;

	int columnsCount;
	QStringList headerLabels;
	bool isAsk;
    QList<double> volumeList;
    QList<double> sizeList;
    double &sizeListAt(int);
    double sizeListGet(int) const;
    void sizeListRemoveAt(int);
    QList<double> sizePriceList;
	QList<double> priceList;
	QStringList volumeListStr;
	QStringList sizeListStr;
	QStringList priceListStr;
	QList<int> directionList;
};

#endif // DEPTHMODEL_H



#ifndef ALLEXCHANGESMODEL_H
#define ALLEXCHANGESMODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class AllExchangesModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	AllExchangesModel();
	~AllExchangesModel();

	quint32 rowsCount;
	void addExchange(quint32,QString, QString);

private:
    int columnsCount;
	QList<quint32> exchangeIdList;
	QStringList nameList;
	QStringList currenciesList;
	QStringList headerList;

	QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

};

#endif // ALLEXCHANGESMODEL_H

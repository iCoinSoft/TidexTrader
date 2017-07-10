

#include "allexchangesmodel.h"
#include "main.h"

AllExchangesModel::AllExchangesModel()
	: QAbstractItemModel()
{
	rowsCount=0;
	columnsCount=2;

	headerList.append(julyTr("NAME","Name"));
	headerList.append(julyTr("SUPPORTED_CUEEENCIRS","Supported currencies"));
}

AllExchangesModel::~AllExchangesModel()
{

}

QModelIndex AllExchangesModel::index(int row, int column, const QModelIndex &parent) const
{
	if(!hasIndex(row, column, parent))return QModelIndex();
	return createIndex(row,column);
}

QModelIndex AllExchangesModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int AllExchangesModel::rowCount(const QModelIndex &) const
{
	return rowsCount;
}

int AllExchangesModel::columnCount(const QModelIndex &) const
{
	return columnsCount;
}

QVariant AllExchangesModel::data(const QModelIndex &index, int role) const
{
	if(role==Qt::TextAlignmentRole)
	{
		return 0x0084;
	}
	if(role==Qt::DisplayRole)
	{
		QString unswer="";
		if(index.column()==0)unswer=nameList.at(index.row());
		else if(index.column()==1)unswer=currenciesList.at(index.row());
		return unswer;
	}
	return QVariant();
}

QVariant AllExchangesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role!=Qt::DisplayRole)return QVariant();
	if(headerList.count()!=columnsCount)return QVariant();

	if(orientation==Qt::Horizontal)return headerList.at(section);
	return section+1;
}

void AllExchangesModel::addExchange(quint32 id, QString name, QString currencies)
{
	beginInsertRows(QModelIndex(), 0, 0);
	rowsCount++;
	exchangeIdList.append(id);
	nameList.append(name);
	currenciesList.append(currencies);
	endInsertRows();
}

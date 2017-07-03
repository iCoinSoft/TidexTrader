//  This file is part of Qt Bitcoin Trader
//      https://github.com/JulyIGHOR/QtBitcoinTrader
//  Copyright (C) 2013-2017 July IGHOR <julyighor@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  In addition, as a special exception, the copyright holders give
//  permission to link the code of portions of this program with the
//  OpenSSL library under certain conditions as described in each
//  individual source file, and distribute linked combinations including
//  the two.
//
//  You must obey the GNU General Public License in all respects for all
//  of the code used other than OpenSSL. If you modify file(s) with this
//  exception, you may extend this exception to your version of the
//  file(s), but you are not obligated to do so. If you do not wish to do
//  so, delete this exception statement from your version. If you delete
//  this exception statement from all source files in the program, then
//  also delete it here.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "depthmodel.h"
#include "main.h"
#include <QTimer>
#include "julymath.h"

DepthModel::DepthModel(bool isAskData)
	: QAbstractItemModel()
{
	widthPriceTitle=75;
	widthVolumeTitle=75;
	widthSizeTitle=75;

	groupedPrice=0.0;
	groupedVolume=0.0;
	widthPrice=75;
	widthVolume=75;
	widthSize=75;
	somethingChanged=true;
	isAsk=isAskData;
	originalIsAsk=isAsk;
	columnsCount=5;
}

DepthModel::~DepthModel()
{

}

double &DepthModel::sizeListAt(int row)
{
    if(!originalIsAsk)return sizeList[sizeList.count()-row-1];
    return sizeList[row];
}

double DepthModel::sizeListGet(int row) const
{
    if(!originalIsAsk)return sizeList[sizeList.count()-row-1];
    return sizeList[row];
}

void DepthModel::sizeListRemoveAt(int row)
{
    if(!originalIsAsk)sizeList.removeAt(sizeList.count()-row-1);
    else
    sizeList.removeAt(row);
}

double DepthModel::getPriceByVolume(double amount)
{
    if(sizeList.count()==0)return 0.0;
    int currentIndex=qUpperBound(sizeList.begin(),sizeList.end(),amount)-sizeList.begin();
    if(currentIndex<0)return 0.0;
    if(currentIndex>=sizeList.count())currentIndex=sizeList.count()-1;
    if(!originalIsAsk)currentIndex=priceList.count()-currentIndex-1;
    return priceList.at(currentIndex)*(currentIndex==priceList.count()-1?-1:1)*(currentIndex==0?-1:1);
}

double DepthModel::getVolumeByPrice(double price)
{
    if(priceList.count()==0)return 0.0;
    int currentIndex=qUpperBound(priceList.begin(),priceList.end(),price)-priceList.begin();
    if(currentIndex<0)return 0.0;
    if(currentIndex>=priceList.count())currentIndex=priceList.count()-1;
    return sizeListAt(currentIndex)*(currentIndex==priceList.count()-1?-1:1)*(currentIndex==0?-1:1);
}

int DepthModel::rowCount(const QModelIndex &) const
{
	return priceList.count()+grouped;
}

int DepthModel::columnCount(const QModelIndex &) const
{
	return columnsCount;
}

QVariant DepthModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())return QVariant();
	int currentRow=index.row();

	if(role==Qt::WhatsThisRole)
	{
		return baseValues.currentPair.currBSign+priceListStr.at(currentRow)+" "+baseValues.currentPair.currASign+volumeListStr.at(currentRow)+" "+baseValues.currentPair.currASign+sizeListStr.at(currentRow);
	}

	if(role!=Qt::DisplayRole&&role!=Qt::ToolTipRole&&role!=Qt::StatusTipRole&&role!=Qt::ForegroundRole&&role!=Qt::BackgroundRole&&role!=Qt::TextAlignmentRole)return QVariant();

	int indexColumn=index.column();
	if(isAsk)indexColumn=columnsCount-indexColumn-1;

	if(role==Qt::TextAlignmentRole)
	{
		if(indexColumn==0)return 0x0081;
		if(indexColumn==1)return 0x0082;
		if(indexColumn==3)return 0x0082;
		return 0x0084;
	}

	if(grouped&&currentRow<2)
	{
		if(role==Qt::ForegroundRole)return baseValues.appTheme.black;
		if(currentRow==1||groupedPrice==0.0)return QVariant();
		QString firstRowText;
		switch(indexColumn)
		{
		case 0: //Price
                firstRowText=textFromDouble(groupedPrice);
				if(role==Qt::ToolTipRole)firstRowText.prepend(baseValues.currentPair.currBSign);	
				break; 
		case 1: //Volume
                firstRowText=textFromDouble(groupedVolume,baseValues.currentPair.currADecimals);
				if(role==Qt::ToolTipRole)firstRowText.prepend(baseValues.currentPair.currASign);
				break;
		}
		if(firstRowText.isEmpty())return QVariant();
		return firstRowText;
	}

	if(grouped)currentRow-=grouped;
	if(currentRow<0||currentRow>=priceList.count())return QVariant();

	if(!originalIsAsk)currentRow=priceList.count()-currentRow-1;

	if(role==Qt::StatusTipRole)
	{
		QString direction;

		switch(directionList.at(currentRow))
		{
		case -1: direction=downArrowStr+"\t"; break;
		case 1: direction=upArrowStr+"\t"; break;
		}
		return baseValues.currentPair.currBSign+priceListStr.at(currentRow)+"\t"+baseValues.currentPair.currASign+volumeListStr.at(currentRow)+"\t"+direction+baseValues.currentPair.currASign+sizeListStr.at(currentRow);
	}

	if(role==Qt::ForegroundRole)
	{
		if(indexColumn==1)
		{
            double volume=volumeList.at(currentRow);
            double smallValue=baseValues.currentPair.currAInfo.valueSmall;
			if(volume<=smallValue)return baseValues.appTheme.gray; smallValue*=10.0;
			if(volume<=smallValue)return baseValues.appTheme.black; smallValue*=10.0;
			if(volume<=smallValue)return baseValues.appTheme.darkGreen; smallValue*=10.0;
			if(volume<=smallValue)return baseValues.appTheme.darkRedBlue;
			return baseValues.appTheme.red;
		}
		return baseValues.appTheme.black;
	}

    double requestedPrice=priceList.at(currentRow);
	if(requestedPrice<=0.0)return QVariant();

	if(role==Qt::BackgroundRole)
	{
		if(originalIsAsk)
		{
			if(mainWindow.ordersModel->currentAsksPrices.value(requestedPrice,false))return baseValues.appTheme.lightGreen;
		}
		else
		{
			if(mainWindow.ordersModel->currentBidsPrices.value(requestedPrice,false))return baseValues.appTheme.lightGreen;
		}
		return QVariant();
	}

	QString returnText;

	switch(indexColumn)
	{
	case 0://Price
		if(role==Qt::ToolTipRole)baseValues.currentPair.currBSign+priceListStr.at(currentRow);
		return priceListStr.at(currentRow);
		break;
	case 1:
		{//Volume
		if(volumeList.at(currentRow)<=0.0)return QVariant();
		if(role==Qt::ToolTipRole)baseValues.currentPair.currASign+volumeListStr.at(currentRow);
		return volumeListStr.at(currentRow);
		}
		break;
	case 2:
		{//Direction
			switch(directionList.at(currentRow))
			{
			case -1: return downArrowStr;
			case 1: return upArrowStr;
			default: return QVariant();
			}
		}
	case 3:
		{//Size
        if(sizeListGet(currentRow)<=0.0)return QVariant();
		if(role==Qt::ToolTipRole)baseValues.currentPair.currASign+sizeListStr.at(currentRow);
		return sizeListStr.at(currentRow);
		}
		break;
	default: break;
	}
	if(!returnText.isEmpty())return returnText;
	return QVariant();
}

void DepthModel::reloadVisibleItems()
{
	QTimer::singleShot(100,this,SLOT(delayedReloadVisibleItems()));
}

void DepthModel::delayedReloadVisibleItems()
{
	emit dataChanged(index(0,0),index(priceList.count()-1,columnsCount-1));
}

void DepthModel::calculateSize()
{
	if(!somethingChanged)return;
	somethingChanged=true;

    double maxPrice=0.0;
    double maxVolume=0.0;
    double maxTotal=0.0;

    double totalSize=0.0;
    double totalPrice=0.0;

	if(originalIsAsk)
	{
		for(int n=0;n<priceList.count();n++)
		{
			int currentRow=n;
			if(!originalIsAsk)currentRow=priceList.count()-currentRow-1;

			totalSize+=volumeList.at(currentRow);
            totalPrice+=volumeList.at(currentRow)*priceList.at(currentRow);

            sizeListAt(currentRow)=totalSize;
            sizePriceList[currentRow]=totalPrice;
            sizeListStr[currentRow]=textFromDouble(totalSize,qMin(baseValues.currentPair.currADecimals,baseValues.decimalsTotalOrderBook));

			maxPrice=qMax(maxPrice,priceList.at(currentRow));
			maxVolume=qMax(maxVolume,volumeList.at(currentRow));
            maxTotal=qMax(maxTotal,sizeListAt(currentRow));
		}
	}
	else
	{
		for(int n=priceList.count()-1;n>=0;n--)
		{
			int currentRow=n;
			if(originalIsAsk)currentRow=priceList.count()-currentRow-1;
			totalSize+=volumeList.at(currentRow);
            totalPrice+=volumeList.at(currentRow)*priceList.at(currentRow);
            sizeListAt(currentRow)=totalSize;
            sizePriceList[currentRow]=totalPrice;
            sizeListStr[currentRow]=textFromDouble(totalSize,qMin(baseValues.currentPair.currADecimals,baseValues.decimalsTotalOrderBook));

			maxPrice=qMax(maxPrice,priceList.at(currentRow));
			maxVolume=qMax(maxVolume,volumeList.at(currentRow));
            maxTotal=qMax(maxTotal,sizeListAt(currentRow));
		}
	}

    widthPrice=10+textFontWidth(textFromDouble(maxPrice,baseValues.currentPair.priceDecimals));
    widthVolume=10+textFontWidth(textFromDouble(maxVolume,baseValues.currentPair.currADecimals));
    widthSize=10+textFontWidth(textFromDouble(maxTotal,baseValues.currentPair.currADecimals));
	
	widthPrice=qMax(widthPrice,widthPriceTitle);
	widthVolume=qMax(widthVolume,widthVolumeTitle);
	widthSize=qMax(widthSize,widthSizeTitle);

	int sizeColumn=2;
	if(isAsk)sizeColumn=1;
	emit dataChanged(index(0,sizeColumn),index(priceList.count()-1,sizeColumn));
}

QModelIndex DepthModel::index(int row, int column, const QModelIndex &parent) const
{
	if(!hasIndex(row, column, parent))return QModelIndex();
	return createIndex(row,column);
}

QModelIndex DepthModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

void DepthModel::clear()
{
	if(priceList.isEmpty())return;
	beginResetModel();
	groupedPrice=0.0;
	groupedVolume=0.0;
	directionList.clear();
	priceList.clear();
	volumeList.clear();
	sizeList.clear();
    sizePriceList.clear();
	priceListStr.clear();
	volumeListStr.clear();
	sizeListStr.clear();
	endResetModel();
	somethingChanged=false;
}

Qt::ItemFlags DepthModel::flags(const QModelIndex &index) const
{
	if(!index.isValid())return 0;
	if(grouped)
	{
        if(index.row()==1||(groupedPrice==0.0&&priceList.isEmpty()))return Qt::NoItemFlags;
	}
	return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QVariant DepthModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	int indexColumn=section;
	if(isAsk)indexColumn=columnsCount-indexColumn-1;

	if(orientation!=Qt::Horizontal)return QVariant();

	if(role==Qt::TextAlignmentRole)
	{
		if(indexColumn==0)return 0x0081;
		if(indexColumn==1)return 0x0082;
		if(indexColumn==2)return 0x0082;
		return 0x0084;
	}

	if(role==Qt::SizeHintRole)
	{
		switch(indexColumn)
		{
		case 0: return QSize(widthPrice,defaultHeightForRow);//Price
		case 1: return QSize(widthVolume,defaultHeightForRow);//Volume
		case 3: return QSize(widthSize,defaultHeightForRow);//Size
		}
		return QVariant();
	}

	if(role!=Qt::DisplayRole)return QVariant();
	if(headerLabels.count()!=columnsCount)return QVariant();

	switch(indexColumn)
	{
	case 0: return headerLabels.at(indexColumn)+QLatin1String(" ")+baseValues.currentPair.currBSign;
	case 1: return headerLabels.at(indexColumn)+QLatin1String(" ")+baseValues.currentPair.currASign;
	case 3: return headerLabels.at(indexColumn)+QLatin1String(" ")+baseValues.currentPair.currASign;
	}

	return headerLabels.at(indexColumn);
}

void DepthModel::fixTitleWidths()
{
	int curASize=textFontWidth(" "+baseValues.currentPair.currASign);
	int curBSize=textFontWidth(" "+baseValues.currentPair.currBSign);
	widthPriceTitle=textFontWidth(headerLabels.at(0))+20+curBSize;
	widthVolumeTitle=textFontWidth(headerLabels.at(1))+20+curASize;
	widthSizeTitle=textFontWidth(headerLabels.at(3))+20+curASize;
}

void DepthModel::setHorizontalHeaderLabels(QStringList list)
{
	if(list.count()!=columnsCount)return;
	headerLabels=list;
	fixTitleWidths();
	emit headerDataChanged(Qt::Horizontal, 0, columnsCount-1);
}

void DepthModel::depthFirstOrder(double price, double volume)
{
	if(!grouped)return;
	if(price==groupedPrice&&groupedVolume==volume)return;
	groupedPrice=price;
	groupedVolume=volume;
	if(isAsk)
		emit dataChanged(index(0,3),index(0,4));
	else 
		emit dataChanged(index(0,0),index(0,1));
}

void DepthModel::depthUpdateOrders(QList<DepthItem> *items)
{
	if(items==0)return;
	for(int n=0;n<items->count();n++)depthUpdateOrder(items->at(n));
	delete items;
	calculateSize();
}

void DepthModel::depthUpdateOrder(DepthItem item)
{
    double price=item.price;
    double volume=item.volume;
	if(price==0.0)return;
	int currentIndex=qLowerBound(priceList.begin(),priceList.end(),price)-priceList.begin();
	bool matchListRang=currentIndex>-1&&priceList.count()>currentIndex;

	if(volume==0.0)
	{//Remove item
		if(matchListRang)
		{
			beginRemoveRows(QModelIndex(), currentIndex+grouped, currentIndex+grouped);
			directionList.removeAt(currentIndex);
			priceList.removeAt(currentIndex);
			volumeList.removeAt(currentIndex);
            sizeListRemoveAt(currentIndex);
            sizePriceList.removeAt(currentIndex);
			priceListStr.removeAt(currentIndex);
			volumeListStr.removeAt(currentIndex);
			sizeListStr.removeAt(currentIndex);
			endRemoveRows();
			somethingChanged=true;
		}
		return;
	}
	if(matchListRang&&priceList.at(currentIndex)==price)
	{//Update
		if(volumeList.at(currentIndex)==volume)return;
		directionList[currentIndex]=volumeList.at(currentIndex)<volume?1:-1;
		volumeList[currentIndex]=volume;
        sizeListAt(currentIndex)=0.0;
        sizePriceList[currentIndex]=0.0;
		priceListStr[currentIndex]=item.priceStr;
		volumeListStr[currentIndex]=item.volumeStr;
		sizeListStr[currentIndex]="0.0";
		somethingChanged=true;
		emit dataChanged(index(currentIndex+grouped,0),index(currentIndex+grouped,columnsCount-1));
	}
	else
	{//Insert
		beginInsertRows(QModelIndex(), currentIndex+grouped, currentIndex+grouped);
		priceList.insert(currentIndex,price);
		volumeList.insert(currentIndex,volume);
		sizeList.insert(currentIndex,volume);
        sizePriceList.insert(currentIndex,volume*price);
		directionList.insert(currentIndex,0);
		priceListStr.insert(currentIndex,item.priceStr);
		volumeListStr.insert(currentIndex,item.volumeStr);
		sizeListStr.insert(currentIndex,item.volumeStr);
		endInsertRows();
		somethingChanged=true;
	}
}

double DepthModel::rowPrice(int row)
{
	if(grouped&&row<2)
	{
		if(row==0)return groupedPrice;
		return 0.0;
	}
	row-=grouped;
	if(!originalIsAsk)row=priceList.count()-row-1;
	if(row<0||row>=priceList.count())return 0.0;
	return priceList.at(row);
}

double DepthModel::rowVolume(int row)
{
	if(grouped&&row<2)
	{
		if(row==0)return groupedVolume;
		return 0.0;
	}
	row-=grouped;
	if(!originalIsAsk)row=priceList.count()-row-1;
	if(row<0||row>=priceList.count())return 0.0;
	return volumeList.at(row);
}

double DepthModel::rowSize(int row)
{
	if(grouped&&row<2)return 0.0;
	row-=grouped;
	if(!originalIsAsk)row=priceList.count()-row-1;
	if(row<0||row>=priceList.count())return 0.0;
    return sizeListAt(row);
}


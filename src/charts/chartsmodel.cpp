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

#include <QDateTime>
#include "main.h"
#include "tradesitem.h"
#include "timesync.h"
#include "charts/chartsmodel.h"

ChartsModel::ChartsModel()
    : QObject(),
      intervalDate(60),
      perfomanceStep(1),
      intervalCount(10),
      fontMetrics(new QFontMetrics(QApplication::font()))
{

}

ChartsModel::~ChartsModel()
{
    delete fontMetrics;
}

void ChartsModel::addLastTrades(QList<TradesItem> *newItems)
{
    for (qint32 n = 0; n < newItems->count(); ++n)
    {
        if (tradesDate.count() && tradesDate.last() > newItems->at(n).date)
            continue;

        tradesDate .append(newItems->at(n).date);
        tradesPrice.append(newItems->at(n).price);
        tradesType .append(newItems->at(n).orderType);

        if (amountDate.count() && newItems->at(n).date < (amountDate.last() + intervalDate))
            amountPrice.last() += newItems->at(n).amount;
        else
        {
            amountPrice.append(newItems->at(n).amount);
            amountDate.append(int(newItems->at(n).date / intervalDate) * intervalDate);
        }
    }

    delete newItems;

    baseValues_->mainWindow_->chartsView->comeNewData();
}

void ChartsModel::addBound(double price, bool type)
{
    if (type == true)
    {
        boundsSellDate .append(TimeSync::getTimeT());
        boundsSellPrice.append(price);
    }
    else
    {
        boundsBuyDate .append(TimeSync::getTimeT());
        boundsBuyPrice.append(price);
    }
}

void ChartsModel::clearCharts()
{
    amountDate .clear();
    amountPrice.clear();
    tradesDate .clear();
    tradesPrice.clear();
    tradesType .clear();
    boundsSellDate .clear();
    boundsSellPrice.clear();
    boundsBuyDate  .clear();
    boundsBuyPrice .clear();
}

double ChartsModel::stepRound(double step)
{
    qint16 signCount = 0;
    while (step > 100)
    {
        step /= 10;
        ++signCount;
    }
    while (step < 10)
    {
        step *= 10;
        --signCount;
    }

    if (step > 50)
        step = 50;
    else
    {
        if(step > 25)
            step = 25;
        else
        {
            if(step > 20)
                step = 20;
            else
                step = 10;
        }
    }

    for (qint16 i = 0; i < signCount; ++i)
        step *= 10;
    for (qint16 i = 0; i< -signCount; ++i)
        step /= 10;

    return step;
}

double ChartsModel::axisRound(double old_x, double old_step)
{
    double x = old_x;
    double step = old_step;
    qint16 signCount = 0;

    while (step > 1)
    {
        x /= 10;
        step /= 10;
        ++signCount;
    }
    while (step < 0.1)
    {
        x *= 10;
        step *= 10;
        --signCount;
    }

    x = qRound(x - 0.5);

    for (qint16 i = 0; i < signCount; ++i)
        x *= 10;
    for (qint16 i = 0; i < -signCount; ++i)
        x /= 10;

    while (x + old_step < old_x)
        x += old_step;

    return x;
}

void ChartsModel::prepareInit()
{
    graphDateText .clear();
    graphDateTextX.clear();
    graphAmountText .clear();
    graphAmountTextY.clear();
    graphPriceText .clear();
    graphPriceTextY.clear();

    graphAmountX.clear();
    graphAmountY.clear();
    graphTradesX   .clear();
    graphTradesY   .clear();
    graphTradesType.clear();
    graphBoundsSellX.clear();
    graphBoundsSellY.clear();
    graphBoundsBuyX.clear();
    graphBoundsBuyY.clear();

    nowTime        = TimeSync::getTimeT();
    graphLastDate  = int(nowTime / intervalDate) * intervalDate+intervalDate;
    graphFirstDate = graphLastDate - intervalDate * 10;
}

void ChartsModel::prepareXAxis()
{
    graphXScale = double(chartsWidth) / double(graphLastDate - graphFirstDate);
    for (quint32 i = graphFirstDate; i <= graphLastDate; i += intervalDate){
        graphDateTextX.append(qRound(graphXScale * (i - graphFirstDate)));
        graphDateText .append(QDateTime::fromTime_t(i).toString("h:mm"));
    }
}

void ChartsModel::prepareAmountYAxis()
{
    iAmountFirst = qLowerBound(amountDate.begin(), amountDate.end(), graphFirstDate) - amountDate.begin();
    widthAmountYAxis = 5;
    if (iAmountFirst < amountDate.count())
    {
        amountMax    = amountPrice.at(std::max_element(amountPrice.begin() + iAmountFirst,amountPrice.end()) - amountPrice.begin());
        amountYScale = double(chartsHeight) * 0.9 / amountMax;
        double amountStepY = stepRound(amountMax / 5);
        for(double amountY = 0; amountY < amountMax; amountY += amountStepY)
        {
            graphAmountText .append(baseValues.currentPair.currASign + " " + QString::number(amountY));
            graphAmountTextY.append(qRound(amountYScale * amountY));
            widthAmountYAxis = qMax(fontMetrics->width(graphAmountText.last()), widthAmountYAxis);
        }
    }
    widthAmountYAxis += 14;
}

void ChartsModel::prepareAmount()
{
    if(iAmountFirst < amountDate.count())
    {
        for(qint32 i = iAmountFirst; i < amountPrice.count(); ++i)
        {
            graphAmountX.append(qRound(graphXScale * (amountDate.at(i) - graphFirstDate + intervalDate / 2)));
            graphAmountY.append(qRound(amountYScale * amountPrice.at(i)));
        }
    }
}

void ChartsModel::preparePriceMinMax()
{
    priceInit = false;
    priceMin = -1;
    priceMax = -1;
    iTradesFirst     = qLowerBound(tradesDate.begin(),     tradesDate.end(),     graphFirstDate) - tradesDate.begin();
    iBoundsSellFirst = qLowerBound(boundsSellDate.begin(), boundsSellDate.end(), graphFirstDate) - boundsSellDate.begin();
    iBoundsBuyFirst  = qLowerBound(boundsBuyDate.begin(),  boundsBuyDate.end(),  graphFirstDate) - boundsBuyDate.begin();

    double temp;
    QList<double> min;
    QList<double> max;

    if (iTradesFirst < tradesDate.count())
    {
        temp = tradesPrice.at(std::min_element(tradesPrice.begin() + iTradesFirst, tradesPrice.end()) - tradesPrice.begin());
        if (temp > 0)
            min.push_back(temp);
        temp = tradesPrice.at(std::max_element(tradesPrice.begin() + iTradesFirst,tradesPrice.end()) - tradesPrice.begin());
        if (temp > 0)
            max.push_back(temp);
    }
    if (iBoundsSellFirst < boundsSellDate.count())
    {
        int tempIBoundsSellFirst = iBoundsSellFirst;
        if (tempIBoundsSellFirst > 0)
            --tempIBoundsSellFirst;
        temp = boundsSellPrice.at(std::min_element(boundsSellPrice.begin() + tempIBoundsSellFirst, boundsSellPrice.end()) - boundsSellPrice.begin());
        if (temp > 0)
            min.push_back(temp);
        temp = boundsSellPrice.at(std::max_element(boundsSellPrice.begin() + tempIBoundsSellFirst, boundsSellPrice.end()) - boundsSellPrice.begin());
        if (temp > 0)
            max.push_back(temp);
    }
    if (iBoundsBuyFirst < boundsBuyDate.count())
    {
        int tempIBoundsBuyFirst = iBoundsBuyFirst;
        if (tempIBoundsBuyFirst > 0)
            --tempIBoundsBuyFirst;
        temp = boundsBuyPrice.at(std::min_element(boundsBuyPrice.begin() + tempIBoundsBuyFirst, boundsBuyPrice.end()) - boundsBuyPrice.begin());
        if (temp > 0)
        {
            if (min.count() < 2)
                min.push_back(temp);
            else
                min.last() = qMin(min.last(), temp);
        }
        temp = boundsBuyPrice.at(std::max_element(boundsBuyPrice.begin() + iBoundsBuyFirst, boundsBuyPrice.end()) - boundsBuyPrice.begin());
        if (temp > 0)
        {
            if(max.count() < 2)
                max.push_back(temp);
            else
                max.last() = qMax(max.last(), temp);
        }
    }

    if (min.count() == 0 || max.count() == 0)
        return;
    priceMin = qMin(min.first(), min.last());
    priceMax = qMax(max.first(), max.last());

    if (priceMax != priceMin)
    {
        priceInit = true;
        if ((priceMax - priceMin) * 100 < priceMax)
        {
            priceMax += (priceMax - priceMin) * 0.2;
            priceMin -= (priceMax - priceMin) * 0.1;
        }
        double deltaPrice = (priceMax - priceMin) * 0.1;
        priceMin -= deltaPrice;
        if (priceMin < 0)
            priceMin = 0;
        priceMax += deltaPrice;

        priceStepY = stepRound((priceMax - priceMin) / 5);
        priceMin   = axisRound(priceMin, priceStepY);
    }
}

void ChartsModel::preparePriceYAxis()
{
    widthPriceYAxis = 5;
    if (priceInit)
    {
        priceYScale = double(chartsHeight) / (priceMax - priceMin);
        for (double priceY = priceMin; priceY < priceMax; priceY += priceStepY)
        {
            graphPriceText .append(baseValues.currentPair.currBSign + " " + QString::number(priceY));
            graphPriceTextY.append(qRound(priceYScale * (priceY - priceMin)));
            widthPriceYAxis = qMax(fontMetrics->width(graphPriceText.last()), widthPriceYAxis);
        }
    }
    widthPriceYAxis += 14;
}

void ChartsModel::preparePrice()
{
    qint16 x, y;
    if (priceInit)
    {
        for(qint32 i = iTradesFirst; i < tradesDate.count(); ++i)
        {
            x = qRound(graphXScale * (tradesDate .at(i) - graphFirstDate));
            y = qRound(priceYScale * (tradesPrice.at(i) - priceMin));
            if (graphTradesX.count())
            {
                if (abs(x - graphTradesX.last()) <= perfomanceStep && abs(y - graphTradesY.last()) <= perfomanceStep)
                    continue;
            }
            graphTradesX.append(x);
            graphTradesY.append(y);
            graphTradesType.append(tradesType.at(i));
        }
    }
}

void ChartsModel::prepareBound()
{
    if(priceInit)
    {
        qint16 graphBoundX;
        if (iBoundsSellFirst > 0 && iBoundsSellFirst <= boundsSellPrice.count())
        {
            graphBoundsSellX.append(1);
            graphBoundsSellY.append(qRound(priceYScale * (boundsSellPrice[iBoundsSellFirst - 1] - priceMin)));
        }
        if(iBoundsBuyFirst > 0 && iBoundsBuyFirst <= boundsBuyPrice.count())
        {
            graphBoundsBuyX.append(1);
            graphBoundsBuyY.append(qRound(priceYScale * (boundsBuyPrice[iBoundsBuyFirst - 1] - priceMin)));
        }
        for(qint32 i = iBoundsSellFirst; i < boundsSellPrice.count(); ++i)
        {
            graphBoundX = qRound(graphXScale * (boundsSellDate[i] - graphFirstDate));
            if(graphBoundsSellY.count())
            {
                graphBoundsSellX.append(graphBoundX);
                graphBoundsSellY.append(graphBoundsSellY.last());
            }
            graphBoundsSellX.append(graphBoundX);
            graphBoundsSellY.append(qRound(priceYScale * (boundsSellPrice[i] - priceMin)));
        }
        for(qint32 i = iBoundsBuyFirst; i < boundsBuyPrice.count(); ++i)
        {
            graphBoundX = qRound(graphXScale * (boundsBuyDate[i] - graphFirstDate));
            if(graphBoundsBuyY.count())
            {
                graphBoundsBuyX.append(graphBoundX);
                graphBoundsBuyY.append(graphBoundsBuyY.last());
            }
            graphBoundsBuyX.append(graphBoundX);
            graphBoundsBuyY.append(qRound(priceYScale * (boundsBuyPrice[i] - priceMin)));
        }
        if(graphBoundsSellX.count())
        {
              graphBoundsSellX.append(qRound(graphXScale * (nowTime - graphFirstDate)));
              graphBoundsSellY.append(graphBoundsSellY.last());
        }
        if(graphBoundsBuyX.count())
        {
              graphBoundsBuyX.append(qRound(graphXScale * (nowTime - graphFirstDate)));
              graphBoundsBuyY.append(graphBoundsBuyY.last());
        }
    }
}

bool ChartsModel::prepareChartsData(qint16 parentWidth, qint16 parentHeight)
{
    if (!boundsSellDate.count())
        return false;
    chartsHeight = parentHeight - 5;

    prepareInit();
    prepareAmountYAxis();
    preparePriceMinMax();
    preparePriceYAxis();

    chartsWidth = parentWidth - widthAmountYAxis - widthPriceYAxis - 1;
    prepareXAxis();
    prepareAmount();
    preparePrice();
    prepareBound();

    return true;
}



#ifndef RULEHOLDER_H
#define RULEHOLDER_H

#include <QString>

struct RuleHolder
{
    RuleHolder();
    QString description;
    bool isTradingRule();
    bool isValidComparation(QString &text);
    bool isValidSymbol(QString &symbol);
    bool isValidPlusMinus(QString &plusMinus);
    bool isValidCode(QString &code);
    bool isValid();

    bool thanAmountPercentChecked;
    bool thanPricePercentChecked;
    bool variableBPercentChecked;
    int thanAmountFeeIndex;
    int thanPriceFeeIndex;
    int thanTypeIndex;
    int variableBFeeIndex;
    int variableBModeIndex;
    double delayMilliseconds;
    double thanAmount;
    double thanPrice;
    double variableBExact;
    QString comparationText;
    QString thanPricePlusMinusText;
    QString thanPriceTypeCode;
    QString thanText;
    QString tradeSymbolCode;
    QString valueASymbolCode;
    QString valueBSymbolCode;
    QString variableACode;
    QString variableBCode;
    QString variableBplusMinus;
    QString variableBSymbolCode;
    QString sayCode;
};

#endif // RULEHOLDER_H

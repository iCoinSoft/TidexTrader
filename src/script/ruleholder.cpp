

#include "ruleholder.h"
#include "main.h"

RuleHolder::RuleHolder()
{
    thanAmountPercentChecked=false;
    thanPricePercentChecked=false;
    variableBPercentChecked=false;
    thanAmountFeeIndex=-1;
    thanPriceFeeIndex=-1;
    thanTypeIndex=-1;
    variableBFeeIndex=-1;
    variableBModeIndex=-1;
    thanAmount=0.0;
    thanPrice=0.0;
    variableBExact=0.0;
    delayMilliseconds=0.0;
}

bool RuleHolder::isValidComparation(QString &text)
{
    return text==QLatin1String("=")||text==QLatin1String("<")||text==QLatin1String(">")||text==QLatin1String("<=")||text==QLatin1String(">=")||text==QLatin1String("!=")||text==QLatin1String("==")||text==QLatin1String("<>");
}

bool RuleHolder::isValidSymbol(QString &symbol)
{
    CurrencyPairItem pairItem;
    pairItem=baseValues.currencyPairMap.value(symbol.toUpper(),pairItem);
    if(pairItem.symbol.isEmpty())return false;
    return true;
}

bool RuleHolder::isValidPlusMinus(QString &plusMinus)
{
    return plusMinus==QLatin1String("+")||plusMinus==QLatin1String("-")||plusMinus==QLatin1String("*")||plusMinus==QLatin1String("/");
}

bool RuleHolder::isValidCode(QString &code)
{
    return code==QLatin1String("EXACT")||
           code==QLatin1String("IMMEDIATELY")||
           code==QLatin1String("LastTrade")||
           code==QLatin1String("MyLastTrade")||
           mainWindow.indicatorsMap.value(code,0)!=0;
}

bool RuleHolder::isTradingRule()
{
    return thanTypeIndex<=6;
}

bool RuleHolder::isValid()
{
    bool immediately=variableACode==QLatin1String("IMMEDIATELY")||variableACode==QLatin1String("LastTrade")||variableACode==QLatin1String("MyLastTrade");
    bool exactB=variableBCode==QLatin1String("EXACT");
    if(thanAmountFeeIndex<0||thanPriceFeeIndex<0||thanPriceFeeIndex<0||thanTypeIndex<0)return false;
    if(thanTypeIndex>3||thanAmount>0.0);else return false;
    if(immediately||!exactB||variableBExact!=0);else return false;
    if(thanPriceTypeCode==QLatin1String("EXACT")?thanPrice>0.0:true);else return false;
    if(!isValidComparation(comparationText))return false;
    if(!isValidPlusMinus(thanPricePlusMinusText))return false;
    if(!isValidCode(thanPriceTypeCode))return false;
    if(!isValidSymbol(tradeSymbolCode))return false;
    if(!isValidSymbol(valueASymbolCode))return false;
    if(!isValidSymbol(valueBSymbolCode))return false;
    if(!isValidCode(variableACode))return false;
    if(!isValidCode(variableBCode))return false;
    if(!isValidPlusMinus(variableBplusMinus))return false;
    if(!isValidSymbol(variableBSymbolCode))return false;
    if(delayMilliseconds<0.0)return false;
    return true;
}

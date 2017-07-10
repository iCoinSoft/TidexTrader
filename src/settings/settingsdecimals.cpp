

#include "settingsdecimals.h"
#include "main.h"

SettingsDecimals::SettingsDecimals()
    : QWidget()
{
    ui.setupUi(this);
    decimalsSettings=new QSettings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);

    loadDecimals();

    ui.revertChangesButton->setEnabled(false);
    ui.saveButton->setEnabled(false);
}

SettingsDecimals::~SettingsDecimals()
{
    delete decimalsSettings;
}

void SettingsDecimals::loadDecimals()
{
    /*ui.amountMyTransactionsSpinBox->setValue(baseValues.decimalsAmountMyTransactions);
    ui.amountMyTransactionsSpinBox->setMaximum(baseValues.currentPair.currADecimals);
    ui.priceMyTransactionsSpinBox->setValue(baseValues.decimalsPriceMyTransactions);
    ui.priceMyTransactionsSpinBox->setMaximum(8);
    ui.totalMyTransactionsSpinBox->setValue(baseValues.decimalsTotalMyTransactions);
    ui.totalMyTransactionsSpinBox->setMaximum(baseValues.currentPair.currADecimals);

    ui.amountOrderBookSpinBox->setValue(baseValues.decimalsAmountOrderBook);
    ui.amountOrderBookSpinBox->setMaximum(baseValues.currentPair.currADecimals);
    ui.priceOrderBookSpinBox->setValue(baseValues.decimalsPriceOrderBook);
    ui.priceOrderBookSpinBox->setMaximum(baseValues.currentPair.priceDecimals);
    ui.totalOrderBookSpinBox->setValue(baseValues.decimalsTotalOrderBook);
    ui.totalOrderBookSpinBox->setMaximum(baseValues.currentPair.currADecimals);

    ui.amountLastTradesSpinBox->setValue(baseValues.decimalsAmountLastTrades);
    ui.amountLastTradesSpinBox->setMaximum(baseValues.currentPair.currADecimals);
    ui.priceLastTradesSpinBox->setValue(baseValues.decimalsPriceLastTrades);
    ui.priceLastTradesSpinBox->setMaximum(baseValues.currentPair.priceDecimals);
    ui.totalLastTradesSpinBox->setValue(baseValues.decimalsTotalLastTrades);
    ui.totalLastTradesSpinBox->setMaximum(baseValues.currentPair.currADecimals);*/

    ui.amountMyTransactionsSpinBox->setValue(baseValues.decimalsAmountMyTransactions);
    ui.amountMyTransactionsSpinBox->setMaximum(8);
    ui.priceMyTransactionsSpinBox->setValue(baseValues.decimalsPriceMyTransactions);
    ui.priceMyTransactionsSpinBox->setMaximum(8);
    ui.totalMyTransactionsSpinBox->setValue(baseValues.decimalsTotalMyTransactions);
    ui.totalMyTransactionsSpinBox->setMaximum(8);

    ui.amountOrderBookSpinBox->setValue(baseValues.decimalsAmountOrderBook);
    ui.amountOrderBookSpinBox->setMaximum(8);
    ui.priceOrderBookSpinBox->setValue(baseValues.decimalsPriceOrderBook);
    ui.priceOrderBookSpinBox->setMaximum(8);
    ui.totalOrderBookSpinBox->setValue(baseValues.decimalsTotalOrderBook);
    ui.totalOrderBookSpinBox->setMaximum(8);

    ui.amountLastTradesSpinBox->setValue(baseValues.decimalsAmountLastTrades);
    ui.amountLastTradesSpinBox->setMaximum(8);
    ui.priceLastTradesSpinBox->setValue(baseValues.decimalsPriceLastTrades);
    ui.priceLastTradesSpinBox->setMaximum(8);
    ui.totalLastTradesSpinBox->setValue(baseValues.decimalsTotalLastTrades);
    ui.totalLastTradesSpinBox->setMaximum(8);
}

void SettingsDecimals::saveDecimals()
{
    decimalsSettings->beginGroup("Decimals");
    decimalsSettings->setValue("AmountMyTransactions",ui.amountMyTransactionsSpinBox->value());
    decimalsSettings->setValue("PriceMyTransactions",ui.priceMyTransactionsSpinBox->value());
    decimalsSettings->setValue("TotalMyTransactions",ui.totalMyTransactionsSpinBox->value());
    decimalsSettings->setValue("AmountOrderBook",ui.amountOrderBookSpinBox->value());
    decimalsSettings->setValue("PriceOrderBook",ui.priceOrderBookSpinBox->value());
    decimalsSettings->setValue("TotalOrderBook",ui.totalOrderBookSpinBox->value());
    decimalsSettings->setValue("AmountLastTrades",ui.amountLastTradesSpinBox->value());
    decimalsSettings->setValue("PriceLastTrades",ui.priceLastTradesSpinBox->value());
    decimalsSettings->setValue("TotalLastTrades",ui.totalLastTradesSpinBox->value());
    decimalsSettings->endGroup();
}

void SettingsDecimals::activateDecimals()
{
    baseValues.decimalsAmountMyTransactions=ui.amountMyTransactionsSpinBox->value();
    baseValues.decimalsPriceMyTransactions=ui.priceMyTransactionsSpinBox->value();
    baseValues.decimalsTotalMyTransactions=ui.totalMyTransactionsSpinBox->value();
    baseValues.decimalsAmountOrderBook=ui.amountOrderBookSpinBox->value();
    baseValues.decimalsPriceOrderBook=ui.priceOrderBookSpinBox->value();
    baseValues.decimalsTotalOrderBook=ui.totalOrderBookSpinBox->value();
    baseValues.decimalsAmountLastTrades=ui.amountLastTradesSpinBox->value();
    baseValues.decimalsPriceLastTrades=ui.priceLastTradesSpinBox->value();
    baseValues.decimalsTotalLastTrades=ui.totalLastTradesSpinBox->value();
}

void SettingsDecimals::on_saveButton_clicked()
{
    saveDecimals();
    activateDecimals();

    ui.revertChangesButton->setEnabled(false);
    ui.saveButton->setEnabled(false);
}

void SettingsDecimals::on_revertChangesButton_clicked()
{
    loadDecimals();

    ui.revertChangesButton->setEnabled(false);
    ui.saveButton->setEnabled(false);
}

void SettingsDecimals::on_restoreDefaultsButton_clicked()
{
    /*ui.amountMyTransactionsSpinBox->setValue(baseValues.currentPair.currADecimals);
    ui.priceMyTransactionsSpinBox->setValue(8);
    ui.totalMyTransactionsSpinBox->setValue(baseValues.currentPair.currADecimals);
    ui.amountOrderBookSpinBox->setValue(baseValues.currentPair.currADecimals);
    ui.priceOrderBookSpinBox->setValue(baseValues.currentPair.priceDecimals);
    ui.totalOrderBookSpinBox->setValue(baseValues.currentPair.currADecimals);
    ui.amountLastTradesSpinBox->setValue(baseValues.currentPair.currADecimals);
    ui.priceLastTradesSpinBox->setValue(baseValues.currentPair.priceDecimals);
    ui.totalLastTradesSpinBox->setValue(baseValues.currentPair.currADecimals);*/
    ui.amountMyTransactionsSpinBox->setValue(8);
    ui.priceMyTransactionsSpinBox->setValue(8);
    ui.totalMyTransactionsSpinBox->setValue(8);
    ui.amountOrderBookSpinBox->setValue(8);
    ui.priceOrderBookSpinBox->setValue(8);
    ui.totalOrderBookSpinBox->setValue(8);
    ui.amountLastTradesSpinBox->setValue(8);
    ui.priceLastTradesSpinBox->setValue(8);
    ui.totalLastTradesSpinBox->setValue(8);

    ui.restoreDefaultsButton->setEnabled(false);
}

void SettingsDecimals::anyValueChanged()
{
    if(!ui.revertChangesButton->isEnabled())ui.revertChangesButton->setEnabled(true);
    if(!ui.restoreDefaultsButton->isEnabled())ui.restoreDefaultsButton->setEnabled(true);
    if(!ui.saveButton->isEnabled())ui.saveButton->setEnabled(true);
}


#ifndef FEECALCULATOR_H
#define FEECALCULATOR_H

#include <QDialog>
#include "ui_feecalculator.h"

class FeeCalculator : public QDialog
{
	Q_OBJECT

public:
	FeeCalculator();
	~FeeCalculator();

private:
	bool buyPaidLocked;
	bool buyBtcReceivedLocked;
	bool buyBtcLocked;
	Ui::FeeCalculator ui;
public slots:
	void languageChanged();
private slots:
	void on_singleInstance_toggled(bool);
	void setStaysOnTop(bool);
	void setZeroProfitPrice();
	void profitLossChanged(double);
	void buyBtcChanged(double);
	void buyPriceChanged(double);
	void buyTotalPaidChanged(double);
	void buyBtcReceivedChanged(double);
	void sellPriceChanged(double);
	void sellAmountChanged(double);
    void sellFiatReceived(double);
	void feeChanged(double);

};

#endif // FEECALCULATOR_H

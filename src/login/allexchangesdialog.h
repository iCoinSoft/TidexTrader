

#ifndef ALLEXCHANGESDIALOG_H
#define ALLEXCHANGESDIALOG_H

#include <QDialog>
#include "ui_allexchangesdialog.h"
#include "allexchangesmodel.h"

class AllExchangesDialog : public QDialog
{
	Q_OBJECT

public:
	AllExchangesDialog();
	~AllExchangesDialog();
	qint32 exchangeNum;

private:
    int startIndex;
	Ui::AllExchangesDialog ui;
	AllExchangesModel *allExchangesModel;
	QString loadCurrencies(QString);

private slots:
	void on_okButton_clicked();
	void on_backButton_clicked();
	void on_exchangesTableView_doubleClicked();

public slots:
	void selectionExchange();
};

#endif // ALLEXCHANGESDIALOG_H

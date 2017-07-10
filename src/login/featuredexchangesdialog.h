

#ifndef FEATUREDEXCHANGESDIALOG_H
#define FEATUREDEXCHANGESDIALOG_H

#include <QDialog>

namespace Ui {
class FeaturedExchangesDialog;
}

class FeaturedExchangesDialog : public QDialog
{
	Q_OBJECT

public:
	FeaturedExchangesDialog();
	~FeaturedExchangesDialog();
	void selectExchange(qint32);
	qint32 exchangeNum;		// Exchange; -1 is error; -2 is show all exchange

private:
    QByteArray cacheData;
    Ui::FeaturedExchangesDialog *ui;
	QStringList featuredExchangesList;
	QStringList allExchangesList;
	QList<quint32> featuredExchangesListIndex;
	QString loadCurrencies(QString);
	QString fixLogo(QString);
	QString fixURL(QString);
	void removeNotValidExchanges();

private slots:
    void dataReceived(QByteArray,int);
	void on_okButton_clicked();
	void on_otherExchangesButton_clicked();
    void on_groupBoxExchange_clicked();
};

#endif // FEATUREDEXCHANGESDIALOG_H

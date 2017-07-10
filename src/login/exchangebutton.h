

#ifndef EXCHANGEBUTTON_H
#define EXCHANGEBUTTON_H

#include <QWidget>
#include "ui_exchangebutton.h"
#include <QMouseEvent>
#include "featuredexchangesdialog.h"

class ExchangeButton : public QWidget
{
	Q_OBJECT

public:
	ExchangeButton(QString, QString, QString, qint32, FeaturedExchangesDialog *);
	~ExchangeButton();
	qint32 exchangeNum;

private:
	Ui::ExchangeButton ui;
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	FeaturedExchangesDialog *parrentForm;
	void setLogo(QString);
	void setCurrencies(QString);
	void setURL(QString);
};

#endif // EXCHANGEBUTTON_H

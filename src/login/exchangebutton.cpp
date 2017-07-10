

#include "exchangebutton.h"

ExchangeButton::ExchangeButton(QString logo, QString currencies, QString url, qint32 num, FeaturedExchangesDialog *toParrentForm)
	: QWidget()
{
	parrentForm=toParrentForm;
	ui.setupUi(this);
	exchangeNum=num;

	setLogo(logo);
	setCurrencies(currencies);
	setURL(url);
	setFixedSize(208,124);
}

ExchangeButton::~ExchangeButton()
{

}

void ExchangeButton::mouseReleaseEvent(QMouseEvent *event)
{
	event->accept();
	if(event->button()==Qt::LeftButton){
		foreach(ExchangeButton* exchangeButton, this->parent()->findChildren<ExchangeButton*>()){
			exchangeButton->ui.widgetButton->setStyleSheet("#widgetButton{}");
		}
		ui.widgetButton->setStyleSheet("#widgetButton{background:rgba(0,0,0,7%);border-left:1px solid #333;border-top:1px solid #333;border-right:1px solid #fff;border-bottom:1px solid #fff}");
		parrentForm->selectExchange(exchangeNum);
	}
}

void ExchangeButton::mouseDoubleClickEvent(QMouseEvent *event)
{
	event->accept();
	parrentForm->selectExchange(exchangeNum);
	parrentForm->accept();
}

void ExchangeButton::setLogo(QString logo)
{
	ui.imageLabel->setPixmap(QPixmap(":/Resources/Exchanges/Logos/"+logo));
	ui.imageLabel->setFixedSize(180,50);
}

void ExchangeButton::setCurrencies(QString currencies)
{
	ui.currenciesLabel->setToolTip(currencies);
	if(currencies.length()>28)currencies=currencies.left(28)+" ...";
	ui.currenciesLabel->setText(currencies);
	ui.currenciesLabel->setWordWrap(true);
	ui.currenciesLabel->setFixedSize(180,14);
}

void ExchangeButton::setURL(QString url)
{
	ui.urlLabel->setText("<a href=\""+url+"\" title=\""+url+"\">"+url+"</a>");
	ui.urlLabel->setToolTip(url);
	ui.urlLabel->setOpenExternalLinks(true);
	ui.urlLabel->setFixedSize(ui.urlLabel->minimumSizeHint());
}

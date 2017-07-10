

#include "logobutton.h"
#include "main.h"
#include <QDesktopServices>
#include <QUrl>

LogoButton::LogoButton(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	themeChanged();
	setCursor(Qt::PointingHandCursor);
}

LogoButton::~LogoButton()
{

}

void LogoButton::mouseReleaseEvent(QMouseEvent *event)
{
	event->accept();
	QPoint pressPos=event->pos();
	if(pressPos.x()<0||pressPos.y()<0||pressPos.y()>height()||pressPos.x()>width())return;

	if(event->button()==Qt::LeftButton)
        QDesktopServices::openUrl(QUrl("https://centrabit.com"));
}

void LogoButton::themeChanged()
{
	static QPixmap logoDay(":/Resources/CentrabitDay.png");
	static QPixmap logoNight(":/Resources/CentrabitNight.png");
	logoSize=logoDay.size();

    if(baseValues.currentTheme==1)
		ui.logo->setPixmap(logoNight);
	else
		ui.logo->setPixmap(logoDay);
}

void LogoButton::resizeEvent(QResizeEvent *event)
{
	event->accept();
	QSize newSize=logoSize;
	newSize.scale(size(),Qt::KeepAspectRatio);
	ui.logo->setGeometry((width()-newSize.width())/2,(height()-newSize.height())/2,newSize.width(),newSize.height());
}

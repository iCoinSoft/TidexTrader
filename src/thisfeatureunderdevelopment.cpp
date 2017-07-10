

#include "thisfeatureunderdevelopment.h"
#include "main.h"

ThisFeatureUnderDevelopment::ThisFeatureUnderDevelopment(QWidget *parent)
	: QDialog(parent)
{
	if(parent==0)parent=this;
	ui.setupUi(this);
	setWindowFlags(Qt::WindowCloseButtonHint);
	themeChanged();
	connect(baseValues.mainWindow_,SIGNAL(themeChanged()),this,SLOT(themeChanged()));
}

ThisFeatureUnderDevelopment::~ThisFeatureUnderDevelopment()
{

}

void ThisFeatureUnderDevelopment::themeChanged()
{
	ui.labelNotAvailable->setStyleSheet("background: "+baseValues.appTheme.white.name()+"; border-radius: 8px; border: 1px solid "+baseValues.appTheme.gray.name());
}

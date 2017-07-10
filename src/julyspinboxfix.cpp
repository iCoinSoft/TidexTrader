

#include "julyspinboxfix.h"
#include "main.h"

JulySpinBoxFix::JulySpinBoxFix(QDoubleSpinBox *parentSB, int minWid)
	: QObject()
{
	if(baseValues.forceDotInSpinBoxes)parentSB->setLocale(QLocale::English);
    else parentSB->setLocale(QLocale::Ukrainian);
	pMinimumWidth=minWid;
	spinMargin=30;
	if(parentSB->buttonSymbols()==QDoubleSpinBox::NoButtons)spinMargin=6;
	parentSpinBox=parentSB;
    widthFix(parentSB->text());
	if(!parentSB->suffix().isEmpty())
	{
		pMinimumWidth=parentSB->minimumWidth();
		parentSB->setMinimumWidth(pMinimumWidth);
        widthFix(parentSB->text());
    }
    connect(parentSB,SIGNAL(valueChanged(QString)),this,SLOT(widthFix(QString)));
}

void JulySpinBoxFix::widthFix(QString text)
{
	if(pMinimumWidth==0)
		parentSpinBox->setMinimumWidth(textFontWidth(text)+spinMargin);
	else 
		parentSpinBox->setMinimumWidth(qMax(textFontWidth(text)+spinMargin,pMinimumWidth));
}

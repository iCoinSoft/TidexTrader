

#include "julylightchanges.h"
#include <QApplication>
#include "main.h"

JulyLightChanges::JulyLightChanges(QDoubleSpinBox *parent)
	: QObject()
{
	lastValue=0.0;
	parentSpinBox=parent;
	setParent(parentSpinBox);
	changeTimer=new QTimer;
	connect(changeTimer,SIGNAL(timeout()),this,SLOT(changeTimerSlot()));
	changeTimer->setSingleShot(true);
	valueChanged(parentSpinBox->value());
	connect(parent,SIGNAL(valueChanged(double)),this,SLOT(valueChanged(double)));
}

JulyLightChanges::~JulyLightChanges()
{
	if(changeTimer)delete changeTimer;
}

void JulyLightChanges::changeTimerSlot()
{
	parentSpinBox->setStyleSheet("");
}

void JulyLightChanges::valueChanged(double val)
{
	changeTimer->stop();
	if(lastValue<=val)
		parentSpinBox->setStyleSheet("QDoubleSpinBox:disabled{color:"+baseValues.appTheme.black.name()+"; background: \""+baseValues.appTheme.lightGreen.name()+"\";} QDoubleSpinBox {color:"+baseValues.appTheme.black.name()+";background: \""+baseValues.appTheme.lightGreen.name()+"\";}");
	else
		parentSpinBox->setStyleSheet("QDoubleSpinBox:disabled{color:"+baseValues.appTheme.black.name()+"; background: \""+baseValues.appTheme.lightRed.name()+"\";} QDoubleSpinBox {color:"+baseValues.appTheme.black.name()+";background: \""+baseValues.appTheme.lightRed.name()+"\";}");
	lastValue=val;
	changeTimer->start(2000);
}

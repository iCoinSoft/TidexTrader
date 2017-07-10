

#include "percentpicker.h"

PercentPicker::PercentPicker(QDoubleSpinBox *_spinBox, double _maxValue)
    : QMenu()
{
    maxValue=_maxValue;
    spinBox=_spinBox;
    if(maxValue==0.0)maxValue=0.000000001;
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setFixedSize(minimumSizeHint().width(),200);
    ui.verticalSlider->setValue(spinBox->value()*100.0/maxValue);
    ui.spinBox->setFocus();
    ui.spinBox->selectAll();
}

PercentPicker::~PercentPicker()
{

}

void PercentPicker::mouseReleaseEvent(QMouseEvent *event)
{
	event->ignore();
}

void PercentPicker::keyPressEvent(QKeyEvent *event)
{
    event->accept();
    if(event->key()==Qt::Key_Return||event->key()==Qt::Key_Enter||event->key()==Qt::Key_Escape)
        close();
}

void PercentPicker::on_percentTo1_clicked()
{
	if(ui.percentTo1->text()=="1%")
	{
		ui.verticalSlider->setValue(1);
		ui.percentTo1->setText("100%");
	}
	else
	if(ui.percentTo1->text()=="100%")
	{
		ui.verticalSlider->setValue(100);
		ui.percentTo1->setText("1%");
	}
}

void PercentPicker::on_verticalSlider_valueChanged(int val)
{
	if(val==1)ui.percentTo1->setText("100%");else
	if(val==100)ui.percentTo1->setText("1%");
	if(isVisible())spinBox->setValue((double)val*maxValue/100.0);
}

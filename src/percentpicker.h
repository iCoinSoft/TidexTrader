

#ifndef PERCENTPICKER_H
#define PERCENTPICKER_H

#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QDoubleSpinBox>
#include "ui_percentpicker.h"
#include <QMouseEvent>
#include <QKeyEvent>

class PercentPicker : public QMenu
{
	Q_OBJECT

public:
    PercentPicker(QDoubleSpinBox *spinBox, double maxValue);
	~PercentPicker();
	void setValue(int val){ui.verticalSlider->setValue(val);}
private:
    void keyPressEvent(QKeyEvent *event);
	void mouseReleaseEvent(QMouseEvent *);
    QDoubleSpinBox *spinBox;
	double maxValue;
	Ui::PercentPicker ui;
private slots:
	void on_percentTo1_clicked();
	void on_verticalSlider_valueChanged(int);
};

#endif // PERCENTPICKER_H

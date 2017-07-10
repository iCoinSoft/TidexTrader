

#ifndef JULYSPINBOXPICKER_H
#define JULYSPINBOXPICKER_H

#include <QLabel>
class QDoubleSpinBox;
class QMouseEvent;

class JulySpinBoxPicker : public QLabel
{
	Q_OBJECT

public:
	JulySpinBoxPicker(QDoubleSpinBox *parent, double *forceMinimumValue=0, double internalMinimumValue=0);
	~JulySpinBoxPicker();

private:
	double *forceMinimumValue;
	double internalMinimumValue;
	double maximumValue;
	int scrollDirection;
	void setIcon(int);
	double minimumValue;
	bool j_debugMode;
	QRect currentScreenRect;
	QPoint j_cursorLastPos;
	QPoint j_cursorLastMove;
	bool j_isPressing;
	QPixmap j_simpleState;
	QPixmap j_pressedState;
	QPixmap j_disabledState;

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	QDoubleSpinBox *parentSpinBox;
};

#endif // JULYSPINBOXPICKER_H

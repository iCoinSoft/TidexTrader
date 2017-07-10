

#ifndef JULYLIGHTCHANGES_H
#define JULYLIGHTCHANGES_H

#include <QObject>
#include <QDoubleSpinBox>
#include <QTimer>

class JulyLightChanges : public QObject
{
	Q_OBJECT

public:
	JulyLightChanges(QDoubleSpinBox *parent);
	~JulyLightChanges();
private:
	double lastValue;
	QDoubleSpinBox *parentSpinBox;
	QTimer *changeTimer;
private slots:
	void changeTimerSlot();
public slots:
	void valueChanged(double);
	
};

#endif // JULYLIGHTCHANGES_H

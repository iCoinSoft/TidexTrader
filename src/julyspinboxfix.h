

#ifndef JULYSPINBOXFIX_H
#define JULYSPINBOXFIX_H

#include <QObject>
#include <QDoubleSpinBox>
#include <QFontMetrics>

class JulySpinBoxFix : public QObject
{
	Q_OBJECT

public:
    JulySpinBoxFix(QDoubleSpinBox *parent, int minimumWidth=0);

private:
	int pMinimumWidth;
	int spinMargin;
	QDoubleSpinBox *parentSpinBox;
private slots:
    void widthFix(QString);
};

#endif // JULYSPINBOXFIX_H

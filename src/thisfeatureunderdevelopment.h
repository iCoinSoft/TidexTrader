

#ifndef THISFEATUREUNDERDEVELOPMENT_H
#define THISFEATUREUNDERDEVELOPMENT_H

#include <QDialog>
#include "ui_thisfeatureunderdevelopment.h"

class ThisFeatureUnderDevelopment : public QDialog
{
	Q_OBJECT

public:
	ThisFeatureUnderDevelopment(QWidget *parent=0);
	~ThisFeatureUnderDevelopment();

private:
	Ui::ThisFeatureUnderDevelopment ui;
private slots:
	void themeChanged();
};

#endif // THISFEATUREUNDERDEVELOPMENT_H

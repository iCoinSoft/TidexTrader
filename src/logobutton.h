

#ifndef LOGOBUTTON_H
#define LOGOBUTTON_H

#include <QWidget>
#include "ui_logobutton.h"
#include <QMouseEvent>
#include <QResizeEvent>

class LogoButton : public QWidget
{
	Q_OBJECT

public:
	LogoButton(QWidget *parent = 0);
	~LogoButton();

private:
	QSize logoSize;
	void resizeEvent(QResizeEvent *);
	void mouseReleaseEvent(QMouseEvent *event);
	Ui::LogoButton ui;
public slots:
	void themeChanged();
};

#endif // LOGOBUTTON_H

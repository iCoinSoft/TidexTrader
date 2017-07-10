
#ifndef APPTHEME_H
#define APPTHEME_H

#include <QString>
#include <QColor>
#include <QPalette>
#include <QStyle>

struct AppTheme
{
	AppTheme();
	void loadTheme(QString);
	QString styleSheet;
	QColor getColor(QString);
	QPalette palette;
	QColor swapColor(QColor color);
	QColor altRowColor;
	QColor gray;
	QColor lightGray;
	QColor red;
	QColor green;
	QColor blue;
	QColor lightRed;
	QColor lightGreen;
	QColor lightBlue;
	QColor darkRed;
	QColor darkGreen;
	QColor darkBlue;
	QColor lightRedGreen;
	QColor lightGreenBlue;
	QColor lightRedBlue;
	QColor darkRedBlue;
	QColor black;
	QColor white;
};

#endif // APPTHEME_H

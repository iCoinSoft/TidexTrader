
#include "apptheme.h"
#include <QSettings>
#include "main.h"

AppTheme::AppTheme()
{
	gray=Qt::gray;
	altRowColor=QColor(240,240,240);
	lightGray=Qt::lightGray;
	red=Qt::red;
	green=Qt::green;
	blue=Qt::blue;
	lightRed.setRgb(255,200,200);
	lightGreen.setRgb(200,255,200);
	lightBlue.setRgb(200,200,255);
	lightGreenBlue.setRgb(200,255,255);
	lightRedBlue.setRgb(255,200,255);
	darkRedBlue.setRgb(155,100,155);
	lightRedGreen.setRgb(255,255,200);
	darkRed=Qt::darkRed;
	darkGreen=Qt::darkGreen;
	darkBlue=Qt::darkBlue;
	black=Qt::black;
	white=Qt::white;
}

QColor AppTheme::getColor(QString str)
{
	QStringList colorList=str.split(",");
	if(colorList.count()<3)return Qt::black;
	if(colorList.count()<4)colorList<<"255";
	return QColor(colorList.at(0).toInt(),colorList.at(1).toInt(),colorList.at(2).toInt(),colorList.at(3).toInt());
}

void AppTheme::loadTheme(QString name)
{
    QSettings themeLoad(baseValues.themeFolder+name+".thm",QSettings::IniFormat);

    themeLoad.beginGroup("Normal");
	QStringList colorList=themeLoad.childKeys();
	themeLoad.endGroup();
	for(int n=0;n<colorList.count();n++)
	{
		QStringList split_=colorList.at(n).split("_");
		if(split_.count()<2)continue;
		split_.removeFirst();
		int colNum=split_.first().toInt();
		if(colNum<0||colNum>=20)continue;
		
		palette.setColor(QPalette::Normal,QPalette::ColorRole(colNum),getColor(themeLoad.value("Normal/"+colorList.at(n)).toString()));
	}

	themeLoad.beginGroup("Disabled");
    colorList=themeLoad.childKeys();
	themeLoad.endGroup();
	for(int n=0;n<colorList.count();n++)
	{
		QStringList split_=colorList.at(n).split("_");
		if(split_.count()<2)continue;
		split_.removeFirst();
		int colNum=split_.first().toInt();
		if(colNum<0||colNum>=20)continue;

		palette.setColor(QPalette::Disabled,QPalette::ColorRole(colNum),getColor(themeLoad.value("Disabled/"+colorList.at(n)).toString()));
	}

	themeLoad.beginGroup("Inactive");
	colorList=themeLoad.childKeys();
	themeLoad.endGroup();
	for(int n=0;n<colorList.count();n++)
	{
		QStringList split_=colorList.at(n).split("_");
		if(split_.count()<2)continue;
		split_.removeFirst();
		int colNum=split_.first().toInt();
		if(colNum<0||colNum>=20)continue;

		palette.setColor(QPalette::Inactive,QPalette::ColorRole(colNum),getColor(themeLoad.value("Inactive/"+colorList.at(n)).toString()));
	}

	altRowColor=palette.color(QPalette::AlternateBase);
	gray=getColor(themeLoad.value("Gray").toString());
	red=getColor(themeLoad.value("Red").toString());
	green=getColor(themeLoad.value("Green").toString());
	blue=getColor(themeLoad.value("Blue").toString());
	lightRed=getColor(themeLoad.value("LightRed").toString());
	lightGreen=getColor(themeLoad.value("LightGreen").toString());
	lightBlue=getColor(themeLoad.value("LightRedBlue").toString());
	lightGreenBlue=getColor(themeLoad.value("LightGreenBlue").toString());
	lightRedBlue=getColor(themeLoad.value("LightRedBlue").toString());
	darkRedBlue=getColor(themeLoad.value("DarkRedBlue").toString());
	lightRedGreen=getColor(themeLoad.value("LightRedGreen").toString());
	darkRed=getColor(themeLoad.value("DarkRed").toString());
	darkGreen=getColor(themeLoad.value("DarkGreen").toString());
	darkBlue=getColor(themeLoad.value("DarkBlue").toString());
	black=getColor(themeLoad.value("Black").toString());
	white=getColor(themeLoad.value("White").toString());

	palette.setColor(QPalette::Text,black);

	styleSheet="QHeaderView::section {color: "+black.name()+";}"
		"QToolButton {color: "+black.name()+";}"
		"QPushButton {color: "+black.name()+";}"
        "QGroupBox {background: rgba(255,255,255,60); color: "+black.name()+"; border: 1px solid "+gray.name()+";border-radius: 3px;margin-top: 8px;}"
		"QGroupBox:title {subcontrol-origin: margin; position: relative; left: 6px; color: "+black.name()+"; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 transparent, stop: 0.45 "+white.name()+", stop: 0.5 "+white.name()+", stop: 0.55 "+white.name()+", stop: 1.0 transparent);  padding-left: 2px;} QLabel {color: "+black.name()+";}"
		"QTabBar::tab {color: "+black.name()+";}"
		"QRadioButton {color: "+black.name()+";}"
		"QDoubleSpinBox {background: "+white.name()+";}"
		"QTextEdit {background: "+white.name()+";}"
		"QPlainTextEdit {background: "+white.name()+";}"
		"QCheckBox {color: "+black.name()+";}"
        "QLineEdit {color: "+black.name()+"; background: "+white.name()+"; border: 1px solid "+gray.name()+";}"
        "*[IsDockable] {background: rgba(255,255,255,40); border: 1px solid " + gray.name() + "; border-radius: 3px;margin-top:2px}"
        "QDockWidget::title {background: rgba(255,255,255,100); border: 1px solid " + gray.name() + "; border-radius: 2px; padding:3px; text-align: left center;}"
        "QDockWidget::close-button {top:2px;right:3px}"
        "QDockWidget::float-button {top:2px;right:18px}";
}

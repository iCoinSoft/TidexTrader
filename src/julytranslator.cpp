

#include "julytranslator.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QDockWidget>
#include "main.h"

int JulyTranslator::loadFromFile(const QString &fileName)
{
	clearMaps();
	QFile loadFile(fileName);
	if(loadFile.open(QIODevice::ReadOnly))
	{
		fillMapsFromList(QString::fromUtf8(loadFile.readAll().replace("\r","")).split("\n"));
		loadFile.close();
		lastLangFile=fileName;
		emit languageChanged();
		return 0;
	}
	return 1;
}

void JulyTranslator::fillMapsFromList(const QStringList &list)
{
	for(int n=0;n<list.count();n++)
	{
		QString currentRow=list.at(n);
		if(currentRow.isEmpty()||!currentRow.at(0).isLetter())continue;
		if(fillMapsFromLine(&buttonMap,currentRow,"Button_"))
			if(fillMapsFromLine(&labelMap,currentRow,"Label_"))
				if(fillMapsFromLine(&checkBoxMap,currentRow,"CheckBox_"))
					if(fillMapsFromLine(&spinBoxMap,currentRow,"SpinBox_"))
						if(fillMapsFromLine(&stringMap,currentRow,"String_"))
							fillMapsFromLine(&groupBoxMap,currentRow,"GroupBox_");
	}
}

bool JulyTranslator::fillMapsFromLine(QMap<QString,QString> *map, QString line, const QString &prefix)
{
	if(!line.startsWith(prefix))return true;
	line.remove(0,prefix.length());
	int splitPos=line.indexOf('=');
	if(splitPos==-1||splitPos+1>=line.length())return true;
	QString currentTid=line.left(splitPos);
	line.remove(0,splitPos+1);
	(*map)[currentTid]=line;
	return false;
}

int JulyTranslator::saveToFile(const QString &fileName)
{
	QStringList resultList;
	resultList.append(getMapList(&buttonMap,"Button_"));
	resultList.append(getMapList(&labelMap,"Label_"));
	resultList.append(getMapList(&checkBoxMap,"CheckBox_"));
	resultList.append(getMapList(&groupBoxMap,"GroupBox_"));
	resultList.append(getMapList(&spinBoxMap,"SpinBox_"));
	resultList.append(getMapList(&stringMap,"String_"));
	if(resultList.isEmpty())return 1;
	resultList.sort();
	QFile writeFile(fileName);
	if(writeFile.open(QIODevice::WriteOnly|QIODevice::Truncate))
	{
		writeFile.write(QString(resultList.join("\r\n")+"\r\n").toUtf8());
		writeFile.close();
		return 0;
	}
	return 2;
}

QStringList JulyTranslator::getMapList(QMap<QString,QString> *map, QString prefix)
{
	QStringList mapTids=map->keys();
	for(int n=0;n<mapTids.count();n++)
	{
		mapTids[n]=prefix+mapTids.at(n)+"="+map->value(mapTids.at(n),"");
		mapTids[n].replace("\n","<br>");
		mapTids[n].replace("\r","");
		mapTids[n].replace("\t"," ");
	}
	return mapTids;
}

QString JulyTranslator::translateButton(const QString &tid, const QString &defaultText){return buttonMap.value(tid,defaultText);}
QString JulyTranslator::translateLabel(const QString &tid, const QString &defaultText){return labelMap.value(tid,defaultText);}
QString JulyTranslator::translateCheckBox(const QString &tid, const QString &defaultText){return checkBoxMap.value(tid,defaultText);}
QString JulyTranslator::translateGroupBox(const QString &tid, const QString &defaultText){return groupBoxMap.value(tid,defaultText);}
QString JulyTranslator::translateSpinBox(const QString &tid, const QString &defaultText){return spinBoxMap.value(tid,defaultText);}

QString JulyTranslator::translateString(const QString &tid, const QString &defaultText)
{
	QString result=stringMap.value(tid,defaultText);
	if(stringMap.values(tid).count()==0)stringMap[tid]=defaultText;
	return result;
}

void JulyTranslator::loadMapFromUi(QWidget *par)
{
	Q_FOREACH(QPushButton* curButton, par->findChildren<QPushButton*>())
		if(!curButton->accessibleName().isEmpty())
			buttonMap[curButton->accessibleName()]=curButton->text().replace("\n","<br>").replace("\r","");

	Q_FOREACH(QToolButton* curButton, par->findChildren<QToolButton*>())
		if(!curButton->accessibleName().isEmpty())
			buttonMap[curButton->accessibleName()]=curButton->text().replace("\n","<br>").replace("\r","");
		
	Q_FOREACH(QCheckBox* curCheckBox, par->findChildren<QCheckBox*>())
		if(!curCheckBox->accessibleName().isEmpty())
			checkBoxMap[curCheckBox->accessibleName()]=curCheckBox->text().replace("\n","<br>").replace("\r","");

		Q_FOREACH(QRadioButton* curCheckBox, par->findChildren<QRadioButton*>())
			if(!curCheckBox->accessibleName().isEmpty())
				checkBoxMap[curCheckBox->accessibleName()]=curCheckBox->text().replace("\n","<br>").replace("\r","");

	Q_FOREACH(QLabel* curLabel, par->findChildren<QLabel*>())
		if(!curLabel->accessibleName().isEmpty())
			labelMap[curLabel->accessibleName()]=curLabel->text().replace("\n","<br>").replace("\r","");
		
	Q_FOREACH(QGroupBox* curGroupBox, par->findChildren<QGroupBox*>())
		if(!curGroupBox->accessibleName().isEmpty())
			groupBoxMap[curGroupBox->accessibleName()]=curGroupBox->title().replace("\n","<br>").replace("\r","");

	Q_FOREACH(QDoubleSpinBox* curSpinBox, par->findChildren<QDoubleSpinBox*>())
		if(!curSpinBox->accessibleName().isEmpty())
			spinBoxMap[curSpinBox->accessibleName()]=curSpinBox->suffix();
}

void JulyTranslator::translateUi(QWidget *par)
{
    if(par==0)return;

	Q_FOREACH(QPushButton* curButton, par->findChildren<QPushButton*>())
		if(!curButton->accessibleName().isEmpty())
			curButton->setText(translateButton(curButton->accessibleName(),curButton->text()));

	Q_FOREACH(QToolButton* curButton, par->findChildren<QToolButton*>())
		if(!curButton->accessibleName().isEmpty())
			curButton->setText(translateButton(curButton->accessibleName(),curButton->text()));

	Q_FOREACH(QCheckBox* curCheckBox, par->findChildren<QCheckBox*>())
		if(!curCheckBox->accessibleName().isEmpty())
			curCheckBox->setText(translateCheckBox(curCheckBox->accessibleName(),curCheckBox->text()));

	Q_FOREACH(QRadioButton* curCheckBox, par->findChildren<QRadioButton*>())
		if(!curCheckBox->accessibleName().isEmpty())
			curCheckBox->setText(translateCheckBox(curCheckBox->accessibleName(),curCheckBox->text()));

	Q_FOREACH(QLabel* curLabel, par->findChildren<QLabel*>())
		if(!curLabel->accessibleName().isEmpty())
			curLabel->setText(translateLabel(curLabel->accessibleName(),curLabel->text()));

	Q_FOREACH(QGroupBox* curGroupBox, par->findChildren<QGroupBox*>())
		if(!curGroupBox->accessibleName().isEmpty())
			curGroupBox->setTitle(translateGroupBox(curGroupBox->accessibleName(),curGroupBox->title()));

	Q_FOREACH(QDoubleSpinBox* curSpinBox, par->findChildren<QDoubleSpinBox*>())
		if(!curSpinBox->accessibleName().isEmpty())
			curSpinBox->setSuffix(translateSpinBox(curSpinBox->accessibleName(),curSpinBox->suffix()));

    Q_FOREACH(QWidget* curWidget, par->findChildren<QWidget*>()) {
        QDockWidget* dock = static_cast<QDockWidget*>(curWidget->parentWidget());
        if (dock) {
            if(!curWidget->accessibleName().isEmpty()) {
                QString key = curWidget->accessibleName();
                QString s = translateGroupBox(key, dock->windowTitle());
                dock->setWindowTitle(s);
            }
        }
    }
}

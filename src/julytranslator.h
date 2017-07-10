

#ifndef JULYTRANSLATOR_H
#define JULYTRANSLATOR_H

#include <QObject>
#include <QMap>
#include <QWidget>

class JulyTranslator : public QObject
{
	Q_OBJECT

public:
	QMap<QString,QString>labelMap;
	QMap<QString,QString>groupBoxMap;
	QMap<QString,QString>checkBoxMap;
	QMap<QString,QString>buttonMap;
	QMap<QString,QString>spinBoxMap;
	QMap<QString,QString>stringMap;

	QString lastFile(){return lastLangFile;}
	int loadFromFile(const QString &fileName);
	int saveToFile(const QString &fileName);
	void clearMaps(){labelMap.clear();lastLangFile.clear();groupBoxMap.clear();checkBoxMap.clear();buttonMap.clear();}
	void loadMapFromUi(QWidget *wid);
	void translateUi(QWidget *wid);

	QString translateButton(const QString &tid, const QString &defaultText);
	QString translateLabel(const QString &tid, const QString &defaultText);
	QString translateCheckBox(const QString &tid, const QString &defaultText);
	QString translateGroupBox(const QString &tid, const QString &defaultText);
	QString translateSpinBox(const QString &tid, const QString &defaultText);
	QString translateString(const QString &tid, const QString &defaultText);
private:
	QString lastLangFile;
	bool fillMapsFromLine(QMap<QString,QString> *map, QString line, const QString &prefix);
	void fillMapsFromList(const QStringList &list);
	QStringList getMapList(QMap<QString,QString> *map, QString prefix);
signals:
	void languageChanged();
};

#endif // JULYTRANSLATOR_H

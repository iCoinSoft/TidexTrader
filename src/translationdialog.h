

#ifndef TRANSLATIONDIALOG_H
#define TRANSLATIONDIALOG_H

#include <QDialog>
#include "translationline.h"
#include "ui_translationdialog.h"
#include <QResizeEvent>
#include "julytranslator.h"

class TranslationDialog : public QDialog
{
	Q_OBJECT

public:
	TranslationDialog(QWidget *parent = 0);
	~TranslationDialog();

private:
	void resizeEvent(QResizeEvent *event);
	QGridLayout *gridLayout;
	TranslationLine *authorAbout;
	void fillLayoutByMap(QMap<QString,QString>*, QString subName, QMap<QString,QString>* dMap);
	QList<TranslationLine*> lineEdits;
	QWidget fonWidget;
	Ui::TranslationDialog ui;
public slots:
	void deleteTranslationButton();
	void lineTextChanged();
	void fixLayout();
	void searchLang(QString);
	void applyButton();
	void saveAsButton();
};

#endif // TRANSLATIONDIALOG_H

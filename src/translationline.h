

#ifndef TRANSLATIONLINE_H
#define TRANSLATIONLINE_H

#include <QTextEdit>
#include <QResizeEvent>
#include <QFocusEvent>

class TranslationLine : public QTextEdit
{
	Q_OBJECT

public:
	bool isChanged(){return toPlainText()!=defaultText;}
	QString getValidText();
	void setItemText(QString);
	void setDefaultText(QString defText);
	TranslationLine(QWidget *parent=0);
	~TranslationLine();
private:
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);
	QString defaultText;
	bool fixingSize;
	void resizeEvent(QResizeEvent *event);
	void fixSize();
signals:
	void lineTextChanged();
private slots:
	void textChangedSlot();	
};

#endif // TRANSLATIONLINE_H



#ifndef ADDRULEGROUP_H
#define ADDRULEGROUP_H

#include <QDialog>
#include "ui_addrulegroup.h"

class AddRuleGroup : public QDialog
{
	Q_OBJECT

public:
    QStringList groupsList;
    QString fileName;
    QString groupName;
	AddRuleGroup(QWidget *parent = 0);
	~AddRuleGroup();

private:
	Ui::AddRuleGroup ui;

private slots:
	void checkValidButton();
	void on_ruleOpen_clicked();
	void onGroupContentChanged(bool);
	void on_groupName_textChanged(QString);
	void on_buttonAddRule_clicked();
};

#endif // ADDRULEGROUP_H

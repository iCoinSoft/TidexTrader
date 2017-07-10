

#ifndef RULEWIDGET_H
#define RULEWIDGET_H

#include <QWidget>
#include "ui_rulewidget.h"
#include "rulesmodel.h"
#include <QTime>

class RuleWidget : public QWidget
{
	Q_OBJECT

public:
	void addRuleByHolder(RuleHolder &holder, bool isEnabled);
    bool isBeepOnDone();
    void currencyChanged();
	void updateStyleSheets();
	void saveRulesData();
	bool haveWorkingRules();
    bool haveAnyRules();
    bool removeGroup();
    void languageChanged();
	Ui::RuleWidget ui;
	RulesModel *rulesModel;
    RuleWidget(QString filePath);
	~RuleWidget();

private:
    bool agreeRuleImmediately(QString);
	QTime ordersCancelTime;
	QMenu *rulesEnableDisableMenu;
	QString groupName;
    QString filePath;
private slots:
    void ruleEnableAllSlot();
public slots:
	void writeLog(QString);
	void on_limitRowsValue_valueChanged(int);
	void on_buttonSave_clicked();
    void ruleDone();
	void on_ruleUp_clicked();
	void on_ruleDown_clicked();
	void rulesMenuRequested(const QPoint&);
	void ruleDisableEnableMenuFix();
	void on_ruleConcurrentMode_toggled(bool);
	void ruleEnableSelected();
    void ruleDisableSelected();
    void ruleEnableAll();
    void ruleDisableAll();
	void on_ruleAddButton_clicked();
	void on_ruleEditButton_clicked();
	void on_ruleRemoveAll_clicked();
	void checkValidRulesButtons();
	void on_ruleRemove_clicked();
	void on_ruleSave_clicked();
};

#endif // RULEWIDGET_H

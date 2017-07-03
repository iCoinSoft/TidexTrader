//  This file is part of Qt Bitcoin Trader
//      https://github.com/JulyIGHOR/QtBitcoinTrader
//  Copyright (C) 2013-2017 July IGHOR <julyighor@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  In addition, as a special exception, the copyright holders give
//  permission to link the code of portions of this program with the
//  OpenSSL library under certain conditions as described in each
//  individual source file, and distribute linked combinations including
//  the two.
//
//  You must obey the GNU General Public License in all respects for all
//  of the code used other than OpenSSL. If you modify file(s) with this
//  exception, you may extend this exception to your version of the
//  file(s), but you are not obligated to do so. If you do not wish to do
//  so, delete this exception statement from your version. If you delete
//  this exception statement from all source files in the program, then
//  also delete it here.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "rulewidget.h"
#include "main.h"
#include <QMessageBox>
#include <QtCore/qmath.h>
#include <QFileDialog>
#include <QDesktopServices>
#include "rulescriptparser.h"
#include "addruledialog.h"
#include "exchange/exchange.h"
#include "utils/utils.h"

RuleWidget::RuleWidget(QString fileName)
	: QWidget()
{
	ui.setupUi(this);
	ui.rulesTabs->setCurrentIndex(0);

    filePath=fileName;

    setProperty("FileName",filePath);
    setProperty("GroupType","Rule");

    QSettings loadRule(fileName,QSettings::IniFormat);
    loadRule.beginGroup("JLRuleGroup");
	groupName=loadRule.value("Name","Unknown").toString();
    ui.limitRowsValue->setValue(loadRule.value("LogRowsCount",20).toInt());
	ui.ruleBeep->setChecked(loadRule.value("BeepOnDone",false).toBool());
	ui.notes->setPlainText(loadRule.value("Notes","").toString());
    loadRule.endGroup();

    ordersCancelTime=QTime(1,0,0,0);
    setAttribute(Qt::WA_DeleteOnClose,true);

	updateStyleSheets();

	ui.rulesNoMessage->setVisible(true);
	ui.rulesTabs->setVisible(false);

    rulesModel=new RulesModel(groupName);
	connect(rulesModel,SIGNAL(writeLog(QString)),this,SLOT(writeLog(QString)));
	rulesModel->setParent(this);
	ui.rulesTable->setModel(rulesModel);
    mainWindow.setColumnResizeMode(ui.rulesTable,0,QHeaderView::ResizeToContents);
    mainWindow.setColumnResizeMode(ui.rulesTable,1,QHeaderView::Stretch);

    connect(rulesModel,SIGNAL(ruleDone()),this,SLOT(ruleDone()));

	connect(ui.rulesTable->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(checkValidRulesButtons()));
	ui.rulesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.rulesTable, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(rulesMenuRequested(const QPoint&)));

	rulesEnableDisableMenu=new QMenu;
	rulesEnableDisableMenu->addAction("Enable Selected");
	connect(rulesEnableDisableMenu->actions().last(),SIGNAL(triggered(bool)),this,SLOT(ruleEnableSelected()));
	rulesEnableDisableMenu->addAction("Disable Selected");
	connect(rulesEnableDisableMenu->actions().last(),SIGNAL(triggered(bool)),this,SLOT(ruleDisableSelected()));
	rulesEnableDisableMenu->addSeparator();
	rulesEnableDisableMenu->addAction("Enable All");
    connect(rulesEnableDisableMenu->actions().last(),SIGNAL(triggered(bool)),this,SLOT(ruleEnableAll()));
	rulesEnableDisableMenu->addAction("Disable All");
    connect(rulesEnableDisableMenu->actions().last(),SIGNAL(triggered(bool)),this,SLOT(ruleDisableAll()));
	ui.ruleEnableDisable->setMenu(rulesEnableDisableMenu);
	connect(rulesEnableDisableMenu,SIGNAL(aboutToShow()),this,SLOT(ruleDisableEnableMenuFix()));

	languageChanged();

    setWindowTitle(groupName);

    QStringList rulesList=loadRule.childGroups();
    Q_FOREACH(QString group, rulesList)
    {
        if(!group.startsWith("Rule_"))continue;
        RuleHolder holder=RuleScriptParser::readHolderFromSettings(loadRule,group);
        if(holder.isValid())rulesModel->addRule(holder);
    }

	saveRulesData();

	checkValidRulesButtons();

	mainWindow.fixTableViews(this);

    QSettings iniSettings(baseValues.iniFileName,QSettings::IniFormat,this);
    if(iniSettings.value("UI/OptimizeInterface",false).toBool())recursiveUpdateLayouts(this);
}

RuleWidget::~RuleWidget()
{
    if(!filePath.isEmpty())saveRulesData();
}

void RuleWidget::writeLog(QString text)
{
    text.prepend(QDateTime::fromTime_t(TimeSync::getTimeT()).time().toString(baseValues.timeFormat)+"> ");
    ui.consoleOutput->appendPlainText(text);
}

void RuleWidget::on_buttonSave_clicked()
{
	saveRulesData();
}

bool RuleWidget::isBeepOnDone()
{
    return ui.ruleBeep->isChecked();
}

void RuleWidget::ruleDone()
{
    if(isBeepOnDone())mainWindow.beep();
}

void RuleWidget::updateStyleSheets()
{
	ui.rulesNoMessage->setStyleSheet("border: 1px solid gray; background: "+baseValues.appTheme.white.name()+"; color: "+baseValues.appTheme.gray.name());
}

bool RuleWidget::removeGroup()
{
    bool removed=true;
    if(!filePath.isEmpty()){QFile::remove(filePath);removed=!QFile::exists(filePath);};
    filePath.clear();
    return removed;
}

void RuleWidget::languageChanged()
{
	julyTranslator.translateUi(this);

	ui.rulesTabs->setTabText(0,julyTr("TAB_RULES_FOR_ORDERS","Rules"));
	ui.rulesTabs->setTabText(1,julyTr("CONSOLE_OUT","Console output"));
	ui.rulesTabs->setTabText(2,julyTr("SCRIPT_NOTES","Notes"));

    rulesModel->setHorizontalHeaderLabels(QStringList()<<julyTr("RULES_T_STATE","State")<<julyTr("RULES_T_DESCR","Description"));
            //Removed <<julyTr("RULES_T_ACTION","Action")<<julyTr("ORDERS_AMOUNT","Amount")<<julyTr("RULES_T_PRICE","Price"));

	rulesEnableDisableMenu->actions().at(0)->setText(julyTr("RULE_ENABLE","Enable Selected"));
	rulesEnableDisableMenu->actions().at(1)->setText(julyTr("RULE_DISABLE","Disable Selected"));
	rulesEnableDisableMenu->actions().at(3)->setText(julyTr("RULE_ENABLE_ALL","Enable All"));
	rulesEnableDisableMenu->actions().at(4)->setText(julyTr("RULE_DISABLE_ALL","Disable All"));

	mainWindow.fixAllChildButtonsAndLabels(this);
}

void RuleWidget::on_limitRowsValue_valueChanged(int val)
{
	ui.consoleOutput->setMaximumBlockCount(val);
	QSettings(filePath,QSettings::IniFormat).setValue("JLRuleGroup/LogRowsCount",val);
}

void RuleWidget::saveRulesData()
{
	ui.saveFon->setVisible(false);
    if(QFile::exists(filePath))QFile::remove(filePath);
    QSettings saveScript(filePath,QSettings::IniFormat);
    saveScript.beginGroup("JLRuleGroup");
	saveScript.setValue("Version",baseValues.jlScriptVersion);
	saveScript.setValue("Name",groupName);
	saveScript.setValue("LogRowsCount",ui.limitRowsValue->value());
	saveScript.setValue("Notes",ui.notes->toPlainText());
    saveScript.setValue("BeepOnDone",ui.ruleBeep->isChecked());
    saveScript.endGroup();

    for(int n=0;n<rulesModel->holderList.count();n++)
        RuleScriptParser::writeHolderToSettings(rulesModel->holderList[n],saveScript,"Rule_"+QString::number(n+101));
    saveScript.sync();
}

void RuleWidget::addRuleByHolder(RuleHolder &holder, bool isEnabled)
{
	rulesModel->addRule(holder,isEnabled);

	ui.rulesNoMessage->setVisible(false);
	ui.rulesTabs->setVisible(true);
	checkValidRulesButtons();
	saveRulesData();
}

void RuleWidget::on_ruleAddButton_clicked()
{
    AddRuleDialog ruleWindow(groupName,this);
    ruleWindow.setWindowFlags(mainWindow.windowFlags());
    if(ruleWindow.exec()==QDialog::Rejected)return;

    RuleHolder holder=ruleWindow.getRuleHolder();
    if(!holder.isValid())return;

    mainWindow.addRuleByHolder(holder,ruleWindow.isRuleEnabled(),ruleWindow.getGroupName(),"");
}

void RuleWidget::on_ruleConcurrentMode_toggled(bool on)
{
	rulesModel->isConcurrentMode=on;
}

void RuleWidget::on_ruleEditButton_clicked()
{
	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
	int curRow=selectedRows.first().row();
	if(curRow<0)return;

    AddRuleDialog ruleWindow(groupName,baseValues.mainWindow_);
    ruleWindow.setWindowFlags(mainWindow.windowFlags());
    ruleWindow.fillByHolder(rulesModel->holderList[curRow],rulesModel->getStateByRow(curRow)==1);
    if(ruleWindow.exec()==QDialog::Rejected)return;

    RuleHolder holder=ruleWindow.getRuleHolder();
    if(!holder.isValid())return;

    if(ruleWindow.saveClicked)
    {
    rulesModel->setRuleStateByRow(curRow,0);
    rulesModel->updateRule(curRow,holder,ruleWindow.isRuleEnabled());

	checkValidRulesButtons();
	saveRulesData();
    }
    else
    mainWindow.addRuleByHolder(holder,ruleWindow.isRuleEnabled(),ruleWindow.getGroupName(),"");
}

void RuleWidget::on_ruleRemoveAll_clicked()
{
	QMessageBox msgBox(baseValues.mainWindow_);
	msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(julyTr("APPLICATION_TITLE",windowTitle()));
	msgBox.setText(julyTr("RULE_CONFIRM_REMOVE_ALL","Are you sure to remove all rules?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	msgBox.setButtonText(QMessageBox::Yes,julyTr("YES","Yes"));
	msgBox.setButtonText(QMessageBox::No,julyTr("NO","No"));
	if(msgBox.exec()!=QMessageBox::Yes)return;

	rulesModel->clear();
	checkValidRulesButtons();
	saveRulesData();
}

void RuleWidget::on_ruleRemove_clicked()
{
	QMessageBox msgBox(baseValues.mainWindow_);
	msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(julyTr("APPLICATION_TITLE",windowTitle()));
	msgBox.setText(julyTr("RULE_CONFIRM_REMOVE","Are you sure to remove this rule?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	msgBox.setButtonText(QMessageBox::Yes,julyTr("YES","Yes"));
	msgBox.setButtonText(QMessageBox::No,julyTr("NO","No"));
	if(msgBox.exec()!=QMessageBox::Yes)return;

	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
	int curRow=selectedRows.first().row();
	rulesModel->removeRuleByRow(curRow);
	checkValidRulesButtons();
	saveRulesData();
}

void RuleWidget::rulesMenuRequested(const QPoint& point)
{
	rulesEnableDisableMenu->exec(ui.rulesTable->viewport()->mapToGlobal(point));
}

void RuleWidget::ruleDisableEnableMenuFix()
{
	bool haveRules_=rulesModel->rowCount()>0;
	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();

	int selectedRulesCount=selectedRows.count();
	bool ifSelectedOneRuleIsItEnabled=selectedRulesCount==1;
	if(ifSelectedOneRuleIsItEnabled)
	{
        int state=rulesModel->getStateByRow(selectedRows.first().row());
        ifSelectedOneRuleIsItEnabled=state==1||state==2;
        rulesEnableDisableMenu->actions().at(0)->setEnabled(!ifSelectedOneRuleIsItEnabled||state==3);
        rulesEnableDisableMenu->actions().at(1)->setEnabled(ifSelectedOneRuleIsItEnabled||state==3);
	}
	else
	{
		rulesEnableDisableMenu->actions().at(0)->setEnabled(selectedRulesCount>0);
		rulesEnableDisableMenu->actions().at(1)->setEnabled(selectedRulesCount>0);
	}
	rulesEnableDisableMenu->actions().at(2)->setEnabled(haveRules_);
	rulesEnableDisableMenu->actions().at(3)->setEnabled(haveRules_);
}

bool RuleWidget::haveWorkingRules()
{
	return rulesModel->haveWorkingRule();
}

bool RuleWidget::haveAnyRules()
{
	return rulesModel->rowCount()>0;
}

void RuleWidget::currencyChanged()
{
    if(baseValues.currentExchange_->multiCurrencyTradeSupport)return;
    rulesModel->currencyChanged();
}

void RuleWidget::checkValidRulesButtons()
{
	int selectedCount=ui.rulesTable->selectionModel()->selectedRows().count();
	ui.ruleEditButton->setEnabled(selectedCount==1);
	ui.ruleRemove->setEnabled(selectedCount);
	rulesEnableDisableMenu->actions().at(0)->setEnabled(selectedCount);
	rulesEnableDisableMenu->actions().at(1)->setEnabled(selectedCount);
	ui.ruleEnableDisable->setEnabled(rulesModel->rowCount());
    ui.ruleRemoveAll->setEnabled(rulesModel->rowCount());
    ui.ruleConcurrentMode->setEnabled(rulesModel->rowCount()==0||!rulesModel->haveWorkingRule());
    ui.ruleSequencialMode->setEnabled(ui.ruleConcurrentMode->isEnabled());

	ui.rulesNoMessage->setVisible(rulesModel->rowCount()==0);
	ui.rulesTabs->setVisible(rulesModel->rowCount());

	ui.ruleUp->setEnabled(ui.ruleEditButton->isEnabled()&&rulesModel->rowCount()>1);
	ui.ruleDown->setEnabled(ui.ruleEditButton->isEnabled()&&rulesModel->rowCount()>1);
}

void RuleWidget::on_ruleUp_clicked()
{
	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
	int curRow=selectedRows.first().row();
	if(curRow<1)return;
	rulesModel->moveRowUp(curRow);
    ui.rulesTable->selectRow(curRow-1);
}

void RuleWidget::on_ruleDown_clicked()
{
	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
	int curRow=selectedRows.first().row();
	if(curRow>=rulesModel->rowCount()-1)return;

	rulesModel->moveRowDown(curRow);
    ui.rulesTable->selectRow(curRow+1);
}

bool RuleWidget::agreeRuleImmediately(QString text)
{
    text.replace(QLatin1Char('<'),QLatin1String("&#60;"));
    text.replace(QLatin1Char('='),QLatin1String("&#61;"));
    text.replace(QLatin1Char('>'),QLatin1String("&#62;"));

    QMessageBox msgBox(baseValues.mainWindow_);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(windowTitle());
    msgBox.setText(julyTr("INVALID_RULE_CHECK","This rule will be executed instantly.<br>This means that you make a mistake.<br>Please check values you entered.")+"<br><br>\""+text+"\"");
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setButtonText(QMessageBox::Ok,julyTr("RULE_ENABLE","Enable Rule"));
    msgBox.setButtonText(QMessageBox::Cancel,julyTranslator.translateButton("TRCANCEL","Cancel"));
    return msgBox.exec()==QMessageBox::Ok;
}

void RuleWidget::ruleEnableSelected()
{
	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
    int curRow=selectedRows.first().row();
    if(rulesModel->testRuleByRow(curRow))
        if(!agreeRuleImmediately(rulesModel->holderList.at(curRow).description))return;
	rulesModel->setRuleStateByRow(curRow,1);//Enable
	checkValidRulesButtons();
}

void RuleWidget::ruleDisableSelected()
{
	QModelIndexList selectedRows=ui.rulesTable->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
	int curRow=selectedRows.first().row();

    if(!rulesModel->isConcurrentMode&&curRow<rulesModel->rowCount()-1&&rulesModel->getStateByRow(curRow+1)==1&&!rulesModel->isRowPaused(curRow))
    {
        if(rulesModel->testRuleByRow(curRow+1,true))
            if(!agreeRuleImmediately(rulesModel->holderList.at(curRow+1).description))return;
    }
	rulesModel->setRuleStateByRow(curRow,0);//Disable
	checkValidRulesButtons();
}

void RuleWidget::ruleEnableAll()
{
    QTimer::singleShot(100,this,SLOT(ruleEnableAllSlot()));
}

void RuleWidget::ruleEnableAllSlot()
{
    for(int curRow=0;curRow<rulesModel->holderList.count();curRow++)
    {
        if(rulesModel->getStateByRow(curRow)==1)continue;
        if(qobject_cast<QAction*>(sender())&&rulesModel->testRuleByRow(curRow))
            if(!agreeRuleImmediately(rulesModel->holderList.at(curRow).description))continue;
        rulesModel->setRuleStateByRow(curRow,1);
    }
	checkValidRulesButtons();
}

void RuleWidget::ruleDisableAll()
{
    mainWindow.clearPendingGroup(groupName);
	rulesModel->disableAll();
	checkValidRulesButtons();
}

void RuleWidget::on_ruleSave_clicked()
{
    QString lastRulesDir=mainWindow.iniSettings->value("UI/LastRulesPath",baseValues.desktopLocation).toString();
    if(!QFile::exists(lastRulesDir))lastRulesDir=baseValues.desktopLocation;
    QString fileName=QFileDialog::getSaveFileName(baseValues.mainWindow_, julyTr("SAVE_GOUP","Save Rules Group"),lastRulesDir+"/"+QString(groupName).replace("/","_").replace("\\","").replace(":","").replace("?","")+".JLR","JL Ruels (*.JLR)");

	if(fileName.isEmpty())return;
	mainWindow.iniSettings->setValue("UI/LastRulesPath",QFileInfo(fileName).dir().path());
	mainWindow.iniSettings->sync();
	if(QFile::exists(fileName))QFile::remove(fileName);

    QSettings saveScript(fileName,QSettings::IniFormat);
    saveScript.beginGroup("JLRuleGroup");
    saveScript.setValue("Version",baseValues.jlScriptVersion);
    saveScript.setValue("Name",groupName);
    saveScript.endGroup();

    for(int n=0;n<rulesModel->holderList.count();n++)
        RuleScriptParser::writeHolderToSettings(rulesModel->holderList[n],saveScript,"Rule_"+QString::number(n+101));
    saveScript.sync();

    if(!QFile::exists(fileName))
    {
        QMessageBox::warning(baseValues.mainWindow_,windowTitle(),"Can not write file");
        return;
    }
}

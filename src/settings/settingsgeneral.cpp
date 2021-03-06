

#include "settingsgeneral.h"
#include "main.h"
#include "translationmessage.h"
#include <QDir>

SettingsGeneral::SettingsGeneral()
	: QWidget()
{
	ui.setupUi(this);
    iniSettings=new QSettings(baseValues.iniFileName,QSettings::IniFormat,this);
    mainSettings=new QSettings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);

	loadLanguage();
    loadOther();
	loadUpdates();
    loadTime();

    ui.revertChangesButton->setEnabled(false);
    ui.saveButton->setEnabled(false);

#ifdef Q_OS_MAC
    ui.closeToTrayLabel->setVisible(false);
    ui.closeToTrayCheckBox->setVisible(false);
#endif
}

SettingsGeneral::~SettingsGeneral()
{
    delete iniSettings;
    delete mainSettings;
}

void SettingsGeneral::loadLanguage()
{
    QStringList langList;
    QFile resLanguage(":/Resources/Language/LangList.ini");
    resLanguage.open(QIODevice::ReadOnly);
    QStringList resourceLanguages=QString(resLanguage.readAll().replace("\r","")).split("\n");
    for(int n=0;n<resourceLanguages.count();n++)if(!resourceLanguages.at(n).isEmpty())langList<<":/Resources/Language/"+resourceLanguages.at(n);
    QStringList folderLangList=QDir(appDataDir+"Language","*.lng").entryList();
    folderLangList.sort();
    for(int n=0;n<folderLangList.count();n++)langList<<appDataDir+"Language/"+folderLangList.at(n);
    int selectedLangId=-1;

    QString preferedLangFile=julyTranslator.lastFile();
    if(!QFile::exists(preferedLangFile))preferedLangFile.clear();
    if(preferedLangFile.isEmpty())preferedLangFile=baseValues.defaultLangFile;

    ui.languageComboBox->clear();
    for(int n=0;n<langList.count();n++)
    {
        JulyTranslator translateName;
        translateName.loadFromFile(langList.at(n));
        QString langName=translateName.translateString("LANGUAGE_NAME","");
        if(langName.isEmpty())continue;
        if(preferedLangFile==langList.at(n))selectedLangId=n;
        ui.languageComboBox->insertItem(ui.languageComboBox->count(),langName,langList.at(n));
    }
    if(selectedLangId>-1)ui.languageComboBox->setCurrentIndex(selectedLangId);
}

void SettingsGeneral::saveLanguage()
{
    QString loadFromFile=ui.languageComboBox->itemData(ui.languageComboBox->currentIndex(),Qt::UserRole).toString();
    if(!loadFromFile.isEmpty()){
        julyTranslator.loadFromFile(loadFromFile);
        mainSettings->setValue("LanguageFile",loadFromFile);
        baseValues.mainWindow_->chartsView->refreshCharts();
    }
}

void SettingsGeneral::loadOther()
{
    ui.confirmOpenOrderCheckBox->setChecked(iniSettings->value("UI/ConfirmOpenOrder",true).toBool());
    ui.closeToTrayCheckBox->setChecked(iniSettings->value("UI/CloseToTray",false).toBool());
    ui.optimizeInterfaceCheckBox->setChecked(iniSettings->value("UI/OptimizeInterface",false).toBool());
}

void SettingsGeneral::saveOther()
{
    iniSettings->setValue("UI/ConfirmOpenOrder",ui.confirmOpenOrderCheckBox->isChecked());
    baseValues.mainWindow_->confirmOpenOrder=ui.confirmOpenOrderCheckBox->isChecked();

#ifdef Q_OS_MAC
    ui.closeToTrayCheckBox->setChecked(false);
#endif
    iniSettings->setValue("UI/CloseToTray",ui.closeToTrayCheckBox->isChecked());
    baseValues.mainWindow_->closeToTray=ui.closeToTrayCheckBox->isChecked();

    iniSettings->setValue("UI/OptimizeInterface",ui.optimizeInterfaceCheckBox->isChecked());
}

void SettingsGeneral::loadUpdates()
{
    ui.checkForUpdatesCheckBox->setChecked(mainSettings->value("CheckForUpdates",true).toBool());
    ui.checkForUpdatesBetaCheckBox->setChecked(mainSettings->value("CheckForUpdatesBeta",false).toBool());
    ui.autoUpdateCheckBox->setChecked(mainSettings->value("AutoUpdate",false).toBool());
}

void SettingsGeneral::saveUpdates()
{
    mainSettings->setValue("CheckForUpdates",ui.checkForUpdatesCheckBox->isChecked());
    mainSettings->setValue("CheckForUpdatesBeta",ui.checkForUpdatesBetaCheckBox->isChecked());
    mainSettings->setValue("AutoUpdate",ui.autoUpdateCheckBox->isChecked());
}

void SettingsGeneral::loadTime()
{
    ui.use24HourTimeFormatCheckBox->setChecked(mainSettings->value("Use24HourTimeFormat",true).toBool());
    ui.timeSynchronizationCheckBox->setChecked(mainSettings->value("TimeSynchronization",true).toBool());
}

void SettingsGeneral::saveTime()
{
    mainSettings->setValue("Use24HourTimeFormat",ui.use24HourTimeFormatCheckBox->isChecked());
    mainSettings->setValue("TimeSynchronization",ui.timeSynchronizationCheckBox->isChecked());

    baseValues_->use24HourTimeFormat=ui.use24HourTimeFormatCheckBox->isChecked();
}

void SettingsGeneral::on_revertChangesButton_clicked()
{
	loadLanguage();
    loadOther();
	loadUpdates();
    loadTime();

	ui.revertChangesButton->setEnabled(false);
	ui.saveButton->setEnabled(false);
}

void SettingsGeneral::on_restoreDefaultsButton_clicked()
{
    ui.languageComboBox->setCurrentIndex(ui.languageComboBox->findData(baseValues.defaultLangFile,Qt::UserRole,Qt::MatchExactly|Qt::MatchCaseSensitive));
    ui.confirmOpenOrderCheckBox->setChecked(true);
    ui.closeToTrayCheckBox->setChecked(false);
    ui.optimizeInterfaceCheckBox->setChecked(false);
	ui.checkForUpdatesCheckBox->setChecked(true);
    ui.checkForUpdatesBetaCheckBox->setChecked(false);
    ui.use24HourTimeFormatCheckBox->setChecked(true);
    ui.timeSynchronizationCheckBox->setChecked(true);

	ui.restoreDefaultsButton->setEnabled(false);
}

void SettingsGeneral::on_saveButton_clicked()
{
    saveLanguage();
    saveOther();
    saveUpdates();
    saveTime();

	ui.revertChangesButton->setEnabled(false);
	ui.saveButton->setEnabled(false);
}

void SettingsGeneral::anyValueChanged()
{
	if(!ui.revertChangesButton->isEnabled())ui.revertChangesButton->setEnabled(true);
	if(!ui.restoreDefaultsButton->isEnabled())ui.restoreDefaultsButton->setEnabled(true);
	if(!ui.saveButton->isEnabled())ui.saveButton->setEnabled(true);
}

void SettingsGeneral::on_showTranslationButton_clicked()
{
    this->parentWidget()->parentWidget()->close();

    TranslationMessage translationMessage;
    translationMessage.exec();
}

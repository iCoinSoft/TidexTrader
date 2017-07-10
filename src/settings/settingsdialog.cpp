

#include <QMessageBox>
#include "settingsdialog.h"
#include "main.h"

SettingsDialog::SettingsDialog()
	: QDialog()
{
	ui.setupUi(this);
    ui.scrollAreaWidgetContents->setBackgroundRole(QPalette::Base);
	setWindowFlags(Qt::WindowCloseButtonHint);
    listLayout=new QVBoxLayout;
    setWindowTitle(julyTr("SETTINGS_TITLE","Settings Dialog"));

	settingsGeneral = new SettingsGeneral;
    addDialog(julyTr("GENERAL","General"),"settings_global_32x32.png",settingsGeneral);
    settingsNetworkProxy = new SettingsNetworkProxy;
    addDialog(julyTr("NETWORK","Network"),"settings_network_32x32.png",settingsNetworkProxy);
    settingsDecimals = new SettingsDecimals;
    addDialog(julyTr("DECIMALS","Decimals"),"settings_decimals_32x32.png",settingsDecimals);

    configureNameList();

    julyTranslator.translateUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete listLayout;
    listListElement.clear();
    delete settingsGeneral;
    delete settingsNetworkProxy;
    delete settingsDecimals;
}

bool SettingsDialog::isSettingsSaved()
{
    switch (ui.settingsStackedWidget->currentIndex()) {
        case 0:
            return !settingsGeneral->ui.saveButton->isEnabled();
        case 1:
            return !settingsNetworkProxy->ui.saveButton->isEnabled();
        case 2:
            return !settingsDecimals->ui.saveButton->isEnabled();
        default:
            break;
    }
    return true;
}

void SettingsDialog::settingsSave()
{
    switch (ui.settingsStackedWidget->currentIndex()) {
        case 0:
            settingsGeneral->on_saveButton_clicked();
            return;
        case 1:
            settingsNetworkProxy->on_saveButton_clicked();
            return;
        case 2:
            settingsDecimals->on_saveButton_clicked();
            return;
        default:
            return;
    }
}

void SettingsDialog::settingsDiscard()
{
    switch (ui.settingsStackedWidget->currentIndex()) {
        case 0:
            settingsGeneral->on_revertChangesButton_clicked();
            return;
        case 1:
            settingsNetworkProxy->on_revertChangesButton_clicked();
            return;
        case 2:
            settingsDecimals->on_revertChangesButton_clicked();
            return;
        default:
            return;
    }
}

void SettingsDialog::resizeNameList()
{
    quint32 maxWidth=0;
    for(int i=0;i<listListElement.size();i++){
        if(maxWidth<listListElement.at(i)->width)maxWidth=listListElement.at(i)->width;
    }
    ui.scrollArea->setFixedWidth(maxWidth+70);
}

void SettingsDialog::configureNameList()
{
    listLayout->setSpacing(0);
    listLayout->setContentsMargins(5,5,5,5);
    listLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui.scrollAreaWidgetContents->setLayout(listLayout);
    if(listListElement.size())listListElement.at(0)->selectedItem();

    resizeNameList();
}

void SettingsDialog::addDialog(QString name, QString icon, QWidget *widget)
{
    ui.settingsStackedWidget->addWidget(widget);

    SettingsDialogListElement *listElement=new SettingsDialogListElement(this,listListElement.size(),name,icon);
    listListElement.append(listElement);
    listLayout->addWidget(listElement);
}

void SettingsDialog::clickOnList(qint32 index)
{
    if(index<0||index>=listListElement.size())return;
    if(index==ui.settingsStackedWidget->currentIndex())return;
    if(listListElement.size()!=ui.settingsStackedWidget->count())return;

    bool settingsSaved=isSettingsSaved();
    if(!settingsSaved){
        QMessageBox closeMsgBox;
        closeMsgBox.setWindowTitle(julyTr("SETTINGS_MODIFIED","The settings has been modified"));
        closeMsgBox.setText(julyTr("SETTINGS_WANT_SAVE_CHANGES","Do you want to save your changes?"));
        closeMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
        closeMsgBox.setDefaultButton(QMessageBox::Save);
        closeMsgBox.setButtonText(QMessageBox::Save,julyTr("YES","Save"));
        closeMsgBox.setButtonText(QMessageBox::No,julyTr("NO","Discard"));
        closeMsgBox.setButtonText(QMessageBox::Cancel,julyTranslator.translateButton("TRCANCEL","Cancel"));
        closeMsgBox.setIcon(QMessageBox::Warning);

        switch (closeMsgBox.exec()) {
            case QMessageBox::Save:
                settingsSave();
                break;
            case QMessageBox::Discard:
                settingsDiscard();
                break;
            case QMessageBox::Cancel:
                return;
            default:
                break;
        }
    }

    listListElement.at(ui.settingsStackedWidget->currentIndex())->clearSelection();
    listListElement.at(index)->selectedItem();
    ui.settingsStackedWidget->setCurrentIndex(index);
}

void SettingsDialog::closeEvent(QCloseEvent *event)
{
	bool settingsSaved=isSettingsSaved();
	if(settingsSaved)return;

    QMessageBox closeMsgBox;
    closeMsgBox.setWindowTitle(julyTr("SETTINGS_MODIFIED","The settings has been modified"));
    closeMsgBox.setText(julyTr("SETTINGS_WANT_SAVE_CHANGES","Do you want to save your changes?"));
    closeMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
    closeMsgBox.setDefaultButton(QMessageBox::Save);
    closeMsgBox.setButtonText(QMessageBox::Save,julyTr("YES","Save"));
    closeMsgBox.setButtonText(QMessageBox::No,julyTr("NO","Discard"));
    closeMsgBox.setButtonText(QMessageBox::Cancel,julyTranslator.translateButton("TRCANCEL","Cancel"));
    closeMsgBox.setIcon(QMessageBox::Warning);

    switch (closeMsgBox.exec()) {
		case QMessageBox::Save:
			settingsSave();
			break;
        case QMessageBox::No:
			break;
		case QMessageBox::Cancel:
			event->ignore();
			break;
		default:
			break;
	}
}

void SettingsDialog::disableTranslateButton()
{
    settingsGeneral->ui.showTranslationButton->setEnabled(false);
}

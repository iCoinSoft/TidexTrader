
#include "passworddialog.h"
#include "main.h"
#include <QDir>
#include <QSettings>
#include <QMessageBox>
#include <QDesktopServices>
#include <QCryptographicHash>
#include "logobutton.h"
#include "timesync.h"

PasswordDialog::PasswordDialog(QWidget *parent)
	: QDialog(parent)
{
	resetData=false;
	newProfile=false;
	ui.setupUi(this);
    setWindowTitle(windowTitle()+" v"+baseValues.appVerStr);
	setWindowFlags(Qt::WindowCloseButtonHint);
	ui.okButton->setEnabled(false);

	QSettings settings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
    QString lastProfile=settings.value("LastProfile","").toString();
	int lastProfileIndex=-1;
	int firstUnlockedProfileIndex=-1;

	QMap<int,QString> logosMap;
	QSettings listSettings(":/Resources/Exchanges/List.ini",QSettings::IniFormat);
	QStringList exchangesList=listSettings.childGroups();
	for(int n=0;n<exchangesList.count();n++)
	{
		QString currentLogo=listSettings.value(exchangesList.at(n)+"/Logo").toString();
		if(currentLogo.isEmpty())continue;
        logosMap.insert(n,":/Resources/Exchanges/Logos/"+currentLogo);
	}

    QStringList scriptsOldPlace=QDir(baseValues.scriptFolder).entryList(QStringList()<<"*.JLR"<<"*.JLS");
    QStringList iniNames;

	QStringList settingsList=QDir(appDataDir,"*.ini").entryList();
	for(int n=0;n<settingsList.count();n++)
	{
        if(scriptsOldPlace.count())iniNames<<QFileInfo(settingsList.at(n)).completeBaseName();
		QSettings settIni(appDataDir+settingsList.at(n),QSettings::IniFormat);

		if(baseValues.appVerLastReal<1.0772)
		{
			QString cryptedData=settIni.value("CryptedData","").toString();
			if(!cryptedData.isEmpty())settIni.setValue("EncryptedData/ApiKeySign",cryptedData);

			QString profileNameOld=settIni.value("ProfileName","").toString();
			if(!profileNameOld.isEmpty())
			{
				settIni.remove("ProfileName");
				settIni.setValue("Profile/Name",profileNameOld);
			}
			QString profileExchangeIdOld=settIni.value("ExchangeId","").toString();
			if(!profileExchangeIdOld.isEmpty())
			{
				settIni.remove("ExchangeId");
				settIni.setValue("Profile/ExchangeId",profileExchangeIdOld);
			}
			settIni.sync();
		}

		if(settIni.value("EncryptedData/ApiKeySign","").toString().isEmpty())
		{
			QFile::remove(appDataDir+settingsList.at(n));
			continue;
		}
		int currentProfileExchangeId=settIni.value("Profile/ExchangeId",0).toInt();

		if(baseValues.appVerLastReal<1.0775)
		{
			if(currentProfileExchangeId==2)
			{
				QFile::remove(appDataDir+settingsList.at(n));
				static bool haveBitstampProfile=false;
				if(!haveBitstampProfile)
				{
					haveBitstampProfile=true;
                    QMessageBox::warning(0,windowTitle(),"From now Bitstamp support API keys with permissions. To ensure the security of your Bitstamp account you must create new API keys and add a new profile of Bitstamp to the TidexTrader.");
				}
				continue;
			}
		}
		QString currentLogo=logosMap.value(currentProfileExchangeId);
		if(!QFile::exists(currentLogo))currentLogo=":/Resources/Exchanges/Logos/Unknown.png";
        ui.profileComboBox->addItem(QIcon(currentLogo),settIni.value("Profile/Name",QFileInfo(settingsList.at(n)).fileName()).toString(),settingsList.at(n));
		bool isProfLocked=isProfileLocked(settingsList.at(n));

		if(!isProfLocked&&lastProfileIndex==-1&&lastProfile==settingsList.at(n))lastProfileIndex=n;
		if(firstUnlockedProfileIndex==-1&&!isProfLocked)firstUnlockedProfileIndex=n-1;
	}

    if(iniNames.count())
    {
        Q_FOREACH(QString scriptFolderName,iniNames)
        {
            scriptFolderName=baseValues.scriptFolder+scriptFolderName+"/";
            QDir().mkpath(scriptFolderName);
            Q_FOREACH(QString curScript,scriptsOldPlace)
                QFile::copy(baseValues.scriptFolder+curScript,scriptFolderName+curScript);
        }
        Q_FOREACH(QString curScript, scriptsOldPlace)
            QFile::remove(baseValues.scriptFolder+curScript);
    }

	if(ui.profileComboBox->count()==0)ui.profileComboBox->addItem(julyTr("DEFAULT_PROFILE_NAME","Default Profile"));
	if(firstUnlockedProfileIndex!=-1&&lastProfileIndex==-1)lastProfileIndex=firstUnlockedProfileIndex;
    if(lastProfileIndex>-1)ui.profileComboBox->setCurrentIndex(lastProfileIndex);

    ui.label_info->setText("Version: "+baseValues.appVerStr);

	julyTranslator.translateUi(this);

	foreach(QCheckBox* checkBoxes, findChildren<QCheckBox*>())
        checkBoxes->setMinimumWidth(qMin(checkBoxes->maximumWidth(),textFontWidth(checkBoxes->text())+20));


	QLayout *groupboxLayout=ui.LogoGroupBox->layout();
	if(groupboxLayout==0)
	{
		groupboxLayout=new QGridLayout;
		groupboxLayout->setContentsMargins(0,0,0,0);
		groupboxLayout->setSpacing(0);
		ui.LogoGroupBox->setLayout(groupboxLayout);
		LogoButton *logoButton=new LogoButton;
		groupboxLayout->addWidget(logoButton);
	}

    QSize minSizeHint=minimumSizeHint();
    if(mainWindow.isValidSize(&minSizeHint))setFixedSize(minimumSizeHint());

    if(settings.value("HidePasswordDescription",false).toBool())
        ui.descriptionGroupBox->setChecked(false);

    connect(TimeSync::global(),SIGNAL(warningMessage(QString)),this,SLOT(showTimeMessage(QString)));
    TimeSync::syncNow();
}

PasswordDialog::~PasswordDialog()
{
}

QString PasswordDialog::lockFilePath(QString name)
{
    return baseValues.tempLocation+"/QtBitcoinTrader_lock_"+QString(QCryptographicHash::hash(QString(appDataDir+"/"+QFileInfo(name).fileName()).toUtf8(),QCryptographicHash::Sha1).toHex());
}

bool PasswordDialog::isProfileLocked(QString name)
{
	QString lockFileP=lockFilePath(name);

#ifdef Q_OS_WIN
	if(QFile::exists(lockFileP))QFile::remove(lockFileP);
#endif
	return QFile::exists(lockFileP);
}

void PasswordDialog::accept()
{
	QSettings settings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
	int currIndex=ui.profileComboBox->currentIndex();
	if(currIndex>=0)settings.setValue("LastProfile",ui.profileComboBox->itemData(currIndex).toString());
	QDialog::accept();
}

QString PasswordDialog::getIniFilePath()
{
	int currIndex=ui.profileComboBox->currentIndex();
	if(currIndex==-1)return appDataDir+"QtBitcoinTrader.ini";
	return appDataDir+ui.profileComboBox->itemData(currIndex).toString();
}

void PasswordDialog::addNewProfile()
{
	newProfile=true;
	accept();
}

QString PasswordDialog::getPassword()
{
	return ui.passwordEdit->text();
}

void PasswordDialog::resetDataSlot()
{
	QMessageBox msgBox(this);
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setWindowTitle(windowTitle());
	msgBox.setText(julyTr("CONFIRM_DELETE_PROFILE","Are you sure to delete \"%1\" profile?").arg(ui.profileComboBox->currentText()));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	msgBox.setButtonText(QMessageBox::Yes,julyTr("YES","Yes"));
	msgBox.setButtonText(QMessageBox::No,julyTr("NO","No"));
	if(msgBox.exec()!=QMessageBox::Yes)return;

	resetData=true;
	accept();
}

void PasswordDialog::checkToEnableButton(QString pass)
{
	ui.okButton->setEnabled(pass.length());
}

void PasswordDialog::on_descriptionGroupBox_toggled(bool)
{
    ui.descriptionGroupBox->setVisible(false);

    QSettings settings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
    settings.setValue("HidePasswordDescription",true);

    QSize minSizeHint=minimumSizeHint();
    setFixedHeight(minSizeHint.height());
}

void PasswordDialog::showTimeMessage(QString message)
{
    QMessageBox::warning(this,julyTr("TIME_ERROR","Time error"),message);
}

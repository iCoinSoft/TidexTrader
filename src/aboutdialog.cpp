#include "aboutdialog.h"
#include "main.h"
#include "translationdialog.h"
#include "logobutton.h"

TranslationAbout::TranslationAbout(QWidget *par)
	: QDialog()
{
//	ui.setupUi(this);
//    //ui.buttonCheckUpdates->setVisible(false);
//    setWindowTitle("TidexTrader v"+baseValues.appVerStr);
//	setWindowFlags(Qt::WindowCloseButtonHint|par->windowFlags());
//	setWindowModality(Qt::ApplicationModal);
//	setAttribute(Qt::WA_DeleteOnClose,true);
//	//setFixedSize(size());
//	ui.aboutTextLabel->setStyleSheet("QLabel {color: "+baseValues.appTheme.black.name()+"; border: 1px solid "+baseValues.appTheme.gray.name()+"; background: "+baseValues.appTheme.white.name()+"; padding:6px}");
//	ui.translationAuthor->setStyleSheet(ui.aboutTextLabel->styleSheet());

//	ui.label_info->setText("Centrabit AG, Zug\nreg. CHE-114.254.375\nVersion: "+baseValues.appVerStr);

//	QLayout *groupboxLayout=ui.LogoGroupBox->layout();
//	if(groupboxLayout==0)
//	{
//		groupboxLayout=new QGridLayout;
//		groupboxLayout->setContentsMargins(0,0,0,0);
//		groupboxLayout->setSpacing(0);
//		ui.LogoGroupBox->setLayout(groupboxLayout);
//		LogoButton *logoButton=new LogoButton;
//		groupboxLayout->addWidget(logoButton);
//    }
//#ifdef Q_OS_WIN
//    resize(420,height());
//    resize(width(),minimumSizeHint().height()+20);
//#else
//    resize(500,height());
//    resize(width(),minimumSizeHint().height()+60);
//#endif
//	setMinimumSize(size());
}

TranslationAbout::~TranslationAbout()
{
}

void TranslationAbout::showWindow()
{
//    julyTranslator.translateUi(this);
//    setWindowTitle("TidexTrader v"+baseValues.appVerStr);
//	ui.languageField->setText(julyTr("LANGUAGE_NAME","Invalid Language"));
//	ui.translationAuthor->setText(julyTr("LANGUAGE_AUTHOR","Invalid About"));
//    ui.aboutBitcoinTraderGroupBox->setTitle(julyTr("ABOUT_QT_BITCOIN_TRADER","About %1").arg("TidexTrader"));
//	ui.aboutTextLabel->setText(julyTr("ABOUT_QT_BITCOIN_TRADER_TEXT","TidexTrader is a free Open Source project<br>developed on C++ Qt and OpenSSL.<br>If you want to help make project better please donate.<br>Feel free to send me recommendations and fixes to: %1").arg("<a href=\"mailto:julyighor@gmail.com\">julyighor@gmail.com</a>"));
//    show();
}

void TranslationAbout::createTranslation()
{
	accept();
	TranslationDialog *translationDialog=new TranslationDialog;
	translationDialog->setWindowFlags(windowFlags());
	translationDialog->show();
}

void TranslationAbout::buttonCheckUpdates()
{
	mainWindow.checkUpdate();
}

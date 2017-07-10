

#include "featuredexchangesdialog.h"
#include <QSettings>
#include <QtCore/qmath.h>
#include <QLabel>
#include "exchangebutton.h"
#include "main.h"
#include "julyhttp.h"
#include <QTimer>
#include "ui_featuredexchangesdialog.h"

FeaturedExchangesDialog::FeaturedExchangesDialog() :
    QDialog(),
    ui(new Ui::FeaturedExchangesDialog),
    exchangeNum(-1)
{
    ui->setupUi(this);
    ui->okButton->setEnabled(false);
	setWindowFlags(Qt::WindowCloseButtonHint);
    setWindowTitle("TidexTrader v"+baseValues.appVerStr+" - "+julyTr("FEATURED_EXCHANGES","Featured Exchanges"));

	QSettings listSettings(":/Resources/Exchanges/List.ini",QSettings::IniFormat);
	allExchangesList=listSettings.childGroups();

    {
    JulyHttp *httpGet=new JulyHttp("qbtapi.centrabit.com",0,this,true,false);
    connect(httpGet,SIGNAL(dataReceived(QByteArray,int)),this,SLOT(dataReceived(QByteArray,int)));
    httpGet->noReconnect=true;
    httpGet->sendData(145,"GET /?Object=General&Method=FeaturedExchanges");

	QElapsedTimer elapsedRequest;
	elapsedRequest.restart();
    int counter=0;
    while(cacheData.isEmpty()&&counter++<30&&elapsedRequest.elapsed()<3000)
    {
        QEventLoop loop;
        QTimer::singleShot(100,&loop,SLOT(quit()));
        loop.exec();
	}
    delete httpGet;
    featuredExchangesList=QString(mainWindow.getMidData("Exchanges\":[","]",&cacheData)).split(",");

    cacheData.clear();
    }

    for(int n=featuredExchangesList.count()-1;n>=0;n--)
    {
        if(featuredExchangesList.at(n).isEmpty())featuredExchangesList.removeAt(n);
    }
    if(featuredExchangesList.isEmpty())
    {
        QSettings settings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
        featuredExchangesList=settings.value("LastFeaturedExchanges",featuredExchangesList).toStringList();
    }
    else
    {
        QSettings settings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
        settings.setValue("LastFeaturedExchanges",featuredExchangesList);
    }

    for(int n=featuredExchangesList.count()-1;n>=0;n--)
    {
        if(featuredExchangesList.at(n).isEmpty())featuredExchangesList.removeAt(n);
        else
        if(featuredExchangesList.at(n).length()<2)featuredExchangesList[n].prepend("0");
    }

    if(featuredExchangesList.count()==0)
    {
        exchangeNum=-2;
        return;
    }

    removeNotValidExchanges();

	quint32 countCol=3;
	if(featuredExchangesList.count()<=4)countCol=2;
	for(int i=0;i<featuredExchangesList.count();i++)
	{
		QString currentName=listSettings.value(featuredExchangesList.at(i)+"/Name").toString();
		QString currentLogo=fixLogo(listSettings.value(featuredExchangesList.at(i)+"/Logo").toString());
        QString currentURL =fixURL(listSettings.value(featuredExchangesList.at(i)+"/GetApiUrl").toString());
		if(currentName.isEmpty()||currentLogo.isEmpty()||currentURL.isEmpty())continue;

		ExchangeButton *exchangeItem = new ExchangeButton(currentLogo,loadCurrencies(currentName),currentURL,featuredExchangesListIndex.at(i),this);

		if(countCol==3 && i==featuredExchangesList.count()-1 && (i-qFloor(i/countCol)*countCol)==0)i++;
        ui->gridLayoutExchange->addWidget(exchangeItem,qFloor(i/countCol),i-qFloor(i/countCol)*countCol,Qt::AlignCenter);
	}

    julyTranslator.translateUi(this);

    QSize minSizeHint=minimumSizeHint();
    if(mainWindow.isValidSize(&minSizeHint))setFixedSize(minSizeHint);
}

FeaturedExchangesDialog::~FeaturedExchangesDialog()
{

}

void FeaturedExchangesDialog::dataReceived(QByteArray data,int)
{
    cacheData=data;
}

void FeaturedExchangesDialog::removeNotValidExchanges()
{
	bool isValid;
	for(int i=0;i<featuredExchangesList.count();i++)
	{
		isValid=false;
		for(int j=0;j<allExchangesList.count();j++)
		{
			if(featuredExchangesList.at(i)==allExchangesList.at(j))
			{
				featuredExchangesListIndex.insert(i,j);
				isValid=true;
				break;
			}
		}
		if(!isValid)
		{
			featuredExchangesList.removeAt(i);
			i--;
		}
	}
}

QString FeaturedExchangesDialog::loadCurrencies(QString name)
{
    QString nameIni = name.remove(" ").remove("-").remove(".");
	QSettings listSettings(":/Resources/Exchanges/"+nameIni+".ini",QSettings::IniFormat);
	QStringList exchangesList=listSettings.childGroups();
	QString currencies="";
	QString currencies12, currencies1, currencies2;
	for(int n=0;n<exchangesList.count();n++){
		currencies12=listSettings.value(exchangesList.at(n)+"/Symbol").toString();
		currencies1=currencies12.left(3);
        currencies2=currencies12.right(3);
		if(!currencies.contains(currencies1))currencies+=currencies1+", ";
		if(!currencies.contains(currencies2))currencies+=currencies2+", ";
	}
    currencies.chop(2);
	return currencies;
}

void FeaturedExchangesDialog::selectExchange(qint32 num)
{
	exchangeNum=num;
    ui->okButton->setEnabled(true);
}

QString FeaturedExchangesDialog::fixLogo(QString currentLogo)
{
	qint32 dotPosition=currentLogo.lastIndexOf(".");
	if(dotPosition>=0)return currentLogo.insert(dotPosition,"_Big");
	return currentLogo;
}

QString FeaturedExchangesDialog::fixURL(QString currentURL)
{
	quint32 slashNum=0;
	for(qint32 i=0; i<currentURL.length();i++){
		if(currentURL[i]=='/')slashNum++;
		if(slashNum>2){
			currentURL.chop(currentURL.length()-i);
			break;
		}
	}
	return currentURL;
}

void FeaturedExchangesDialog::on_okButton_clicked()
{
	accept();
}

void FeaturedExchangesDialog::on_otherExchangesButton_clicked()
{
	exchangeNum=-2;
	accept();
}

void FeaturedExchangesDialog::on_groupBoxExchange_clicked()
{

}

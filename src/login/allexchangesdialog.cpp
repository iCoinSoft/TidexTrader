

#include "allexchangesdialog.h"
#include <QSettings>
#include "main.h"

AllExchangesDialog::AllExchangesDialog()
	: QDialog()
{
    startIndex=1;
	exchangeNum=-1;
	ui.setupUi(this);
	ui.okButton->setEnabled(false);
	setWindowFlags(Qt::WindowCloseButtonHint);
    setWindowTitle("TidexTrader v"+baseValues.appVerStr+" - "+julyTr("ALL_EXCHANGES","All Exchanges"));

	QSettings listSettings(":/Resources/Exchanges/List.ini",QSettings::IniFormat);
	QStringList exchangesList=listSettings.childGroups();

	allExchangesModel=new AllExchangesModel;
	ui.exchangesTableView->setModel(allExchangesModel);

    for(int n=startIndex;n<exchangesList.count();n++)
	{
		QString currentName=listSettings.value(exchangesList.at(n)+"/Name").toString();
		if(currentName.isEmpty())continue;
		allExchangesModel->addExchange(n, currentName, loadCurrencies(currentName));
	}

    mainWindow.setColumnResizeMode(ui.exchangesTableView,0,QHeaderView::ResizeToContents);
    mainWindow.setColumnResizeMode(ui.exchangesTableView,1,QHeaderView::Stretch);

	connect(ui.exchangesTableView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),this,SLOT(selectionExchange()));

    julyTranslator.translateUi(this);

	mainWindow.fixTableViews(this);
}

AllExchangesDialog::~AllExchangesDialog()
{
    if(allExchangesModel)delete allExchangesModel;
}

void AllExchangesDialog::on_okButton_clicked()
{
	accept();
}

void AllExchangesDialog::on_backButton_clicked()
{
	exchangeNum=-2;
	accept();
}

void AllExchangesDialog::on_exchangesTableView_doubleClicked()
{
	selectionExchange();
	accept();
}

QString AllExchangesDialog::loadCurrencies(QString name)
{
    QString nameIni=name.remove(" ").remove("-").remove(".");
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

void AllExchangesDialog::selectionExchange()
{
	QModelIndexList selectedRows=ui.exchangesTableView->selectionModel()->selectedRows();
	if(selectedRows.count()==0)return;
	int curRow=selectedRows.first().row();
	if(curRow<0)return;
    exchangeNum=curRow+startIndex;
	ui.okButton->setEnabled(true);
}



#include "networkmenu.h"
#include "ui_networkmenu.h"
#include "main.h"

NetworkMenu::NetworkMenu(QToolButton *_parentButton) :
    JulyButtonMenu(_parentButton,JulyButtonMenu::Right),
    ui(new Ui::NetworkMenu)
{
    ui->setupUi(this);
    julyTranslator.translateUi(this);
}

NetworkMenu::~NetworkMenu()
{
    delete ui;
}

void NetworkMenu::setSuffix(QString suf)
{
    ui->trafficTotal->setSuffix(suf);
}

int NetworkMenu::getNetworkTotal()
{
    return ui->trafficTotal->value();
}

int NetworkMenu::getNetworkTotalMaximum()
{
    return ui->trafficTotal->maximum();
}

void NetworkMenu::setNetworkTotalMaximum(int val)
{
    ui->trafficTotal->setMaximum(val);
}

void NetworkMenu::setNetworkTotal(int val)
{
    ui->trafficTotal->setValue(val);
}

void NetworkMenu::on_trafficTotalToZero_clicked()
{
    emit trafficTotalToZero_clicked();
}


#ifndef NETWORKMENU_H
#define NETWORKMENU_H

#include "julybuttonmenu.h"

namespace Ui {
class NetworkMenu;
}

class NetworkMenu : public JulyButtonMenu
{
    Q_OBJECT
public:
    explicit NetworkMenu(QToolButton *parentButton);
    ~NetworkMenu();
    int getNetworkTotal();
    void setNetworkTotal(int);
    int getNetworkTotalMaximum();
    void setNetworkTotalMaximum(int);
    void setSuffix(QString);
private:
    Ui::NetworkMenu *ui;
private slots:
    void on_trafficTotalToZero_clicked();
signals:
    void trafficTotalToZero_clicked();
};

#endif // NETWORKMENU_H

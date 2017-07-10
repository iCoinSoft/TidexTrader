

#include "orderstablecancelbutton.h"
#include <QPushButton>
#include "main.h"
#include <QTableView>

OrdersTableCancelButton::OrdersTableCancelButton(QObject *parent)
	: QItemDelegate(parent)
{
	parentTable=dynamic_cast<QTableView *>(parent);
}

OrdersTableCancelButton::~OrdersTableCancelButton()
{

}

void OrdersTableCancelButton::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(parentTable&&!mainWindow.currentlyAddingOrders)
	{
		QPushButton* buttonCancel=dynamic_cast<QPushButton*>(parentTable->indexWidget(index));
		if(!buttonCancel)
		{
			buttonCancel=new QPushButton("X");
			buttonCancel->setFixedSize(defaultHeightForRow,defaultHeightForRow);
			connect(buttonCancel,SIGNAL(clicked()),baseValues.mainWindow_,SLOT(cancelOrderByXButton()));
			parentTable->setIndexWidget(index, buttonCancel);
        }
        buttonCancel->setProperty("OrderId",index.data(Qt::UserRole).toString());
        buttonCancel->setProperty("Symbol",index.data(Qt::AccessibleTextRole).toString());
	}
	QItemDelegate::paint(painter, option, index);
}

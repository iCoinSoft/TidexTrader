

#include "settingsdialoglistelement.h"

SettingsDialogListElement::SettingsDialogListElement(SettingsDialog *tempParent, qint32 tempIndex, QString name, QString icon)
    : QWidget()
{
    ui.setupUi(this);
    ui.textListLabel->setText(name);
    ui.iconListLabel->setPixmap(QPixmap(":/Resources/"+icon));
    setCursor(Qt::PointingHandCursor);

    parent=tempParent;
    index=tempIndex;

    QFontMetrics fontMetrics(ui.textListLabel->font());
    width=fontMetrics.width(name)+ui.iconListLabel->pixmap()->width()+17;
    setFixedHeight(qMax(fontMetrics.height(),ui.iconListLabel->pixmap()->height())+10);
}

SettingsDialogListElement::~SettingsDialogListElement()
{
}

void SettingsDialogListElement::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    if(event->button()==Qt::LeftButton)
    {
        parent->clickOnList(index);
    }
}

void SettingsDialogListElement::selectedItem()
{
    setStyleSheet("background:blue;color:white");
}

void SettingsDialogListElement::clearSelection()
{
    setStyleSheet("");
}

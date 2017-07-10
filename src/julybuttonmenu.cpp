

#include "julybuttonmenu.h"
#include <QToolButton>
#include <QMouseEvent>
#include <QCursor>

JulyButtonMenu::JulyButtonMenu(QToolButton *_parentButton, Align _position) :
    QMenu()
{
    position=_position;
    parentButton=_parentButton;
    widgetUnderButton=parentButton->parentWidget();
    parentWindow=widgetUnderButton;
    while(parentWindow->parentWidget())
        parentWindow=parentWindow->parentWidget();

    parentButton->setCheckable(true);
    connect(this,SIGNAL(setCheckedButton(bool)),parentButton,SLOT(setChecked(bool)));
    connect(this,SIGNAL(aboutToHide()),this,SLOT(aboutToHideWindow()));
    connect(parentButton,SIGNAL(clicked()),this,SLOT(displayMenuClicked()));
}

void JulyButtonMenu::displayMenuClicked()
{
    resize(minimumSizeHint());
    QPoint pointToShow;
    if(widgetUnderButton==0)pointToShow=QCursor::pos();
    else
    {
    if(position==Left)
        pointToShow=parentButton->geometry().bottomLeft();
    else
    {
        pointToShow=parentButton->geometry().bottomRight();
        pointToShow.setX(pointToShow.x()-this->width());
    }

    pointToShow=widgetUnderButton->mapToGlobal(pointToShow);
    show();
    exec(pointToShow);return;
    if(pointToShow.x()<parentWindow->geometry().x())pointToShow.setX(parentWindow->geometry().x());
    if(pointToShow.y()<parentWindow->geometry().y())pointToShow.setY(parentWindow->geometry().y());
    if(pointToShow.x()>parentWindow->geometry().right()-this->width())pointToShow.setX(parentWindow->geometry().right()-this->width());
    if(pointToShow.y()>parentWindow->geometry().bottom()-this->height())pointToShow.setY(parentWindow->geometry().bottom()-this->height());
    }
    exec(pointToShow);
}

void JulyButtonMenu::aboutToHideWindow()
{
    emit setCheckedButton(false);
}

void JulyButtonMenu::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

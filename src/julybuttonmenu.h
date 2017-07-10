

#ifndef JULYBUTTONMENU_H
#define JULYBUTTONMENU_H

#include <QMenu>
class QToolButton;
class QMouseEvent;

class JulyButtonMenu : public QMenu
{
    Q_OBJECT
    Q_ENUMS(Align)
public:
    enum Align
    {
       Left,
       Right
    };

    explicit JulyButtonMenu(QToolButton *parentButton, Align position=Left);
private:
    Align position;
    void mouseReleaseEvent(QMouseEvent *);
    QToolButton *parentButton;
    QWidget *widgetUnderButton;
    QWidget *parentWindow;
private slots:
    void displayMenuClicked();
    void aboutToHideWindow();
signals:
    void setCheckedButton(bool);
};

#endif // JULYBUTTONMENU_H

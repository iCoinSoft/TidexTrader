

#include "utils.h"

#include <QFileInfo>
#include <QDir>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>

QString changeFileExt(const QString& fileName, const QString& ext)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.path() + "/" + fileInfo.baseName()  + ext;
}

QString adjustPathSeparators(const QString& path)
{
    return QDir::fromNativeSeparators(path);
}

QString slash(const QString& path1, const QString& path2)
{
    QString s1 = path1;
    QString s2 = path2;
    while (s1.endsWith('/') || s1.endsWith('\\')) {
        s1.truncate(s1.length() - 1);
    }
    while (s2.startsWith('/') || s2.startsWith('\\')) {
        s1.remove(0, 1);
    }
    return s1 + "/" + s2;
}

QString slash(const QString& path1, const QString& path2, const QString& path3)
{
    return slash(slash(path1, path2), path3);
}

void adjustWidgetGeometry(QWidget* widget)
{
    QRect workarea = QApplication::desktop()->availableGeometry(widget);
    QRect bounds = widget->frameGeometry();
    int delta;
    if ((delta = (workarea.right() - bounds.right())) < 0) {
        bounds.translate(delta, 0);
    }
    if ((delta = (workarea.bottom() - bounds.bottom())) < 0) {
        bounds.translate(0, delta);
    }
    if ((delta = (workarea.left() - bounds.left())) > 0) {
        bounds.translate(delta, 0);
    }
    if ((delta = (workarea.top() - bounds.top())) > 0) {
        bounds.translate(0, delta);
    }
    widget->move(bounds.topLeft());
}

void recursiveUpdateLayouts(const QObject *object)
{
    const QWidget *widget = qobject_cast<const QWidget *>(object);
    if (widget->layout())
    {
        if(widget->layout()->spacing()>0)widget->layout()->setSpacing(widget->layout()->spacing()/2);
        if(widget->layout()->margin()>0)widget->layout()->setMargin(3);
        if(widget->layout()->margin()==-1)widget->layout()->setContentsMargins(3,5,3,3);
    }

    QObjectList children = object->children();
    foreach (const QObject *child, children)
    {
        const QWidget *widget = qobject_cast<const QWidget *>(child);
        if (widget)recursiveUpdateLayouts(widget);
    }
}

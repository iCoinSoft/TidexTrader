#ifndef DEBUGVIEWER_H
#define DEBUGVIEWER_H

#include <QWidget>
#include "ui_debugviewer.h"

class DebugViewer : public QWidget
{
	Q_OBJECT

public:
	DebugViewer();
	~DebugViewer();

private:
    bool savingFile;
	Ui::DebugViewer ui;
private slots:
    void on_buttonSaveAs_clicked();
	void sendLogSlot(QByteArray);
	void on_radioDebug_toggled(bool);
};

#endif // DEBUGVIEWER_H



#ifndef JULYSCROLLUPONIDLE_H
#define JULYSCROLLUPONIDLE_H

#include <QObject>
#include <QScrollBar>
#include <QTimer>

class JulyScrollUpOnIdle : public QObject
{
	Q_OBJECT

public:
	JulyScrollUpOnIdle(QScrollBar *parent);
	~JulyScrollUpOnIdle();

private:
	QTimer *idleTimer;
	QScrollBar *scrollBar;
private slots:
	void timeOut();
	void valueChanged(int);

};

#endif // JULYSCROLLUPONIDLE_H

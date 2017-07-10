

#include "julyscrolluponidle.h"

JulyScrollUpOnIdle::JulyScrollUpOnIdle(QScrollBar *parent)
	: QObject(parent)
{
	scrollBar=parent;
	idleTimer=new QTimer(this);
	connect(idleTimer,SIGNAL(timeout()),this,SLOT(timeOut()));
	connect(scrollBar,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int)));
}

JulyScrollUpOnIdle::~JulyScrollUpOnIdle()
{

}

void JulyScrollUpOnIdle::timeOut()
{
	scrollBar->setValue(0);
}

void JulyScrollUpOnIdle::valueChanged(int val)
{
	if(val>0)idleTimer->start(30000);
	else idleTimer->stop();
}

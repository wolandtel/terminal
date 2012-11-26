#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <QtGlobal>
#include <QDateTime>

/*
#define DBG_ ""
*/

#define dbgBase QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz] ") + QString(__FILE__) + " (" + QString::number(__LINE__) + ")"
#define dbg qDebug() << dbgBase

#endif // DEBUG_H

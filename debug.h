#ifndef DEBUG_H
#define DEBUG_H

#include <QtGlobal>
#include <QDateTime>

#define DBG_CONFIG "/home/fox/proj/terminal/terminal.conf"
/*
#define DBG_ ""
*/

#define dbgBase() QDateTime::currentDateTime().toString("[yyyy-MM-dd hh-mm-ss.zzz] ") + QString(__FILE__) + " (" + QString::number(__LINE__) + ")"
#define dbg() qDebug() << dbgBase()
#define debug(str) qDebug() << dbgBase() + ": " + str;

#endif // DEBUG_H

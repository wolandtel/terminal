#include <QApplication>
#include "mainwindow.h"
#include "pindialog.h"
#include "cardreader.h"
#include "terminal.h"
#include "debug.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QPalette p = a.palette();
	p.setColor(QPalette::Window, "white");
	p.setColor(QPalette::Active, QPalette::Button, *(new QColor(223, 128, 38)));
	p.setColor(QPalette::Disabled, QPalette::Button, *(new QColor(225, 225, 225)));
	p.setColor(QPalette::ButtonText, "white");
	a.setPalette(p);
	MainWindow w;
	PinDialog pd(&w);
	w.showFullScreen();
	
	Cardreader c(DBG_TTY);
	
	QObject::connect(&c, SIGNAL(initSucceeded()), &w, SLOT(displayReady()));
	QObject::connect(&c, SIGNAL(initFailed()), &w, SLOT(displayError()));
	QObject::connect(&c, SIGNAL(cardEject(bool)), &w, SLOT(ejectCard(bool)));
	QObject::connect(&c, SIGNAL(cardEjected()), &w, SLOT(displayReady()));
	
	QObject::connect(&c, SIGNAL(cardInserted()), &pd, SLOT(open()));
	QObject::connect(&pd, SIGNAL(rejected()), &c, SLOT(ejectCard()));
	QObject::connect(&pd, SIGNAL(accepted()), &c, SLOT(ejectCard()));
	
	Terminal t(&c, &pd, DBG_TERMINAL_ID, DBG_SECRET, DBG_URL, DBG_CA);
	
	c.init();
	
	return a.exec();
}

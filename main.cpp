#include <QApplication>
#include "mainwindow.h"
#include "pindialog.h"
#include "cardreader.h"

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
	w.showFullScreen();
	
	Cardreader c;
	
	QObject::connect(&c, SIGNAL(initSucceeded()), &w, SLOT(displayReady()));
	QObject::connect(&c, SIGNAL(initFailed()), &w, SLOT(displayError()));
	QObject::connect(&c, SIGNAL(cardInserted()), &(w.pd), SLOT(open()));
	QObject::connect(&c, SIGNAL(cardEject(bool)), &w, SLOT(ejectCard(bool)));
	QObject::connect(&c, SIGNAL(cardEjected()), &w, SLOT(displayReady()));
	QObject::connect(&(w.pd), SIGNAL(rejected()), &c, SLOT(ejectCard()));
	QObject::connect(&(w.pd), SIGNAL(accepted()), &c, SLOT(ejectCard()));
	
	c.init();
	
	return a.exec();
}

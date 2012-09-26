#include <QApplication>
#include "mainwindow.h"
#include "pindialog.h"
#include "cardreader.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QPalette p = a.palette();
	p.setColor(QPalette::Window, "white");
	p.setColor(QPalette::Button, *(new QColor(223, 128, 38)));
	p.setColor(QPalette::ButtonText, "white");
	a.setPalette(p);
	MainWindow w;
	w.showFullScreen();
	
	Cardreader c;
	
	QObject::connect(&c, SIGNAL(initSucceeded()), &w, SLOT(displayReady()));
	QObject::connect(&c, SIGNAL(initFailed()), &w, SLOT(displayError()));
	QObject::connect(&c, SIGNAL(hasCard()), &w, SLOT(getPin()));
	
	c.init();
	
	return a.exec();
}

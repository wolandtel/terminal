#include <QApplication>
#include "debug.h"
#include "config.h"
#include "jconfig.h"
#include "terminal.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	JConfig conf(argc > 1 ? QString::fromUtf8(argv[1]) : DEF_CONFIG);
	if (conf.error() != JConfig::ErrorNone)
		return 1;
	
	QPalette p = a.palette();
	p.setColor(QPalette::Window, "white");
	p.setColor(QPalette::Active, QPalette::Button, *(new QColor(223, 128, 38)));
	p.setColor(QPalette::Disabled, QPalette::Button, *(new QColor(225, 225, 225)));
	p.setColor(QPalette::ButtonText, "white");
	a.setPalette(p);
	
	Terminal t(conf);
	
	return a.exec();
}

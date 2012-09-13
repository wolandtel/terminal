#include <QApplication>
#include "mainwindow.h"

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

    return a.exec();
}

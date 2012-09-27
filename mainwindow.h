#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pindialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
		
		PinDialog pd;

	public slots:
		void displayReady();
		void displayError();
		void getPin();
		void ejectCard(bool);
	
	private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

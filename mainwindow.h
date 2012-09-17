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

		void displayReady();
		void displayError();

	private:
    Ui::MainWindow *ui;
		PinDialog m_pd;
};

#endif // MAINWINDOW_H

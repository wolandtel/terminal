#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
		
	signals:
#ifdef DEBUG
		void debugDialog();
#endif
		
	public slots:
		void displayReady();
		void displayError();
		void displayWait();
		void ejectCard(bool err = false);
		void ejectCardError();
	
	private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

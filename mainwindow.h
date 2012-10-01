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
		void hasCard();
#endif
		
	public slots:
		void displayReady();
		void displayError();
		void ejectCard(bool err);
	
	private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

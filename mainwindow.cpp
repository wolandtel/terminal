#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
		connect(this->ui->CardPb, SIGNAL(clicked()), &(this->m_pd), SLOT(open()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

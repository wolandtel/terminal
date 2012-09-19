#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef DEBUG
		connect(ui->CardPb, SIGNAL(clicked()), &m_pd, SLOT(open()));
#else
		ui->CardPb->hide();
#endif
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::displayReady()
{
	ui->stateLb->setText(QString::fromUtf8("Вставьте карту"));
}

void MainWindow::displayError()
{
	ui->stateLb->setText(QString::fromUtf8("Ошибка инициализации устройства"));
}

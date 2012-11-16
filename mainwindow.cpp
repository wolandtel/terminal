#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debug.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
#ifdef DEBUG
	connect(ui->debugPb, SIGNAL(clicked()), SIGNAL(debugDialog()));
	setCursor(Qt::ArrowCursor);
#else
#endif
	ui->debugPb->hide();
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

void MainWindow::displayWait()
{
	ui->stateLb->setText(QString::fromUtf8("Подождите…"));
}

void MainWindow::ejectCard(bool err)
{
	QString text = "";
	
	if (err)
		text += QString::fromUtf8("Карта не может быть прочитана. ");
	text += QString::fromUtf8("Достаньте карту");
	
	ui->stateLb->setText(text);
}

void MainWindow::ejectCardError()
{
	ejectCard(true);
}

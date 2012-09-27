#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
		ui->setupUi(this);
#ifdef DEBUG
		connect(ui->CardPb, SIGNAL(clicked()), &pd, SLOT(open()));
		setCursor(Qt::ArrowCursor);
#endif
		ui->CardPb->hide();
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

void MainWindow::getPin()
{
	pd.open();
}

void MainWindow::ejectCard(bool err)
{
	QString text = "";
	
	if (err)
		text += QString::fromUtf8("Карта не может быть прочитана. ");
	text += QString::fromUtf8("Достаньте карту");
	
	ui->stateLb->setText(text);
}

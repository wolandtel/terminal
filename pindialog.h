#ifndef PINDIALOG_H
#define PINDIALOG_H

#include <QWidget>

namespace Ui {
	class PinDialog;
}

class PinDialog : public QWidget
{
		Q_OBJECT
		
	public:
		explicit PinDialog(QWidget *parent = 0);
		~PinDialog();
		
	public slots:
		void open();
		
	private slots:
		void on_kpBsPb_clicked();
		void on_kp0Pb_clicked();
		void on_kp1Pb_clicked();
		void on_kp2Pb_clicked();
		void on_kp3Pb_clicked();
		void on_kp4Pb_clicked();
		void on_kp5Pb_clicked();
		void on_kp6Pb_clicked();
		void on_kp7Pb_clicked();
		void on_kp8Pb_clicked();
		void on_kp9Pb_clicked();

	private:
		Ui::PinDialog *ui;
};

#endif // PINDIALOG_H

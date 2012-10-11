#ifndef PINDIALOG_H
#define PINDIALOG_H

#include <QDialog>

namespace Ui {
	class PinDialog;
}

class PinDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit PinDialog(QWidget *parent = 0);
		~PinDialog();
		
		QString pin();
		
	public slots:
		void open();
	
	signals:
		void gotPin();
		
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
		
		void on_pinLe_textChanged(const QString &arg1);
		
		void on_OkPb_clicked();
		
	private:
		Ui::PinDialog *ui;
};

#endif // PINDIALOG_H

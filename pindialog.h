#ifndef PINDIALOG_H
#define PINDIALOG_H

#include <QDialog>
#include "jconfig.h"

#define PIN_DEF_LEN 4
#define PIN_DEF_ECHO '*'

namespace Ui {
	class PinDialog;
}

class PinDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit PinDialog(const JConfig &conf, QWidget *parent = 0);
		~PinDialog();
		
		inline QString pin() { return m_pin; }
		
	public slots:
		void open();
	
	signals:
		void gotPin();
		
	private slots:
		void on_kpBsPb_clicked();
		void on_okPb_clicked();
		
		void num();
		void refresh();
		
		void on_kpCancelPb_clicked();
		
	private:
		Ui::PinDialog *ui;
		QString m_pin;
		QChar m_echo;
		int m_len;
		
		void pinChanged();
};

#endif // PINDIALOG_H

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
		
	private:
		Ui::PinDialog *ui;
};

#endif // PINDIALOG_H

#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>

namespace Ui
{
class waitingDialog;
}

class waitingDialog : public QDialog
{
  Q_OBJECT

public:
  explicit waitingDialog(QWidget *parent = 0);
  ~waitingDialog();

signals:
  void canceled();

private:
  Ui::waitingDialog *ui;
};

#endif // WAITINGDIALOG_H

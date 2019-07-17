#ifndef INFORMDIALOG_H
#define INFORMDIALOG_H

#include <QDialog>

namespace Ui
{
class informDialog;
}

class informDialog : public QDialog
{
  Q_OBJECT

public:
  explicit informDialog(QWidget *parent = 0);
  ~informDialog();
  int getPort();
  void hideaddress();
  QString getAddress();
  bool isIpLegal(QString str);

private slots:
  void on_buttonBox_accepted();

private:
  Ui::informDialog *ui;
};

#endif // INFORMDIALOG_H

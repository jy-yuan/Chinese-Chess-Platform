#include "waitingdialog.h"
#include "ui_waitingdialog.h"

waitingDialog::waitingDialog(QWidget *parent) : QDialog(parent),
                                                ui(new Ui::waitingDialog)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton, SIGNAL(clicked(bool)), this, SIGNAL(canceled()));
}

waitingDialog::~waitingDialog()
{
    delete ui;
}

#include "informdialog.h"
#include "ui_informdialog.h"

#include <QMessageBox>

informDialog::informDialog(QWidget *parent) : QDialog(parent),
                                              ui(new Ui::informDialog)
{
    ui->setupUi(this);
}

informDialog::~informDialog()
{
    delete ui;
}

int informDialog::getPort()
{
    QString b = ui->lineEdit_port->text();
    int a = b.toInt();
    return a;
}

void informDialog::hideaddress()
{
    ui->label_address->setHidden(true);
    ui->lineEdit_address->setHidden(true);
}

QString informDialog::getAddress()
{
    return ui->lineEdit_address->text();
}

bool informDialog::isIpLegal(QString str)
{
    if (str.length() < 7 || str.length() > 15)
    {
        return false;
    }
    QStringList arr;
    arr = str.split(".");
    if (arr.length() != 4)
    {
        return false;
    }
    for (int i = 0; i < arr.length(); i++)
    {
        QString a = arr.at(i);
        if (a.toInt() < 0 || a.toInt() > 255)
        {
            return false;
        }
    }
    return true;
}

void informDialog::on_buttonBox_accepted()
{
    if (isIpLegal(getAddress()))
    {
        accept();
    }
    else
    {
        QMessageBox::warning(this, "warning", "this IP address is illegal");
    }
}

#include "enddialog.h"
#include "ui_enddialog.h"

enddialog::enddialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::enddialog)
{
    ui->setupUi(this);
    this->setWindowTitle(" ");
}

enddialog::~enddialog()
{
    delete ui;
}

void enddialog::settext(QString a)
{
    ui->label->setText(a);
}

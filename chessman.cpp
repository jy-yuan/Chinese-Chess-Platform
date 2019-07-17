#include "chessman.h"
#include "ui_chessman.h"

Chessman::Chessman(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Chessman)
{
    ui->setupUi(this);
}

Chessman::~Chessman()
{
    delete ui;
}

void Chessman::paintEvent(QPaintEvent *ev)
{
}

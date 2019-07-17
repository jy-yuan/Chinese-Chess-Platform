#ifndef CHESSMAN_H
#define CHESSMAN_H

#include <QWidget>

namespace Ui
{
class Chessman;
}

class Chessman : public QWidget
{
    Q_OBJECT

  public:
    explicit Chessman(QWidget *parent = 0);
    ~Chessman();
    void paintEvent(QPaintEvent *ev);

    int side; //红1或黑2
    int role; //种类：无/将/士/象/马/车/炮/兵
    int rank; //编号

  private:
    Ui::Chessman *ui;
};

#endif // CHESSMAN_H

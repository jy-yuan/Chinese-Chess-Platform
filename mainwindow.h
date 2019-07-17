#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chessman.h"
#include "informdialog.h"
#include "waitingdialog.h"
#include "enddialog.h"
#include <QMainWindow>
#include <QtNetwork>
#include <QVector>
#include <QTimer>
#include <QMediaPlayer>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void paintEvent(QPaintEvent *ev);
  bool eventFilter(QObject *watched, QEvent *event);
  QVector<int> whereCanGo(int m_rank, int m_side, int m_role);
  void aMoveTob(int a, int b);
  void setChess(int m_rank, int m_side, int m_role);
  void startnewgame();
  void startcountdown();

private slots:
  void on_actionnewgame_triggered();

  void on_actionashost_triggered();

  void on_actionasclient_triggered();

  void on_actionload_triggered();

public slots:
  void acceptConnection();
  void recvMessage();
  void connectionCanceled();
  void minusone();
  void giveup();
  void savegame();

signals:
  void setlcdvalue(int);

private:
  Ui::MainWindow *ui;
  int sidelength;
  int selecting;
  bool has_selection;
  QVector<Chessman *> chess;
  int My_Side;
  int Now_Side;
  QTcpServer *listenSocket;
  QTcpSocket *readWriteSocket;
  waitingDialog *waitdiag;
  enddialog *enddial;
  QTimer *timer;
  QMediaPlayer *player;
};

#endif // MAINWINDOW_H

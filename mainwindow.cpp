#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("中国象棋对战平台");
    ui->dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    ui->dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui->dockWidget->setFixedWidth(200);
    QObject::connect(this, SIGNAL(setlcdvalue(int)), ui->lcdNumber, SLOT(display(int)));
    QObject::connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(giveup()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(savegame()));
    sidelength = 50;
    selecting = -1;
    has_selection = false;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            Chessman *temp = new Chessman;
            temp->setParent(this);
            temp->setGeometry(sidelength * j + sidelength * 5 / 8, sidelength * (10 - i) - sidelength * 3 / 8, sidelength * 3 / 4, sidelength * 3 / 4);
            temp->raise();
            temp->role = 0;
            temp->side = 0;
            temp->rank = j + i * 9;
            temp->installEventFilter(this);
            chess.append(temp);
        }
    }
    ui->menuStart->actions()[0]->setEnabled(false);
    ui->menuStart->actions()[1]->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    player = new QMediaPlayer;
    player->setMedia(QMediaContent(QUrl::fromLocalFile("/Users/yjy/Desktop/chessclient/jiang.mp3")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    QPen pen;
    pen.setWidth(3);
    p.setPen(pen);
    p.drawLine(sidelength - 5, sidelength - 5, sidelength * 9 + 5, sidelength - 5);
    p.drawLine(sidelength - 5, sidelength - 5, sidelength - 5, sidelength * 10 + 5);
    p.drawLine(sidelength - 5, sidelength * 10 + 5, sidelength * 9 + 5, sidelength * 10 + 5);
    p.drawLine(sidelength * 9 + 5, sidelength - 5, sidelength * 9 + 5, sidelength * 10 + 5);
    pen.setWidth(1);
    p.setPen(pen);
    for (int i = 1; i < 10; i++)
    {
        p.drawLine(sidelength * i, sidelength, sidelength * i, sidelength * 5);
        p.drawLine(sidelength * i, sidelength * 6, sidelength * i, sidelength * 10);
    }
    p.drawLine(sidelength, sidelength * 5, sidelength, sidelength * 6);
    p.drawLine(sidelength * 9, sidelength * 5, sidelength * 9, sidelength * 6);
    for (int i = 1; i <= 10; i++)
        p.drawLine(sidelength, sidelength * i, sidelength * 9, sidelength * i);
    p.drawLine(sidelength * 4, sidelength, sidelength * 6, sidelength * 3);
    p.drawLine(sidelength * 6, sidelength, sidelength * 4, sidelength * 3);
    p.drawLine(sidelength * 4, sidelength * 8, sidelength * 6, sidelength * 10);
    p.drawLine(sidelength * 6, sidelength * 8, sidelength * 4, sidelength * 10);
    QPointF point(sidelength * 2.7, sidelength * 5.7);
    p.setFont(QFont("宋体", 30));
    p.drawText(point, "楚河            汉界");
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        Chessman *now = static_cast<Chessman *>(watched);
        if (has_selection) //当前有选中的
        {
            QVector<int> possibleplaces = whereCanGo(chess[selecting]->rank, chess[selecting]->side, chess[selecting]->role);

            if (now->rank == selecting) //还选的那地
            {
                has_selection = false;
            }
            else
            {
                for (auto i : possibleplaces)
                {
                    if (now->rank == i) //选了能到的地
                    {
                        int win = -1;
                        if (chess[i]->role == 1)
                            win = 1;
                        aMoveTob(selecting, i);
                        for (int k = 0; k < 90; k++)
                        {
                            if (chess[k]->side == My_Side)
                            {
                                QVector<int> possiblewin = whereCanGo(k, chess[k]->side, chess[k]->role);
                                for (auto j : possiblewin)
                                {
                                    if (chess[j]->role == 1)
                                    {
                                        win = 0;
                                        break;
                                    }
                                }
                            }
                            if (win == 0)
                                break;
                        }
                        delete timer;
                        ui->pushButton->setEnabled(false);
                        QByteArray *array = new QByteArray;
                        array->clear();
                        if (win == 1)
                        { //赢
                            array->append(QString::number(30000 + 100 * (89 - selecting) + 89 - i));
                            //ui->label->setText("赢了！");
                            //ui->label->repaint();
                            ui->menuStart->actions()[0]->setEnabled(true);
                            ui->menuStart->actions()[1]->setEnabled(true);
                            ui->pushButton->setEnabled(false);
                            ui->pushButton_2->setEnabled(false);
                            enddial = new enddialog(this);
                            enddial->settext("你赢了！");
                            enddial->show();
                        }
                        else if (win == 0)
                        { //将军
                            array->append(QString::number(20000 + 100 * (89 - selecting) + 89 - i));
                            //ui->label->setText("将军！");
                            player->play();
                            //ui->label->repaint();
                        }
                        else
                        { //未将军
                            array->append(QString::number(10000 + 100 * (89 - selecting) + 89 - i));
                        }
                        readWriteSocket->write(array->data());
                        has_selection = false;
                        break;
                    }
                }
            }
        }
        else //当前没选中的
        {
            if (now->side == My_Side && now->side == Now_Side)
            {
                has_selection = true;
                selecting = now->rank;
            }
        }
        if (has_selection)
        {
            int i = selecting / 9;
            int j = selecting - 9 * i;
            chess[selecting]->setGeometry(sidelength * j + sidelength / 2, sidelength * (10 - i) - sidelength / 2, sidelength, sidelength);
            setChess(selecting, chess[selecting]->side, chess[selecting]->role);
            chess[selecting]->repaint();
        }
        else
        {
            if (selecting >= 0)
            {
                int i = selecting / 9;
                int j = selecting - 9 * i;
                chess[selecting]->setGeometry(sidelength * j + sidelength * 5 / 8, sidelength * (10 - i) - sidelength * 3 / 8, sidelength * 3 / 4, sidelength * 3 / 4);
                setChess(selecting, chess[selecting]->side, chess[selecting]->role);
                chess[selecting]->repaint();
            }
        }
        //ui->label->setText(QString::number(selecting));
        //ui->label->repaint();
        return true;
    }
    else
        return false;
}

QVector<int> MainWindow::whereCanGo(int m_rank, int m_side, int m_role)
{
    QVector<int> ans;
    int x, y;
    y = m_rank / 9;
    x = m_rank - y * 9;
    if (m_role == 1)
    {
        QVector<int> possibleij = {0, 1, 0, -1, 1, 0, -1, 0};
        for (int i = 0; i < 8; i += 2)
        {
            int a = y + 1; //对面亮将
            while (a < 10)
            {
                int sum = a * 9 + x;
                if (chess[sum]->role > 0)
                {
                    if (chess[sum]->role == 1)
                        ans.append(sum);
                    break;
                }
                a++;
            }

            int tempx = x + possibleij[i];
            int tempy = y + possibleij[i + 1];

            if (tempx < 3 || tempx > 5 || tempy < 0 || tempy > 2)
                continue;

            a = y + 1; //亮将
            bool can = true;
            while (a < 10)
            {
                int sum = a * 9 + tempx;
                if (chess[sum]->role > 0)
                {
                    if (chess[sum]->role == 1)
                        can = false;
                    break;
                }
                a++;
            }
            if (!can)
                continue;

            int s = tempy * 9 + tempx;
            if (chess[s]->side == m_side)
                continue;

            ans.append(s);
        }
    }
    else if (m_role == 2)
    {
        for (int i = -1; i < 2; i += 2)
        {
            for (int j = -1; j < 2; j += 2)
            {
                int tempx = x + i;
                int tempy = y + j;

                if (tempx < 3 || tempx > 5 || tempy < 0 || tempy > 2)
                    continue;

                int s = tempy * 9 + tempx;
                if (chess[s]->side == m_side)
                    continue;

                ans.append(s);
            }
        }
    }
    else if (m_role == 3)
    {
        for (int i = -2; i < 3; i += 4)
        {
            for (int j = -2; j < 3; j += 4)
            {
                int tempx = x + i;
                int tempy = y + j;

                if (tempx < 0 || tempx > 8 || tempy < 0 || tempy > 4)
                    continue;

                int sum = x + i / 2 + (y + j / 2) * 9; //堵象眼
                if (chess[sum]->role > 0)
                    continue;

                int s = tempy * 9 + tempx;
                if (chess[s]->side == m_side)
                    continue;

                ans.append(s);
            }
        }
    }
    else if (m_role == 4)
    {
        QVector<int> possibleij = {-2, -1, -2, 1, 2, -1, 2, 1, 1, -2, 1, 2, -1, -2, -1, 2};
        for (int i = 0; i < 16; i += 2)
        {
            int tempx = x + possibleij[i];
            int tempy = y + possibleij[i + 1];

            if (tempx < 0 || tempx > 8 || tempy < 0 || tempy > 9)
                continue;

            int sum; //别马腿
            if (i < 8)
            {
                sum = x + possibleij[i] / 2 + y * 9;
            }
            else
            {
                sum = x + (possibleij[i + 1] / 2 + y) * 9;
            }
            if (chess[sum]->role > 0)
                continue;

            int s = tempy * 9 + tempx;
            if (chess[s]->side == m_side)
                continue;

            ans.append(s);
        }
    }
    else if (m_role == 5)
    {
        int i = 1;
        while (x + i <= 8)
        {
            int s = x + i + y * 9;
            if (chess[s]->role == 0)
                ans.append(s);
            else
            {
                if (chess[s]->side != m_side)
                    ans.append(s);
                break;
            }
            i++;
        }
        i = 1;
        while (x - i >= 0)
        {
            int s = x - i + y * 9;
            if (chess[s]->role == 0)
                ans.append(s);
            else
            {
                if (chess[s]->side != m_side)
                    ans.append(s);
                break;
            }
            i++;
        }
        i = 1;
        while (y + i <= 9)
        {
            int s = x + (y + i) * 9;
            if (chess[s]->role == 0)
                ans.append(s);
            else
            {
                if (chess[s]->side != m_side)
                    ans.append(s);
                break;
            }
            i++;
        }
        i = 1;
        while (y - i >= 0)
        {
            int s = x + (y - i) * 9;
            if (chess[s]->role == 0)
                ans.append(s);
            else
            {
                if (chess[s]->side != m_side)
                    ans.append(s);
                break;
            }
            i++;
        }
    }
    else if (m_role == 6)
    {
        bool flag = true;
        int i = 1;
        while (x + i <= 8)
        {
            int s = x + i + y * 9;
            if (flag)
            {
                if (chess[s]->role == 0)
                    ans.append(s);
                else
                {
                    flag = false;
                }
            }
            else
            {
                if (chess[s]->role > 0)
                {
                    if (chess[s]->side != m_side)
                        ans.append(s);
                    break;
                }
            }
            i++;
        }
        flag = true;
        i = 1;
        while (x - i >= 0)
        {
            int s = x - i + y * 9;
            if (flag)
            {
                if (chess[s]->role == 0)
                    ans.append(s);
                else
                {
                    flag = false;
                }
            }
            else
            {
                if (chess[s]->role > 0)
                {
                    if (chess[s]->side != m_side)
                        ans.append(s);
                    break;
                }
            }
            i++;
        }
        flag = true;
        i = 1;
        while (y + i <= 9)
        {
            int s = x + (i + y) * 9;
            if (flag)
            {
                if (chess[s]->role == 0)
                    ans.append(s);
                else
                {
                    flag = false;
                }
            }
            else
            {
                if (chess[s]->role > 0)
                {
                    if (chess[s]->side != m_side)
                        ans.append(s);
                    break;
                }
            }
            i++;
        }
        flag = true;
        i = 1;
        while (y - i >= 0)
        {
            int s = x + (y - i) * 9;
            if (flag)
            {
                if (chess[s]->role == 0)
                    ans.append(s);
                else
                {
                    flag = false;
                }
            }
            else
            {
                if (chess[s]->role > 0)
                {
                    if (chess[s]->side != m_side)
                        ans.append(s);
                    break;
                }
            }
            i++;
        }
    }
    else if (m_role == 7)
    {
        if (y > 4)
        {
            int s;
            if (y < 9)
            {
                s = x + (y + 1) * 9;
                if (chess[s]->side != m_side)
                    ans.append(s);
            }
            s = x + 1 + y * 9;
            if (chess[s]->side != m_side)
                ans.append(s);
            s = x - 1 + y * 9;
            if (chess[s]->side != m_side)
                ans.append(s);
        }
        else
        {
            int s = x + (y + 1) * 9;
            if (chess[s]->side != m_side)
                ans.append(s);
        }
    }
    return ans;
}

void MainWindow::aMoveTob(int a, int b)
{
    setChess(b, chess[a]->side, chess[a]->role);
    setChess(a, 0, 0);
    if (Now_Side == 1)
    {
        Now_Side = 2;
    }
    else
    {
        Now_Side = 1;
    }
}

void MainWindow::setChess(int m_rank, int m_side, int m_role)
{
    QPalette palette = chess[m_rank]->palette();
    QString path;
    if (m_role == 0)
    {
        palette.setColor(QPalette::Background, QColor(0, 0, 0, 0));
        chess[m_rank]->setAutoFillBackground(true);
        chess[m_rank]->role = 0;
        chess[m_rank]->side = 0;
        chess[m_rank]->setPalette(palette);
        return;
    }
    else if (m_role == 1)
    {
        if (m_side == 1)
            path = ":/new/pics/r_jiangshuai.png";
        else
            path = ":/new/pics/b_jiangshuai.png";
    }
    else if (m_role == 2)
    {
        if (m_side == 1)
            path = ":/new/pics/r_shi.png";
        else
            path = ":/new/pics/b_shi.png";
    }
    else if (m_role == 3)
    {
        if (m_side == 1)
            path = ":/new/pics/r_xiang.png";
        else
            path = ":/new/pics/b_xiang.png";
    }
    else if (m_role == 4)
    {
        if (m_side == 1)
            path = ":/new/pics/r_ma.png";
        else
            path = ":/new/pics/b_ma.png";
    }
    else if (m_role == 5)
    {
        if (m_side == 1)
            path = ":/new/pics/r_ju.png";
        else
            path = ":/new/pics/b_ju.png";
    }
    else if (m_role == 6)
    {
        if (m_side == 1)
            path = ":/new/pics/r_pao.png";
        else
            path = ":/new/pics/b_pao.png";
    }
    else if (m_role == 7)
    {
        if (m_side == 1)
            path = ":/new/pics/r_bing.png";
        else
            path = ":/new/pics/b_zubing.png";
    }
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(path).scaled(
                         chess[m_rank]->size(),
                         Qt::IgnoreAspectRatio,
                         Qt::SmoothTransformation)));
    chess[m_rank]->side = m_side;
    chess[m_rank]->role = m_role;
    chess[m_rank]->setAutoFillBackground(true);
    chess[m_rank]->setPalette(palette);
}

void MainWindow::startnewgame()
{
    for (int i = 0; i < 90; i++)
    {
        setChess(i, 0, 0);
    }
    ui->menuStart->actions()[0]->setEnabled(false);
    ui->menuStart->actions()[1]->setEnabled(false);
    My_Side = 2;
    Now_Side = 1;
    setChess(0, 2, 5);
    setChess(1, 2, 4);
    setChess(2, 2, 3);
    setChess(3, 2, 2);
    setChess(4, 2, 1);
    setChess(5, 2, 2);
    setChess(6, 2, 3);
    setChess(7, 2, 4);
    setChess(8, 2, 5);
    setChess(27, 2, 7);
    setChess(29, 2, 7);
    setChess(31, 2, 7);
    setChess(33, 2, 7);
    setChess(35, 2, 7);
    setChess(25, 2, 6);
    setChess(19, 2, 6);
    setChess(81, 1, 5);
    setChess(82, 1, 4);
    setChess(83, 1, 3);
    setChess(84, 1, 2);
    setChess(85, 1, 1);
    setChess(86, 1, 2);
    setChess(87, 1, 3);
    setChess(88, 1, 4);
    setChess(89, 1, 5);
    setChess(54, 1, 7);
    setChess(56, 1, 7);
    setChess(58, 1, 7);
    setChess(60, 1, 7);
    setChess(62, 1, 7);
    setChess(64, 1, 6);
    setChess(70, 1, 6);
}

void MainWindow::on_actionnewgame_triggered()
{
    for (int i = 0; i < 90; i++)
    {
        setChess(i, 0, 0);
    }
    ui->menuStart->actions()[0]->setEnabled(false);
    ui->menuStart->actions()[1]->setEnabled(false);
    My_Side = 1;
    Now_Side = 1;
    setChess(0, 1, 5);
    setChess(1, 1, 4);
    setChess(2, 1, 3);
    setChess(3, 1, 2);
    setChess(4, 1, 1);
    setChess(5, 1, 2);
    setChess(6, 1, 3);
    setChess(7, 1, 4);
    setChess(8, 1, 5);
    setChess(27, 1, 7);
    setChess(29, 1, 7);
    setChess(31, 1, 7);
    setChess(33, 1, 7);
    setChess(35, 1, 7);
    setChess(25, 1, 6);
    setChess(19, 1, 6);
    setChess(81, 2, 5);
    setChess(82, 2, 4);
    setChess(83, 2, 3);
    setChess(84, 2, 2);
    setChess(85, 2, 1);
    setChess(86, 2, 2);
    setChess(87, 2, 3);
    setChess(88, 2, 4);
    setChess(89, 2, 5);
    setChess(54, 2, 7);
    setChess(56, 2, 7);
    setChess(58, 2, 7);
    setChess(60, 2, 7);
    setChess(62, 2, 7);
    setChess(64, 2, 6);
    setChess(70, 2, 6);
    QByteArray *array = new QByteArray;
    array->clear();
    array->append(QString::number(1));
    readWriteSocket->write(array->data());
    startcountdown();
}

void MainWindow::on_actionload_triggered()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Open"));
    if (fileDialog->exec() == QDialog::Accepted)
    {
        for (int i = 0; i < 90; i++)
        {
            setChess(i, 0, 0);
        }
        ui->menuStart->actions()[0]->setEnabled(false);
        ui->menuStart->actions()[1]->setEnabled(false);
        QString path = fileDialog->selectedFiles()[0];
        QFile *file = new QFile(path);
        if (file->open(QFile::ReadOnly))
        {
            QTextStream in(file);
            QByteArray *array = new QByteArray;
            array->clear();

            for (int i = 0; i < 16; i++)
            {
                QString temp = in.readLine();
                if (i == 0)
                {
                    if (temp == "red")
                    {
                        My_Side = 1;
                        Now_Side = 1;
                    }
                    else
                    {
                        My_Side = 2;
                        Now_Side = 2;
                    }
                }
                else if (i < 8)
                {
                    int j = 3;
                    while (j < temp.size())
                    {
                        int x = temp[j].toLatin1() - '0';
                        int y = temp[j + 2].toLatin1() - '0';
                        setChess(x + y * 9, My_Side, i);
                        array->append(QString::number(My_Side * 1000 + i * 100 + 89 - x - y * 9)); //side,role,rank

                        j += 6;
                    }
                }
                else if (i > 8)
                {
                    int j = 3;
                    while (j < temp.size())
                    {
                        int x = temp[j].toLatin1() - '0';
                        int y = temp[j + 2].toLatin1() - '0';
                        int tempside = 1;
                        if (My_Side == 1)
                            tempside = 2;
                        setChess(x + y * 9, tempside, i - 8);
                        array->append(QString::number(tempside * 1000 + (i - 8) * 100 + 89 - x - y * 9)); //side,role,rank

                        j += 6;
                    }
                }
            }
            readWriteSocket->write(array->data());
            startcountdown();
        }
    }
}

void MainWindow::startcountdown()
{
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->lcdNumber->display(20);
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(minusone()));
    timer->start(1000);
}

void MainWindow::minusone()
{
    int a = (int)(ui->lcdNumber->value());
    emit setlcdvalue(a - 1);
    if (a == 1)
    {
        delete timer;
        QByteArray *array = new QByteArray;
        array->clear();
        array->append(QString::number(3));
        readWriteSocket->write(array->data());
        Now_Side = -1;
        ui->menuStart->actions()[0]->setEnabled(true);
        ui->menuStart->actions()[1]->setEnabled(true);
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        enddial = new enddialog(this);
        enddial->settext("你输了！");
        enddial->show();
    }
}

void MainWindow::giveup()
{
    delete timer;
    QByteArray *array = new QByteArray;
    array->clear();
    array->append(QString::number(2));
    readWriteSocket->write(array->data());
    ui->menuStart->actions()[0]->setEnabled(true);
    ui->menuStart->actions()[1]->setEnabled(true);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    enddial = new enddialog(this);

    enddial->settext("你输了！");
    enddial->show();
}

void MainWindow::savegame()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Save"));
    if (fileDialog->exec() == QDialog::Accepted)
    {
        QString path = fileDialog->selectedFiles()[0];
        //qDebug() << path;
        QFile f(path + "/test.txt");
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed.";
            return;
        }
        QTextStream txtOutput(&f);

        QVector<QString> lines;
        int a[16];
        for (int i = 0; i < 16; i++)
        {
            a[i] = 0;
            lines.append(" ");
        }
        if (Now_Side == 1)
        {
            lines[0] = "red";
            lines[8] = "black";
        }
        else
        {
            lines[0] = "black";
            lines[8] = "red";
        }
        for (int i = 0; i < 90; i++)
        {
            if (chess[i]->role > 0)
            {
                int j = i;
                if (Now_Side != My_Side)
                    j = 89 - i;
                int y = j / 9;
                int x = j - y * 9;
                QString tmp = "<" + QString::number(x) + "," + QString::number(y) + "> ";
                if (chess[i]->side == Now_Side)
                {
                    a[chess[i]->role]++;
                    lines[chess[i]->role] += tmp;
                }
                else
                {
                    a[chess[i]->role + 8]++;
                    lines[chess[i]->role + 8] += tmp;
                }
            }
        }
        for (int i = 0; i < 16; i++)
        {
            if (i != 0 && i != 8)
                lines[i] = QString::number(a[i]) + lines[i];
            txtOutput << lines[i] << endl;
        }
        f.close();
    }
}

void MainWindow::on_actionashost_triggered()
{
    listenSocket = new QTcpServer;
    informDialog dial(this);
    dial.hideaddress();
    if (dial.exec() == QDialog::Accepted)
    {
        int a = dial.getPort();
        listenSocket->listen(QHostAddress::Any, a);
        QObject::connect(this->listenSocket, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    }
    waitdiag = new waitingDialog(this);
    waitdiag->show();
    QObject::connect(waitdiag, SIGNAL(canceled()), this, SLOT(connectionCanceled()));
    ui->actionasclient->setEnabled(false);
    ui->actionashost->setEnabled(false);
}

void MainWindow::acceptConnection()
{
    waitdiag->close();
    ui->label->setText("State: connected");
    ui->label->repaint();
    this->readWriteSocket = this->listenSocket->nextPendingConnection();
    QObject::connect(this->readWriteSocket, SIGNAL(readyRead()), this, SLOT(recvMessage()));
    ui->actionload->setEnabled(true);
    ui->actionnewgame->setEnabled(true);
}

void MainWindow::connectionCanceled()
{
    waitdiag->close();
    delete listenSocket;
    ui->actionasclient->setEnabled(true);
    ui->actionashost->setEnabled(true);
}

void MainWindow::on_actionasclient_triggered()
{
    readWriteSocket = new QTcpSocket;
    informDialog dial(this);
    if (dial.exec() == QDialog::Accepted)
    {
        int a = dial.getPort();
        QString b = dial.getAddress();
        readWriteSocket->connectToHost(QHostAddress(b), a);
        if (readWriteSocket->waitForConnected(1000))
        {
            ui->label->setText("State: connected");
            ui->actionload->setEnabled(true);
            ui->actionnewgame->setEnabled(true);
            ui->actionasclient->setEnabled(false);
            ui->actionashost->setEnabled(false);
            QObject::connect(readWriteSocket, SIGNAL(readyRead()), this, SLOT(recvMessage()));
        }
    }
}

void MainWindow::recvMessage()
{
    QString info;
    info = this->readWriteSocket->readAll();
    int infoint = info.toInt();
    if (infoint == 1)
    { //正常开局
        startnewgame();
    }
    else if (infoint == 2)
    { //认输
        ui->menuStart->actions()[0]->setEnabled(true);
        ui->menuStart->actions()[1]->setEnabled(true);
        //ui->label->setText("对方已认输");
        ui->pushButton->setEnabled(false);
        enddial = new enddialog(this);
        enddial->settext("你赢了！");
        enddial->show();
    }
    else if (infoint == 3)
    { //超时
        ui->menuStart->actions()[0]->setEnabled(true);
        ui->menuStart->actions()[1]->setEnabled(true);
        //ui->label->setText("对方已超时");
        ui->pushButton->setEnabled(false);
        enddial = new enddialog(this);
        enddial->settext("你赢了！");
        enddial->show();
    }
    else if (infoint / 10000 == 1)
    { //走子
        int a = (infoint - 10000) / 100;
        int b = infoint - 10000 - 100 * a;
        aMoveTob(a, b);
        startcountdown();
    }
    else if (infoint / 10000 == 2)
    { //将军
        int a = (infoint - 20000) / 100;
        int b = infoint - 20000 - 100 * a;
        aMoveTob(a, b);
        //ui->label->setText("将军！");
        player->play();
        startcountdown();
    }
    else if (infoint / 10000 == 3)
    { //胜利
        int a = (infoint - 30000) / 100;
        int b = infoint - 30000 - 100 * a;
        aMoveTob(a, b);
        ui->menuStart->actions()[0]->setEnabled(true);
        ui->menuStart->actions()[1]->setEnabled(true);
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        //ui->label->setText("输了！");
        enddial = new enddialog(this);
        enddial->settext("你输了！");
        enddial->show();
    }
    else
    { //存档导入
        int i = 0;
        for (int i = 0; i < 90; i++)
        {
            setChess(i, 0, 0);
        }
        ui->menuStart->actions()[0]->setEnabled(false);
        ui->menuStart->actions()[1]->setEnabled(false);
        while (i < info.size())
        {
            QString temp = info.mid(i, 4);
            int tempint = temp.toInt();
            int m_side = tempint / 1000;
            int m_role = (tempint - m_side * 1000) / 100;
            int m_rank = tempint - m_side * 1000 - m_role * 100;
            if (i == 0)
            {
                Now_Side = m_side;
                if (Now_Side == 1)
                {
                    My_Side = 2;
                }
                else
                {
                    My_Side = 1;
                }
            }
            setChess(m_rank, m_side, m_role);
            i += 4;
        }
    }
}

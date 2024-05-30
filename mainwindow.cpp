#include "mainwindow.h"

#include"startgamedialog.h"
#include"sweepmap.h"
#include<QLabel>
#include<QPushButton>
#include <QMessageBox>
#include <QBoxLayout>
#include <QTimer>
#include <QtDebug>

struct MainWindow::MainWindowPrivae {
    QPushButton* btn_restart;
    QLabel* lbl_resolveNum;

    SweepMap* sweepmap;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new MainWindowPrivae)
{
    d->btn_restart = new QPushButton(tr("ReStart"), this);
    d->btn_restart->setFixedSize(80, 30);
    connect(d->btn_restart, &QPushButton::clicked, this, &MainWindow::StartGame);
    d->lbl_resolveNum = new QLabel("--", this);
    d->lbl_resolveNum->setFixedSize(50, 30);
    d->sweepmap = new SweepMap(this);
    QHBoxLayout* blay = new QHBoxLayout;
    blay->addWidget(d->btn_restart);
    blay->addStretch(0);
    blay->addWidget(new QLabel(tr("Remian:"), this));
    blay->addWidget(d->lbl_resolveNum);
    QVBoxLayout* vlay = new QVBoxLayout;
    vlay->addLayout(blay);
    vlay->addWidget(d->sweepmap);
    QWidget* w = new QWidget(this);
    w->setLayout(vlay);
    this->setCentralWidget(w);
    QTimer::singleShot(1000, this, &MainWindow::StartGame);
}

MainWindow::~MainWindow()
{
    delete d;
}

void MainWindow::StartGame()
{
    disconnect(d->sweepmap, &SweepMap::MineNumberChAanged, this, &MainWindow::MineNumberChanged);
    disconnect(d->sweepmap, &SweepMap::GameOver, this, &MainWindow::GameOver);
    StartGameDialog dlg;
    dlg.SetData(d->sweepmap->getRow(), d->sweepmap->getCol(), d->sweepmap->getNum());

    if(dlg.exec() == QDialog::Accepted) {
        d->sweepmap->StartGame(dlg.getRow(), dlg.getCol(), dlg.getNUm());
        connect(d->sweepmap, &SweepMap::MineNumberChAanged, this, &MainWindow::MineNumberChanged);
        connect(d->sweepmap, &SweepMap::GameOver, this, &MainWindow::GameOver);
        MineNumberChanged(0);
    }

}

void MainWindow::GameOver(bool win)
{
    disconnect(d->sweepmap, &SweepMap::MineNumberChAanged, this, &MainWindow::MineNumberChanged);
    disconnect(d->sweepmap, &SweepMap::GameOver, this, &MainWindow::GameOver);


    QMessageBox::information(this, tr("Game Over"), win ? tr("Congratulation") : tr("Regret"));
}

void MainWindow::MineNumberChanged(int i)
{
    int remainNum = d->sweepmap->getNum() - i;
    d->lbl_resolveNum->setText(QString::number(remainNum));
}

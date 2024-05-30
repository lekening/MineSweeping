#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    struct MainWindowPrivae;
    MainWindowPrivae* d;

private slots:
    void StartGame();
    void GameOver(bool win);
    void MineNumberChanged(int i);

private:

};

#endif // MAINWINDOW_H

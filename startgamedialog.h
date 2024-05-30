#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QSpinBox>

class StartGameDialog : public QDialog
{
    Q_OBJECT
public:
    StartGameDialog(QWidget* parent = nullptr);
    void SetData(int row, int col, int num);

    int getRow() {return m_spb_row->value();}
    int getCol() {return m_spb_col->value();}
    int getNUm() {return m_spb_num->value();}
private:
    QSpinBox* m_spb_row;
    QSpinBox* m_spb_col;
    QSpinBox* m_spb_num;

};

#endif // STARTGAMEDIALOG_H

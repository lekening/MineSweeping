#include "startgamedialog.h"
#include<QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>

StartGameDialog::StartGameDialog(QWidget *parent)
    : QDialog(parent)
{
    m_spb_row = new QSpinBox(this);
    m_spb_row->setValue(10);
    m_spb_col = new QSpinBox(this);
    m_spb_col->setValue(15);
    m_spb_num = new QSpinBox(this);
    m_spb_num->setValue(20);

    QPushButton* btn_confirm = new QPushButton(tr("Confirm"), this);
    connect(btn_confirm, &QPushButton::clicked, this, &QDialog::accept);

    QFormLayout* form = new QFormLayout;
    form->addRow(tr("Row:"), m_spb_row);
    form->addRow(tr("Col:"), m_spb_col);
    form->addRow(tr("Num:"), m_spb_num);

    QVBoxLayout* vlay = new QVBoxLayout;
    vlay->addLayout(form);
    vlay->addWidget(btn_confirm);

    this->setLayout(vlay);

}

void StartGameDialog::SetData(int row, int col, int num)
{
    m_spb_row->setValue(row);
    m_spb_col->setValue(col);
    m_spb_num->setValue(num);
}

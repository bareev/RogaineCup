#include "editparticipiant.h"
#include "ui_editparticipiant.h"
#include <QTextCodec>

EditParticipiant::EditParticipiant(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditParticipiant)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onOkSlot()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onCancelSlot()));
}

EditParticipiant::~EditParticipiant()
{
    if (ui)
        delete ui;
}

void EditParticipiant::init(QString group, QString region, QString name, QString gr)
{
    ui->lineEdit->setText(name);
    ui->lineEdit_2->setText(group);
    ui->lineEdit_3->setText(gr);
    ui->lineEdit_4->setText(region);
    return;
}

void EditParticipiant::onOkSlot()
{
    emit onOk(ui->lineEdit_2->text(), ui->lineEdit_4->text(), ui->lineEdit->text(), ui->lineEdit_3->text());
    return;
}

void EditParticipiant::onCancelSlot()
{
    emit onCancel();
    return;
}

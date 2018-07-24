#include "addnewfile.h"
#include "ui_addnewfile.h"
#include <QFileDialog>

AddNewFile::AddNewFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddNewFile)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    protocolCur = new Protocol;
    etapCur = new etapDS;

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(OnOk()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(OnC()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(OnAddProt()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(OnMapAdd()));

}

AddNewFile::~AddNewFile()
{
    if (protocolCur)
        delete protocolCur;
    if (etapCur)
        delete etapCur;
    if (ui)
        delete ui;
}

void AddNewFile::init(int i, SettingsDS set)
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->lineEdit_2->setText(QString("%1").arg(i));
    ui->lineEdit_3->setText("4");
    ui->lineEdit_4->setText("1.0");
    ui->lineEdit->setText(QString("Этап %1 кубка Нижегородской области по рогейну").arg(i));
    ui->lineEdit_5->setText("");
    ui->checkBox->setChecked(true);
    ui->label_8->setText("");
    ui->label_7->setText("");

    sets = set;
}

void AddNewFile::OnOk()
{
    int error;

    etapCur->calc = ui->checkBox->isChecked();
    etapCur->etapnumber = ui->lineEdit_2->text().toInt();
    etapCur->format = ui->lineEdit_3->text().toDouble();
    etapCur->koeff = ui->lineEdit_4->text().toDouble();
    etapCur->map = QPixmap(ui->label_7->text());
    etapCur->text.clear();
    etapCur->text.append(ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd_hh-mm-ss"));
    etapCur->text.append(ui->lineEdit->text());
    etapCur->text.append(ui->lineEdit_5->text());

    //добавление протокола
    int err = protocolCur->ReadProtocol(ui->label_8->text(), sets, &error);
    protocolCur->GenMessage(err, error);

    if (err != 0)
        emit OnCancel();
    else
        emit OnNewFile(protocolCur->prot, *etapCur);

}

void AddNewFile::OnAddProt()
{
    //диалоговое окно
    QString newFile = QFileDialog::getOpenFileName(this, QString("Открыть новый протокол"), QDir::currentPath(), QString("Файл протокола (*.csv)"));
    ui->label_8->setText(newFile);
}

void AddNewFile::OnMapAdd()
{
    //диалоговое окно
    QString newFile = QFileDialog::getOpenFileName(this, QString("Добавить карту к общей информации"), QDir::currentPath(), QString("Файл карты (*.png; *.bmp; *.jpg; *.gif)"));
    ui->label_7->setText(newFile);
}

void AddNewFile::OnC()
{
    emit OnCancel();
}

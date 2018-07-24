#include "settings.h"
#include "ui_settings.h"
#include <QDebug>
#include <QTextCodec>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onClicked()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onCancelClicked()));
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(checked(bool)));

    defaultsettings = new SettingsDS;
    currentsettings = new SettingsDS;

    defaultsettings->summa = 4;
    defaultsettings->text.append(QString("МО, ММ, МВ, МСВ, МУВ, ЖО, ЖМ, ЖВ, ЖСВ, ЖУВ"));
    defaultsettings->text.append(QString(";"));
    defaultsettings->text.append(QString("Кубок Нижегородской области по рогейну 2017"));
    defaultsettings->priznak = false;

    currentsettings->priznak = false;
}

Settings::~Settings()
{
    if (defaultsettings)
        delete defaultsettings;
    if (currentsettings)
        delete currentsettings;
    if (ui)
        delete ui;
}

void Settings::onDefLoad()
{
    onLoad(*defaultsettings);
}

void Settings::onLoad(SettingsDS sets)
{
    ui->checkBox->setChecked(false);

    //загрузка текущих параметров
    ui->lineEdit->setText(QString("%1").arg(sets.summa));
    ui->lineEdit_2->setText(QString(sets.text.at(1)));
    ui->lineEdit_3->setText(QString(sets.text.at(0)));
    ui->lineEdit_4->setText(QString(sets.text.at(2)));

    show();
}

void Settings::onClicked()
{
    //сохраняем текущие настройки

    *currentsettings = newSets();
    hide();

    emit saveSets();
}

void Settings::onCancelClicked()
{
    currentsettings->priznak = false;
    hide();

    emit closeSets();
}

void Settings::checked(bool check)
{
    ui->lineEdit_3->setEnabled(check);
}

//новые настройки
SettingsDS Settings::newSets()
{
    SettingsDS ns;
    ns.text.clear();
    ns.text.append(ui->lineEdit_3->text());
    ns.text.append(ui->lineEdit_2->text());
    ns.summa = ui->lineEdit->text().toInt();
    ns.text.append(ui->lineEdit_4->text());
    ns.priznak = true;

    return ns;
}

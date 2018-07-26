#include "rogainecup.h"
#include "ui_rogainecup.h"
#include <QFileDialog>
#include <QDebug>
#include <settings.h>
#include <QMessageBox>
#include <QTextCodec>
#include <QMenu>
#include <QAction>

inline QDataStream &operator << (QDataStream & ds, const SettingsDS &setData)
{
    ds << setData.summa << setData.text << setData.priznak;
    return ds;
}

inline QDataStream &operator >> (QDataStream & ds, SettingsDS &setData)
{
    ds >> setData.summa >> setData.text >> setData.priznak;
    return ds;
}

inline QDataStream &operator << (QDataStream & ds, const etapDS &setData)
{
    ds << setData.etapnumber << setData.format << setData.koeff << setData.map << setData.text << setData.calc;
    return ds;
}

inline QDataStream &operator >> (QDataStream & ds, etapDS &setData)
{
    ds >> setData.etapnumber >> setData.format >> setData.koeff >> setData.map >> setData.text >> setData.calc;
    return ds;
}

inline QDataStream &operator << (QDataStream & ds, const ProtocolDS &setData)
{
    ds << setData.number << setData.text;
    return ds;
}

inline QDataStream &operator >> (QDataStream & ds, ProtocolDS &setData)
{
    ds >> setData.number >> setData.text;
    return ds;
}

inline QDataStream &operator << (QDataStream & ds, const generalpos &setData)
{
    ds << setData.numberpart << setData.numetaps;
    return ds;
}

inline QDataStream &operator >> (QDataStream & ds, generalpos &setData)
{
    ds >> setData.numberpart >> setData.numetaps;
    return ds;
}


RogaineCup::RogaineCup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RogaineCup)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    //установка иконок
    ui->pushButton_4->setIcon(QIcon(":/res/new.png"));
    ui->pushButton->setIcon(QIcon(":/res/open.png"));
    ui->pushButton_2->setIcon(QIcon(":/res/save.png"));
    ui->pushButton_3->setIcon(QIcon(":/res/exit.png"));
    ui->pushButton_5->setIcon(QIcon(":/res/settings.png"));
    ui->pushButton_6->setIcon(QIcon(":/res/add.png"));
    ui->pushButton_7->setIcon(QIcon(":/res/close.png"));
    ui->pushButton_12->setIcon(QIcon(":/res/saveas.png"));
    ui->pushButton_13->setIcon(QIcon(":/res/update.png"));


    //установка поля ввода/вывода
    ui->tableWidget->setColumnCount(7);
    QString *list = new QString;
    list->clear();
    *list = "Место, Группа, Место в группе, Участник, Регион, Год рождения, Текущая сумма";
    for (int i = 0; i < 7; i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(QStringList(list->split(",")).at(i));
        ui->tableWidget->setHorizontalHeaderItem(i, item);
    }
    if (list)
        delete list;

    // не открыт ни один файл
    enableAll(false);

    //сигналы-слоты основных кнопок
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(onExit()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(onOpenNewFile()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onOpenExistFile()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onSaveFile()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(onCloseFile()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(onSettingsClicked()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(onAddNewFile()));

    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(ExportAllSumm()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(ExportEtapMap()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(ExportEtapInfo()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(ExportLeaders()));

    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(onSaveAsFile()));

    //вспомогательная форма настроек
    set = new Settings;
    set->hide();

    //форма нового этапа
    newData = new AddNewFile;
    newData->hide();

    //форма редактирования участника
    particip = new EditParticipiant;
    particip->hide();

    //настройки
    geninfo = new generalpos;

    //текущий протокол
    protocolcurrent = new ProtocolDS;
    protocolcurrent->number = -1;
    protocolcurrent->text.clear();

    //информация по этапам
    etapInfos.clear();
    etapProtoclos.clear();

    connect(set, SIGNAL(closeSets()), this, SLOT(CancelSets()));
    connect(set, SIGNAL(saveSets()), this, SLOT(SaveSets()));

    connect(newData, SIGNAL(OnCancel()), this, SLOT(NewFileCancel()));
    connect(newData, SIGNAL(OnNewFile(ProtocolDS,etapDS)), this, SLOT(NewFileAdd(ProtocolDS,etapDS)));

    openQFile = new QFile(this);

    TxtWork = new TextAndMath;

    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

    connect(particip, SIGNAL(onCancel()), this, SLOT(slotCancelEdit()));
    connect(particip, SIGNAL(onOk(QString,QString,QString,QString)), this, SLOT(slotOkEdit(QString,QString,QString,QString)));

    //имя окна меняем
    connect(this, SIGNAL(openQFileSignal(bool)), this, SLOT(onOpenQFile(bool)));
}

RogaineCup::~RogaineCup()
{
    if (set)
    {
        set->close();
        delete set;
    }
    if (newData)
    {
        newData->close();
        delete newData;
    }
    if (particip)
    {
        particip->close();
        delete particip;
    }
    if (openQFile)
        delete openQFile;
    if (geninfo)
        delete geninfo;
    if (protocolcurrent)
        delete protocolcurrent;
    if (TxtWork)
        delete TxtWork;
    if (ui)
        delete ui;
}

//открыт/закрыт файл
void RogaineCup::enableAll(bool pr)
{
    ui->pushButton_5->setEnabled(pr);
    ui->pushButton_6->setEnabled(pr);
    ui->tableWidget->setEnabled(pr);
    ui->label->setEnabled(pr);
    ui->label_2->setEnabled(pr);
    ui->label_2->clear();
    ui->label_3->setEnabled(pr);
    ui->label_3->clear();
    ui->pushButton_8->setEnabled(pr);
    ui->pushButton_9->setEnabled(pr);
    ui->pushButton_10->setEnabled(pr);
    ui->pushButton_11->setEnabled(pr);
    ui->spinBox->setEnabled(pr);
    ui->label_4->setEnabled(pr);
    ui->pushButton_13->setEnabled(pr);
    openfile = pr;
    if (openfile)
        GetInfoFromMemoryToForm();
    else
    {
        ui->tableWidget->setRowCount(0);
    }

    //сигнал на изменение имени
    emit openQFileSignal(pr);

}

// изменяем имя окна
void RogaineCup::onOpenQFile(bool open)
{
    QString name("RogaineCup");

    if (open && !openQFile->fileName().isEmpty())
        name.append(" - ").append(openQFile->fileName());

    setWindowTitle(name);
}

//создание нового файла!!!!!!!
void RogaineCup::onOpenNewFile()
{
    //диалоговое окно
    QString newFile = QFileDialog::getSaveFileName(this, QString("Создать новый файл"), QDir::currentPath(), QString("Файл менеджера кубка по рогейну (*.rcm)"));

    if (newFile.isEmpty())
        return;

    //закрываем файл
    onCloseFile();

    //создать или проверить файл
    openQFile->setFileName(newFile);
    //файл существует
    if (openQFile->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Внимание", "Файл переписан!!!");
        openQFile->close();
    }

    openQFile->open(QIODevice::WriteOnly);

    //загружаем настройки с умолчанием
    set->onDefLoad();
    //пока ноль участников и этапов, обнуляем
    geninfo->numberpart = 0;
    geninfo->numetaps = 0;

    setEnabled(false);

}

//сохранение настроек нового файла
void RogaineCup::SaveSets()
{
    //сохраняем файл, если настройки сохранены
    if (set->currentsettings->priznak)
    {
        WriteGenSets(openQFile);
        WriteGenInfo(openQFile);
    }

    //закрываем файл
    openQFile->close();
    setEnabled(true);

    //отображаем содержимое
    CheckFileSize();

}

void RogaineCup::CancelSets()
{
    //закрываем файл
    openQFile->close();
    setEnabled(true);

    CheckFileSize();
}

//открытие текущего файла
void RogaineCup::onOpenExistFile()
{
    //диалоговое окно
    QString newFile = QFileDialog::getOpenFileName(this, QString("Открыть новый файл"), QDir::currentPath(), QString("Файл менеджера кубка по рогейну (*.rcm)"));

    //файл не выбран
    if (newFile.isEmpty())
            return;


    //закрываем файл
    onCloseFile();

    //создать или проверить файл
    openQFile->setFileName(newFile);

    //файл существует
    if (!openQFile->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this, "Внимание", "Файл неисправен!!!");
        return;
    }

    //считываем информацию из файла, если он не пустой
    if (QFileInfo(openQFile->fileName()).size() > 0)
    {
        ReadGenSets(openQFile);
        ReadGenInfo(openQFile);
    }

    //изменяем размер информации по этапам
    ProtocolDS tempProt;
    etapDS tempEtap;
    onNewInfos(&tempProt);
    onNewInfos(&tempEtap);

    for (int i = 0; i < geninfo->numetaps; i++)
    {
        etapInfos.append(tempEtap);
        etapProtoclos.append(tempProt);
    }

    //считываем текущую сумму
    if (!openQFile->atEnd())
    {
        ReadCurSumma(openQFile);      
    }

    //считываем информацию по этапам
    int etn = 0;
    while (!openQFile->atEnd())
    {
        ReadEtapInfo(openQFile, etn);
        ReadEtapProt(openQFile, etn);
        etn++;
    }

    //закрываем файл
    openQFile->close();

    //файл открыт!!!
    CheckFileSize();

}


//открытие текущего файла без диалогового окна
void RogaineCup::onOpenExistFileCmd(QString name)
{

    //файл не выбран
    if (name.isEmpty())
        return;

    //закрываем файл
    onCloseFile();

    //создать или проверить файл
    openQFile->setFileName(name);

    //файл существует
    if (!openQFile->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this, "Внимание", "Файл неисправен!!!");
        return;
    }

    //считываем информацию из файла, если он не пустой
    if (QFileInfo(openQFile->fileName()).size() > 0)
    {
        ReadGenSets(openQFile);
        ReadGenInfo(openQFile);
    }

    //изменяем размер информации по этапам
    ProtocolDS tempProt;
    etapDS tempEtap;
    onNewInfos(&tempProt);
    onNewInfos(&tempEtap);

    for (int i = 0; i < geninfo->numetaps; i++)
    {
        etapInfos.append(tempEtap);
        etapProtoclos.append(tempProt);
    }

    //считываем текущую сумму
    if (!openQFile->atEnd())
    {
        ReadCurSumma(openQFile);
    }

    //считываем информацию по этапам
    int etn = 0;
    while (!openQFile->atEnd())
    {
        ReadEtapInfo(openQFile, etn);
        ReadEtapProt(openQFile, etn);
        etn++;
    }

    //закрываем файл
    openQFile->close();

    //файл открыт!!!
    CheckFileSize();

}


//сохранение текущего файла
void RogaineCup::onSaveFile()
{
    //диалоговое окно
    //QString newFile = QFileDialog::getSaveFileName(this, QString("Сохранить текущий файл как"), QDir::currentPath(), QString("Файл менеджера кубка по рогейну (*.rcm)"));
    if ((openfile) && (!openQFile->fileName().isEmpty()))
    {
        if (openQFile->isOpen())
            openQFile->close();

        openQFile->open(QIODevice::ReadWrite);

        //записываем информацию в файл
        WriteGenSets(openQFile);
        WriteGenInfo(openQFile);

        if (protocolcurrent->number != -1)
        {
            WriteCurSumma(openQFile);
            for (int j = 0; j < protocolcurrent->number; j++)
            {
                WriteEtapInfo(openQFile, j);
                WriteEtapProt(openQFile, j);
            }
        }


        //отображаем содержимое
        CheckFileSize();

        openQFile->close();
    }

}

//сохранение текущего файла
void RogaineCup::onSaveAsFile()
{
    //диалоговое окно
    QString newFile = QFileDialog::getSaveFileName(this, QString("Сохранить текущий файл как"), QDir::currentPath(), QString("Файл менеджера кубка по рогейну (*.rcm)"));
    if ((openfile) && (!newFile.isEmpty()))
    {

        if (openQFile->isOpen())
            openQFile->close();

        openQFile->setFileName(newFile);

        openQFile->open(QIODevice::ReadWrite);

        //записываем информацию в файл
        WriteGenSets(openQFile);
        WriteGenInfo(openQFile);

        if (protocolcurrent->number != -1)
        {
            WriteCurSumma(openQFile);
            for (int j = 0; j < protocolcurrent->number; j++)
            {
                WriteEtapInfo(openQFile, j);
                WriteEtapProt(openQFile, j);
            }
        }


        //отображаем содержимое
        CheckFileSize();

        openQFile->close();
    }

}

//запись настроек в файл
void RogaineCup::WriteGenSets(QFile *file)
{
    if (openQFile->isOpen())
    {

        QDataStream stream(file);
        SettingsDS newSets;
        onNewInfos(&newSets);

        newSets = *(set->currentsettings);

        stream << newSets;

        set->currentsettings->priznak = false;
    }
}

//чтение настроек из файла
void RogaineCup::ReadGenSets(QFile *file)
{
    if (openQFile->isOpen())
    {

        QDataStream stream(file);
        SettingsDS newSets;
        onNewInfos(&newSets);

        stream >> newSets;

        *(set->currentsettings) = newSets;

    }

}

//запись общей информации в файл
void RogaineCup::WriteGenInfo(QFile *file)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        generalpos newSets;
        onNewInfos(&newSets);

        newSets = *geninfo;

        stream << newSets;
    }
}

//чтение общей информации из файла
void RogaineCup::ReadGenInfo(QFile *file)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        generalpos newSets;
        onNewInfos(&newSets);

        stream >> newSets;

        *geninfo = newSets;
    }
}

//запись текущего протокола в файл
void RogaineCup::WriteCurSumma(QFile *file)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        ProtocolDS newSets;
        onNewInfos(&newSets);

        newSets = *protocolcurrent;

        stream << newSets;
    }
}

//чтение  текущего протокола из файла
void RogaineCup::ReadCurSumma(QFile *file)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        ProtocolDS newSets;
        onNewInfos(&newSets);

        stream >> newSets;

        *protocolcurrent = newSets;
    }
}

//запись протокола этапа в файл
void RogaineCup::WriteEtapProt(QFile *file, int i)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        ProtocolDS newSets;
        onNewInfos(&newSets);

        newSets = etapProtoclos.at(i);

        stream << newSets;
    }
}

//чтение  протокола этапа из файла
void RogaineCup::ReadEtapProt(QFile *file, int i)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        ProtocolDS newSets;
        onNewInfos(&newSets);

        stream >> newSets;

        if (i < geninfo->numetaps)
            etapProtoclos.replace(i, newSets);
    }
}

//запись протокола этапа в файл
void RogaineCup::WriteEtapInfo(QFile *file, int i)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        etapDS newSets;
        onNewInfos(&newSets);

        newSets = etapInfos.at(i);

        stream << newSets;
    }
}

//чтение  протокола этапа из файла
void RogaineCup::ReadEtapInfo(QFile *file, int i)
{
    if (openQFile->isOpen())
    {
        QDataStream stream(file);
        etapDS newSets;
        onNewInfos(&newSets);

        stream >> newSets;

        if (i < geninfo->numetaps)
            etapInfos.replace(i, newSets);
    }
}


//очистка вновь созданного
void RogaineCup::onNewInfos(etapDS* newSets)
{
    newSets->calc = false;
    newSets->etapnumber = 0;
    newSets->format = 0;
    newSets->koeff = 0;
    newSets->map = QPixmap();
    newSets->text.clear();

    return;
}

//очистка вновь созданного
void RogaineCup::onNewInfos(ProtocolDS* newSets)
{
    newSets->number = 0;
    newSets->text.clear();

    return;
}

//очистка вновь созданного
void RogaineCup::onNewInfos(generalpos *newSets)
{
    newSets->numberpart = 0;
    newSets->numetaps = 0;

    return;
}

void RogaineCup::onNewInfos(SettingsDS *newSets)
{
    newSets->priznak = false;
    newSets->summa = 0;
    newSets->text.clear();

    return;
}


//закрытие приложения
void RogaineCup::onExit()
{
    //закрываем файл
    onCloseFile();
    close();
}

//заполнение данными
void RogaineCup::CheckFileSize()
{
    //проверить размер файла и отобразить информацию
    if (QFileInfo(openQFile->fileName()).size() > 0)
        enableAll(true);
    else
        enableAll(false);
}

//заполнение данными
void RogaineCup::GetInfoFromMemoryToForm()
{
    ui->label_2->setText(QString(set->currentsettings->text.at(2)));
    ui->label_3->setText(QString("Количество проведенных этапов: %1, количество участников: %2").arg(geninfo->numetaps).arg(geninfo->numberpart));

    //заполняем текущий протокол
    if (protocolcurrent->number != -1)
    {
        ui->tableWidget->setRowCount(geninfo->numberpart);
        for (int j = 0; j < geninfo->numberpart; j++)
            for (int i = 0; i < 7; i++)
            {
                QTableWidgetItem* item = new QTableWidgetItem;
                item->setText(protocolcurrent->text.at(j).split(";").at(i));
                ui->tableWidget->setItem(j, i, item);
            }
    }
}

//закрытие одного файла
void RogaineCup::onCloseFile()
{
    //закрываем открытый файл
    if (openfile)
        QMessageBox::warning(this, "Внимание", "Открытый файл закрыт");

    if (openQFile->isOpen())
        openQFile->close();


    enableAll(false);

    //удаляем всю текущую информацию
    geninfo->numberpart = 0;
    geninfo->numetaps = 0;
    protocolcurrent->number = -1;
    protocolcurrent->text.clear();
    //информация по этапам
    etapInfos.clear();
    etapProtoclos.clear();
}


//вызов глобальных настроек
void RogaineCup::onSettingsClicked()
{
    set->onLoad(*(set->currentsettings));
    setEnabled(false);
}

//вызов формы нового файла
void RogaineCup::onAddNewFile()
{
    newData->init(geninfo->numetaps + 1, *(set->currentsettings));
    newData->show();
    setEnabled(false);
}

//закрытие формы нового файла
void RogaineCup::NewFileCancel()
{
    newData->hide();
    setEnabled(true);
}

//добавление нового протокола
void RogaineCup::NewFileAdd(ProtocolDS curprot,etapDS curinfo)
{
    newData->hide();
    setEnabled(true);

    //здесь обработчик текущий + общий зачёт

    if (curinfo.etapnumber > geninfo->numetaps)
    {
        geninfo->numetaps++;
        while (etapInfos.length() < geninfo->numetaps)
        {
            //изменяем размер информации по этапам
            ProtocolDS tempProt;
            etapDS tempEtap;

            etapInfos.append(tempEtap);
            etapProtoclos.append(tempProt);
        }
    }

    int idx = curinfo.etapnumber - 1;

    etapInfos.replace(curinfo.etapnumber - 1, curinfo);
    etapProtoclos.replace(curinfo.etapnumber - 1, curprot);

    //Добавили протокол и информацию, теперь посчитаем

    QStringList newFullSumma;

    TxtWork->WriteNewSumma(protocolcurrent->text, etapProtoclos.at(idx).text, &newFullSumma, etapInfos.at(idx).calc, etapInfos.at(idx).koeff, etapInfos.at(idx).format, (idx + 1), set->currentsettings->text.at(0), set->currentsettings->summa);

    protocolcurrent->number = (idx + 1);
    protocolcurrent->text = newFullSumma;

    geninfo->numberpart = protocolcurrent->text.length();

    QMessageBox::warning(this, "Внимание", "Новый протокол успешно добавлен!!!");
    //отобразим новое
    CheckFileSize();

}

//экспорт суммы в *csv или *html
void RogaineCup::ExportAllSumm()
{
    if (protocolcurrent->number <= 0)
    {
        QMessageBox::warning(this, "Ошибка", "Ни один этап не учтён!!!");
        return;
    }
    //диалоговое окно
    QString newFile = QFileDialog::getSaveFileName(this, QString("Экспорт общей суммы"), QDir::currentPath(), QString("Таблица CSV (*.csv);;Файл html (*.html)"));

    if (newFile.isEmpty())
        return;

    QFile* csv;
    csv = new QFile(this);
    csv->setFileName(newFile);

    if (csv->open(QIODevice::WriteOnly))
    {
        QTextStream stream(csv);
        stream.setCodec("windows1251");

        QFile* templates;
        templates = new QFile(this);

        if (newFile.right(3).compare("csv", Qt::CaseInsensitive) == 0)
        {
            templates->setFileName(":/res/template_csv.txt");

            if (templates->open(QIODevice::ReadOnly))
            {

                QTextStream temp(templates);
                temp.setCodec("windows1251");
                QString head = temp.readLine();

                for (int j = 0; j < geninfo->numetaps; j++)
                {
                    head += ";";
                    head += etapInfos.at(j).text.at(0).left(10);
                    head += ". ";
                    head += etapInfos.at(j).text.at(1);
                    head += QString(". Формат %1 часов").arg(etapInfos.at(j).format, 0, 'f', 1);
                    if (etapInfos.at(j).calc)
                        head += QString(". Коэффициент %2").arg(etapInfos.at(j).koeff, 0, 'f', 3);
                    else
                        head += ". Зачёт за участие";
                }

                stream << head;
                stream << "\n";

                if (newFile.right(3).compare("csv", Qt::CaseInsensitive) == 0)
                {
                    for (int i = 0; i < geninfo->numberpart; i++)
                    {
                        bool finddata = false;
                        QStringList s = protocolcurrent->text.at(i).split(";");
                        if (s.at(6).toDouble() > 0)
                            finddata = true;
                        else
                        {
                            for (int fd = 7; fd < s.length(); fd++)
                            {
                                if ((!s.at(fd).isEmpty()) && (s.at(fd).compare("+") != 0))
                                    finddata = true;
                            }
                        }
                        if (finddata)
                        {
                            stream << protocolcurrent->text.at(i);
                            stream << "\n";
                        }
                    }
                }
                templates->close();

                QMessageBox::warning(this, "Внимание", "Экспорт произведен успешно!!!");
            }
            else
                QMessageBox::warning(this, "Ошибка", "Не найден файл заголовка template_csv.txt!!!");

        }

        else if (newFile.right(4).compare("html", Qt::CaseInsensitive) == 0)
        {
            templates->setFileName(":/res/template_htm.html");
            if (templates->open(QIODevice::ReadOnly))
            {

                QTextStream temp(templates);
                temp.setCodec("windows1251");
                QString head = temp.readAll();

                for (int j = 0; j < geninfo->numetaps; j++)
                {
                    QString headTemp = etapInfos.at(j).text.at(0).left(10);
                    headTemp += ". ";
                    headTemp += etapInfos.at(j).text.at(1);
                    headTemp += QString(". Формат %1 часов").arg(etapInfos.at(j).format, 0, 'f', 1);
                    if (etapInfos.at(j).calc)
                        headTemp += QString(". Коэффициент %2").arg(etapInfos.at(j).koeff, 0, 'f', 3);
                    else
                        headTemp += ". Зачёт за участие";

                    head += QString("<th class=\"vl\">%1</th>").arg(headTemp);
                }

                //вписали заголовок
                stream << head;
                stream << "\n   </tr>\n";

                //запись основного тела
                if (newFile.right(4).compare("html", Qt::CaseInsensitive) == 0)
                {
                    for (int i = 0; i < geninfo->numberpart; i++)
                    {
                        bool finddata = false;
                        QStringList s = protocolcurrent->text.at(i).split(";");
                        if (s.at(6).toDouble() > 0)
                            finddata = true;
                        else
                        {
                            for (int fd = 7; fd < s.length(); fd++)
                            {
                                if ((!s.at(fd).isEmpty()) && (s.at(fd).compare("+") != 0))
                                    finddata = true;
                            }
                        }
                        if (finddata)
                        {
                            QString strokatemp;
                            strokatemp.clear();
                            strokatemp += "   <tr>\n";
                            strokatemp += "    ";
                            for (int tmp = 0; tmp < s.length(); tmp++)
                            {
                                if (!s.at(tmp).isEmpty())
                                    strokatemp += QString("<td class=\"vl\">%1</td>").arg(s.at(tmp));
                                else
                                    strokatemp += QString("<td class=\"vl\">%1</td>").arg("&nbsp;");
                            }
                            stream << strokatemp;
                            stream << "\n   </tr>\n";
                        }
                    }
                }

                //финальная вставка
                stream << "  </table>\n";
                stream << " </body>\n";
                stream << "</html>\n";
                templates->close();

                QMessageBox::warning(this, "Внимание", "Экспорт произведен успешно!!!");
            }
            else
                QMessageBox::warning(this, "Ошибка", "Не найден файл заголовка template_htm.html!!!");

        }

        csv->close();
        if (templates)
            delete templates;
    }
    else
        QMessageBox::warning(this, "Ошибка", "Ошибка экспорта!!!");

    if (csv)
        delete csv;

    return;
}

//экспорт карты этапа в низком качестве (пока, необходим QSettings для всех иконок, настроек, качества отображения)
void RogaineCup::ExportEtapMap()
{
    if (protocolcurrent->number <= 0)
    {
        QMessageBox::warning(this, "Ошибка", "Ни один этап не учтён!!!");
        return;
    }
    int numberEtap = ui->spinBox->text().toInt();
    if (protocolcurrent->number < numberEtap)
    {
        QMessageBox::warning(this, "Ошибка", "Данные об этом этапе отсутствуют!!!");
        return;
    }

    //диалоговое окно
    QString newFile = QFileDialog::getSaveFileName(this, QString("Экспорт карты этапа"), QDir::currentPath(), QString("Файл jpeg (*.jpg)"));

    if (newFile.isEmpty())
        return;

    bool sss = etapInfos.at(numberEtap - 1).map.save(newFile);

    if (!sss)
        QMessageBox::warning(this, "Ошибка", "Ошибка экспорта!!!");
    else
        QMessageBox::warning(this, "Внимание", "Экспорт произведен успешно!!!");

    return;

}

//информации о этапе
void RogaineCup::ExportEtapInfo()
{
    if (protocolcurrent->number <= 0)
    {
        QMessageBox::warning(this, "Ошибка", "Ни один этап не учтён!!!");
        return;
    }
    int numberEtap = ui->spinBox->text().toInt();
    if (protocolcurrent->number < numberEtap)
    {
        QMessageBox::warning(this, "Ошибка", "Данные об этом этапе отсутствуют!!!");
        return;
    }

    //диалоговое окно
    QString newFile = QFileDialog::getSaveFileName(this, QString("Экспорт протокола этапа"), QDir::currentPath(), QString("Таблица CSV (*.csv);;Файл html (*.html)"));

    if (newFile.isEmpty())
        return;

    QFile* csv;
    csv = new QFile(this);
    csv->setFileName(newFile);

    if (csv->open(QIODevice::WriteOnly))
    {
        QTextStream stream(csv);
        stream.setCodec("windows1251");

        if (newFile.right(3).compare("csv", Qt::CaseInsensitive) == 0)
        {

            QFile* templates;
            templates = new QFile(this);
            templates->setFileName(":/res/template_etap_csv.txt");

            if (templates->open(QIODevice::ReadOnly))
            {

                stream << etapInfos.at(numberEtap).text.at(0);
                stream << "\n";
                stream << etapInfos.at(numberEtap).text.at(1);
                stream << "\n";
                if (!etapInfos.at(numberEtap).text.at(2).isEmpty())
                {
                    stream << etapInfos.at(numberEtap).text.at(2);
                    stream << "\n";
                }
                stream << QString("Формат: %1").arg(etapInfos.at(numberEtap).format, 0, 'f', 1);
                stream << "\n";

                QTextStream temp(templates);
                temp.setCodec("windows1251");
                QString head = temp.readLine();

                stream << head;
                stream << "\n";

                for (int i = 0; i < etapProtoclos.at(numberEtap).text.length(); i++)
                {
                    stream << etapProtoclos.at(numberEtap).text.at(i);
                    if (etapProtoclos.at(numberEtap).text.at(i).right(1) != "\n")
                        stream << "\n";
                }

                templates->close();

                QMessageBox::warning(this, "Внимание", "Экспорт произведен успешно!!!");
            }
            else
                QMessageBox::warning(this, "Ошибка", "Не найден файл заголовка template_csv.txt!!!");

            if (templates)
                delete templates;
        }

        csv->close();
    }
    else
        QMessageBox::warning(this, "Ошибка", "Ошибка экспорта!!!");

    if (csv)
        delete csv;

    return;

}

//информация о лидерах
void RogaineCup::ExportLeaders()
{
    if (protocolcurrent->number <= 0)
    {
        QMessageBox::warning(this, "Ошибка", "Ни один этап не учтён!!!");
        return;
    }
    //диалоговое окно
    QString newFile = QFileDialog::getSaveFileName(this, QString("Экспорт лидеров общего зачёта"), QDir::currentPath(), QString("Таблица CSV (*.csv);;Файл html (*.html)"));

    if (newFile.isEmpty())
        return;

    QFile* csv;
    csv = new QFile(this);
    csv->setFileName(newFile);

    if (csv->open(QIODevice::WriteOnly))
    {
        QTextStream stream(csv);
        stream.setCodec("windows1251");

        stream << set->currentsettings->text.at(2);
        stream << "\n";
        stream << QString("Текущие лидеры после %1 этапов").arg(protocolcurrent->number);
        stream << "\n";
        stream << "\n";

        QStringList leaders = TxtWork->getLeadersForGroups(protocolcurrent->text, set->currentsettings->text.at(0));

        for (int i = 0; i < leaders.length(); i++)
        {
            QStringList stroki = leaders.at(i).split("---");
            for (int j = 0; j < stroki.length(); j++)
            {
                stream << stroki.at(j);
                stream << "\n";
            }

            stream << "\n";
        }

        csv->close();

        QMessageBox::warning(this, "Внимание", "Экспорт произведен успешно!!!");

    }
    else
        QMessageBox::warning(this, "Ошибка", "Ошибка экспорта!!!");

    if (csv)
        delete csv;

    return;

}

//редактировать запись общей суммы
void RogaineCup::slotCustomMenuRequested(QPoint pos)
{
    /* Создаем объект контекстного меню */
    QMenu * menu = new QMenu(this);
    /* Создаём действия для контекстного меню */
    QAction * editDevice = new QAction("Редактировать участника", this);
    QAction * deleteDevice = new QAction("Удалить участника", this);
    /* Подключаем СЛОТы обработчики для действий контекстного меню */
    ui->tableWidget->clearSelection();
    connect(editDevice, SIGNAL(triggered()), this, SLOT(slotEditRecord()));     // Обработчик вызова диалога редактирования
    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemoveRecord())); // Обработчик удаления записи
    /* Устанавливаем действия в меню */
    menu->addAction(editDevice);
    menu->addAction(deleteDevice);
    /* Вызываем контекстное меню */
    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));

    positionTable = pos;

    return;
}

void RogaineCup::slotEditRecord()
{
    QTableWidgetItem * items = ui->tableWidget->itemAt(positionTable);
    int row = items->row();

    particip->init(ui->tableWidget->item(row, 1)->text(), ui->tableWidget->item(row, 4)->text(), ui->tableWidget->item(row, 3)->text(), ui->tableWidget->item(row, 5)->text());
    particip->show();

    setEnabled(false);

    return;
}

void RogaineCup::slotRemoveRecord()
{
    QTableWidgetItem * items = ui->tableWidget->itemAt(positionTable);
    int row = items->row();
    protocolcurrent->text.removeAt(row);
    //2018/05/22 не убираем количество этапов
    //protocolcurrent->number--;
    geninfo->numberpart--;

    QStringList list;
    TxtWork->UpdateSumma(protocolcurrent->text, &list, set->currentsettings->text.at(0), set->currentsettings->summa);
    protocolcurrent->text = list;
    CheckFileSize();

    QMessageBox::warning(this, "Внимание", "Запись удалена из базы!!! Места в группе обновлены");

    return;
}

void RogaineCup::slotCancelEdit()
{
    particip->hide();
    setEnabled(true);
    return;
}

void RogaineCup::slotOkEdit(QString group, QString region, QString name, QString gr)
{
    int row = ui->tableWidget->itemAt(positionTable)->row();

    QString new_text = protocolcurrent->text.at(row);
    QStringList new_list = new_text.split(";");
    int ll = new_list.length();

    new_list.replace(1, group);
    new_list.replace(3, name);
    new_list.replace(4, region);
    new_list.replace(5, gr);

    new_text.clear();

    for (int i = 0; i < ll; i++)
    {
        new_text += new_list.at(i);
        new_text += ";";
    }

    new_text.chop(1);

    protocolcurrent->text.replace(row, new_text);

    particip->hide();

    QStringList list;
    TxtWork->UpdateSumma(protocolcurrent->text, &list, set->currentsettings->text.at(0), set->currentsettings->summa);
    protocolcurrent->text = list;

    CheckFileSize();

    QMessageBox::warning(this, "Внимание", "Запись в базе изменена!!! Места в группе обновлены");

    setEnabled(true);
    return;
}


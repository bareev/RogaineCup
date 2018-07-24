#include <structs.h>
#include <QFile>
#include <settings.h>
#include <textandmath.h>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>

Protocol::Protocol()
{
    errorReading.clear();
    errorReading << "Нет подходящего файла протокола" << "Не удаётся открыть файл протокола" <<
                    "Не хватает данных протокола в строке" << "Группы в протоколе не соответствуют группам в настройках" <<
                    "Разная размерность групп и остальной информации в строке" << "Результат не является целым числом" <<
                    "Время задано неправильно";
    txt = new TextAndMath;
}

Protocol::~Protocol()
{
    if (txt)
        delete txt;
}


int Protocol::ReadProtocol(QString fileName, SettingsDS genSets, int* errorStroka)
{
    int strerror = 0;

    if (fileName.isEmpty())
    {
        *errorStroka = strerror;
        return 1;
    }

    QFile* protocol;
    protocol = new QFile;
    protocol->setFileName(fileName);
    bool openfile = protocol->open(QIODevice::ReadOnly);

    if (!openfile)
    {
        *errorStroka = strerror;
        if (protocol)
            delete protocol;
        return 2;
    }

    prot.text.clear();
    prot.number = 0;


    QString stroka;
    QStringList listStroka;
    QTextStream stream(protocol);
    stream.setCodec("windows1251");
    //индексы с многомерными данными
    QList<int> array;
    array.clear();
    array << 1 << 2 << 3 << 4;
    //пропуск заголовка
    stroka = QString(stream.readLine());
    strerror++;

    while (!stream.atEnd())
    {
        stroka = QString(stream.readLine());
        strerror++;
        listStroka.clear();
        listStroka = stroka.split(genSets.text.at(1));

        //пропуск строки
        if ((listStroka.isEmpty()) || (listStroka.length() == 1))
            continue;

        if (listStroka.length() < 7)
        {
            *errorStroka = strerror;
            protocol->close();
            if (protocol)
                delete protocol;
            return 3;
        }

        bool groups = txt->compareGroup(listStroka.at(1), genSets.text.at(0));

        if (!groups)
        {
            *errorStroka = strerror;
            protocol->close();
            if (protocol)
                delete protocol;
            return 4;
        }

        bool razm = txt->razmer(listStroka, array);

        if (!razm)
        {
            *errorStroka = strerror;
            protocol->close();
            if (protocol)
                delete protocol;
            return 5;
        }

        bool rez = txt->celoe(listStroka.at(5));

        if (!rez)
        {
            *errorStroka = strerror;
            protocol->close();
            if (protocol)
                delete protocol;
            return 6;
        }

        bool time = txt->timecheck(listStroka.at(6));

        if (!time)
        {
            *errorStroka = strerror;
            protocol->close();
            if (protocol)
                delete protocol;
            return 6;
        }

        prot.text.append(QString("%1;%2;%3;%4;%5;%6;%7\n").arg(listStroka.at(0)).arg(listStroka.at(1)).arg(listStroka.at(2)).arg(listStroka.at(3)).arg(listStroka.at(4)).arg(listStroka.at(5)).arg(listStroka.at(6)));
    }

    prot.number = strerror - 1;

    protocol->close();
    if (protocol)
        delete protocol;

    return 0;
}

void Protocol::GenMessage(int number, int stroka)
{
    if (number == 0)
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Внимание! Ошибка!");
    msgBox.setText(QString("ПРОТОКОЛ НЕ ДОБАВЛЕН!!! Причина: %1. Строка: %2").arg(errorReading.at(number - 1)).arg(stroka));
    msgBox.exec();

    return;

}

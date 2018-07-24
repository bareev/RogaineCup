#include "textandmath.h"
#include <QStringList>
#include <QDebug>
#include <math.h>

TextAndMath::TextAndMath()
{
}


bool TextAndMath::compareGroup(QString strokaGR, QString settingsGR)
{
    QStringList settingsGroup = settingsGR.split(",");
    QStringList strokaGroup = strokaGR.split(",");

    int counter = 0;

    for (int i = 0; i < strokaGroup.length(); i++)
    {
        for (int j = 0; j < settingsGroup.length(); j++)
        {
            QString currGR = strokaGroup.at(i).trimmed();
            QString currSetGR = settingsGroup.at(j).trimmed();
            //сравнение групп

            int compare = currGR.compare(currSetGR, Qt::CaseInsensitive);

            if (compare == 0)
            {
                counter++;
                break;
            }

        }
    }


    if (counter == strokaGroup.length())
        return true;
    else
        return false;
}

bool TextAndMath::razmer(QStringList stroka, QList<int> array)
{
    int curlength = 0;
    for (int i = 0; i < array.length(); i++)
    {
        int partlenght = QStringList(stroka.at(array.at(i)).split(",")).length();
        if (i == 0)
            curlength = partlenght;
        if (curlength != partlenght)
        {
            return false;
        }
        else
            curlength = partlenght;
    }
    return true;
}

bool TextAndMath::celoe(QString str)
{
    bool ok = false;

    double drob = str.toDouble(&ok);
    if (!ok)
        return false;

    int cel = str.toInt(&ok);
    if (!ok)
        return false;

    if (fabs((double)(cel) - drob) != 0)
        return false;

    return true;
}

bool TextAndMath::timecheck(QString str)
{
    bool ok = false;
    QStringList times = str.split(":");
    if (times.length() != 3)
        return false;

    for (int i = 0; i < 3; i++)
    {
        int hms = times.at(i).toInt(&ok);
        if (!ok)
            return false;
    }

    return true;
}

//сосчитаем новый протокол
bool TextAndMath::WriteNewSumma(QStringList cursumma, QStringList summa, QStringList *newsumma, bool calc, double koeff, double format, int ns, QString settingsGR, int kolvostarts)
{
    QStringList participall;   
    participall.clear();
    int offset = 6;

    double max = calcRes(summa, koeff, format);

    for (int i = 0; i < summa.length(); i++)
    {
        QStringList participList = summa.at(i).split(";").at(2).split(",");
        for (int j = 0; j < participList.length(); j++)
        {
            QString PeopleProtocol = participList.at(j).trimmed().replace("ё","е").replace("Ё", "Е").remove(" ");
            bool findd = false;
            int idxx = 0;

            if (!cursumma.isEmpty())
            {
                for (int k = 0; k < cursumma.length(); k++)
                {
                    int compare = PeopleProtocol.compare(cursumma.at(k).split(";").at(3).trimmed().replace("ё","е").replace("Ё", "Е").remove(" "), Qt::CaseInsensitive);
                    if (compare == 0)
                    {
                        findd = true;
                        idxx = k;
                        break;
                    }
                }

                if (findd)
                {
                    QString text = cursumma.at(idxx);
                    QString textAppend;
                    if (calc)
                        textAppend = QString("%1").arg(((resultTrue(summa.at(i).split(";").at(5).toInt(), koeff, format, summa.at(i).split(";").at(6)) / max) * koeff * 1000), 0, 'f', 3);
                    else
                        textAppend = "+";
                    WriteToPos(&text, ns, textAppend, offset);
                    participall.append(text);

                }
                else
                {
                    //новая строка общей суммы
                    QString newprtcp = newPeople(summa.at(i), ns, j);
                    QString textAppend;
                    if (calc)
                    {
                        textAppend = QString("%1").arg(((resultTrue(summa.at(i).split(";").at(5).toInt(), koeff, format, summa.at(i).split(";").at(6)) / max) * koeff * 1000), 0, 'f', 3);
                        newprtcp += textAppend;
                    }
                    else
                    {
                        textAppend = "+";
                        newprtcp += "0.000";
                    }

                    WriteToPos(&newprtcp, ns, textAppend, offset);
                    participall.append(newprtcp);

                }

            }

            //создаём новую сумму!!!
            else
            {
                QString newprtcp = newPeople(summa.at(i), ns, j);
                QString textAppend;
                if (calc)
                {
                    textAppend = QString("%1").arg(((resultTrue(summa.at(i).split(";").at(5).toInt(), koeff, format, summa.at(i).split(";").at(6)) / max) * koeff * 1000), 0, 'f', 3);
                    newprtcp += textAppend;
                }
                else
                {
                    textAppend = "+";
                    newprtcp += "0.000";
                }

                WriteToPos(&newprtcp, ns, textAppend, offset);
                participall.append(newprtcp);
            }

        }

    }

    //записываем "старых"
    if (!cursumma.isEmpty())
    {
        int numberprt = participall.length();
        for (int cs = 0; cs < cursumma.length(); cs++)
        {
            QString name = cursumma.at(cs).split(";").at(3);
            bool find = false;
            for (int prt = 0; prt < numberprt; prt++)
            {
                if (name.compare(participall.at(prt).split(";").at(3), Qt::CaseInsensitive) == 0)
                {
                    find = true;
                    break;
                }
            }
            if (!find)
            {
                QString stroka = cursumma.at(cs);
                //добавляем новую строку
                WriteToPos(&stroka, ns, "", offset);
                participall.append(stroka);
            }
        }
    }

    //подсчитываем общую сумму!!!
    Summa4(&participall, kolvostarts, offset);

    //записали новую сумму в лист participall
    qSort(participall.begin(), participall.end(), compareTwoStrings);

    //записываем места по группам!!!
    NewCounter(&participall, settingsGR);

    *newsumma = participall;


    return true;
}


bool TextAndMath::UpdateSumma(QStringList cursumma, QStringList *newsumma, QString settingsGR, int kolvostarts)
{

    int offset = 6;

    //подсчитываем общую сумму!!!
    Summa4(&cursumma, kolvostarts, offset);

    //записали новую сумму в лист participall
    qSort(cursumma.begin(), cursumma.end(), compareTwoStrings);

    //записываем места по группам!!!
    NewCounter(&cursumma, settingsGR);

    *newsumma = cursumma;


    return true;
}

//ищем максимум в протоколе
double TextAndMath::calcRes(QStringList spisok, double koeff, double format)
{


    double max = resultTrue(spisok.at(0).split(";").at(5).toInt(), koeff, format, spisok.at(0).split(";").at(6));

    for (int i = 1; i < spisok.length(); i++)
    {
        double curmax = resultTrue(spisok.at(i).split(";").at(5).toInt(), koeff, format, spisok.at(i).split(";").at(6));
        if (curmax > max)
            max = curmax;
    }

    //нашли максимум, теперь записывем
    return max;

}

//пересчитываем результат
double TextAndMath::resultTrue(int resInput, double koeff, double format, QString times)
{
    double popravka = 0;

    if (resInput != 0)
        popravka = (double)(resInput) - (times.split(":").at(0).toInt() * 3600 + times.split(":").at(1).toInt() * 60 + times.split(":").at(2).toInt()) / (format * 3600 + 1800);

    return popravka;
}

//добавляем очки в нужную позицию
void TextAndMath::WriteToPos(QString *text, int pos, QString textAppend, int offset)
{
    QStringList listOfText = text->split(";");

    if ((offset + pos) < (listOfText.length() - 1))
    {
        listOfText.replace((offset + pos), textAppend);
    }
    else
    {
        for (int i = listOfText.length(); i < (offset + pos); i++)
            listOfText.append("");

        listOfText.append(textAppend);
    }

    QString txt;
    txt.clear();

    txt = QString(listOfText.at(0));

    for (int i = 1; i < listOfText.length(); i++)
    {
        txt += QString(";%1").arg(listOfText.at(i));
    }

    *text = txt;

    return;
}

//записываем новую строку нового формата
QString TextAndMath::newPeople(QString currentPeopleStroka, int ns, int np)
{
    QStringList cps = currentPeopleStroka.split(";");

    QString Text;
    Text.clear();

    Text += "1;";
    Text += cps.at(1).split(",").at(np).trimmed();
    Text += ";";
    Text += QString(cps.at(1).split(",").at(np).trimmed() + ": 1;");
    Text += cps.at(2).split(",").at(np).trimmed();
    Text += ";";
    Text += cps.at(3).split(",").at(np).trimmed();
    Text += ";";
    Text += cps.at(4).split(",").at(np).trimmed();
    Text += ";";

    return Text;
}

//сортировка двух строк
bool TextAndMath::compareTwoStrings(QString stroka1, QString stroka2)
{
    QStringList list1 = stroka1.split(";");
    QStringList list2 = stroka2.split(";");

    if ((list1.length() < 7) || (list2.length() < 7))
        return true;

    double summa1 = list1.at(6).toDouble();
    double summa2 = list2.at(6).toDouble();

    return (summa1 > summa2);

}

//записываем по группам
void TextAndMath::NewCounter(QStringList *newList, QString settingsGR)
{
    QStringList groups = settingsGR.split(",");
    QList<int> counter;
    QList<double> res;
    QList<int> odinak;
    counter.clear();

    //загнать в настройки!!!
    QStringList svyaz;
    svyaz.append("");
    svyaz.append("0");
    svyaz.append("0");
    svyaz.append("0,2");
    svyaz.append("0,2,3");

    svyaz.append("");
    svyaz.append("5");
    svyaz.append("5");
    svyaz.append("5,7");
    svyaz.append("5,7,8");

    for (int s = 0; s < groups.length(); s++)
    {
        counter.append(0);
        res.append(-1.0);
        odinak.append(1);
    }

    int placeTotalInt = 0;

    QStringList fulltext;
    fulltext = *newList;

    QString mestoGroup;
    QString group;

    for (int i = 0; i < fulltext.length(); i++)
    {
        QString strokaCurr = fulltext.at(i);
        QStringList strokaCurrList = strokaCurr.split(";");
        QString placeTotal;

        //общее место вписываем
        if (i != 0)
        {
            if (strokaCurrList.at(6).toDouble() < fulltext.at(i - 1).split(";").at(6).toDouble())
                placeTotalInt = (i + 1);
        }
        else
            placeTotalInt++;

        placeTotal = QString("%1").arg(placeTotalInt);

        //место по группам вписываем

        group = strokaCurrList.at(1);

        for (int j = 0; j < groups.length(); j++)
        {
            QString gc = groups.at(j).trimmed();

            int tr = group.compare(gc, Qt::CaseInsensitive);

            if (tr == 0)
            {
                if ((strokaCurrList.at(6).toDouble() < res.at(j)) || (res.at(j) == -1))
                {
                    counter.replace(j, counter.at(j) + odinak.at(j));
                    odinak.replace(j, 1);
                }
                else
                    odinak.replace(j, odinak.at(j) + 1);

                res.replace(j, strokaCurrList.at(6).toDouble());

                mestoGroup = QString("%1: %2").arg(gc).arg(counter.at(j));

                //участие в других группах
                bool ok = false;
                QList<int> svz = getsvyaz(j, svyaz, &ok);

                if (ok)
                {
                    for (int countsv = 0; countsv < svz.length(); countsv++)
                    {
                        if ((strokaCurrList.at(6).toDouble() < res.at(svz.at(countsv))) || (res.at(svz.at(countsv)) == -1))
                        {
                            counter.replace(svz.at(countsv), counter.at(svz.at(countsv)) + odinak.at(svz.at(countsv)));
                            odinak.replace(svz.at(countsv), 1);
                        }
                        else
                            odinak.replace(svz.at(countsv), odinak.at(svz.at(countsv)) + 1);

                        res.replace(svz.at(countsv), strokaCurrList.at(6).toDouble());

                        mestoGroup = mestoGroup + QString(", %1: %2").arg(groups.at(svz.at(countsv)).trimmed()).arg(counter.at(svz.at(countsv)));
                    }
                }

                break;
            }
            else
                mestoGroup.clear();
        }

        strokaCurrList.replace(0, placeTotal);
        strokaCurrList.replace(2, mestoGroup);

        strokaCurr.clear();

        for (int comb = 0; comb < strokaCurrList.length(); comb++)
        {
            strokaCurr += strokaCurrList.at(comb);
            strokaCurr += ";";
        }
        strokaCurr.chop(1);

        fulltext.replace(i, strokaCurr);

    }

    *newList = fulltext;

    return;

}

//связь групп
QList<int> TextAndMath::getsvyaz(int numbergr, QStringList spisok, bool *yes)
{
    bool res = false;
    QList<int> result;
    result.clear();

    //ничего
    if (spisok.at(numbergr).isEmpty())
    {
        *yes = res;
        return result;
    }
    else
    {
        if (spisok.at(numbergr).indexOf(",") == -1)
        {
            result.append(spisok.at(numbergr).toInt());
            res = true;
        }
        else
        {
            QStringList grs = spisok.at(numbergr).split(",");
            for (int i = 0; i < grs.length(); i++ )
                result.append(grs.at(i).toInt());
            res = true;
        }

    }

    *yes = res;
    return result;
}

//общая сумма
void TextAndMath::Summa4(QStringList *newList, int kolvostarts, int offset)
{
    double summa, curcount;
    QStringList fulllist = *newList;
    for (int i = 0; i < fulllist.length(); i++)
    {
        summa = curcount = 0;
        QStringList participiant = fulllist.at(i).split(";");
        QList<double> ochko;
        ochko.clear();
        for (int j = (offset + 1); j < participiant.length(); j++)
        {
            if (participiant.at(j).compare("+") != 0)
                ochko.append(participiant.at(j).toDouble());
        }

        int counter = 0;
        int idx;
        while (counter < kolvostarts)
        {
            if (ochko.isEmpty())
                break;

            curcount = max(ochko, &idx);
            if (idx != -1)
            {
                summa += curcount;
                ochko.removeAt(idx);
                counter++;
            }
        }

        participiant.replace(offset, QString("%1").arg(summa, 0, 'f', 3));
        QString stroka;
        stroka.clear();

        for (int pp = 0; pp < participiant.length(); pp++)
        {
            stroka += participiant.at(pp);
            stroka += ";";
        }

        stroka.chop(1);

        fulllist.replace(i, stroka);


    }

    *newList = fulllist;
    return;
}

//максимум
double TextAndMath::max(QList<double> array, int *idx)
{
    int res = -1;
    double result = -1;

    for (int i = 0; i < array.length(); i++)
    {
        if (array.at(i) > result)
        {
            result = array.at(i);
            res = i;
        }
    }

    *idx = res;
    return result;
}

//лидеры
QStringList TextAndMath::getLeadersForGroups(QStringList curprot, QString settingsGR)
{
    QStringList ret;
    ret.clear();
    QStringList groups = settingsGR.split(",");

    for (int gr = 0; gr < groups.length(); gr++)
        ret.append(QString("Группа %1").arg(groups.at(gr).trimmed()));

    for (int j = 0; j < curprot.length(); j++)
    {
        QString MestoUch = curprot.at(j).split(";").at(2);
        QStringList listofgr = MestoUch.split(",");

        for (int k = 0; k < listofgr.length(); k++)
        {
            QString groupFromSpisok = listofgr.at(k).split(":").at(0).trimmed();
            int groupFromSpisokRes = listofgr.at(k).split(":").at(1).trimmed().toInt();

            for (int i = 0; i < groups.length(); i++)
            {
                QString groupCurrent = groups.at(i).trimmed();

                if (groupCurrent.compare(groupFromSpisok, Qt::CaseInsensitive) == 0)
                {
                    if ((groupFromSpisokRes == 1) || (groupFromSpisokRes == 2) || (groupFromSpisokRes == 3))
                    {
                        ret.replace(i, QString(ret.at(i) + "---" + QString("%1").arg(groupFromSpisokRes) + ";" + curprot.at(j).split(";").at(3) + ";" + curprot.at(j).split(";").at(6)));
                    }
                    break;
                }

            }
        }
    }

    return ret;
}



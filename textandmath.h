#ifndef TEXTANDMATH_H
#define TEXTANDMATH_H
#include <QString>

class TextAndMath
{
public:
    TextAndMath();
    bool compareGroup(QString strokaGR, QString settingsGR);
    bool razmer(QStringList stroka, QList<int> array);
    bool celoe(QString str);
    bool timecheck(QString str);
    QStringList getLeadersForGroups(QStringList curprot, QString settingsGR);
    //запись нового протокола
    bool WriteNewSumma(QStringList cursumma, QStringList summa, QStringList *newsumma, bool calc, double koeff, double format, int ns, QString settingsGR, int kolvostarts);

private:
    double calcRes(QStringList spisok, double koeff, double format);
    double resultTrue(int resInput, double koeff, double format, QString times);
    void WriteToPos(QString *text, int pos, QString textAppend, int offset);
    QString newPeople(QString currentPeopleStroka, int ns, int np);
    static bool compareTwoStrings(QString stroka1, QString stroka2);
    void NewCounter(QStringList *newList, QString settingsGR);
    void Summa4(QStringList *newList, int kolvostarts, int offset);
    QList<int> getsvyaz(int numbergr, QStringList spisok, bool* yes);
    double max(QList<double> array, int* idx);
};

#endif // TEXTANDMATH_H

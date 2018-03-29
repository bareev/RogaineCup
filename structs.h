#ifndef STRUCTS_H
#define STRUCTS_H
#include <QStringList>
#include <QPixmap>
#include <textandmath.h>

//настройки
/*struct GenSettings
{
    char* title;
    int summa;
    char* sep;
    char* groups;
    bool priznak;
};*/

//информация об общем зачёте
struct generalpos
{
    int numetaps;
    int numberpart;
};

//информация об этапе
/*struct etapInfo
{
    char* title;
    char* date;
    double koeff;
    double format;
    int etapnumber;
    QPixmap map;
    char* comment;
};*/

//структура протокола конкретного старта или текущего зачёта
/*struct protocol
{
    char* line;
    bool priznak;
};*/


//для записи в DataStream
struct SettingsDS
{
    QStringList text;
    int summa;
    bool priznak;
};

struct ProtocolDS
{
    QStringList text;
    int number;
};

struct etapDS
{
    QStringList text;
    double koeff;
    double format;
    int etapnumber;
    bool calc;
    QPixmap map;
};

class Protocol
{
    public:
        explicit Protocol();
        ~Protocol();
        int ReadProtocol(QString fileName, SettingsDS genSets, int *errorStroka);
        void GenMessage(int number, int stroka);
        ProtocolDS prot;
        TextAndMath* txt;

    private:
        QStringList errorReading;


};


#endif // STRUCTS_H

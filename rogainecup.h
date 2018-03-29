#ifndef ROGAINECUP_H
#define ROGAINECUP_H

#include <QMainWindow>
#include <settings.h>
#include <structs.h>
#include <addnewfile.h>
#include <QFile>
#include "editparticipiant.h"


namespace Ui {
class RogaineCup;
}

class RogaineCup : public QMainWindow
{
    Q_OBJECT

public:
    explicit RogaineCup(QWidget *parent = 0);
    ~RogaineCup();

    friend QDataStream & operator <<(QDataStream &stream, const SettingsDS &sC);
    friend QDataStream & operator >>(QDataStream &stream, SettingsDS &sC);

    friend QDataStream & operator <<(QDataStream &stream, const ProtocolDS &sC);
    friend QDataStream & operator >>(QDataStream &stream, ProtocolDS &sC);

    friend QDataStream & operator <<(QDataStream &stream, const etapDS &sC);
    friend QDataStream & operator >>(QDataStream &stream, etapDS &sC);

    friend QDataStream & operator <<(QDataStream &stream, const generalpos &sC);
    friend QDataStream & operator >>(QDataStream &stream, generalpos &sC);


private:
    Ui::RogaineCup *ui;
    bool openfile;
    Settings *set;
    generalpos* geninfo;
    AddNewFile* newData;
    EditParticipiant* particip;
    ProtocolDS* protocolcurrent;
    QList<etapDS> etapInfos;
    QList<ProtocolDS> etapProtoclos;
    QFile* openQFile;
    TextAndMath* TxtWork;
    QPoint positionTable;

private slots:
    void onExit();
    void enableAll(bool pr);
    void onOpenNewFile();
    void onOpenExistFile();
    void onSaveFile();
    void onCloseFile();
    void onSettingsClicked();
    void onAddNewFile();

    //блок записи
    void WriteGenSets(QFile* file);
    void WriteGenInfo(QFile* file);
    void WriteCurSumma(QFile* file);
    void WriteEtapInfo(QFile* file, int i);
    void WriteEtapProt(QFile* file, int i);

    //блок считывания
    void ReadGenSets(QFile* file);
    void ReadGenInfo(QFile* file);
    void ReadCurSumma(QFile* file);
    void ReadEtapInfo(QFile* file, int i);
    void ReadEtapProt(QFile* file, int i);

    //сигналы от формы настроек
    void SaveSets();
    void CancelSets();

    //сигналы от формы нового файла
    void NewFileAdd(ProtocolDS,etapDS);
    void NewFileCancel();

    //заполнение формы
    void GetInfoFromMemoryToForm();
    void CheckFileSize();

    //экспорты
    void ExportAllSumm();
    void ExportEtapMap();
    void ExportEtapInfo();
    void ExportLeaders();

    //меню редактировать запись
    void slotCustomMenuRequested(QPoint);
    void slotEditRecord();
    void slotRemoveRecord();

    //отмена и подтверждение редактирования участника
    void slotCancelEdit();
    void slotOkEdit(QString group, QString region, QString name, QString gr);

};



#endif // ROGAINECUP_H

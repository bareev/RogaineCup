#ifndef ADDNEWFILE_H
#define ADDNEWFILE_H

#include <QWidget>
#include <structs.h>

namespace Ui {
class AddNewFile;
}

class AddNewFile : public QWidget
{
    Q_OBJECT

public:
    explicit AddNewFile(QWidget *parent = 0);
    ~AddNewFile();
    void init(int i, SettingsDS set);

private:
    Ui::AddNewFile *ui;
    Protocol* protocolCur;
    etapDS* etapCur;
    SettingsDS sets;

signals:
    void OnNewFile(ProtocolDS, etapDS);
    void OnCancel();

private slots:
    void OnAddProt();
    void OnOk();
    void OnC();
    void OnMapAdd();

};

#endif // ADDNEWFILE_H

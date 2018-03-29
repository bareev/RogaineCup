#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAbstractButton>
#include <structs.h>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private:
    Ui::Settings *ui;
    SettingsDS* defaultsettings;

public:
    SettingsDS* currentsettings;

private slots:
    void checked(bool check);
    void onClicked();
    void onCancelClicked();

public slots:
    void onLoad(SettingsDS sets);
    void onDefLoad();
    SettingsDS newSets();

signals:
    void closeSets();
    void saveSets();
};

#endif // SETTINGS_H

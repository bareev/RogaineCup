#ifndef EDITPARTICIPIANT_H
#define EDITPARTICIPIANT_H

#include <QWidget>

namespace Ui {
class EditParticipiant;
}

class EditParticipiant : public QWidget
{
    Q_OBJECT

public:
    explicit EditParticipiant(QWidget *parent = 0);
    ~EditParticipiant();
    void init(QString group, QString region, QString name, QString gr);

private:
    Ui::EditParticipiant *ui;

private slots:
    void onOkSlot();
    void onCancelSlot();

signals:
    void onOk(QString group, QString region, QString name, QString gr);
    void onCancel();
};

#endif // EDITPARTICIPIANT_H

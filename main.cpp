#include <QApplication>
#include <QTextCodec>
#include <rogainecup.h>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    QApplication a(argc, argv);
    RogaineCup w;
    w.show();

    return a.exec();
}

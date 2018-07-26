#include <QApplication>
#include <QTextCodec>
#include <rogainecup.h>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

    QApplication a(argc, argv);
    RogaineCup w;

    if (argc > 1)
    {
        QString namefile = QString(argv[1]);
        w.onOpenExistFileCmd(namefile);
    }

    w.show();

    return a.exec();
}

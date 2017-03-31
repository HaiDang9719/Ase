#include "uhv2testapp.h"
#include <QApplication>
#include "serialinterface.h"
#include <QThread>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UHV2TestApp w;
    w.show();

    return a.exec();
}

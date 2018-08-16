#include "hw/interface.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Protocoller");
    a.setOrganizationName("XrSoft");

    hwi hwi_;
    Q_UNUSED(hwi_)

    MainWindow w;
    w.show();

    return a.exec();
}

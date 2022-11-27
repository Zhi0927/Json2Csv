#include "QT_Json2Csv.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QT_Json2Csv w;
    w.show();
    return a.exec();
}

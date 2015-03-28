// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QtDebug>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // установка значений, используемых в работе с настройками программы (QSettings)
    QCoreApplication::setOrganizationName("DharmaSoft");
    QCoreApplication::setApplicationName("SunMoonTime");

    // подгрузка русских переводов для элементов Qt
    QTranslator translator(&a);
    translator.load("qtbase_ru");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}

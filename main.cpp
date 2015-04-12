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
    bool result (false);
    result = translator.load("qtbase_ru");
    result = a.installTranslator(&translator);
    if (false == result)
        qWarning() << "main" << "load translator error";

    MainWindow w;
    w.show();

    return a.exec();
}

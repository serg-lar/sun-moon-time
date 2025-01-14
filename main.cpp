// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QtDebug>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // установка значений, используемых в работе с настройками программы (QSettings)
    QCoreApplication::setOrganizationName("KulaBhaktiSoft");
    // Здесь нужно задать имя приложения, так как имя выполняемого файла берётся до первой точки
    // и в таком случае не будет соответствовать полному имени файла приложения.
    QCoreApplication::setApplicationName("sun-moon-time");

    // не выходить из программы при закрытии окон
    a.setQuitOnLastWindowClosed(false);

    // подгрузка русских переводов для элементов Qt
//    QTranslator translator(&a);
//    bool result (false);
//    result = translator.load(QApplication::applicationDirPath()+"/"+"qtbase_ru.qm");
//    result = a.installTranslator(&translator);
//    if (false == result)
//        qWarning() << "main" << "load translator error";

    // Центрирование главного окна на экране.
    MainWindow w;
    QDesktopWidget* desktopWidget = QApplication::desktop();

    qint32 width (w.frameGeometry().width());
    qint32 height (w.frameGeometry().height());
    qint32 screenWidth (desktopWidget->screen()->width());
    qint32 screenHeight (desktopWidget->screen()->height());
    w.setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);

    // Отображать главное окно только если в параметрах запуска не было указано обратное.
    QStringList arguments;
    for (qint32 i = 0; i < argc; ++i)
        arguments << argv[i];
    if ((false == arguments.contains("-m")) && (false == arguments.contains("--minimized"))) {
        w.show();
    }
    else {
        w.afterShow();
    }

    return a.exec();
}

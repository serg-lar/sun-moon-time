#ifndef GENERALMISC_H
#define GENERALMISC_H
// Qt
#include <QtCore>
#include <QString>


/// \brief Разные утилиты для главного модуля приложения
class SunMoonTimeGeneralMisc {
public:
    /// \brief Ошибки и их описания
    class errors {
    public:
        static QString fileOpenError() {return QObject::tr("File open error");}
    };
    //---------
};
//---------------------------


#endif // GENERALMISC_H

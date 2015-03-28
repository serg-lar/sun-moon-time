#ifndef CALENDAR_H
#define CALENDAR_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QtGlobal>

//#include "aaplus/AA+.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: Классы
/// \class Ведический календарь
class TVedicCalendar
{
public:
//    TVedicCalendar();
//    ~TVedicCalendar();

    /// \brief Получить номер заданного года в Кали-юге, основываясь на общеизвестной дате начала Кали-юги
    /// по Грегорианскому календарю 18 февраля 3102 года до нашей эры
    /// \param year - номер года по Грегорианскому календарю
    /// \retval номер года (1 - 432.000), в случае не верного аргумента возвращается 0
    static quint64 gregorianToVedicKaliYuga(const qint32 year);

    /// \brief Получить номер заданного года в Маха-юге, основываясь на общеизвестной дате начала Кали-юги
    /// \param year - номер года по Грегорианскому календарю
    /// \retval номер года (1 - 4.320.000), в случае не верного аргумента возвращается 0
    static quint64 gregorianToVedicMahaYuga(const qint32 year);

    /// \brief Получить номер заданного года в Манвантаре, основываясь на общеизвестной дате начала Кали-юги
    /// \param year - номер года по Грегорианскому календарю
    /// \retval номер года (1 - 30.672.000), в случае не верного аргумента возвращается 0
    static quint64 gregorianToVedicManvantara(const qint32 year);

    /// \brief Получить номер заданного года в Кальпе, основываясь на общеизвестной дате начала Кали-юги
    /// \param year - номер года по Грегорианскому календарю
    /// \retval номер года (1 - 4.320.000.000), в случае не верного аргумента возвращается 0
    static quint64 gregorianToVedicKalpa(const qint32 year);

    /// \brief Получить номер заданного года в МахаКальпе, основываясь на общеизвестной дате начала Кали-юги
    /// \param year - номер года по Грегорианскому календарю
    /// \retval номер года (1 - 311.040.000.000.000), в случае не верного аргумента возвращается 0
    static quint64 gregorianToVedicMahaKalpa(const qint32 year);

    /// \brief отформатировать большое число ведического номера года точками разделяющими порядки
    /// \param year - год по ведическому летоисчислению
    /// \retval номер года в виде строки с точками-разделителями, вслучае ошибки пустая строка
    static QString formatLongYearWithDots(const quint64 year);

protected:
    // Временные промежутки ведического летоисчисления в годах человеческого уровня
    enum VedicTimePeriods : quint64
    {
        charana = 432000,
        satyaYugaLen = 4*charana,
        tretaYugaLen = 3*charana,
        dvaparaYugaLen = 2*charana,
        kaliYugaLen = charana,
        mahaYugaLen = satyaYugaLen + tretaYugaLen + dvaparaYugaLen + kaliYugaLen,
        manvantaraNum = 7,
        mahaYugaInManvantaraNum = 28,
        manvantaraLen = 71*mahaYugaLen,
        manvantaraFullLen = 71*mahaYugaLen + 4*charana,
        kalpaLen = 14*manvantaraFullLen + 4*charana,
        brahmaDayLen = kalpaLen,
        universeDayLen = kalpaLen,
        brahmaNightLen = brahmaDayLen,
        universeNightLen = universeDayLen,
        brahmaTwentyFourHours = brahmaDayLen + brahmaNightLen,
        universeTwentyFourHours = universeDayLen + universeNightLen,
        brahmaYearLen = 360*brahmaTwentyFourHours,
        universeYearLen = 360*universeTwentyFourHours,
        parardha = 50*brahmaYearLen,
        mahaKalpaLen = 100*brahmaYearLen,
        brahmaLifeLen = mahaKalpaLen,
        universeActiveLifeLen = mahaKalpaLen,
        universeNotActiveLifeLen = universeActiveLifeLen,
        universeLifeLen = universeActiveLifeLen + universeNotActiveLifeLen
    };

private:


};
//---------------------------
// КОНЕЦ: Классы
//---------------------------------------------------------------------------------

#endif // CALENDAR_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include "calendar.h"
#include <QString>

//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: TVedicCalendar - public
quint64 TVedicCalendar::gregorianToVedicKaliYuga(const qint32 year)
{
    quint64 result (0);

    if ((year >= -3102) && (year != 0) && (year <= 428898))
    {
        if (year > 0)
            result = year + 3102;
        else
            result = 3102 + year + 1;
    }
    return result;
}
//---------------------------

quint64 TVedicCalendar::gregorianToVedicMahaYuga(const qint32 year)
{
    quint64 result (gregorianToVedicKaliYuga(year));
    if (0 != result)
        result += satyaYugaLen + tretaYugaLen + dvaparaYugaLen;
    return result;
}
//---------------------------

quint64 TVedicCalendar::gregorianToVedicManvantara(const qint32 year)
{
    quint64 result (gregorianToVedicMahaYuga(year));
    if (0 != result)
        result += (mahaYugaInManvantaraNum - static_cast<VedicTimePeriods>(1))*mahaYugaLen;
    return result;
}
//---------------------------

quint64 TVedicCalendar::gregorianToVedicKalpa(const qint32 year)
{
    quint64 result (gregorianToVedicManvantara(year));
    if (0 != result)
        result += (manvantaraNum - static_cast<VedicTimePeriods>(1))*manvantaraFullLen;
    return result;
}
//---------------------------

quint64 TVedicCalendar::gregorianToVedicMahaKalpa(const qint32 year)
{
    quint64 result (gregorianToVedicKalpa(year));
    if (0 != result)
        result += (parardha);
    return result;
}
//---------------------------

QString TVedicCalendar::formatLongYearWithDots(const quint64 year)
{
    QString str (QString::number(year));

    for (int i = str.size()-3; i > 0; i -= 3)
        str.insert(i,'.');

    return str;
}
//---------------------------
// КОНЕЦ: TVedicCalendar - public
//---------------------------------------------------------------------------------

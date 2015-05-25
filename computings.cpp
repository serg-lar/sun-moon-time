// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "computings.h"
#include <QtMath>
#include <limits>

QDateTime TComputings::m_previousNewMoon;
QDateTime TComputings::m_nextNewMoon;
quint32 TComputings::m_MoonDayNum = 0;
quint32 TComputings::m_PrevMoonDayNum = 0;
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: TComputings - public
QTime TComputings::sunTimeAverage(const double longitude)
{
    return QDateTime::currentDateTimeUtc().addSecs(sunTimeAverageOffset(longitude)).time();
}
//---------------------------

QTime TComputings::sunTimeTrue(const double longitude, const quint32 N)
{
    return QDateTime::currentDateTimeUtc().addSecs(sunTimeTrueOffset(longitude,N)).time();
}
//---------------------------

QTime TComputings::sunTimeRise(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    QTime result;
    CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
    CAARiseTransitSetDetails riseTransitSetDetails;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        double JD (d.Julian());
        riseTransitSetDetails = getSunRiseTransitSet(JD, longitude, latitude);

        if (true == riseTransitSetDetails.bRiseValid)
        {
            // получено время восхода Солнца
            CAADate riseD (JD + (riseTransitSetDetails.Rise / 24.) + (timeZoneOffset / 24.), true);
            long year, month, day, hour, minute;
            double second;
            riseD.Get(year,month,day,hour,minute,second);
            result = QTime::fromMSecsSinceStartOfDay(hour*minsInHour*secsInMin*msecsInSec + minute*secsInMin*msecsInSec + second*msecsInSec);
        }
    }

    return result;
}
//---------------------------

QTime TComputings::sunTimeSet(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    QTime result;
    CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
    CAARiseTransitSetDetails riseTransitSetDetails;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        double JD (d.Julian());
        riseTransitSetDetails = getSunRiseTransitSet(JD, longitude, latitude);

        if (true == riseTransitSetDetails.bSetValid)
        {
            // получено время захода Солнца
            CAADate setD (JD + (riseTransitSetDetails.Set / 24.) + (timeZoneOffset / 24.), true);
            long year, month, day, hour, minute;
            double second;
            setD.Get(year,month,day,hour,minute,second);
            result = QTime::fromMSecsSinceStartOfDay(hour*minsInHour*secsInMin*msecsInSec + minute*secsInMin*msecsInSec + second*msecsInSec);
        }
    }

    return result;
}
//---------------------------

QTime TComputings::sunTimeTransit(const double longitude, const double latitude, bool& aboveHorizont,
                                  const double timeZoneOffset, const QDate& date)
{
    QTime result;
    CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
    CAARiseTransitSetDetails riseTransitSetDetails;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        double JD (d.Julian());
        riseTransitSetDetails = getSunRiseTransitSet(JD, longitude, latitude);

        // получено время зенита Солнца
        aboveHorizont = riseTransitSetDetails.bTransitAboveHorizon;
        CAADate setD (JD + (riseTransitSetDetails.Transit / 24.) + (timeZoneOffset / 24.), true);
        long year, month, day, hour, minute;
        double second;
        setD.Get(year,month,day,hour,minute,second);
        result = QTime::fromMSecsSinceStartOfDay(hour*minsInHour*secsInMin*msecsInSec + minute*secsInMin*msecsInSec + second*msecsInSec);
    }

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeMorningTwilight(const double longitude, const double latitude, const double height,
                                                       const double timeZoneOffset, const double degree, const QDate& date)
{
    QPair<QTime, QTime> result;
    QDateTime dt (date,sunTimeRise(longitude,latitude,UtcOffset,date));         // UTC время восхода для начала поиска
    QPair<double, double> horizontalCoords, prevHorizontalCoords;
    qint64 step = -1*msecsInSec*secsInMin;                                      // начальный шаг поиска примем в минуту времени

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (degree >= -90) && (degree <= 0) && (true == date.isValid()))
    {
        // время завершения сумерек равно времени восхода
        result.second = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();

        // вычислить время начала сумерек
        horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).date() == date))
        {
            // горизонтальные координаты Солнца
            horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);

            // найдено ли время начала сумерек
            if (horizontalCoords.second <= degree)
            {
                // если используется крупный шаг
                if (msecsInSec*secsInMin == step)
                {
                    // шаг назад и продолжить с мелким шагом
                    dt = dt.addMSecs(-step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время начала сумерек
                    result.first = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();
                }
            }

            // шаг
            dt = dt.addMSecs(+step);
            prevHorizontalCoords = horizontalCoords;
        }
    }

    // если не найдено начало значит сумерки не определены вовсе
    if ((false == result.first.isValid()))
        result = QPair<QTime, QTime>();

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeMorningTwilight(const double longitude, const double latitude, const QTime& sunRise, const double height,
                                                       const double timeZoneOffset, const double degree, const QDate& date)
{
    QPair<QTime, QTime> result;    
    QDateTime dt (QDateTime::currentDateTimeUtc());                         // уловка, необходимая для правильной инициализации
    dt.setDate(date);
    dt.setTime(sunRise);
    dt = dt.addMSecs(-1*msecsInSec*secsInMin*minsInHour*timeZoneOffset);    // UTC время заката для начала поиска
    QPair<double, double> horizontalCoords, prevHorizontalCoords;
    qint64 step = -1*msecsInSec*secsInMin;                                  // начальный шаг поиска примем в минуту времени

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (degree >= -90) && (degree <= 0) && (true == date.isValid()) && (true == sunRise.isValid()))
    {
        // время завершения сумерек равно времени восхода
        result.second = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();

        // вычислить время начала сумерек
        horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).date() == date))
        {
            // горизонтальные координаты Солнца
            horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);

            // найдено ли время начала сумерек
            if (horizontalCoords.second <= degree)
            {
                // если используется крупный шаг
                if (msecsInSec*secsInMin == step)
                {
                    // шаг назад и продолжить с мелким шагом
                    dt = dt.addMSecs(-step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время начала сумерек
                    result.first = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();
                }
            }

            // шаг
            dt = dt.addMSecs(+step);
            prevHorizontalCoords = horizontalCoords;
        }
    }

    // если не найдено начало значит сумерки не определены вовсе
    if ((false == result.first.isValid()))
        result = QPair<QTime, QTime>();

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeMorningTwilightWithRefraction(const double longitude, const double latitude, const double height, const double pressure,
                                                                     const double temperature, const double timeZoneOffset, const double degree, const QDate& date)
{
    QPair<QTime, QTime> result;
    QDateTime dt (date,sunTimeRise(longitude,latitude,UtcOffset,date));         // UTC время восхода для начала поиска
    QPair<double, double> horizontalCoords, prevHorizontalCoords;
    qint64 step = -1*msecsInSec*secsInMin;                                      // начальный шаг поиска примем в минуту времени

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (degree >= -90) && (degree <= 0) && (true == date.isValid()))
    {
        // время завершения сумерек равно времени восхода
        result.second = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();

        // вычислить время начала сумерек
        horizontalCoords = sunHorizontalCoordsWithRefraction(longitude,latitude,height,pressure,temperature,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).date() == date))
        {
            // горизонтальные координаты Солнца
            horizontalCoords = sunHorizontalCoordsWithRefraction(longitude,latitude,height,pressure,temperature,dt);

            // найдено ли время начала сумерек
            if (horizontalCoords.second <= degree)
            {
                // если используется крупный шаг
                if (msecsInSec*secsInMin == step)
                {
                    // шаг назад и продолжить с мелким шагом
                    dt = dt.addMSecs(-step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время начала сумерек
                    result.first = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();
                }
            }

            // шаг
            dt = dt.addMSecs(+step);
            prevHorizontalCoords = horizontalCoords;
        }
    }

    // если не найдено начало значит сумерки не определены вовсе
    if ((false == result.first.isValid()))
        result = QPair<QTime, QTime>();

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeEveningTwilight(const double longitude, const double latitude, const double height, const double timeZoneOffset,
                                                       const double degree, const QDate& date)
{
    QPair<QTime, QTime> result;
    QDateTime dt (date,sunTimeSet(longitude,latitude,UtcOffset,date));      // UTC время заката для начала поиска
    QPair<double, double> horizontalCoords, prevHorizontalCoords;
    qint64 step = msecsInSec*secsInMin;                                     // начальный шаг поиска примем в минуту времени

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (degree >= -90) && (degree <= 0) && (true == date.isValid()))
    {
        // время начала сумерек равно времени заката
        result.first = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();

        // вычислить время завершения сумерек
        horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).date() == date))
        {
            // горизонтальные координаты Солнца
            horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);

            // найдено ли время завершения сумерек
            if (horizontalCoords.second <= degree)
            {
                // если используется крупный шаг
                if (msecsInSec*secsInMin == step)
                {
                    // шаг назад и продолжить с мелким шагом
                    dt = dt.addMSecs(-step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время завершения сумерек
                    result.second = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();
                }
            }

            // шаг
            dt = dt.addMSecs(+step);
            prevHorizontalCoords = horizontalCoords;
        }
    }

    // если не найдено завершение значит сумерки не определены вовсе
    if ((false == result.second.isValid()))
        result = QPair<QTime, QTime>();

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeEveningTwilight(const double longitude, const double latitude, const QTime& sunSet, const double height,
                                                       const double timeZoneOffset, const double degree, const QDate& date)
{
    QPair<QTime, QTime> result;
    QDateTime dt (QDateTime::currentDateTimeUtc());                         // уловка, необходимая для правильной инициализации
    dt.setDate(date);
    dt.setTime(sunSet);
    dt = dt.addMSecs(-1*msecsInSec*secsInMin*minsInHour*timeZoneOffset);    // UTC время заката для начала поиска
    QPair<double, double> horizontalCoords, prevHorizontalCoords;
    qint64 step = msecsInSec*secsInMin;                                     // начальный шаг поиска примем в минуту времени

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (degree >= -90) && (degree <= 0) && (true == date.isValid()) && (true == sunSet.isValid()))
    {
        // время начала сумерек равно времени заката
        result.first = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();

        // вычислить время завершения сумерек
        horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).date() == date))
        {
            // горизонтальные координаты Солнца
            horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);

            // найдено ли время завершения сумерек
            if (horizontalCoords.second <= degree)
            {
                // если используется крупный шаг
                if (msecsInSec*secsInMin == step)
                {
                    // шаг назад и продолжить с мелким шагом
                    dt = dt.addMSecs(-step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время завершения сумерек
                    result.second = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();
                }
            }

            // шаг
            dt = dt.addMSecs(+step);
            prevHorizontalCoords = horizontalCoords;
        }
    }

    // если не найдено завершение значит сумерки не определены вовсе
    if ((false == result.second.isValid()))
        result = QPair<QTime, QTime>();

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeEveningTwilightWithRefraction(const double longitude, const double latitude, const double height, const double pressure,
                                                                     const double temperature, const double timeZoneOffset, const double degree,
                                                                     const QDate& date)
{
    QPair<QTime, QTime> result;
    QDateTime dt (date,sunTimeSet(longitude,latitude,UtcOffset,date));      // UTC время заката для начала поиска
    QPair<double, double> horizontalCoords, prevHorizontalCoords;
    qint64 step = msecsInSec*secsInMin;                                     // начальный шаг поиска примем в минуту времени

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (degree >= -90) && (degree <= 0) && (true == date.isValid()))
    {
        // время начала сумерек равно времени заката
        result.first = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();

        // вычислить время завершения сумерек
        horizontalCoords = sunHorizontalCoordsWithRefraction(longitude,latitude,height,pressure,temperature,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).date() == date))
        {
            // горизонтальные координаты Солнца
            horizontalCoords = sunHorizontalCoordsWithRefraction(longitude,latitude,height,pressure,temperature,dt);

            // найдено ли время завершения сумерек
            if (horizontalCoords.second <= degree)
            {
                // если используется крупный шаг
                if (msecsInSec*secsInMin == step)
                {
                    // шаг назад и продолжить с мелким шагом
                    dt = dt.addMSecs(-step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время завершения сумерек
                    result.second = dt.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset).time();
                }
            }

            // шаг
            dt = dt.addMSecs(+step);
            prevHorizontalCoords = horizontalCoords;
        }
    }

    // если не найдено завершение значит сумерки не определены вовсе
    if ((false == result.second.isValid()))
        result = QPair<QTime, QTime>();

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeSandhyaAsDayPart(const double longitude, const double latitude, const double timeZoneOffset,
                                                        const bool morningSandhya, const QDate& date)
{
    QPair<QTime,QTime> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (true == date.isValid()))
    {
        if (true == morningSandhya)
        {
            // утренняя сандхья как 1/10 часть от половины суток
            result.second = sunTimeRise(longitude,latitude,timeZoneOffset,date);
            if (true == result.second.isValid())
                result.first = result.second.addMSecs(-msecsInSec*secsInMin*minsInHour*(hoursInDay/2)/10);
        }
        else
        {
            // вечерняя сандхья как 1/10 часть от половины суток
            result.first = sunTimeSet(longitude,latitude,timeZoneOffset,date);
            if (true == result.first.isValid())
                result.second = result.first.addSecs(secsInMin*minsInHour*(hoursInDay/2) / 10);
        }
    }

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeSandhyaAsDayPart(const double longitude, const double latitude, const QTime& sunRiseSet, const double timeZoneOffset,
                                                        const bool morningSandhya, const QDate& date)
{
    QPair<QTime,QTime> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == sunRiseSet.isValid()) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        if (true == morningSandhya)
        {
            // утренняя сандхья как 1/10 часть от половины суток
            result.second = sunRiseSet;
            if (true == result.second.isValid())
                result.first = result.second.addSecs(-secsInMin*minsInHour*(hoursInDay/2) / 10);
        }
        else
        {
            // вечерняя сандхья как 1/10 часть от половины суток
            result.first = sunRiseSet;
            if (true == result.first.isValid())
                result.second = result.first.addSecs(secsInMin*minsInHour*(hoursInDay/2) / 10);
        }
    }

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeSandhyaAsLightDayPart(const double longitude, const double latitude, const double timeZoneOffset,
                                                             const bool morningSandhya, const QDate& date)
{
    QPair<QTime,QTime> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == isTimeZoneOffsetValid(timeZoneOffset)) &&
            (true == date.isValid()))
    {
        // восход/заход
        QTime sunRise (sunTimeRise(longitude,latitude,timeZoneOffset,date));
        QTime sunSet (sunTimeSet(longitude,latitude,timeZoneOffset,date));

        if ((true == sunRise.isValid()) && (true == sunSet.isValid()))
        {
            // 1/10 часть светового дня
            int lightDayPart = (sunSet.msecsSinceStartOfDay() - sunRise.msecsSinceStartOfDay()) / 10;

            if (true == morningSandhya)
            {
                // утренняя сандхья как 1/10 часть светового дня
                result.second = sunRise;
                result.first =  sunRise.addMSecs(-lightDayPart);
            }
            else
            {
                // вечерняя сандхья как 1/10 часть светового дня
                result.first = sunSet;
                result.second = sunSet.addMSecs(lightDayPart);
            }
        }
    }

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::sunTimeSandhyaAsLightDayPart(const double longitude, const double latitude, const QTime& sunRise, const QTime& sunSet,
                                                             const double timeZoneOffset, const bool morningSandhya, const QDate& date)
{
    QPair<QTime,QTime> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == sunRise.isValid()) && (true == sunSet.isValid()) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        // восход/заход
        QTime sunRiseTime (sunRise);
        QTime sunSetTime (sunSet);

        if ((true == sunRiseTime.isValid()) && (true == sunSetTime.isValid()))
        {
            // 1/10 часть светового дня
            int lightDayPart = (sunSetTime.msecsSinceStartOfDay() - sunRiseTime.msecsSinceStartOfDay()) / 10;

            if (true == morningSandhya)
            {
                // утренняя сандхья как 1/10 часть светового дня
                result.second = sunRiseTime;
                result.first = sunRiseTime.addMSecs(-lightDayPart);
            }
            else
            {
                // вечерняя сандхья как 1/10 часть светового дня
                result.first = sunSetTime;
                result.second = sunSetTime.addMSecs(lightDayPart);
            }
        }
    }

    return result;
}
//---------------------------

QTime TComputings::moonTimeRise(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    QTime result;
    CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
    CAARiseTransitSetDetails riseTransitSetDetails;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        double JD (d.Julian());
        riseTransitSetDetails = getMoonRiseTransitSet(JD, longitude, latitude);

        if (true == riseTransitSetDetails.bRiseValid)
        {
            // получено время восхода Луны
            CAADate riseD (JD + (riseTransitSetDetails.Rise / 24.) + (timeZoneOffset / 24.), true);
            long year, month, day, hour, minute;
            double second;
            riseD.Get(year,month,day,hour,minute,second);
            result = QTime::fromMSecsSinceStartOfDay(hour*minsInHour*secsInMin*msecsInSec + minute*secsInMin*msecsInSec + second*msecsInSec);
        }
    }

    return result;
}
//---------------------------

QTime TComputings::moonTimeSet(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    QTime result;
    CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
    CAARiseTransitSetDetails riseTransitSetDetails;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        double JD (d.Julian());
        riseTransitSetDetails = getMoonRiseTransitSet(JD, longitude, latitude);

        if (true == riseTransitSetDetails.bSetValid)
        {
            // получено время захода Луны
            CAADate setD (JD + (riseTransitSetDetails.Set / 24.) + (timeZoneOffset / 24.), true);
            long year, month, day, hour, minute;
            double second;
            setD.Get(year,month,day,hour,minute,second);
            result = QTime::fromMSecsSinceStartOfDay(hour*minsInHour*secsInMin*msecsInSec + minute*secsInMin*msecsInSec + second*msecsInSec);
        }
    }

    return result;
}
//---------------------------

QTime TComputings::moonTimeTransit(const double longitude, const double latitude, bool& aboveHorizont,
                                   const double timeZoneOffset, const QDate& date)
{
    QTime result;
    CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
    CAARiseTransitSetDetails riseTransitSetDetails;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        double JD (d.Julian());
        riseTransitSetDetails = getMoonRiseTransitSet(JD, longitude, latitude);

        // получено время зенита Луны
        aboveHorizont = riseTransitSetDetails.bTransitAboveHorizon;
        CAADate setD (JD + (riseTransitSetDetails.Transit / 24.) + (timeZoneOffset / 24.), true);
        long year, month, day, hour, minute;
        double second;
        setD.Get(year,month,day,hour,minute,second);
        result = QTime::fromMSecsSinceStartOfDay(hour*minsInHour*secsInMin*msecsInSec + minute*secsInMin*msecsInSec + second*msecsInSec);
    }

    return result;
}
//---------------------------

QDateTime TComputings::moonTimeFindPreviousNewMoon(const double timeZoneOffset, const QDateTime& dateTime)
{
    QDateTime result;

    // проверка входных данных
    if ((true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime.isValid()))
    {
        qint32 step = msecsInSec*secsInMin;
        QDateTime dt1 (dateTime);    // дата-время начала отсчёта
        // дата-время аварийного завершения рассчётов
        QDateTime dt2 (dt1.addMSecs(-1*static_cast<qint64>(msecsInSec*secsInMin*minsInHour*hoursInDay)*60));

        // поиск даты-времени новолуния
        while ((false == result.isValid()) && (dt1 > dt2))
        {
            if (true == moonTimeIsNewMoon(dt1,newMoonFindLongitudeThreshold()))
                result = dt1.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset);

            // шаг
            dt1 = dt1.addMSecs(-step);
        }
    }

    return result;
}
//---------------------------

QDateTime TComputings::moonTimeFindNextNewMoon(const double timeZoneOffset, const QDateTime& dateTime)
{
    QDateTime result;

    // проверка входных данных
    if ((true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime.isValid()))
    {
        qint32 step = msecsInSec*secsInMin;
        QDateTime dt1 (dateTime);    // дата-время начала отсчёта
        // дата-время аварийного завершения рассчётов
        QDateTime dt2 (dt1.addMonths(2));

        // поиск даты-времени новолуния
        while ((false == result.isValid()) && (dt1 < dt2))
        {
            if (true == moonTimeIsNewMoon(dt1,newMoonFindLongitudeThreshold()))
                result = dt1.addMSecs(msecsInSec*secsInMin*minsInHour*timeZoneOffset);

            // шаг
            dt1 = dt1.addMSecs(+step);
        }
    }

    return result;
}
//---------------------------

QList<QDateTime> TComputings::moonTimeFindNewMoonForYear(const double timeZoneOffset, const QDateTime& dateTime)
{
    QList<QDateTime> result;

    // проверка входных данных
    if ((true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime.isValid()))
    {
        QDateTime dt (dateTime);

        while (dt <= dateTime.addYears(1))
        {
            // поиск следующего новолуния и занесение в список результата, если подходит во временные рамки
            QDateTime subResult (moonTimeFindNextNewMoon(UtcOffset,dt));
            if ((true == subResult.isValid()) && (subResult <= dateTime.addYears(1)))
                result << subResult.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
            dt = dt.addDays(29);    // шаг чуть меньше продолжительности лунного месяца
        }
    }

    // удаление повторяющихся элементов и сортировка по возрастанию
    QSet<QDateTime> unique (result.toSet());
    result = QList<QDateTime>::fromSet(unique);
    std::sort(result.begin(),result.end());

    return result;
}
//---------------------------

QList<QDateTime> TComputings::moonTimeFindNewMoonForPeriod(const QDateTime& dateTime1, const QDateTime& dateTime2, const double timeZoneOffset)
{
    QList<QDateTime> result;

    // проверка входных данных
    if ((true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime1.isValid()) && (true == dateTime2.isValid()) && (dateTime2 > dateTime1))
    {
        QDateTime dt (dateTime1);

        while (dt <= dateTime2)
        {
            // поиск новолуния и занесение его в список результатов
            QDateTime nextNewMoon (moonTimeFindNextNewMoon(UtcOffset,dt));
            if ((true == nextNewMoon.isValid()) && (nextNewMoon < dateTime2))
                result << nextNewMoon.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
            dt = dt.addDays(29);        // шаг чуть меньше продолжительности лунного месяца
        }
    }

    // удаление повторяющихся элементов и сортировка по возрастанию
    QSet<QDateTime> unique (result.toSet());
    result = QList<QDateTime>::fromSet(unique);
    std::sort(result.begin(),result.end());

    return result;
}
//---------------------------

QList<TComputings::TMoonDay> TComputings::moonTimeRiseTransitSet(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    QList<TMoonDay> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        // восход, заход, зенит Луны на три дня
        TMoonDay dayBefore;
        TMoonDay day;
        TMoonDay dayAfter;

        dayBefore.date = date.addDays(-1);
        dayBefore.rise = moonTimeRise(longitude,latitude,timeZoneOffset,dayBefore.date);
        dayBefore.set = moonTimeSet(longitude,latitude,timeZoneOffset,dayBefore.date);
        dayBefore.transit = moonTimeTransit(longitude,latitude,dayBefore.transitAboveHorizont,timeZoneOffset,dayBefore.date);

        day.date = date;
        day.rise = moonTimeRise(longitude,latitude,timeZoneOffset,day.date);
        day.set = moonTimeSet(longitude,latitude,timeZoneOffset,day.date);
        day.transit = moonTimeTransit(longitude,latitude,day.transitAboveHorizont,timeZoneOffset,day.date);

        dayAfter.date = date.addDays(1);
        dayAfter.rise = moonTimeRise(longitude,latitude,timeZoneOffset,dayAfter.date);
        dayAfter.set = moonTimeSet(longitude,latitude,timeZoneOffset,dayAfter.date);
        dayAfter.transit = moonTimeTransit(longitude,latitude,dayAfter.transitAboveHorizont,timeZoneOffset,dayAfter.date);

        if ((true == dayBefore.rise.isValid()) && (true == dayBefore.set.isValid()) && (true == dayBefore.transit.isValid()) &&
                (true == day.rise.isValid()) && (true == day.set.isValid()) && (true == day.transit.isValid()) &&
                (true == dayAfter.rise.isValid()) && (true == dayAfter.set.isValid()) && (true == dayAfter.transit.isValid()))
            result << dayBefore << day << dayAfter;
    }

    return result;
}
//---------------------------

QList<TComputings::TMoonDay> TComputings::moonTimeMoonDaysFast(const double longitude, const double latitude, const double timeZoneOffset,
                                                               const QDate& date1, const QDate &date2)
{
    QList<TMoonDay> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date1.isValid()) && (true == date2.isValid()))
    {
        QDate d1 (date1);
        TMoonDay moonDay;

        // рассчитать информацию по лунным дням за период дат
        while (d1 <= date2)
        {
            moonDay.date = d1;
            moonDay.rise = moonTimeRise(longitude,latitude,timeZoneOffset,d1);
            moonDay.set = moonTimeSet(longitude,latitude,timeZoneOffset,d1);
            moonDay.transit = moonTimeTransit(longitude,latitude,moonDay.transitAboveHorizont,timeZoneOffset,d1);

            if ((true == moonDay.rise.isValid()) && (true == moonDay.set.isValid()) && (true == moonDay.transit.isValid()))
                result << moonDay;

            d1 = d1.addDays(1);
        }
    }

    return result;
}
//---------------------------

quint32 TComputings::moonTimeMoonDayNum(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    quint32 result (0);

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        // обновление кэшируемых переменных, если необходимо
        if ((date < m_previousNewMoon.date()) || (false == m_previousNewMoon.isValid()))
        {
            m_previousNewMoon = moonTimeFindPreviousNewMoon(timeZoneOffset,QDateTime(date,QTime(23,59,59,999)));
            m_nextNewMoon = moonTimeFindNextNewMoon(timeZoneOffset,QDateTime(date,QTime(0,0)));
        }
        if ((date > m_nextNewMoon.date()) || (false == m_nextNewMoon.isValid()))
        {
            m_previousNewMoon = moonTimeFindPreviousNewMoon(timeZoneOffset,QDateTime(date,QTime(23,59,59,999)));
            m_nextNewMoon = moonTimeFindNextNewMoon(timeZoneOffset,QDateTime(date,QTime(0,0)));
        }

        // лунный день предыдущего новолуния и сегодняшний лунный день
        TMoonDay moonDayPNewMoon;
        TMoonDay moonDayToday;

        moonDayPNewMoon.date = m_previousNewMoon.date();
        moonDayPNewMoon.rise = moonTimeRise(longitude,latitude,timeZoneOffset,moonDayPNewMoon.date);

        moonDayToday.date = date;
        moonDayToday.rise = moonTimeRise(longitude,latitude,timeZoneOffset,moonDayToday.date);

        if ((true == moonDayPNewMoon.rise.isValid()) && (true == moonDayToday.rise.isValid()))
        {
            if (m_nextNewMoon.date() == date)
            {
                // --сегодня "следующее" новолуние--
                if (m_nextNewMoon.time() < moonDayToday.rise)
                    result = 2;   // второй лунный день
                else if ((m_nextNewMoon.time() >= moonDayToday.rise) && (m_nextNewMoon.time() <= moonDayToday.set))
                    result = 1;   // первый лунный день
                else if (m_nextNewMoon.time() > moonDayToday.set)
                {
                    // новолуние после захода луны
                    result = moonDayPNewMoon.date.daysTo(date) + 1;
                    // если новолуние случилось раньше восхода, то добавить ещё один день, образовавшийся в этот временной промежуток
                    if (m_previousNewMoon.time() < moonDayPNewMoon.rise)
                        result++;
                }
            }
            else
            {
                // --сегодня "предыдущее" новолуние или просто лунный день--
                result = moonDayPNewMoon.date.daysTo(date) + 1;
                // если новолуние случилось раньше восхода, то добавить ещё один день, образовавшийся в этот временной промежуток
                if (m_previousNewMoon.time() < moonDayPNewMoon.rise)
                    result++;
            }
        }
    }

    // кэширование
    m_PrevMoonDayNum = m_MoonDayNum;
    m_MoonDayNum = result;

    return result;
}
//---------------------------

qint32 TComputings::moonTimePhase(const QDate& date)
{
    qint32 result (-1);

    // проверка входных данных
    if (true == date.isValid())
    {
        CAADate d (date.year(),date.month(),date.day(),0,0,0,true);
        double JD (d.Julian());
        double illuminated_fraction;
        double position_angle;
        double phase_angle;

        //Phase:
        //right side illuminated: 0 - 180 degrees
        //left side illuminated:  180 - 360 degrees
        //0 degrees = new moon
        //90 degrees = first quarter (right half illuminated)
        //180 degrees = full moon
        //270 degrees = last quarter (left half illuminated)
        getMoonIllumination(JD, illuminated_fraction, position_angle, phase_angle);
        double phase(position_angle < 180 ? phase_angle + 180 : 180 - phase_angle);

        quint32 phasePercent (static_cast<quint32>(qFloor(phase/3.6 + 0.5)));
        if (phasePercent <= 100)
            result = phasePercent;
    }

    return result;
}
//---------------------------

bool TComputings::TMoonDay2::operator == (const TComputings::TMoonDay2& rv) const
{
    bool result (true);

    if ((this->num != rv.num) || (this->rise != rv.rise) || (this->set != rv.set) || (this->transit != rv.transit))
        result = false;

    return result;
}
//---------------------------

// ф-ия утилита для метода moonTimeMoonDays
// определяет содержит ли список дат-времени заданную дату
bool containsDate(const QList<QDateTime>& list, const QDate& date)
{
    bool result (false);

    for (qint32 i = 0; i < list.size(); ++i)
        if (list.at(i).date() == date)
            result = true;

    return result;
}
//---------------------------

// ф-ия утилита для метода moonTimeMoonDays
// возвращает первую найденную дату-время из списка по заданной дате
QDateTime fromListByDate(const QList<QDateTime>& list, const QDate& date)
{
    QDateTime result;

    for (qint32 i = 0; i < list.size(); ++i)
        if (list.at(i).date() == date)
            result = list.at(i);

    return result;
}
//---------------------------

// ф-ия утилита для метода moonTimeMoonDays
// определяет содержит ли список новолуний заданную дату-время с учётом шага (мс)
// step примем положительным
bool containsDateTime(const QList<QDateTime>& list, const QDateTime& dt, qint32 step)
{
    bool result (false);

    for (qint32 i = 0; i < list.size(); ++i)
    {
        if ((dt > list[i].addMSecs(-step)) && (dt < list[i].addMSecs(step)))
            result = true;
    }

    return result;
}
//---------------------------

// ф-ия утилита для метода moonTimeMoonDays
// определяет, содержится ли в списке дата-время, входящая в промежуток между dt1 и dt2
// возвращает последнюю подходящую дату-время из списка, либо пустую дату-время в случае отсутствия необходимой в списке
QDateTime fromListByDateTimeRange(const QList<QDateTime>& list, const QDateTime& dt1, const QDateTime& dt2)
{
    QDateTime result;

    for (qint32 i = list.size()-1; i >= 0; --i)
    {
        if ((list.at(i) > dt1) && (list.at(i) < dt2))
            result = list.at(i);
    }

    return result;
}
//---------------------------

QList<TComputings::TMoonDay2> TComputings::moonTimeMoonDays(const double longitude, const double latitude, const double timeZoneOffset,
                                                            const QDateTime& dateTime1, const QDateTime& dateTime2, const double height)
{
    QList<TMoonDay2> result;
    TMoonDay2 moonDay;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime1.isValid()) && (true == dateTime2.isValid()))
    {
        QDateTime dt1 (moonTimeFindPreviousNewMoon(UtcOffset,dateTime1));
        QDateTime dt2 (moonTimeFindNextNewMoon(UtcOffset,dateTime2));
        QList<QDateTime> newMoonList (moonTimeFindNewMoonForPeriod(dt1,dt2,UtcOffset));
        qint32 step (msecsInSec*secsInMin/2);
        double prevYCoord (moonHorizontalCoords(longitude,latitude,height,dt1).second);
        quint32 moonDayNum (1);

        moonDay.rise = dt1;             // начало первого лунного дня примем в новолуние
        dt1 = dt1.addMSecs(+step);      // шаг

        while (dt1 <= dt2)
        {
            QPair<double,double> hCoords (moonHorizontalCoords(longitude,latitude,0,dt1));

            if ((prevYCoord < 0) && (hCoords.second >= 0))
            {
                // Восход Луны
                moonDay.rise = dt1;
                ++moonDayNum;
            }
            else if ((false == moonDay.transit.isValid()) && (prevYCoord > 0) /*&& (hCoords.second > 0)*/ && (hCoords.second < prevYCoord))
            {
                // Зенит Луны на предыдущем шаге                
                moonDay.transit = dt1.addMSecs(-step);
            }
            else if ((prevYCoord >= 0) && (hCoords.second < 0))
            {
                // Заход Луны
                moonDay.set = dt1;

                // определить номер лунного дня
                if (true == containsDate(newMoonList,moonDay.rise.date()))
                {
                    // дата новолуния
                    QDateTime newMoon (fromListByDate(newMoonList,moonDay.rise.date()));
                    moonDay.newMoon = newMoon.addSecs(secsInMin*minsInHour*timeZoneOffset);
                    if (newMoon < moonDay.rise)
                    {
                        // новолуние до восхода Луны
                        moonDay.num = "1-2";
                        moonDayNum = 2;
                    }
                    else if ((newMoon >= moonDay.rise) && (newMoon <= moonDay.set))
                    {
                        // новолуние между восходом и заходом Луны
                        switch (moonDayNum)
                        {
                        case 29:
                            moonDay.num = "29-1";
                            break;
                        case 30:
                            moonDay.num = "30-1";
                            break;
                        case 31:
                            moonDay.num = "31-1";   // скорее всего не реально
                            break;
                        case 1:
                            moonDay.num = "1";
                            break;
                        }
                        moonDayNum = 1;
                    }
                    else if (newMoon > moonDay.set)
                    {
                        // новолуние после захода Луны
                        moonDay.num = QString::number(moonDayNum);
                        moonDayNum = 1;
                    }
                }
                else
                {
                    // дата не новолуние
                    moonDay.num = QString::number(moonDayNum);
                }

                // добавить в список
                if ((true == moonDay.rise.isValid()) && (true == moonDay.set.isValid()) && (true == moonDay.transit.isValid()))
                {
                    // смещение часового пояса
                    moonDay.rise = moonDay.rise.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
                    moonDay.set = moonDay.set.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
                    moonDay.transit = moonDay.transit.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));

                    result << moonDay;          // нормальный лунный день
                }

                // сбросить значения
                moonDay.rise = QDateTime();
                moonDay.set = QDateTime();
                moonDay.transit = QDateTime();
                moonDay.newMoon = QDateTime();
            }

            // подготовка к следующему шагу
            prevYCoord = hCoords.second;

            // шаг
            dt1 = dt1.addMSecs(+step);
        }
    }

    // удаление лишних элементов, не входящих в заданные временные рамки
    for (qint32 i = 0; i < result.size(); ++i)
    {
        if ((result.at(i).rise < dateTime1) || (result.at(i).set > dateTime2))
        {
            result.removeAt(i);
            --i;
        }
    }

    return result;
}
//---------------------------

QList<TComputings::TMoonDay2> TComputings::moonTimeMoonDaysExt(const double longitude, const double latitude, const double timeZoneOffset,
                                                               const QDateTime& dateTime1, const QDateTime& dateTime2, const double height)
{
    QList<TMoonDay2> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime1.isValid()) && (true == dateTime2.isValid()))
    {
        TMoonDay2 moonDay;
        QDateTime dt1 (moonTimeFindPreviousNewMoon(UtcOffset,dateTime1));
        QDateTime dt2 (moonTimeFindNextNewMoon(UtcOffset,dateTime2));
        QList<QDateTime> newMoonList (moonTimeFindNewMoonForPeriod(dt1,dt2,UtcOffset));
        qint32 step (msecsInSec*secsInMin/2);
        double prevYCoord (moonHorizontalCoords(longitude,latitude,height,dt1).second);
        quint32 moonDayNum (1);
        quint32 prevMoonDayNum (moonDayNum);

        moonDay.rise = dt1;             // начало первого лунного дня примем в новолуние
        dt1 = dt1.addMSecs(+step);      // шаг

        while (dt1 <= dt2)
        {
            // горизонтальные координаты Луны сейчас
            QPair<double,double> hCoords (moonHorizontalCoords(longitude,latitude,0,dt1));

            // проверка на новолуние
            if (containsDateTime(newMoonList,dt1,step))
            {
                // сейчас новолуние
                prevMoonDayNum = moonDayNum;
                moonDayNum = 1;
                moonDay.newMoon = dt1;
            }

            // проверка позиции Луны относительно горизонта
            if ((prevYCoord < 0) && (hCoords.second >= 0))
            {
                // Восход Луны
                moonDay.rise = dt1;
                prevMoonDayNum = moonDayNum;
                ++moonDayNum;
            }
            else if ((false == moonDay.transit.isValid()) && (prevYCoord > 0) /*&& (hCoords.second > 0)*/ && (hCoords.second < prevYCoord))
            {
                // Зенит Луны на предыдущем шаге
                moonDay.transit = dt1.addMSecs(-step);
            }
            else if ((prevYCoord >= 0) && (hCoords.second < 0))
            {
                // Заход Луны
                moonDay.set = dt1;

                // определить номер лунного дня
                // произошло ли новолуние
                if (true == containsDate(newMoonList,moonDay.rise.date()))
                {
                    // если новолуние произошло в дату восхода Луны
                    QDateTime newMoon (fromListByDate(newMoonList,moonDay.rise.date()));

                    if (newMoon < moonDay.rise)
                    {
                        // новолуние до восхода Луны
                        // второе условие покрывает ситуацию, когда новолуние уже было учтено в преыдущем лунном дне
                        if ((result.size() > 0) && (true == result.last().newMoon.isValid()))
                            moonDay.num = "2";
                        else if (result.size() >= 0)
                            moonDay.num = "1-2";
                    }
                    else
                    {
                        // новолуние произошло между восходом и заходом Луны
                        moonDay.num = QString::number(prevMoonDayNum)+"-1";
                    }
                }
                else if (true == containsDate(newMoonList,moonDay.set.date()))
                {
                    // если новолуние произошло в дату захода Луны
                    QDateTime newMoon (fromListByDate(newMoonList,moonDay.set.date()));

                    if (newMoon <= moonDay.set)
                    {
                        // новолуние произошло между восходом и заходом Луны
                        moonDay.num = QString::number(prevMoonDayNum)+"-1";
                    }
                    else
                    {
                        // новолуние произошло после захода Луны
                        moonDay.num = QString::number(moonDayNum);
                    }
                }
                else if (true == fromListByDateTimeRange(newMoonList,moonDay.rise,moonDay.set).isValid())
                {
                    // если новолуние произошло в дату между восходом и заходом Луны (актуально для заполярья)
                    moonDay.num = QString::number(prevMoonDayNum)+"-1";
                }
                else
                {
                    // новолуние не произошло
                    moonDay.num = QString::number(moonDayNum);
                }

                // добавить в список
                if ((true == moonDay.rise.isValid()) && (true == moonDay.set.isValid()) && (true == moonDay.transit.isValid()))
                {
                    // смещение часового пояса
                    moonDay.rise = moonDay.rise.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
                    moonDay.set = moonDay.set.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
                    moonDay.transit = moonDay.transit.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));
                    if (true == moonDay.newMoon.isValid())
                        moonDay.newMoon = moonDay.newMoon.addSecs(static_cast<qint64>(secsInMin*minsInHour*timeZoneOffset));

                    result << moonDay;          // нормальный лунный день
                }

                // сбросить значения
                moonDay.rise = QDateTime();
                moonDay.set = QDateTime();
                moonDay.transit = QDateTime();
                moonDay.newMoon = QDateTime();

            }

            // подготовка к следующему шагу
            prevYCoord = hCoords.second;

            // шаг
            dt1 = dt1.addMSecs(+step);
        }
    }

    // удаление лишних элементов, не входящих в заданные временные рамки
    for (qint32 i = 0; i < result.size(); ++i)
    {
        if ((result.at(i).rise < dateTime1) || (result.at(i).set > dateTime2))
        {
            result.removeAt(i);
            --i;
        }
    }

    return result;
}
//---------------------------

QPair<quint8, QPair<QDateTime,QDateTime> > TComputings::moonTimeNearestMoonDay(const double longitude, const double latitude, const double timeZoneOffset, const QDateTime& dateTime)
{
    QPair<quint8, QPair<QDateTime, QDateTime> > result;
    result.first = std::numeric_limits<quint8>::max();

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == dateTime.isValid()))
    {
        // вычислить список лунных дней
        QList<TMoonDay2> moonDays (TComputings::moonTimeMoonDays(longitude,latitude,UtcOffset,moonTimeFindPreviousNewMoon(UtcOffset,dateTime),dateTime));
        const quint32 timeOffsetMs = msecsInSec*secsInMin*minsInHour*timeZoneOffset;
        quint32 minDiff (std::numeric_limits<quint32>::max());
        quint32 minIndex (moonDays.size()-1);

        qint32 cr (moonDays.size()-1);
        while ((cr >= 0) && (false == result.second.second.isValid()))
        {
            if ((dateTime >= moonDays.at(cr).rise) && (dateTime <= moonDays.at(cr).set))
            {
                // ближайший лунный день сейчас
                result.first = cr+1;
                result.second.first = moonDays.at(cr).rise.addMSecs(+timeOffsetMs);
                result.second.second = moonDays.at(cr).set.addMSecs(+timeOffsetMs);
            }
            else if (dateTime < moonDays.at(cr).rise)
            {
                // дата-время до текущего лунного дня
                if (minDiff > dateTime.secsTo(moonDays.at(cr).rise))
                {
                    minDiff = dateTime.secsTo(moonDays.at(cr).rise);
                    minIndex = cr;
                }
            }
            else if (dateTime > moonDays.at(cr).set)
            {
                // дата-время после текущего лунного дня
                if (minDiff > moonDays.at(cr).set.secsTo(dateTime))
                {
                    minDiff = moonDays.at(cr).set.secsTo(dateTime);
                    minIndex = cr;
                }
            }
            --cr;
        }

        if (false == result.second.second.isValid())
        {
            // записать вычисленный результат по индексу в списке
            result.first = minIndex+1;
            result.second.first = moonDays.at(minIndex).rise.addMSecs(+timeOffsetMs);
            result.second.second = moonDays.at(minIndex).set.addMSecs(+timeOffsetMs);
        }
    }

    return result;
}
//---------------------------

QList<TComputings::TSvara> TComputings::sunMoonTimeSvaraList(const double longitude, const double latitude, const double timeZoneOffset, const QDate& date)
{
    QList<TSvara> svaras;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {
        // восход, заход Солнца и список свар
        QTime sunRise (sunTimeRise(longitude,latitude,timeZoneOffset,date));
        QTime sunSet (sunTimeSet(longitude,latitude,timeZoneOffset,date));

        svaras = sunMoonTimeSvaraList(sunRise,sunSet);
    }

    return svaras;
}
//---------------------------

QList<TComputings::TSvara> TComputings::sunMoonTimeSvaraList(const QTime& sunRise, const QTime& sunSet)
{
    QList<TSvara> svaras;

    // проверка входных данных
    if ((true == sunRise.isValid()) && (true == sunSet.isValid()))
    {
        QTime t (sunRise);        
        quint32 c (0);

        // расчёт свар продолжительностью по полтара часа
        while (t < sunSet)
        {
            TSvara svara;
            if (1 == (c % 2))
                svara.moonSvara = true;
            else
                svara.moonSvara = false;
            svara.num = c+1;
            svara.begin = t;
            svara.end = t.addSecs(secsInMin*minsInHour*1.5);
            if (svara.end > sunSet)
                svara.end = sunSet;

            svaras << svara;
            if (t.secsTo(sunSet) > secsInMin*minsInHour*1.5)
                t = t.addSecs(secsInMin*minsInHour*1.5);
            else
                t = sunSet;
            ++c;
        }
    }

    return svaras;
}
//---------------------------

TComputings::TSvara TComputings::sunMoonTimeCurrentSvara(const double longitude, const double latitude, const double timeZoneOffset)
{
    TSvara svara;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)))
    {
        QList<TSvara> svaras (sunMoonTimeSvaraList(longitude,latitude,timeZoneOffset));

        qint32 c (0);
        while ((false == svara.begin.isValid()) && (c < svaras.size()))
        {
            if ((QTime::currentTime() >= svaras.at(c).begin) && (QTime::currentTime() < svaras.at(c).end))
                svara = svaras.at(c);

            ++c;
        }
    }

    return svara;
}
//---------------------------

TComputings::TSvara TComputings::sunMoonTimeCurrentSvara(const QTime& sunRise, const QTime& sunSet)
{
    TSvara svara;

    // проверка входных данных
    if ((true == sunRise.isValid()) && (true == sunSet.isValid()))
    {
        QList<TSvara> svaras (sunMoonTimeSvaraList(sunRise,sunSet));

        qint32 c (0);
        while ((false == svara.begin.isValid()) && (c < svaras.size()))
        {
            if ((QTime::currentTime() >= svaras.at(c).begin) && (QTime::currentTime() < svaras.at(c).end))
                svara = svaras.at(c);

            ++c;
        }
    }

    return svara;
}
//---------------------------

QPair<double, double> TComputings::sunHorizontalCoords(const double longitude, const double latitude, const double height, const QDateTime& dateTime)
{
    QPair<double, double> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == dateTime.isValid()))
    {
        CAADate d (dateTime.date().year(),dateTime.date().month(),dateTime.date().day(),
                   dateTime.time().hour(),dateTime.time().minute(),dateTime.time().second() + dateTime.time().msec()/msecsInSec,true);

        double JD = d.Julian();
        double JDSun = CAADynamicalTime::UTC2TT(JD);
        double lambda = CAASun::ApparentEclipticLongitude(JDSun);
        double beta = CAASun::ApparentEclipticLatitude(JDSun);
        double epsilon = CAANutation::TrueObliquityOfEcliptic(JDSun);
        CAA2DCoordinate Solarcoord = CAACoordinateTransformation::Ecliptic2Equatorial(lambda, beta, epsilon);

        double SunRad = CAAEarth::RadiusVector(JDSun);
        CAA2DCoordinate SunTopo = CAAParallax::Equatorial2Topocentric(Solarcoord.X, Solarcoord.Y, SunRad, longitude, latitude, height, JDSun);
        double AST = CAASidereal::ApparentGreenwichSiderealTime(d.Julian());
        double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(longitude);
        double LocalHourAngle = AST - LongtitudeAsHourAngle - SunTopo.X;
        CAA2DCoordinate SunHorizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, SunTopo.Y, latitude);
    //    SunHorizontal.Y += CAARefraction::RefractionFromTrue(SunHorizontal.Y, 1013, 10);

        result.first = SunHorizontal.X;
        result.second = SunHorizontal.Y;
    }

    return result;
}
//---------------------------

QPair<double, double> TComputings::moonHorizontalCoords(const double longitude, const double latitude, const double height, const QDateTime& dateTime)
{
    QPair<double, double> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == dateTime.isValid()))
    {
        CAADate d (dateTime.date().year(),dateTime.date().month(),dateTime.date().day(),
                   dateTime.time().hour(),dateTime.time().minute(),dateTime.time().second() + dateTime.time().msec()/static_cast<double>(msecsInSec),true);

        double JD = d.Julian();
        double JDMoon = CAADynamicalTime::UTC2TT(JD);
        double lambda = CAAMoon::EclipticLongitude(JDMoon);
        double beta = CAAMoon::EclipticLatitude(JDMoon);
        double epsilon = CAANutation::TrueObliquityOfEcliptic(JDMoon);
        CAA2DCoordinate moonCoord = CAACoordinateTransformation::Ecliptic2Equatorial(lambda, beta, epsilon);

        double MoonRad = CAAMoon::RadiusVector(JDMoon);
//        MoonRad /= 149597870.691; //Convert KM to AU
        CAA2DCoordinate MoonTopo = CAAParallax::Equatorial2Topocentric(moonCoord.X, moonCoord.Y, MoonRad, longitude, latitude, height, JDMoon);
        double AST = CAASidereal::ApparentGreenwichSiderealTime(JD);
        double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(longitude);
        double LocalHourAngle = AST - LongtitudeAsHourAngle - MoonTopo.X;
        CAA2DCoordinate MoonHorizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, MoonTopo.Y, latitude);
//        MoonHorizontal.Y += CAARefraction::RefractionFromTrue(MoonHorizontal.Y);

        result.first = MoonHorizontal.X;
        result.second = MoonHorizontal.Y;
    }

    return result;
}
//---------------------------

QPair<double, double> TComputings::sunHorizontalCoordsWithRefraction(const double longitude, const double latitude, const double height, const double pressure,
                                                                     const double temperature, const QDateTime& dateTime)
{
    QPair<double, double> result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) && (true == dateTime.isValid()))
    {
        CAADate d (dateTime.date().year(),dateTime.date().month(),dateTime.date().day(),
                   dateTime.time().hour(),dateTime.time().minute(),dateTime.time().second() + dateTime.time().msec()/1000.,true);

        double JD = d.Julian();
        double JDSun = CAADynamicalTime::UTC2TT(JD);
        double lambda = CAASun::ApparentEclipticLongitude(JDSun);
        double beta = CAASun::ApparentEclipticLatitude(JDSun);
        double epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
        CAA2DCoordinate Solarcoord = CAACoordinateTransformation::Ecliptic2Equatorial(lambda, beta, epsilon);

        double SunRad = CAAEarth::RadiusVector(JDSun);
        CAA2DCoordinate SunTopo = CAAParallax::Equatorial2Topocentric(Solarcoord.X, Solarcoord.Y, SunRad, longitude, latitude, height, JDSun);
        double AST = CAASidereal::ApparentGreenwichSiderealTime(d.Julian());
        double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(longitude);
        double LocalHourAngle = AST - LongtitudeAsHourAngle - SunTopo.X;
        CAA2DCoordinate SunHorizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, SunTopo.Y, latitude);
        SunHorizontal.Y += CAARefraction::RefractionFromTrue(SunHorizontal.Y, pressure, temperature);

        result.first = SunHorizontal.X;
        result.second = SunHorizontal.Y;
    }

    return result;
}
//---------------------------

QString TComputings::toStringSunTimeInfo(const QTime& rise, const QTime& set, const QTime& transit, const bool roundToMin)
{
    QString result;

    // проверка входных данных
    if ((true == rise.isValid()) && (true == set.isValid()) && (true == transit.isValid()))
    {
        QTime sunRise (rise), sunSet (set), sunTransit (transit);
        if (true == roundToMin)
        {
            sunRise = roundToMinTime(sunRise);
            sunSet = roundToMinTime(sunSet);
            sunTransit = roundToMinTime(sunTransit);
        }

        result += "Восход: " + sunRise.toString(QString("hh:mm")) + "\n";
        result += "Заход: " + sunSet.toString(QString("hh:mm")) + "\n";
        result += "Зенит: " + sunTransit.toString(QString("hh:mm")) + "\n";
        result += "Долгота дня: " + (QTime::fromMSecsSinceStartOfDay(sunRise.secsTo(sunSet)*msecsInSec)).toString(QString("hh:mm"));
    }
    else if ((false == rise.isValid()) && (false == set.isValid()))
    {
        result += "Полярный(ая) день(ночь)";
    }

    return result;
}
//---------------------------

QString TComputings::toStringSunTimeInfo2(const QPair<QTime,QTime>& civil, const QPair<QTime,QTime>& navigation, const QPair<QTime,QTime>& astronomical,
                                          const bool morningTwilight, const bool roundToMin)
{
    QString result;

    // проверка входных данных
//    if ((true == civil.first.isValid()) && (true == civil.second.isValid()) && (true == navigation.first.isValid()) && (true == navigation.second.isValid()) &&
//            (true == astronomical.first.isValid()) && (true == astronomical.second.isValid()) )
    {
        QPair<QTime,QTime> civilTwilight (civil), navigationTwilight (navigation), astronomicalTwilight (astronomical);
        if (true == roundToMin)
        {
            civilTwilight = roundToMinTime(civilTwilight);
            navigationTwilight = roundToMinTime(navigationTwilight);
            astronomicalTwilight = roundToMinTime(astronomicalTwilight);
        }

        if (true == morningTwilight)
        {            
            result += "\nУтренние сумерки\n";
            if ((true == civil.first.isValid()) && (true == civil.second.isValid()))
                result += "Гражданские: " + civilTwilight.first.toString(QString("hh:mm")) + " - " + civilTwilight.second.toString(QString("hh:mm")) + "\n";
            else
                result += "Гражданские: ---\n";
            if ((true == navigation.first.isValid()) && (true == navigation.second.isValid()))
                result += "Навигационные: " + navigationTwilight.first.toString(QString("hh:mm")) + " - " + civilTwilight.first.toString(QString("hh:mm")) + "\n";
            else
                result += "Навигационные: ---\n";
            if ((true == astronomical.first.isValid()) && (true == astronomical.second.isValid()))
                result += "Астрономические: " + astronomicalTwilight.first.toString(QString("hh:mm")) + " - " + navigationTwilight.first.toString(QString("hh:mm"));
            else
                result += "Астрономические: ---\n";
        }
        else
        {
            result += "\nВечерние сумерки\n";
            if ((true == civil.first.isValid()) && (true == civil.second.isValid()))
                result += "Гражданские: " + civilTwilight.first.toString(QString("hh:mm")) + " - " + civilTwilight.second.toString(QString("hh:mm")) + "\n";
            else
                result += "Гражданские: ---\n";
            if ((true == navigation.first.isValid()) && (true == navigation.second.isValid()))
                result += "Навигационные: " + civilTwilight.second.toString(QString("hh:mm")) + " - " + navigationTwilight.second.toString(QString("hh:mm")) + "\n";
            else
                result += "Навигационные: ---\n";
            if ((true == astronomical.first.isValid()) && (true == astronomical.second.isValid()))
                result += "Астрономические: " + navigationTwilight.second.toString(QString("hh:mm")) + " - " + astronomicalTwilight.second.toString(QString("hh:mm"));
            else
                result += "Астрономические: ---\n";
        }
    }

    return result;
}
//---------------------------

QString TComputings::toStringSunTimeInfo3(const QPair<QTime,QTime>& sandhyaAsDayPart, const QPair<QTime,QTime>& sandhyaAsLightDayPart,
                                          const bool morningTwilight, const bool roundToMin)
{
    QString result;

    // проверка входных данных
    if ((true == sandhyaAsDayPart.first.isValid()) && (true == sandhyaAsDayPart.second.isValid()) && (true == sandhyaAsLightDayPart.first.isValid()) &&
            (true == sandhyaAsLightDayPart.second.isValid()))
    {
        QPair<QTime,QTime> sandhyaDay (sandhyaAsDayPart), sandhyaLightDay (sandhyaAsLightDayPart);

        // Сандхья как 1/10 часть от суток
        QPair<QTime,QTime> sandhyaDay2;
        int offset = (sandhyaDay.first.msecsTo(sandhyaDay.second) * 2);
        if (true == morningTwilight)
        {
            sandhyaDay2.first = sandhyaDay.second.addMSecs(-offset);
            sandhyaDay2.second = sandhyaDay.second;
        }
        else
        {
            sandhyaDay2.first = sandhyaDay.first;
            sandhyaDay2.second = sandhyaDay.first.addMSecs(offset);
        }

        if (true == roundToMin)
        {
            sandhyaDay = roundToMinTime(sandhyaDay);
            sandhyaDay2 = roundToMinTime(sandhyaDay2);
            sandhyaLightDay = roundToMinTime(sandhyaLightDay);
        }

        if (true == morningTwilight)
        {
            result += "\nУтренняя сандхья\n";
            result += "Сандхья как 1/10 от половины суток: ";
            if ((sandhyaDay.second < QTime(12,00)) && (sandhyaDay.first > QTime(12,00)))
                result += "---\n";
            else
                result += sandhyaDay.first.toString(QString("hh:mm")) + " - " + sandhyaDay.second.toString(QString("hh:mm")) + "\n";
            result += "Сандхья как 1/10 от суток: ";
            if ((sandhyaDay2.second < QTime(12,00)) && (sandhyaDay2.first > QTime(12,00)))
                result += "---\n";
            else
                result += sandhyaDay2.first.toString(QString("hh:mm")) + " - " + sandhyaDay2.second.toString(QString("hh:mm")) + "\n";
            result += "Сандхья как 1/10 от светового дня: ";
            if ((sandhyaAsLightDayPart.second < QTime(12,00)) && (sandhyaAsLightDayPart.first > QTime(12,00)))
                result += "---\n";
            else
                result += sandhyaLightDay.first.toString(QString("hh:mm")) + " - " + sandhyaLightDay.second.toString(QString("hh:mm"));
        }
        else
        {
            result += "\nВечерняя сандхья\n";
            result += "Сандхья как 1/10 от половины суток: ";
            if ((sandhyaDay.first > QTime(12,00)) && (sandhyaDay.second < QTime(12,00)))
                result += "---\n";
            else
                result += sandhyaDay.first.toString(QString("hh:mm")) + " - " + sandhyaDay.second.toString(QString("hh:mm")) + "\n";
            result += "Сандхья как 1/10 от суток: ";
            if ((sandhyaDay2.first > QTime(12,00)) && (sandhyaDay2.second < QTime(12,00)))
                result += "---\n";
            else
                result += sandhyaDay2.first.toString(QString("hh:mm")) + " - " + sandhyaDay2.second.toString(QString("hh:mm")) + "\n";
            result += "Сандхья как 1/10 от светового дня: ";
            if ((sandhyaAsLightDayPart.first > QTime(12,00)) && (sandhyaAsLightDayPart.second < QTime(12,00)))
                result += "---\n";
            else
             result += sandhyaLightDay.first.toString(QString("hh:mm")) + " - " + sandhyaLightDay.second.toString(QString("hh:mm"));
        }
    }

    return result;
}
//---------------------------

QString TComputings::toStringMoonTimeInfo(const QTime& set, const QTime& rise, const QTime& transit, const bool roundToMin)
{
    QString result;

    // проверка входных данных
    if ((true == rise.isValid()) && (true == set.isValid()) && (true == transit.isValid()))
    {
        QTime moonRise (rise), moonSet (set), moonTransit (transit);
        if (true == roundToMin)
        {
            moonRise = roundToMinTime(moonRise);
            moonSet = roundToMinTime(moonSet);
            moonTransit = roundToMinTime(moonTransit);
        }

//        if (moonSet > moonRise)
//        {
            result += "Восход: " + moonRise.toString(QString("hh:mm")) + "\n";
            result += "Заход: " + moonSet.toString(QString("hh:mm")) + "\n";
//        }
//        else
//        {
//            result += "Заход: " + moonSet.toString(QString("hh:mm")) + "\n";
//            result += "Восход: " + moonRise.toString(QString("hh:mm")) + "\n";
//        }
        result += "Зенит: " + moonTransit.toString(QString("hh:mm")) + "\n";
//        result += "Долгота дня: " + (QTime::fromMSecsSinceStartOfDay(moonRise.secsTo(moonSet)*msecsInSec)).toString(QString("hh:mm"));
    }

    return result;
}
//---------------------------

QTime TComputings::roundToMinTime(const QTime& time)
{
    QTime result (time);

    if (true == time.isValid())
    {
        int second (result.second());
        if (second >= 30)
            result = result.addSecs(secsInMin-second);
        else
            result.setHMS(result.hour(),result.minute(),0);
    }

    return result;
}
//---------------------------

QPair<QTime,QTime> TComputings::roundToMinTime(const QPair<QTime,QTime>& time)
{
    QPair<QTime,QTime> result (time);

    if ((true == time.first.isValid()) && (true == time.second.isValid()))
    {
        result.first = roundToMinTime(result.first);
        result.second = roundToMinTime(result.second);
    }

    return result;
}
//---------------------------

QDateTime TComputings::prevNewMoon()
{
    return m_previousNewMoon;
}
//---------------------------

QDateTime TComputings::nextNewMoon()
{
    return m_nextNewMoon;
}
//---------------------------

quint32 TComputings::prevMoonDayNum()
{
    return m_PrevMoonDayNum;
}
//---------------------------
// КОНЕЦ: TComputings - public
//---------------------------------------------------------------------------------

// НАЧАЛО: TComputings - protected
double TComputings::newMoonFindLongitudeThreshold()
{
    return 0.0100;
}
//---------------------------

double TComputings::timeEquation(const quint32 N)
{
    double B = 2*M_PI * (static_cast<qint32>(N) - 81) / 365.;
    return 9.87*sin(2*B) - 7.53*cos(B) - 1.5*sin(B);
}
//---------------------------

qint32 TComputings::sunTimeAverageOffset(const double longitude)
{
    return qFloor(longitude*secsInLngDeg + 0.5);
}
//---------------------------

qint32 TComputings::sunTimeTrueOffset(const double longitude, const quint32 N)
{
    return sunTimeAverageOffset(longitude) + qFloor(timeEquation(N)*secsInMin + 0.5);
}
//---------------------------

void TComputings::getSolarRaDecByJulian(double JD, double& RA, double& Dec)
{
    double JDSun = CAADynamicalTime::UTC2TT(JD);
    double lambda = CAASun::ApparentEclipticLongitude(JDSun);
    double beta = CAASun::ApparentEclipticLatitude(JDSun);
    double epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
    CAA2DCoordinate Solarcoord = CAACoordinateTransformation::Ecliptic2Equatorial(lambda, beta, epsilon);
    RA = Solarcoord.X;
    Dec = Solarcoord.Y;
}
//---------------------------

CAARiseTransitSetDetails TComputings::getSunRiseTransitSet(double JD, double longitude, double latitude)
{
    double alpha1 = 0;
    double delta1 = 0;
    getSolarRaDecByJulian(JD - 1, alpha1, delta1);
    double alpha2 = 0;
    double delta2 = 0;
    getSolarRaDecByJulian(JD, alpha2, delta2);
    double alpha3 = 0;
    double delta3 = 0;
    getSolarRaDecByJulian(JD + 1, alpha3, delta3);
    return CAARiseTransitSet::Calculate(CAADynamicalTime::UTC2TT(JD), alpha1, delta1, alpha2, delta2, alpha3, delta3, longitude, latitude, -0.8333);
}
//---------------------------

void TComputings::getLunarRaDecByJulian(double JD, double& RA, double& Dec)
{
  double JDMoon = CAADynamicalTime::UTC2TT(JD);
  double lambda = CAAMoon::EclipticLongitude(JDMoon);
  double beta = CAAMoon::EclipticLatitude(JDMoon);
  double epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
  CAA2DCoordinate Lunarcoord = CAACoordinateTransformation::Ecliptic2Equatorial(lambda, beta, epsilon);
  RA = Lunarcoord.X;
  Dec = Lunarcoord.Y;
}
//---------------------------

CAARiseTransitSetDetails TComputings::getMoonRiseTransitSet(double JD, double longitude, double latitude)
{
  double alpha1 = 0;
  double delta1 = 0;
  getLunarRaDecByJulian(JD - 1, alpha1, delta1);
  double alpha2 = 0;
  double delta2 = 0;
  getLunarRaDecByJulian(JD, alpha2, delta2);
  double alpha3 = 0;
  double delta3 = 0;
  getLunarRaDecByJulian(JD + 1, alpha3, delta3);
  return CAARiseTransitSet::Calculate(CAADynamicalTime::UTC2TT(JD), alpha1, delta1, alpha2, delta2, alpha3, delta3, longitude, latitude, 0.125);
}
//---------------------------

void TComputings::getMoonIllumination(double JD, double& illuminated_fraction, double& position_angle, double& phase_angle)
{
  double moon_alpha = 0;
  double moon_delta = 0;
  getLunarRaDecByJulian(JD, moon_alpha, moon_delta);
  double sun_alpha = 0;
  double sun_delta = 0;
  getSolarRaDecByJulian(JD, sun_alpha, sun_delta);
  double geo_elongation = CAAMoonIlluminatedFraction::GeocentricElongation(moon_alpha, moon_delta, sun_alpha, sun_delta);

  position_angle = CAAMoonIlluminatedFraction::PositionAngle(sun_alpha, sun_delta, moon_alpha, moon_delta);
  phase_angle = CAAMoonIlluminatedFraction::PhaseAngle(geo_elongation, 368410.0, 149971520.0);
  illuminated_fraction = CAAMoonIlluminatedFraction::IlluminatedFraction(phase_angle);
}
//---------------------------

bool TComputings::moonTimeIsNewMoon(const QDateTime& dt, const double threshold)
{
    bool result (false);

    // эклиптическая долгота Солнца и Луны
    CAADate d (dt.date().year(),dt.date().month(),dt.date().day(),dt.time().hour(),dt.time().minute(),dt.time().second()+dt.time().msec()/msecsInSec,true);
    double JD (d.Julian());
    double JDSunMoon (CAADynamicalTime::UTC2TT(JD));
    double sunLng (CAASun::ApparentEclipticLongitude(JDSunMoon));
    double moonLng (CAAMoon::EclipticLongitude(JDSunMoon));

    // если разница между долготами Солнца и Луны меньше принятого шага, то будем считать
    // заданное время новолунием
    if (qAbs(sunLng - moonLng) <= threshold)
        result = true;

    return result;
}
//---------------------------

bool TComputings::isTimeZoneOffsetValid (const double timeZoneOffset)
{
    bool result (true);

    // определение валидности смещения часового пояса
    if ((timeZoneOffset < timeZoneOffsetMin) || (timeZoneOffset > timeZoneOffsetMax))
        result = false;

    return result;
}
//---------------------------
// КОНЕЦ: TComputings - protected
//---------------------------------------------------------------------------------

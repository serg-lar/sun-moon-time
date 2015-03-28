// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "computings.h"
#include <QtMath>
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
        result.second = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();

        // вычислить время начала сумерек
        horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.date() == date))
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
                    dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время начала сумерек
                    result.first = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();
                }
            }

            // шаг
            dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
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
        result.second = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();

        // вычислить время начала сумерек
        horizontalCoords = sunHorizontalCoordsWithRefraction(longitude,latitude,height,pressure,temperature,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.date() == date))
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
                    dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время начала сумерек
                    result.first = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();
                }
            }

            // шаг
            dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
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
        result.first = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();

        // вычислить время завершения сумерек
        horizontalCoords = sunHorizontalCoords(longitude,latitude,height,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.date() < date.addDays(1)))
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
                    dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время завершения сумерек
                    result.second = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();
                }
            }

            // шаг
            dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
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
        result.first = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();

        // вычислить время завершения сумерек
        horizontalCoords = sunHorizontalCoordsWithRefraction(longitude,latitude,height,pressure,temperature,dt);
        prevHorizontalCoords = horizontalCoords;
        while ((horizontalCoords.second >= degree) && (dt.date() < date.addDays(1)))
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
                    dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step);
                    step = msecsInSec;
                    horizontalCoords = prevHorizontalCoords;
                }
                else
                {
                    // записать время завершения сумерек
                    result.second = (QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + msecsInSec*secsInMin*minsInHour*timeZoneOffset)).time();
                }
            }

            // шаг
            dt = QDateTime::fromMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
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
                result.first = QTime::fromMSecsSinceStartOfDay(result.second.hour()*minsInHour*secsInMin*msecsInSec + result.second.minute()*secsInMin*msecsInSec +
                                                               result.second.second()*msecsInSec + result.second.msec() - (msecsInSec*secsInMin*minsInHour*(hoursInDay/2) / 10));
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
                result.first = QTime::fromMSecsSinceStartOfDay(result.second.hour()*minsInHour*secsInMin*msecsInSec + result.second.minute()*secsInMin*msecsInSec +
                                                               result.second.second()*msecsInSec + result.second.msec() - (msecsInSec*secsInMin*minsInHour*(hoursInDay/2) / 10));
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
                result.first =  QTime::fromMSecsSinceStartOfDay(sunRise.msecsSinceStartOfDay() - lightDayPart);
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
                result.first =  QTime::fromMSecsSinceStartOfDay(sunRiseTime.msecsSinceStartOfDay() - lightDayPart);
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
    CAADate d (date.year(),date.month(),date.day(),12,0,0,true);
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

static QDateTime TComputings::moonTimeFindPreviousNewMoon(const double longitude, const double latitude, const double timeZoneOffset,
                                             const QDate& date = QDate::currentDate())
{
    QDateTime result;

    // проверка входных данных
    if ((longitude >= -180) && (longitude <= 180) && (latitude >= -90) && (latitude <= 90) &&
            (true == isTimeZoneOffsetValid(timeZoneOffset)) && (true == date.isValid()))
    {

    }

    return result;
}
//---------------------------

QPair<double, double> TComputings::sunHorizontalCoords(const double longitude, const double latitude, const double height, const QDateTime& dateTime)
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
    //    SunHorizontal.Y += CAARefraction::RefractionFromTrue(SunHorizontal.Y, 1013, 10);

        result.first = SunHorizontal.X;
        result.second = SunHorizontal.Y;
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

    return result;
}
//---------------------------

QString TComputings::toStringSunTimeInfo2(const QPair<QTime,QTime>& civil, const QPair<QTime,QTime>& navigation, const QPair<QTime,QTime>& astronomical,
                                          const bool morningTwilight, const bool roundToMin)
{
    QString result;

    // проверка входных данных
    if ((true == civil.first.isValid()) && (true == civil.second.isValid()) && (true == navigation.first.isValid()) && (true == navigation.second.isValid()) &&
            (true == astronomical.first.isValid()) && (true == astronomical.second.isValid()) )
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
            result += "Гражданские: " + civilTwilight.first.toString(QString("hh:mm")) + " - " + civilTwilight.second.toString(QString("hh:mm")) + "\n";
            result += "Навигационные: " + navigationTwilight.first.toString(QString("hh:mm")) + " - " + civilTwilight.first.toString(QString("hh:mm")) + "\n";
            result += "Астрономические: " + astronomicalTwilight.first.toString(QString("hh:mm")) + " - " + navigationTwilight.first.toString(QString("hh:mm"));
        }
        else
        {
            result += "\nВечерние сумерки\n";
            result += "Гражданские: " + civilTwilight.first.toString(QString("hh:mm")) + " - " + civilTwilight.second.toString(QString("hh:mm")) + "\n";
            result += "Навигационные: " + civilTwilight.second.toString(QString("hh:mm")) + " - " + navigationTwilight.second.toString(QString("hh:mm")) + "\n";
            result += "Астрономические: " + navigationTwilight.second.toString(QString("hh:mm")) + " - " + astronomicalTwilight.second.toString(QString("hh:mm"));
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
        int offset = ((sandhyaDay.second.msecsSinceStartOfDay() - sandhyaDay.first.msecsSinceStartOfDay()) * 2);
        if (true == morningTwilight)
        {
            sandhyaDay2.first = QTime::fromMSecsSinceStartOfDay(sandhyaDay.second.msecsSinceStartOfDay() - offset);
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
        }
        else
        {
            result += "\nВечерняя сандхья\n";
        }
        result += "Сандхья как 1/10 от половины суток: " + sandhyaDay.first.toString(QString("hh:mm")) + " - " + sandhyaDay.second.toString(QString("hh:mm")) + "\n";
        result += "Сандхья как 1/10 от суток: " + sandhyaDay2.first.toString(QString("hh:mm")) + " - " + sandhyaDay2.second.toString(QString("hh:mm")) + "\n";
        result += "Сандхья как 1/10 от светового дня: " + sandhyaLightDay.first.toString(QString("hh:mm")) + " - " + sandhyaLightDay.second.toString(QString("hh:mm"));
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

        result += "Заход: " + moonSet.toString(QString("hh:mm")) + "\n";
        result += "Восход: " + moonRise.toString(QString("hh:mm")) + "\n";
        result += "Зенит: " + moonTransit.toString(QString("hh:mm")) + "\n";
//        result += "Долгота дня: " + (QTime::fromMSecsSinceStartOfDay(moonRise.secsTo(moonSet)*msecsInSec)).toString(QString("hh:mm"));
    }

    return result;
}
//---------------------------
// КОНЕЦ: TComputings - public
//---------------------------------------------------------------------------------

// НАЧАЛО: TComputings - protected
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

bool TComputings::isTimeZoneOffsetValid (const double timeZoneOffset)
{
    bool result (true);

    // TODO добавить реализацию и соответствующие константы

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
// КОНЕЦ: TComputings - protected
//---------------------------------------------------------------------------------

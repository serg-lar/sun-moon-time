// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "tithi.h"
#include <qmath.h>
#include <QDebug>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: TTitha - public
TTitha::TTitha() : dtBegin (QDateTime::currentDateTimeUtc()), dtEnd (QDateTime::currentDateTimeUtc()),
    m_Num (0), m_Paksha (true)
{
}
//---------------------------

TTitha::TTitha(QDateTime beginDateTime, QDateTime endDateTime, quint8 num, bool paksha) : dtBegin (beginDateTime),
    dtEnd (endDateTime), m_Num(num), m_Paksha (paksha)
{
}
//---------------------------

TTitha::~TTitha()
{
}
//---------------------------

bool TTitha::isValid() const
{
    bool result (true);
    if ((m_Num == 0) || (m_Num > 15) || (false == dtBegin.isValid()) || (false == dtEnd.isValid()))
        result = false;

    return result;
}
//---------------------------

bool TTitha::isEkadash() const
{
    bool result (false);
    if (11 == m_Num)
        result = true;

    return result;
}
//---------------------------

quint8 TTitha::num() const
{
    return m_Num;
}
//---------------------------

bool TTitha::paksha() const
{
    return m_Paksha;
}
//---------------------------

QPair<QDateTime,QDateTime> TTitha::beginEndDateTime() const
{
    QPair<QDateTime,QDateTime> result (dtBegin, dtEnd);
    return result;
}
//---------------------------

QDateTime TTitha::beginDateTime() const
{
    return dtBegin;
}
//---------------------------

QDateTime TTitha::endDateTime() const
{
    return dtEnd;
}
//---------------------------

void TTitha::setData(const QDateTime &beginDateTime, const QDateTime &endDateTime,
                     const quint8 num, const bool paksha)
{
    // проверка входных данных
    if ( (true == beginDateTime.isValid()) && (true == endDateTime.isValid()) &&
         (num > 0) && (num <= 15) )
    {
        // перезапись
        dtBegin = beginDateTime;
        dtEnd = endDateTime;
        m_Num = num;
        m_Paksha = paksha;
    }
}
//---------------------------

QString TTitha::nameStr() const
{
    QString tithi[16] = {"пратипад","двитйа","тритйа","чатуртхи","панчами","шаштхи","саптами","аштами","навами",
                       "дашами","экадаши","двадаши","трайодаши","чатурдаши","пурнима","амавасья"};

    QString result ("неизвестно");

    if (true == isValid())
    {
        if ((false == m_Paksha) && (15 == m_Num))
            result = tithi[15];
        else
            result = tithi[m_Num-1];
    }

    return result;
}
//---------------------------

QString TTitha::asTithiStr() const
{
    QString tithiStr;

    if (true == isValid())
    {
        tithiStr += "Начало: " + dtBegin.toString("d MMMM yyyy года  hh:mm") + "\n";
        tithiStr += "Конец:  " + dtEnd.toString("d MMMM yyyy года  hh:mm");
    }

    return tithiStr;
}
//---------------------------

QString TTitha::asTithiStr(const double timeZoneOffset) const
{
    QString result;

    if (true == isValid())
    {
        result = "Часовой пояс: ";
        if (timeZoneOffset >= 0)
            result += "+"+QString::number(timeZoneOffset);
        else
            result += QString::number(timeZoneOffset);
        result += "\n";
    }

    return result += asTithiStr();
}
//---------------------------

QString TTitha::asTithiStrExt() const
{
    QString tithiStr;

    if (true == isValid())
    {
        if (m_Paksha)
            tithiStr += "шукла-пакша";
        else
            tithiStr += "кршна-пакша";
        tithiStr += " - ";
        tithiStr += nameStr();
        tithiStr += "\n";
        tithiStr += "(";
        if (m_Paksha)
            tithiStr += "Растущая Луна";
        else
            tithiStr += "Убывающая Луна";
        tithiStr += " - ";
        tithiStr += QString::number(m_Num) + " титхи";
        tithiStr += ")";
        tithiStr += "\n";

        tithiStr += asTithiStr();
    }

    return tithiStr;
}
//---------------------------

QString TTitha::asTithiStrExt(const double timeZoneOffset) const
{
    QString result;

    if (true == isValid())
    {
        if (m_Paksha)
            result += "шукла-пакша";
        else
            result += "кршна-пакша";
        result += " - ";
        result += nameStr();
        result += "\n";
        result += "(";
        if (m_Paksha)
            result += "Растущая Луна";
        else
            result += "Убывающая Луна";
        result += " - ";
        result += QString::number(m_Num) + " титхи";
        result += ")";
        result += "\n";

        result += asTithiStr(timeZoneOffset);
    }

    return result;
}
//---------------------------

QString TTitha::asCurTithiStr() const
{
    QString tithiStr;

    if (true == isValid())
    {
        tithiStr = "Сейчас\n";
        tithiStr += asTithiStrExt();
    }

    return tithiStr;
}
//---------------------------

QString TTitha::asCurTithiStr(const double timeZoneOffset) const
{
    QString result;

    if (true == isValid())
    {
        result = "Сейчас\n";
    }

    return result += asTithiStrExt(timeZoneOffset);
}
//---------------------------

QString TTitha::asEkadashStr() const
{
    QString ekadashStr;

    if (true == isValid())
    {
        if (m_Paksha)
            ekadashStr += "шукла-пакша (Растущая Луна)\n";
        else
            ekadashStr += "кршна-пакша (Убывающая Луна)\n";
        ekadashStr += asTithiStr();
    }

    return ekadashStr;
}
//---------------------------

QString TTitha::asEkadashStr(const double timeZoneOffset) const
{
    QString result;

    if (true == isValid())
    {
        if (m_Paksha)
            result += "шукла-пакша (Растущая Луна)\n";
        else
            result += "кршна-пакша (Убывающая Луна)\n";
    }

    return result += asTithiStr(timeZoneOffset);
}
//---------------------------

bool TTitha::roundToMin()
{
    bool result (false);

    // округление времени до минуты
    if (true == isValid())
    {
        QTime t;
        int second;

        t = dtBegin.time();
        second = t.second();
        t.setHMS(t.hour(),t.minute(),0);
        dtBegin.setTime(t);
        if (second >= secsInMin/2)
            dtBegin.addSecs(secsInMin);

        t = dtEnd.time();
        second = t.second();
        t.setHMS(t.hour(),t.minute(),0);
        dtEnd.setTime(t);
        if (second >= secsInMin/2)
            dtEnd.addSecs(secsInMin);
        result = true;
    }
    return result;
}
//---------------------------

quint8 TTitha::tithaNum(const QDateTime& dt)
{
    CAADate d;
    double JD;
    double JDSunMoon;
    double sunLng;
    double moonLng;
    quint8 tithaNum (0);

    if (true == dt.isValid())
    {
        d.Set(dt.date().year(),dt.date().month(),dt.date().day(),
              dt.time().hour(),dt.time().minute(),dt.time().second()+dt.time().msec()/double(msecsInSec),true);
        JD = d.Julian();
        JDSunMoon = CAADynamicalTime::UTC2TT(JD);
        sunLng = CAASun::ApparentEclipticLongitude(JDSunMoon);
        moonLng = CAAMoon::EclipticLongitude(JDSunMoon);
        if (moonLng < sunLng)
            tithaNum = qCeil((moonLng + fullRoundDeg - sunLng) / tithaDeg);
        else
            tithaNum = qCeil((moonLng - sunLng) / tithaDeg);
    }
    return tithaNum;
}
//---------------------------

TTitha TTitha::searchForCurrentTitha(const double timeZoneOffset)
{
    TTitha titha;
    bool paksha = true, prevPaksha = paksha;
    QDateTime dt = QDateTime::currentDateTimeUtc(), dt1 = QDateTime::currentDateTimeUtc();
    qint64 step = -1 * msecsInSec*secsInMin*minsInHour*(hoursInDay/2), prevStep = step, minStep = msecsInSec*secsInMin;
    bool f = true;
    quint8 tithaNum1 = 0, curTithaNum = 0;

    dt1 = dt;

    while(true)
    {
        tithaNum1 = tithaNum(dt);

        if (0 == tithaNum1)
            break;
        else if (tithaNum1 > 15)
        {
            paksha = false;
            tithaNum1-=15;
        }
        else
            paksha = true;

        if (f)
        {
            curTithaNum = tithaNum1;
            titha.m_Num = tithaNum1;
            titha.m_Paksha = paksha;
            f = false;
        }
        else if ((step < 0) && ((tithaNum1 < curTithaNum) || (paksha != prevPaksha)))
        {
            if (qAbs(step) > minStep)
            {
                dt.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step);
                step /= 2;
                dt.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
                continue;
            }

            titha.dtBegin.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() + qAbs(step) + (msecsInSec*secsInMin*minsInHour*timeZoneOffset));

            dt = dt1;
            f = true;
            step = qAbs(prevStep);
            continue;
        }
        else if ((step > 0) && ((tithaNum1 > curTithaNum) || (paksha != prevPaksha)))
        {
            if (step > minStep)
            {
                dt.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step);
                step /= 2;
                dt.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
                continue;
            }

            titha.dtEnd.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() - step + (msecsInSec*secsInMin*minsInHour*timeZoneOffset));
            titha.roundToMin();

            break;
        }

        dt.setMSecsSinceEpoch(dt.toMSecsSinceEpoch() + step);
        prevPaksha = paksha;
    }

    return titha;
}
//---------------------------

TTitha TTitha::findCurrentTitha(const double timeZoneOffset, const QDateTime& dt)
{
    TTitha titha;
    qint8 tithaN (0);
    qint8 prevTithaN (tithaN);
    QDateTime dt2 (dt);    
    qint64 step (-1 * maxStepFindCurTitha);         // начальный (максимальный) шаг для поиска времени начала титхи

    // проверка входных данных
    if ((true == dt.isValid()) && (true == isTimeZoneOffsetValid(timeZoneOffset)))
    {
        // первое значение
        tithaN = tithaNum(dt2);
        prevTithaN = tithaN;

        if (tithaN > 0)
        {
            // получено валидное значение номера титхи
            // запись в возвращаемый объект полученной информации
            if (tithaN <= 15)
            {
                titha.m_Num = tithaN;
                titha.m_Paksha = true;
            }
            else
            {
                titha.m_Num = tithaN - 15;
                titha.m_Paksha = false;
            }

            // шаг
            dt2 = dt2.addMSecs(+step);

            while (tithaN == prevTithaN)
            {
                // поиск времени начала текущей титхи
                prevTithaN = tithaN;
                tithaN = tithaNum(dt2);
                if (tithaN > 0)
                {
                    // получено валидное значение номера титхи
                    if ((tithaN < prevTithaN) || ((tithaN == 30) && (prevTithaN == 1)))
                    {
                        // значение титхи изменилось
                        if (qAbs(step) > minStepFindCurTitha)
                        {
                            // шаг "назад" и продолжить с меньшим шагом
                            dt2 = dt2.addMSecs(- step);
                            tithaN = prevTithaN;
                            step /= 2;
                        }
                        else
                        {
                            // время начала титхи найдено (записать с учётом часового пояса)
                            titha.dtBegin = dt2.addMSecs(-step + static_cast<qint64>(msecsInSec*secsInMin*minsInHour*timeZoneOffset));
                        }
                    }
                }
                // шаг
                dt2 = dt2.addMSecs(+ step);
            }

            // установить шаг для поиска завершения титхи
            step = maxStepFindCurTitha;
            tithaN = prevTithaN;
            dt2 = dt;

            while (tithaN == prevTithaN)
            {
                // поиск времени завершения текущей титхи
                prevTithaN = tithaN;
                tithaN = tithaNum(dt2);
                if (tithaN > 0)
                {
                    // получено валидное значение номера титхи
                    if ((tithaN > prevTithaN) || ((tithaN == 1) && (prevTithaN == 30)))
                    {
                        // значение номера титхи изменилось
                        if (step > minStep)
                        {
                            // шаг "назад" и продолжить с меньшим шагом
                            dt2 = dt2.addMSecs(- step);
                            tithaN = prevTithaN;
                            step /= 2;
                        }
                        else
                        {
                            // время завершения титхи найдено (записать с учётом часового пояса и округлить до минуты)
                            titha.dtEnd = dt2.addMSecs(-step + static_cast<qint64>(msecsInSec*secsInMin*minsInHour*timeZoneOffset));
                            titha.roundToMin();
                        }
                    }
                }
                // шаг
                dt2 = dt2.addMSecs(+step);
            }
        }
    }

    return titha;
}
//---------------------------

QList<TTitha> TTitha::searchForTithi(QDateTime& date1, QDateTime& date2, double timeZoneOffset)
{
    QList<TTitha> tithi;
    TTitha titha;
    bool paksha = true, prevPaksha = paksha;
    qint32 step = msecsInSec*secsInMin*minsInHour*hoursInDay, prevStep = step, minStep = msecsInSec*secsInMin;
    quint8 tithaNum1 (0), prevTithaNum (0);
    bool f = true, tf = false;

    while (date1 <= date2)
    {
        tithaNum1 = tithaNum(date1);
        if (0 == tithaNum1)
            break;
        else if (tithaNum1 > 15)
        {
            tithaNum1-=15;
            paksha = false;
        }
        else
            paksha = true;

        if(f)
        {
            prevTithaNum = tithaNum1;
            prevPaksha = paksha;
            f = false;
        }
        else if ((tithaNum1 > prevTithaNum) || (paksha != prevPaksha))
        {
            if (step > minStep)
            {
                date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() - step);
                step /= 2;
                date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + step);
                continue;
            }

            if (tf)
            {
                titha.dtEnd.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() - step + (msecsInSec * secsInMin * minsInHour * timeZoneOffset));

                titha.roundToMin();
                tithi.append(titha);

                step = prevStep;
            }
            else
                tf = true;

            titha.dtBegin.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + (msecsInSec * secsInMin * minsInHour * timeZoneOffset));
            titha.m_Num = tithaNum1;
            titha.m_Paksha = paksha;

            prevTithaNum = tithaNum1;
            prevPaksha = paksha;
        }
        date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + step);
    }

    return tithi;
}
//---------------------------

QList<TTitha> TTitha::findTithi(const QDateTime& dateTime1, const QDateTime& dateTime2, const double timeZoneOffset)
{
    QList<TTitha> tithi;
    TTitha titha;
    QDateTime dt1 (dateTime1);
    QDateTime dt2 (dateTime2);
    quint8 tithaN (0);
    quint8 prevTithaN (tithaN);
    qint64 step (maxStepFindTithi);     // начальный (максимальный) шаг для поиска времени начала титхи
    bool tf (false);                    // флаг того, что в цикле определено время начала первой титхи

    // проверка входных данных
    if ((true == dt1.isValid()) && (true == dt2.isValid()) && (true == isTimeZoneOffsetValid(timeZoneOffset)))
    {
        // определить первую титху
        titha = findCurrentTitha(timeZoneOffset,dt1);
        if (true == titha.isValid())
        {
            tithi.append(titha);
            prevTithaN = titha.num();
            if (false == titha.m_Paksha)
                prevTithaN += 15;

            while (dt1 <= dt2)
            {
                tithaN = tithaNum(dt1);
                if (tithaN > 0)
                {
                    // получено валидное значение номера титхи
                    if ((tithaN > prevTithaN) || ((tithaN == 1) && (prevTithaN == 30)))
                    {
                        // значение номера титхи изменилось
                        if (step > minStepFindTithi)
                        {
                            // шаг "назад" и продолжить с меньшим шагом
                            dt1 = dt1.addMSecs(-step);
                            step /= 2;
                        }
                        else
                        {
                            if (true == tf)
                            {
                                // найдено время завершения текущей титхи
                                // запись в объект с учётом часового пояса и округление до минуты
                                titha.dtEnd = dt1.addMSecs(-step + static_cast<qint64>(msecsInSec*secsInMin*minsInHour*timeZoneOffset));
                                titha.roundToMin();

                                // добавление определённой титхи в список
                                tithi.append(titha);

                                // задать шагу снова максимальное значение
                                step = maxStepFindTithi;
                                // подготовка к следующей итерации
                                prevTithaN = tithaN;
                            }

                            // найдено время начала текущей титхи
                            // запись данных в объект с учётом часового пояса
                            if (tithaN <= 15)
                            {
                                titha.m_Num = tithaN;
                                titha.m_Paksha = true;
                            }
                            else
                            {
                                titha.m_Num = tithaN - 15;
                                titha.m_Paksha = false;
                            }
                            titha.dtBegin = dt1.addMSecs(+static_cast<qint64>(msecsInSec*secsInMin*minsInHour*timeZoneOffset));

                            if (false == tf)
                            {
                                // задать шагу снова максимальное значение
                                step = maxStepFindTithi;
                                // поднять флаг того, что в цикле найдено время начала первой титхи
                                tf = true;
                                // подготовка к следующей итерации
                                prevTithaN = tithaN;
                            }
                        }
                    }
                }
                // шаг
                dt1 = dt1.addMSecs(+step);
            }
        }
    }

    return tithi;
}
//---------------------------

QList<TTitha> TTitha::searchForEkadashi(QDateTime& date1, QDateTime& date2, const double timeZoneOffset)
{
    QList<TTitha> ekadashi;
    TTitha ekadash;
    quint8 tithaNum1 (0);
    bool paksha = true;
    qint64 step = msecsInSec*secsInMin*minsInHour*(hoursInDay/2), prevStep = step, minStep = msecsInSec*secsInMin;
    bool e = false, f = true;

    while (date1 <= date2)
    {
        tithaNum1 = tithaNum(date1);
        if (0 == tithaNum1)
            break;
        else if (tithaNum1 > 15)
        {
            tithaNum1-=15;
            paksha = false;
        }
        else
            paksha = true;

        if ((11 != tithaNum1) && f)
            f = false;
        else if ((11 == tithaNum1) && (false == e) && (false == f))
        {
            if (step > minStep)
            {
                date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() - step);
                step /= 2;
                date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + step);
                continue;
            }

            ekadash.dtBegin.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + (msecsInSec * secsInMin * minsInHour * timeZoneOffset));
            ekadash.m_Num = tithaNum1;
            ekadash.m_Paksha = paksha;

            step = prevStep;
            e = true;
        }
        else if ((12 == tithaNum1) && e)
        {
            if (step > minStep)
            {
                date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() - step);
                step /= 2;
                date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + step);
                continue;
            }

            ekadash.dtEnd.setMSecsSinceEpoch((date1.toMSecsSinceEpoch() - step) + (msecsInSec * secsInMin * minsInHour * timeZoneOffset));
            ekadash.roundToMin();
            ekadashi.append(ekadash);

            e = false;
            step = prevStep;
        }

        date1.setMSecsSinceEpoch(date1.toMSecsSinceEpoch() + step);
    }

    return ekadashi;
}
//---------------------------

QList<TTitha> TTitha::findEkadashi(const QDateTime& dateTime1, const QDateTime& dateTime2, const double timeZoneOffset)
{
    QList<TTitha> ekadashi;
    TTitha titha;
    TTitha ekadash;
    QDateTime dt1 (dateTime1);
    QDateTime dt2 (dateTime2);
    quint8 tithaN (0);
    qint64 step (maxStepFindEkadashi);      // начальный (максимальный) шаг для поиска времени начала титхи
    bool e (false);                         // флаг того, что найдено начало очередного экадаша


    // проверка входных данных
    if ((true == dt1.isValid()) && (true == dt2.isValid()) && (true == isTimeZoneOffsetValid(timeZoneOffset)))
    {
        // всегда будет это значение
        ekadash.m_Num = 11;

        // определить первую титху
        titha = findCurrentTitha(timeZoneOffset,dt1);
        if (true == titha.isValid())
        {
            if (11 == titha.num())
                ekadashi.append(titha);

            while (dt1 <= dt2)
            {
                tithaN = tithaNum(dt1);
                if (tithaN > 0)
                {
                    // получено валидное значение номера титхи
                    if (((11 == tithaN)  || (26 == tithaN)) && (false == e))
                    {
                        // найдено начало экадаши титхи
                        if (step > minStepFindEkadashi)
                        {
                            // шаг "назад" и продолжить с меньшим шагом
                            dt1 = dt1.addMSecs(-step);
                            step /= 2;
                        }
                        else
                        {
                            // запись в переменную с учётом часового пояса
                            ekadash.dtBegin = dt1.addMSecs(+static_cast<qint64>(msecsInSec*secsInMin*minsInHour*timeZoneOffset));
                            if (11 == tithaN)
                                ekadash.m_Paksha = true;
                            else
                                ekadash.m_Paksha = false;

                            // установить шаг снова в максимальное для этого алгоритма значение
                            step = maxStepFindEkadashi;
                            // поднять флаг того, что найдено время начала экадаша
                            e = true;
                        }
                    }
                    else if ((11 != tithaN) && (26 != tithaN) && (true == e))
                    {
                        // найдено завершение экадаши титхи
                        if (step > minStepFindEkadashi)
                        {
                            // шаг "назад" и продолжить с меньшим шагом
                            dt1 = dt1.addMSecs(-step);
                            step /= 2;
                        }
                        else
                        {
                            // запись в переменную с учётом часового пояса и округление до минуты
                            ekadash.dtEnd = dt1.addMSecs(-step + static_cast<qint64>(msecsInSec*secsInMin*minsInHour*timeZoneOffset));
                            ekadash.roundToMin();

                            // добавление вычисленного экадаша в список
                            ekadashi.append(ekadash);

                            // установить шаг снова в максимальное для этого алгоритма значение
                            step = maxStepFindEkadashi;
                            // опустить флаг того, что найдено начало очередного экадаша
                            e = false;
                        }
                    }
                }
                // шаг
                dt1 = dt1.addMSecs(+step);
            }
        }
    }

    return ekadashi;
}
//---------------------------

TTitha TTitha::findNearestEkadash(const double timeZoneOffset)
{
    TTitha result;
    QList<TTitha> ekadashi (findEkadashi(QDateTime::currentDateTimeUtc(),(QDateTime::currentDateTimeUtc()).addMonths(1),timeZoneOffset));

    // первый экадаш в списке является ближайшим экадашем
    if (false == (ekadashi.isEmpty()))
    {
        result = ekadashi.first();

        // Если ближайший экадаш сейчас, то возьмём следующий (при наличии такового)
        if (QDateTime::currentDateTime() >= result.beginDateTime())
            if (ekadashi.size() >= 2)
                result = ekadashi[1];
    }

    return result;
}
//---------------------------
// КОНЕЦ: TTitha - public
//---------------------------------------------------------------------------------

// НАЧАЛО: TTitha - protected
bool TTitha::isTimeZoneOffsetValid (const double timeZoneOffset)
{
    bool result (true);

    // TODO добавить реализацию и соответствующие константы

    return result;
}
//---------------------------
// КОНЕЦ: TTitha - protected
//---------------------------------------------------------------------------------

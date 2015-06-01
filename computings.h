#ifndef COMPUTINGS_H
#define COMPUTINGS_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QtGlobal>
#include <QtCore>
//#include <QDateTime>
#include "aaplus/AA+.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


/// \class различные вычисления, не подходящие под описание в других классах
class TComputings
{
public:

    /// \enum соответствие типа сумерек и положения Солнца под горизонтом в градусах
    enum TwilightType
    {
        // гражданские
        civilTwilight = -6,
        // навигационные
        navigationTwilight = -12,
        // астрономические
        astronomicalTwilight = -18
    };
    //---------------------------

    /// \brief вычислить текущее среднее солнечное время в заданном месте относительно текущего универсального времени (UTC)
    /// @param longitude - географическая долгота места для расчёта (со знаком '+' для восточной долготы!)
    /// @retval текущее среднее солнечное время
    static QTime sunTimeAverage(const double longitude);

    /// \brief вычислить текущее истинное солнечное время в заданном месте относительно текущего универсального времени (UTC)
    /// @param longitude - географическая долгота места для расчёта (со знаком '+' для восточной долготы!)
    /// @param N - номер дня в году: 1 для первого января и т.д.
    /// @retval текущее истинное солнечное время
    static QTime sunTimeTrue(const double longitude, const quint32 N = QDateTime::currentDateTimeUtc().date().dayOfYear());

    /// \brief вычислить время восхода Солнца в заданную дату и в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date -  дата
    /// @retval время в заданном часовом поясе, в случае неудачи невалидное время
    static QTime sunTimeRise(const double longitude, const double latitude, const double timeZoneOffset = 0, const QDate& date = QDate::currentDate());

    /// \brief вычислить время захода Солнца в заданную дату и в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date -  дата
    /// @retval время в заданном часовом поясе, в случае неудачи невалидное время
    static QTime sunTimeSet(const double longitude, const double latitude, const double timeZoneOffset = 0, const QDate& date = QDate::currentDate());

    /// \brief вычислить время самой высокой точки нахождения Солнца (Зенит) в заданную дату и в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param aboveHorizont - точка (зенит) над горизонтом или под горизонтом
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date -  дата
    /// @retval время в заданном часовом поясе, в случае неудачи невалидное время
    static QTime sunTimeTransit(const double longitude, const double latitude, bool& aboveHorizont,
                                const double timeZoneOffset = 0, const QDate& date = QDate::currentDate());

    /// \brief найти время начала и завершения утренних солнечных сумерек в заданном месте в заданную дату
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param degree - значение вертикальной координаты солнца в градусах (-90 - +90) при котором фиксируется начало сумерек
    /// @param date -  дата
    /// @retval пару время начала и время завершения сумерек в заданном часовом поясе, в случае неудачи пару невалидных времен
    static QPair<QTime,QTime> sunTimeMorningTwilight(const double longitude, const double latitude, const double height = 0, const double timeZoneOffset = 0,
                                                     const double degree = static_cast<double>(civilTwilight), const QDate& date = QDate::currentDate());

    /// \brief найти время начала и завершения утренних солнечных сумерек в заданном месте в заданную дату (с использованием уже вычисленного времени восхода)
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param sunRise - ранее вычисленное время восхода (не UTC)
    /// @param height - высота над уровнем моря
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param degree - значение вертикальной координаты солнца в градусах (-90 - +90) при котором фиксируется начало сумерек
    /// @param date -  дата
    /// @retval пару время начала и время завершения сумерек в заданном часовом поясе, в случае неудачи пару невалидных времен
    static QPair<QTime,QTime> sunTimeMorningTwilight(const double longitude, const double latitude, const QTime& sunRise, const double height = 0,
                                                     const double timeZoneOffset = 0, const double degree = static_cast<double>(civilTwilight),
                                                     const QDate& date = QDate::currentDate());

    /// \brief найти время начала и завершения утренних солнечных сумерек в заданном месте в заданную дату с учётом рефракции
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param pressure - атмосферное давление в гПа (гекта Паскалях (10^2))
    /// @param temperature - температура, градусов Цельсия
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param degree - значение вертикальной координаты солнца в градусах (-90 - +90) при котором фиксируется начало сумерек
    /// @param date -  дата
    /// @retval пару время начала и время завершения сумерек в заданном часовом поясе, в случае неудачи пару невалидных времен
    static QPair<QTime,QTime> sunTimeMorningTwilightWithRefraction(const double longitude, const double latitude, const double height = 0, const double pressure = 1013.25,
                                                                   const double temperature = 20, const double timeZoneOffset = 0,
                                                                   const double degree = static_cast<double>(civilTwilight), const QDate& date = QDate::currentDate());

    /// \brief найти время начала и завершения вечерних солнечных сумерек в заданном месте в заданную дату
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param degree - значение вертикальной координаты солнца в градусах (-90 - +90) при котором фиксируется начало сумерек
    /// @param date -  дата
    /// @retval пару время начала и время завершения сумерек в заданном часовом поясе, в случае неудачи пару невалидных времен
    static QPair<QTime,QTime> sunTimeEveningTwilight(const double longitude, const double latitude, const double height = 0, const double timeZoneOffset = 0,
                                                     const double degree = static_cast<double>(civilTwilight), const QDate& date = QDate::currentDate());

    /// \brief найти время начала и завершения вечерних солнечных сумерек в заданном месте в заданную дату (с использованием уже вычисленного времени захода)
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param sunSet - ранее вычисленное время захода (не UTC)
    /// @param height - высота над уровнем моря
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param degree - значение вертикальной координаты солнца в градусах (-90 - +90) при котором фиксируется начало сумерек
    /// @param date -  дата
    /// @retval пару время начала и время завершения сумерек в заданном часовом поясе, в случае неудачи пару невалидных времен
    static QPair<QTime,QTime> sunTimeEveningTwilight(const double longitude, const double latitude, const QTime& sunSet, const double height = 0,
                                                     const double timeZoneOffset = 0, const double degree = static_cast<double>(civilTwilight),
                                                     const QDate& date = QDate::currentDate());

    /// \brief найти время начала и завершения вечерних солнечных сумерек в заданном месте в заданную дату с учётом рефракции
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param pressure - атмосферное давление в гПа (гекта Паскалях (10^2))
    /// @param temperature - температура, градусов Цельсия
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param degree - значение вертикальной координаты солнца в градусах (-90 - +90) при котором фиксируется начало сумерек
    /// @param date -  дата
    /// @retval пару время начала и время завершения сумерек в заданном часовом поясе, в случае неудачи пару невалидных времен
    static QPair<QTime,QTime> sunTimeEveningTwilightWithRefraction(const double longitude, const double latitude, const double height = 0, const double pressure = 1013.25,
                                                                   const double temperature = 20, const double timeZoneOffset = 0,
                                                                   const double degree = static_cast<double>(civilTwilight), const QDate& date = QDate::currentDate());

    /// \brief вычислить утреннюю/вечернюю сандхью как 1/10 часть от половины суток до восхода/после захода Солнца в заданном месте в заданную дату
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param morningSandhya - true для утренней сандхьи, false - для вечерней
    /// @param date -  дата
    /// @retval пара время начала и время завершения сандхьи, в случае неудачи пара невалидных времен
    static QPair<QTime,QTime> sunTimeSandhyaAsDayPart(const double longitude, const double latitude, const double timeZoneOffset = 0,
                                                      const bool morningSandhya = true, const QDate& date = QDate::currentDate());

    /// \brief вычислить утреннюю/вечернюю сандхью как 1/10 часть от половины суток до восхода/после захода Солнца в заданном месте в заданную дату (с использованием уже вычисленного времени восхода/захода)
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param sunRiseSet - время восхода или захода Солнца, вычисленное ранее (не UTC)
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param morningSandhya - true для утренней сандхьи, false - для вечерней
    /// @param date - дата
    /// @retval пара время начала и время завершения сандхьи, в случае неудачи пара невалидных времен
    static QPair<QTime,QTime> sunTimeSandhyaAsDayPart(const double longitude, const double latitude, const QTime& sunRiseSet, const double timeZoneOffset = 0,
                                                      const bool morningSandhya = true, const QDate& date = QDate::currentDate());

    /// \brief вычислить утреннюю/вечернюю сандхью как 1/10 часть от светового дня в заданном месте в заданную дату
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param morningSandhya - true для утренней сандхьи, false - для вечерней
    /// @param date -  дата
    /// @retval пара время начала и время завершения сандхьи, в случае неудачи пара невалидных времен
    static QPair<QTime,QTime> sunTimeSandhyaAsLightDayPart(const double longitude, const double latitude, const double timeZoneOffset = 0,
                                                           const bool morningSandhya = true, const QDate& date = QDate::currentDate());

    /// \brief вычислить утреннюю/вечернюю сандхью как 1/10 часть от светового дня в заданном месте в заданную дату (с использованием уже вычисленного времени восхода/захода)
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param sunRise - время восхода Солнца, вычисленное ранее (не UTC)
    /// @param sunSet - время захода Солнца, вычисленное ранее (не UTC)
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param morningSandhya - true для утренней сандхьи, false - для вечерней
    /// @param date -  дата
    /// @retval пара время начала и время завершения сандхьи, в случае неудачи пара невалидных времен
    static QPair<QTime,QTime> sunTimeSandhyaAsLightDayPart(const double longitude, const double latitude, const QTime& sunRise, const QTime& sunSet,
                                                           const double timeZoneOffset = 0, const bool morningSandhya = true,
                                                           const QDate& date = QDate::currentDate());

    /// \brief вычислить время восхода Луны в заданную дату и в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date -  дата
    /// @retval время в заданном часовом поясе, в случае неудачи невалидное время
    static QTime moonTimeRise(const double longitude, const double latitude, const double timeZoneOffset = 0, const QDate& date = QDate::currentDate());

    /// \brief вычислить время захода Луны в заданную дату и в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date -  дата
    /// @retval время в заданном часовом поясе, в случае неудачи невалидное время
    static QTime moonTimeSet(const double longitude, const double latitude, const double timeZoneOffset = 0, const QDate& date = QDate::currentDate());

    /// \brief вычислить время самой высокой точки нахождения Луны (Зенит) в заданную дату и в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param aboveHorizont - точка (зенит) над горизонтом или под горизонтом
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date -  дата
    /// @retval время в заданном часовом поясе, в случае неудачи невалидное время
    static QTime moonTimeTransit(const double longitude, const double latitude, bool& aboveHorizont,
                                 const double timeZoneOffset = 0, const QDate& date = QDate::currentDate());

    /// \brief найти дату-время предыдущего новолуния
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param dt - UTC дата-время, относительно которой начать поиск
    /// @retval дата-время предыдущего новолуния (с учётом часового пояса), в случае неудачи не валидная дата-время
    static QDateTime moonTimeFindPreviousNewMoon(const double timeZoneOffset = 0, const QDateTime& dateTime = QDateTime::currentDateTimeUtc());

    /// \brief найти дату-время следующего новолуния
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param dt - UTC дата-время, относительно которой начать поиск
    /// @retval дата-время следующего новолуния (с учётом часового пояса), в случае неудачи не валидная дата-время
    static QDateTime moonTimeFindNextNewMoon(const double timeZoneOffset = 0, const QDateTime& dateTime = QDateTime::currentDateTimeUtc());

    /// \brief найти новолуния за заданный период времени
    /// @param dateTime1 - UTC дата-время начала поисков
    /// @param dateTime2 - UTC дата-время завершения поисков
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval список дат-времени новолуний
    static QList<QDateTime> moonTimeFindNewMoonForPeriod(const QDateTime& dateTime1, const QDateTime& dateTime2, const double timeZoneOffset = 0);

    /// \brief вычислить фазу Луны в заданную дату
    /// @param date -  дата
    /// @retval % фазы Луны, в случае не удачи -1
    static qint32 moonTimePhase(const QDate& date = QDate::currentDate());

    // лунный день - восход, заход, зенит, новолуние (валидно, когда новолуние), номер.
    // вариант для рассчётов, основанных на горизонтальных координатах и переменных дата-время
    struct TMoonDay
    {
        QDateTime rise;
        QDateTime set;
        QDateTime transit;
        QDateTime newMoon;
        QString num;

        bool operator ==(const TMoonDay& rv) const;
    };
    //---------------------------

    /// \brief вычислить список "лунных дней", (дата-время начала и завершения дня) расширенный вариант с учётом поведения Луны в приполярье
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param dt1 - UTC дата-время для начала отсчёта
    /// @param dt2 - UTC дата-время для завершения отсчёта
    /// @param height - высота над уровнем моря
    /// @retval список лунных дней, пустой список в случае неудачи
    static QList<TMoonDay> moonTimeMoonDays(const double longitude, const double latitude, const double timeZoneOffset = 0,
                                                const QDateTime& dateTime1 = moonTimeFindPreviousNewMoon(),
                                                const QDateTime& dateTime2 = QDateTime::currentDateTimeUtc(), const double height = 0);

    // свара лунная или солнечная, начало, конец, порядковый номер
    struct TSvara
    {
        bool moonSvara;
        quint32 num;
        QTime begin;
        QTime end;
    };
    //---------------------------

    /// \brief вычислить список свар на заданную дату (с вычислением внутри времени восхода и захода Солнца)
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @param date - дата для вычисления
    /// @retval список свар, в случае неудачи пустой список
    static QList<TSvara> sunMoonTimeSvaraList(const double longitude, const double latitude, const double timeZoneOffset = 0,
                                              const QDate& date = QDate::currentDate());

    /// \brief вычислить список свар на заданную дату (с заранее вычисленным временем восхода и захода Солнца)
    /// @param sunRise - время восхода Солнца (не UTC)
    /// @param sunSet - время захода Солнца (не UTC)
    /// @retval список свар, в случае неудачи пустой список
    static QList<TSvara> sunMoonTimeSvaraList(const QTime& sunRise, const QTime& sunSet);

    /// \brief вычислить свару в данный момент
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval свара, в случае неудачи свара с невалидным временем начала и конца
    static TSvara sunMoonTimeCurrentSvara(const double longitude, const double latitude, const double timeZoneOffset = 0);

    /// \brief вычислить свару в данный момент (с заранее вычисленным временем восхода и захода Солнца)
    /// @param sunRise - время восхода Солнца (не UTC)
    /// @param sunSet - время захода Солнца (не UTC)
    /// @retval свара, в случае неудачи свара с невалидным временем начала и конца
    static TSvara sunMoonTimeCurrentSvara(const QTime& sunRise, const QTime& sunSet);

    /// \brief определить горизонтальные координаты Солнца в заданный момент времени (UTC), в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param dateTime -  дата-время (UTC)
    /// @retval пара координат (X,Y)
    static QPair<double, double> sunHorizontalCoords(const double longitude, const double latitude, const double height = 0,
                                                     const QDateTime& dateTime = QDateTime::currentDateTimeUtc());

    /// \brief определить горизонтальные координаты Луны в заданный момент времени (UTC), в заданном месте
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param dateTime -  дата-время (UTC)
    /// @retval пара координат (X,Y)
    static QPair<double, double> moonHorizontalCoords(const double longitude, const double latitude, const double height = 0,
                                                     const QDateTime& dateTime = QDateTime::currentDateTimeUtc());

    /// \brief определить горизонтальные координаты Солнца в заданный момент времени (UTC), в заданном месте с учётом рефракции
    /// @param longitude - геогр. долгота (со знаком '-' для восточной долготы!)
    /// @param latitude - геогр. широта
    /// @param height - высота над уровнем моря
    /// @param pressure - атмосферное давление в гПа (гекта Паскалях (10^2))
    /// @param temperature - температура, градусов Цельсия
    /// @param dateTime -  дата-время (UTC)
    /// @retval пара координат (X,Y)
    static QPair<double, double> sunHorizontalCoordsWithRefraction(const double longitude, const double latitude, const double height = 0,  const double pressure = 1013.25,
                                                                   const double temperature = 20, const QDateTime& dateTime = QDateTime::currentDateTimeUtc());

    /// \brief преобразовать некоторую информацию о Солнце в текст
    /// @param rise - восход
    /// @param set - заход
    /// @param transit - зенит
    /// @param roundToMin - округлять время до минуты
    /// @retval строка с информацией о Солнце (восход, закат, зенит, долгота дня); в случае неудачи пустая строка
    static QString toStringSunTimeInfo(const QTime& rise, const QTime& set, const QTime& transit, const bool roundToMin = true);

    /// \brief преобразовать некоторую информацию о Солнце в текст
    /// @param civil - гражданские сумерки (до восхода/от захода)
    /// @param navigation - навигационные сумерки (до восхода/от захода)
    /// @param astronomical - астрономические сумерки (до восхода/от захода)
    /// @param morningTwilight - true для утренних сумерек, false для вечерних
    /// @param roundToMin - округлять время до минуты
    /// @retval строка с информацией о Солнце (утренние сумерки: гражданские, навигационные, астрономические); в случае неудачи пустая строка
    static QString toStringSunTimeInfo2(const QPair<QTime,QTime>& civilTwilight, const QPair<QTime,QTime>& navigationTwilight, const QPair<QTime,QTime>& astronomicalTwilight,
                                        const bool morningTwilight = true, const bool roundToMin = true);

    /// \brief преобразовать некоторую информацию о Солнце в текст
    /// @param sandhyaAsDayPart - сумерки как 1/10 часть от половины суток
    /// @param sandhyaAsLightDayPart - сумерки как 1/10 часть от светового дня
    /// @param morningTwilight - true для утренних сумерек, false для вечерних
    /// @param roundToMin - округлять время до минуты
    /// @retval строка с информацией о Солнце (утренняя сандхья, вечерняя сандхья в нескольких вариациях); в случае неудачи пустая строка
    static QString toStringSunTimeInfo3(const QPair<QTime,QTime>& sandhyaAsDayPart, const QPair<QTime,QTime>& sandhyaAsLightDayPart,
                                        const bool morningTwilight = true, const bool roundToMin = true);

    /// \brief преобразовать некоторую информацию о Луне в текст
    /// @param set - заход
    /// @param rise - восход
    /// @param transit - зенит
    /// @param roundToMin - округлять время до минуты
    /// @retval строка с информацией о Луне (восход, закат, зенит, долгота дня); в случае неудачи пустая строка
    static QString toStringMoonTimeInfo(const QTime& set, const QTime& rise, const QTime& transit, const bool roundToMin = true);

    /// \brief округлить время до ближайшей минуты (>= 0.5)
    /// @param time - время
    /// @retval округлённое время, в случае неудачи исходное время
    static QTime roundToMinTime(const QTime& time);

    /// \brief округлить пару времен до ближайшей минуты (>= 0.5)
    /// @param time - пара времен
    /// @retval округлённая пара времен, в случае неудачи исходная пара времен
    static QPair<QTime,QTime> roundToMinTime(const QPair<QTime,QTime>& time);

    /// \brief вернуть закэшированное в процессе рассчётов предыдущее новолуние
    /// @retval дата-время предыдущего новолуния, в случае неудачи не валидное дата-время
    static QDateTime prevNewMoon();

    /// \brief вернуть закэшированное в процессе рассчётов следующее новолуние
    /// @retval дата-время следующего новолуния, в случае не удачи не валидное дата-время
    static QDateTime nextNewMoon();

    /// \brief вернуть закэшированный в процессе рассчётов предыдущий вычисленный номер лунного дня (0 если не вычислен)
    /// @retval целое номер предыдущего вычисленного лунного дня
    static quint32 prevMoonDayNum();

protected:

    /// \enum перечисление используемых констант
    enum Consts
    {
        // секунд времени в одном градусе геогр. долготы
        secsInLngDeg = 240,
        // миллисекунд в секунде
        msecsInSec = 1000,
        // секунд в минуте
        secsInMin = 60,
        // минут в часе
        minsInHour = 60,
        // часов в сутках дня
        hoursInDay = 24,
        // смещение для универсального мирового времени
        UtcOffset = 0,
        // градус положения Солнца под горизонтом для начала/завершения гражданских сумерек
        civilTwilightDegree = -6,
        // градус положения Солнца под горизонтом для начала/завершения навигационных сумерек
        navigationTwilightDegree = -12,
        // градус положения Солнца под горизонтом для начала/завершения астрономических сумерек
        astronomicalTwilightDegree = -18,
        // минимальное и максимальное смещение часовых поясов
        timeZoneOffsetMin = -12,
        timeZoneOffsetMax = 14
    };

    // порог, если разница между долготами Солнца и Луны меньше которого, то считается новолуние
    static inline double newMoonFindLongitudeThreshold();

    /// \brief уравнение времени (используется в рассчётах истинного солнечного времени)
    /// @param N - номер дня в году: 1 для первого января и т.д.
    /// @retval смещение солнечного времени от среднего в минутах
    static double timeEquation(const quint32 N);

    /// \brief вычислить смещение в секундах среднего солнечного времени относительно универсального
    /// @param longitude - географическая долгота места для расчёта
    /// @retval смещение в секундах
    static qint32 sunTimeAverageOffset(const double longitude);

    /// \brief вычислить смещение в секундах истинного солнечного времени относительно универсального
    /// @param longitude - географическая долгота места для расчёта
    /// @param N - номер дня в году: 1 для первого января и т.д.
    /// @retval смещение в секундах
    static qint32 sunTimeTrueOffset(const double longitude, const quint32 N);

    /// \brief вспомогательная функция из астрономической библиотеки AA+
    static void getSolarRaDecByJulian(double JD, double& RA, double& Dec);

    /// \brief функция из астрономической библиотеки AA+ для вычисления восхода-заката-высшей_точки (зенита) Солнца
    static CAARiseTransitSetDetails getSunRiseTransitSet(double JD, double longitude, double latitude);

    /// \brief вспомогательная функция из астрономической библиотеки AA+
    static void getLunarRaDecByJulian(double JD, double& RA, double& Dec);

    /// \brief функция из астрономической библиотеки AA+ для вычисления восхода-заката-высшей_точки (зенита) Луны
    static CAARiseTransitSetDetails getMoonRiseTransitSet(double JD, double longitude, double latitude);

    /// \brief функция из астрономической библиотеки AA+ необходимая для вычисления фазы Луны
    static void getMoonIllumination(double JD, double& illuminated_fraction, double& position_angle, double& phase_angle);

    /// \brief функция на базе астрономической библиотеки AA+ определяет является ли заданное время временем новолуния
    /// @param dt - дата-время
    /// @param threshold - пороговое значение разницы эклиптических долгот Солнца и Луны
    static bool moonTimeIsNewMoon(const QDateTime& dt, const double threshold);

    /// \brief узнать является ли смещение часового пояса верным значением
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval true в случае подтверждения, иначе false
    static bool isTimeZoneOffsetValid(const double timeZoneOffset);
};
// КОНЕЦ: TComputings
//---------------------------------------------------------------------------------


#endif // COMPUTINGS_H

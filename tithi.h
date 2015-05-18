#ifndef TITHI_H
#define TITHI_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QList>
#include <QDateTime>

#include "aaplus/AA+.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: Классы
/// \class Титхи
class TTitha
{
public:
    TTitha();
    TTitha(QDateTime beginDateTime, QDateTime endDateTime, quint8 num, bool paksha);
    ~TTitha();

    /// \brief является ли титха верной
    /// \retval true в случае подтверждения, иначе false
    bool isValid() const;

    /// \brief является ли титха экадашем
    /// \retval true в случае подтверждения, иначе false
    bool isEkadash() const;

    /// \brief получить номер титхи
    /// \retval номер титхи (1-15)
    quint8 num()  const;

    /// \brief получить пакшу титхи
    /// \retval пакша титхи (true - шукла, false - кршна)
    bool paksha() const;

    /// \brief получить дату-время начала и завершения титхи
    /// \retval пара значений: дата-время начала и дата-время завершения титхи
    QPair<QDateTime,QDateTime> beginEndDateTime() const;

    /// \brief получить дату-время начала титхи
    /// \retval дата-время начала титхи
    QDateTime beginDateTime() const;

    /// \brief получить дату-время завершения титхи
    /// \retval дата-время завершения титхи
    QDateTime endDateTime() const;

    /// \brief задать новые данные
    /// \param beginDateTime - дата-время начала титхи
    /// \param endDateTime - дата-время завершения титхи
    /// \param num - номер титхи (1-15)
    /// \param paksha - пакша титхи (true - шукла, false - кршна)
    void setData(const QDateTime& beginDateTime, const QDateTime& endDateTime,
                 const quint8 num, const bool paksha);

//    void setNum(quint8 num);
//    void setPaksha(bool paksha);
//    void setBeginEndDateTime(QPair<QDateTime,QDateTime>& beginEndDateTime);
//    void setBeginDateTime(QDateTime& beginDateTime);
//    void setEndDateTime(QDateTime& endDateTime);

    /// \brief получить имя титхи
    /// \retval строка с именем титхи
    QString nameStr() const;

    /// \brief получить краткое описание титхи (начало и конец)
    /// \retval строка с кратким описанием титхи
    QString asTithiStr() const;

    /// \brief получить краткое описание титхи (начало и конец) с часовым поясом
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval строка с кратким описанием титхи
    QString asTithiStr(const double timeZoneOffset) const;

    /// \brief получить описание титхи
    /// \retval строка с описанием титхи
    QString asTithiStrExt() const;

    /// \brief получить описание титхи с часовым поясом
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval строка с описанием титхи
    QString asTithiStrExt(const double timeZoneOffset) const;

    /// \brief получить описание текущих титхи
    /// \retval строка с описанием текущих титхи
    QString asCurTithiStr() const;

    /// \brief получить описание текущих титхи с часовым поясом
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval строка с описанием текущих титхи
    QString asCurTithiStr(const double timeZoneOffset) const;

    /// \brief получить описание титхи как экадаша
    /// \retval строка с описанием титхи как экадаша
    QString asEkadashStr() const;

    /// \brief получить описание титхи как экадаша с часовым поясом
    /// @param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// @retval строка с описанием титхи как экадаша
    QString asEkadashStr(const double timeZoneOffset) const;

    /// \brief округлить значения даты-времени до минуты
    /// \retval в случае успеха возвращает true, иначе false
    bool roundToMin();

    /// \brief получить номер титхи (от 1 до 30) для заданного даты-времени
    /// \param dt - дата-время UTC
    /// \retval номер титхи (от 1 до 30)
    static quint8 tithaNum(const QDateTime& dt);

    /// \brief определить титху, которая идёт в данный момент времени
    /// (для определения положения планет используется геоцентрическая система координат)
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval титха
    static TTitha searchForCurrentTitha(const double timeZoneOffset);

    /// \brief определить титху, которая идёт в заданный момент времени (округление до минуты встроено)
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \param dt - дата-время вокруг которых необходимо определять титху (по умолчанию настоящий момент), обязательно UTC
    /// \retval титха, в случае неудачи не валидная титха
    static TTitha findCurrentTitha(const double timeZoneOffset, const QDateTime& dt = QDateTime::currentDateTimeUtc());

    /// \brief определить список титх в заданном интервале дата-время
    /// (переменные QDateTime должны быть обязательно инициализированы как UTC дата-время)
    /// (для определения положения планет используется геоцентрическая система координат)
    /// \param date1 - дата-время начала интервала определения титх
    /// \param date2 - дата-время завершения интервала определения титх
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval список титх
    static QList<TTitha> searchForTithi(QDateTime& date1, QDateTime& date2, double timeZoneOffset);

    /// \brief определить список титх в заданном интервале дата-время (округление до минуты встроено)
    /// (переменные QDateTime должны быть обязательно инициализированы как UTC дата-время)
    /// (для определения положения планет используется геоцентрическая система координат)
    /// \param date1 - дата-время начала интервала определения титх
    /// \param date2 - дата-время завершения интервала определения титх
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval список титх, в случае неудачи пустой список
    static QList<TTitha> findTithi(const QDateTime& dateTime1, const QDateTime& dateTime2, const double timeZoneOffset);

    /// \brief определить список экадашей в заданном интервале дата-время
    /// переменные QDateTime должны быть обязательно инициализированы как UTC дата-время
    /// \param date1 - дата-время начала интервала определения экадашей
    /// \param date2 - дата-время завершения интервала определения экадашей
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval список экадаши-титх, в случае неудачи пустой список
    static QList<TTitha> searchForEkadashi(QDateTime& date1, QDateTime& date2, double timeZoneOffset);


    /// \brief определить список экадашей в заданном интервале дата-время (округление до минуты встроено)
    /// (переменные QDateTime должны быть обязательно инициализированы как UTC дата-время)
    /// (для определения положения планет используется геоцентрическая система координат)
    /// \param date1 - дата-время начала интервала определения экадашей
    /// \param date2 - дата-время завершения интервала определения экадашей
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval список экадаши-титх, в случае неудачи пустой список
    static QList<TTitha> findEkadashi(const QDateTime& dateTime1, const QDateTime& dateTime2, const double timeZoneOffset);

    /// \brief Найти ближайший экадаш (округление до минуты встроено)
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval титха, в случае неудачи не валидная титха
    static TTitha findNearestEkadash(const double timeZoneOffset);

protected:
    ///< \enum определение констант связанных с географическими градусами
    enum DegreesConst
    {
        // градусов в полном круге
        fullRoundDeg = 360,
        // градусов в одной титхе
        tithaDeg = 12
    };

    ///< \enum определение констант связанных со временем
    enum TimeConst
    {
        // миллисекунд в секунде
        msecsInSec = 1000,
        // секунд в минуте
        secsInMin = 60,
        // минут в часе
        minsInHour = 60,
        // часов в сутках дня
        hoursInDay = 24
    };

    /// \enum определение констант связанных с шагом вычисления титх
    enum StepConst
    {
        minStepFindCurTitha = msecsInSec*secsInMin,
        maxStepFindCurTitha = msecsInSec*secsInMin*minsInHour*(hoursInDay/2),
        minStepFindTithi = msecsInSec*secsInMin,
        maxStepFindTithi = msecsInSec*secsInMin*minsInHour*(hoursInDay),
        minStepFindEkadashi = msecsInSec*secsInMin,
        maxStepFindEkadashi = msecsInSec*secsInMin*minsInHour*(hoursInDay/2),
        minStep = minStepFindCurTitha
    };

    /// \enum определение констант
    enum Const
    {
        // минимальное и максимальное смещение часовых поясов
        timeZoneOffsetMin = -12,
        timeZoneOffsetMax = 14
    };

    /// \brief узнать является ли смещение часового пояса верным значением
    /// \param timeZoneOffset - смещение в часах от "универсального мирового времени" UTC
    /// \retval true в случае подтверждения, иначе false
    static bool isTimeZoneOffsetValid(const double timeZoneOffset);

private:
    QDateTime dtBegin, dtEnd;       ///< начало, конец
    quint8 m_Num;                   ///< номер 1-15
    bool m_Paksha;                  ///< true - шукла-пакша, false - кршна-пакша
};
//---------------------------
// КОНЕЦ: Классы
//---------------------------------------------------------------------------------

#endif // TITHI_H

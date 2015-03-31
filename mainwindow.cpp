// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogsettings.h"
#include "dialogtithi.h"
#include "tithi.h"
#include "calendar.h"
#include "computings.h"
#include <QtMath>
#include <QtDebug>
#include <QSettings>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: MainWindow - public
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // инициализация переменных
    m_cTimer = 0;
    m_Date = QDate::currentDate();

    // соединения
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    connect(ui->actionExitApp, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

    // проверить на наличие настройки программы
    QSettings settings;
    if (true == settings.allKeys().isEmpty())
    {
        // настройки пусты, нужно запустить диалог настроек
        showSettingsDialog();
    }
//    bool ok;
//    double latitude (settings.value("latitude").toDouble(&ok));
//    double longitude (-1*settings.value("longitude").toDouble(&ok));
//    double height (settings.value("height").toDouble(&ok));
//    double timeZoneOffset (settings.value("timeZoneOffset").toDouble(&ok));

//    if (true != ok)
//    {
//        // не получилось загрузить данные из настроек программы, использовать значения по умолчанию
//        showSettingsDialog();

//        latitude = 55.7541;
//        longitude = -1*37.6204;
//        timeZoneOffset = 3.;
//        height = 0;
//    }

    // отобразить сегодня в календаре-виджете
    ui->calendarWidgetGregorian->showToday();
    ui->calendarWidgetGregorian->setSelectedDate(QDate::currentDate());

    // вывести год по ведическому летоисчислению
    showVedicDate();

    // вывести информацию по Солнцу
    showSunTime();

    // вывести информацию по Луне
    showMoonTime();

    // вывести информацию по титхам
    showTithi();

    // вернуть интерфейс на страницу о Солнце
    ui->tabWidget->setCurrentIndex(0);

    // запустить таймер для обновления времени
    m_Timer.start(timerInterval);

    // текущие титхи
//    qDebug() << "Текущая титха";
//    qDebug() << TTitha::findCurrentTitha(3).asCurTithiStr();
//    qDebug() << TTitha::searchForCurrentTitha(3).asCurTithiStr();

//    qDebug() << "offsetFromUtc" << QDateTime::currentDateTime().timeZone().offsetFromUtc(QDateTime::currentDateTime());

    // ведический год
//    qDebug() << "год от начала Кали-юги" << TVedicCalendar::gregorianToVedicKaliYuga(QDate::currentDate().year());
//    qDebug() << "год от начала Маха-юги" << TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicMahaYuga(QDate::currentDate().year()));
//    qDebug() << "год от начала Манвантары" << TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicManvantara(QDate::currentDate().year()));
//    qDebug() << "год от начала Кальпы" << TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicKalpa(QDate::currentDate().year()));
//    qDebug() << "год от начала МахаКальпы" << TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicMahaKalpa(QDate::currentDate().year()));

    // список титх
//    QDateTime dt (QDateTime::currentDateTimeUtc());
//    QList<TTitha> tithi = TTitha::findTithi(dt, dt.addYears(1),3);
//    qint64 len (0);
//    qint64 shukla (0);
//    qint64 krshna (0);

//    qDebug() << "Список титх";
//    for (QList<TTitha>::const_iterator i = tithi.begin(); i < tithi.end(); ++i)
//    {
//        qDebug() << (*i).asTithiStrExt();
//        len = (*i).beginDateTime().secsTo((*i).endDateTime());
//        qDebug() << "продолжительность" << len / 3600.;
//        if (true == (*i).paksha())
//            shukla += len;
//        else
//            krshna += len;
//    }
//    qDebug() << "продолжительность шукла-пакши" << shukla / 3600.;
//    qDebug() << "продолжительность кришна-пакши" << krshna / 3600.;
//    if (shukla > krshna)
//        qDebug() << "шукла - кршна" << (shukla - krshna) / 3600.;
//    else
//        qDebug() << "кршна - шукла" << (krshna - shukla) / 3600.;

//    // экадаши
//    QList<TTitha> ekadashi = TTitha::findEkadashi(dt,dt.addMonths(1),3);

//    qDebug() << "Список экадашей";
//    for (QList<TTitha>::const_iterator i = ekadashi.begin(); i < ekadashi.end(); ++i)
//    {
//        qDebug() << (*i).asEkadashStr();
//    }

    // геогр. широта и долгота значения по умолчанию для Москвы на лето 2014 года
//    double latitude = 56.323388;
//    double longitude = -38.134657;

//    // время восхода Солнца
//    QTime sunRise;
//    sunRise = TComputings::sunTimeRise(longitude,latitude,3);
//    qDebug() << "sunRise" << sunRise;

//    // время захода Солнца
//    QTime sunSet;
//    sunSet = TComputings::sunTimeSet(longitude,latitude,3);
//    qDebug() << "sunSet" << sunSet;

//    // высшая точка Солнца (зенит)
//    QTime sunTransit;
//    bool aboveHorizont;
//    sunTransit = TComputings::sunTimeTransit(longitude,latitude,aboveHorizont,3);
//    qDebug() << "sunTransit" << sunTransit << ((true == aboveHorizont) ? "above horizont" : "under horizont");

//    // время восхода Луны
//    QTime moonRise (TComputings::moonTimeRise(longitude,latitude,3));
//    qDebug() << "moonRise" << moonRise;

//    // время захода Луны
//    QTime moonSet (TComputings::moonTimeSet(longitude,latitude,3));
//    qDebug() << "moonSet" << moonSet;

//    // время высшей точки Луны (зенит)
//    QTime moonTransit (TComputings::moonTimeTransit(longitude,latitude,aboveHorizont,3));
//    qDebug() << "moonTransit" << moonTransit << ((true == aboveHorizont) ? "above horizont" : "under horizont");

//    // горизонтальные координаты Солнца сейчас
//    QPair <double,double> horizontalCoords (TComputings::sunHorizontalCoords(longitude,latitude));
//    qDebug() << "horizontalCoords" << horizontalCoords;

//    // утренние гражданские сумерки сегодня
//    QPair<QTime,QTime> morningCivilTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,250,3));
//    qDebug() << "morningСivilTwilight" << morningCivilTwilight.first << "-" << morningCivilTwilight.second;

//    // утренние навигационные сумерки
//    QPair<QTime,QTime> morningNavigationTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,250,3,static_cast<double>(TComputings::navigation)));
//    qDebug() << "morningNavigationTwilight" << morningNavigationTwilight.first << "-" << morningNavigationTwilight.second;

//    // утренние астрономические сумерки
//    QPair<QTime,QTime> morningAstronomicalTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,250,3,static_cast<double>(TComputings::astronomical)));
//    qDebug() << "morningAstronomicalTwilight" << morningAstronomicalTwilight.first << "-" << morningAstronomicalTwilight.second;

//    // утренние сумерки (сандхья), как 1/10 часть от половины суток
//    QPair<QTime,QTime> morningSandhya(TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,3));
//    qDebug() << "morningSandhya" << morningSandhya.first << "-" << morningSandhya.second;

//    // утренние сумерки (сандхья), как 1/10 часть от светового дня
//    QPair<QTime,QTime> morningSandhya2(TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,3));
//    qDebug() << "morningSandhya2" << morningSandhya2.first << "-" << morningSandhya2.second;

//    // вечерние гражданские сумерки сегодня
//    QPair<QTime,QTime> eveningCivilTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,250,3));
//    qDebug() << "eveningCivilTwilight" << eveningCivilTwilight.first << "-" << eveningCivilTwilight.second;

//    // вечерние навигационные сумерки
//    QPair<QTime,QTime> eveningNavigationTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,250,3,static_cast<double>(TComputings::navigation)));
//    qDebug() << "eveningNavigationTwilight" << eveningNavigationTwilight.first << "-" << eveningNavigationTwilight.second;

//    // вечерние астрономические сумерки
//    QPair<QTime,QTime> eveningAstronomicalTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,250,3,static_cast<double>(TComputings::astronomical)));
//    qDebug() << "eveningAstronomicalTwilight" << eveningAstronomicalTwilight.first << "-" << eveningAstronomicalTwilight.second;

//    // спец. сумерки
//    QPair<QTime,QTime> specTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,250,3,-55.D));
//    qDebug() << "specTwilight" << specTwilight.first << "-" << specTwilight.second;

//    // вывести вычисленную информацию по Солнцу
//    ui->textEditSunTime->append(TComputings::toStringSunTimeInfo(sunRise,sunSet,sunTransit));
//    ui->textEditSunTime->append(TComputings::toStringSunTimeInfo2(morningCivilTwilight,morningNavigationTwilight,morningAstronomicalTwilight));
//    ui->textEditSunTime->append(TComputings::toStringSunTimeInfo2(eveningCivilTwilight,eveningNavigationTwilight,eveningAstronomicalTwilight,false));

//    QTextCursor textCursorToBegin (ui->textEditSunTime->textCursor());
//    textCursorToBegin.movePosition(QTextCursor::Start);
//    ui->textEditSunTime->setTextCursor(textCursorToBegin);

}
//---------------------------

MainWindow::~MainWindow()
{
    // остановить таймер обновления виджетов отображающих время
    m_Timer.stop();

    delete ui;
}
//---------------------------
// КОНЕЦ: MainWindow - public
//---------------------------------------------------------------------------------

// НАЧАЛО: MainWindow - private slots
void MainWindow::updateTime()
{
    // счётчик срабатывания таймера
    ++m_cTimer;

    // обновить текущее гражданское время
    ui->timeEditCivil->setTime(QTime::currentTime());

    if (0 == (calendarUpdateInterval - timerInterval*m_cTimer))
    {
        // обновить грегорианский календарь
        ui->calendarWidgetGregorian->showToday();
        ui->calendarWidgetGregorian->setSelectedDate(QDate::currentDate());
        m_cTimer = 0;
    }

    if (QDate::currentDate() != m_Date)
    {
        // обновить информацию о Солнце и Луне
        showSunTime();
        showMoonTime();
        showTithi();
    }

    // текущая дата
    m_Date = QDate::currentDate();

    // обновить текущее солнечное время
    const double lng = 38.155600; // геогр. долгота моего дома
    ui->timeEditSunAverage->setTime(TComputings::sunTimeAverage(lng));
    ui->timeEditSunTrue->setTime(TComputings::sunTimeTrue(lng));
}
//---------------------------

void MainWindow::showSettingsDialog()
{
    // показать диалог настроек программы
    DialogSettings settingsDialog(this);

    int result (settingsDialog.exec());

    if (QDialog::Accepted == result)
    {
        // пересчитать все данные согласно новым настройкам
        showSunTime();
        showMoonTime();
        showTithi();

        // открыть вкладку с информацией о Солнце
        ui->tabWidget->setCurrentWidget(ui->tabSun);
    }
}
//---------------------------

void MainWindow::on_radioButtonKaliYuga_clicked()
{
    showVedicDate();
}
//---------------------------

void MainWindow::on_radioButtonMahaYuga_clicked()
{
    showVedicDate();
}
//---------------------------

void MainWindow::on_radioButtonManvantara_clicked()
{
    showVedicDate();
}
//---------------------------

void MainWindow::on_radioButtonKalpa_clicked()
{
    showVedicDate();
}
//---------------------------

void MainWindow::on_radioButtonMahaKalpa_clicked()
{
    showVedicDate();
}
//---------------------------

void MainWindow::on_pushButtonTithiPeriod_clicked()
{
    // показать диалог расчёта и вывода титх
    DialogTithi tithiDialog(false, this);

    int result (tithiDialog.exec());
}
//---------------------------

void MainWindow::on_pushButtonEkadashi_clicked()
{
    // показать диалог рассчёта и вывода экадашей
    DialogTithi ekadashiDialog(true, this);

    int result (ekadashiDialog.exec());
}
//---------------------------
// КОНЕЦ: MainWindow - private slots
//---------------------------------------------------------------------------------

// НАЧАЛО: MainWindow - private
void MainWindow::showVedicDate()
{
    // очистка
    ui->textEditVedicDate->clear();

    // вывести год по ведическому летоисчислению
    if (true == ui->radioButtonKaliYuga->isChecked())
    {
        ui->textEditVedicDate->append(TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicKaliYuga(QDate::currentDate().year())));
        ui->textEditVedicDate->append("год от начала Кали-юги");
    }
    else if (true == ui->radioButtonMahaYuga->isChecked())
    {
        ui->textEditVedicDate->append(TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicMahaYuga(QDate::currentDate().year())));
        ui->textEditVedicDate->append("год от начала Маха-юги");
    }
    else if (true == ui->radioButtonManvantara->isChecked())
    {
        ui->textEditVedicDate->append(TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicManvantara(QDate::currentDate().year())));
        ui->textEditVedicDate->append("год от начала Манвантары");
    }
    else if (true == ui->radioButtonKalpa->isChecked())
    {
        ui->textEditVedicDate->append(TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicKalpa(QDate::currentDate().year())));
        ui->textEditVedicDate->append("год от начала Кальпы");
    }
    else if (true == ui->radioButtonMahaKalpa->isChecked())
    {
        ui->textEditVedicDate->append(TVedicCalendar::formatLongYearWithDots(TVedicCalendar::gregorianToVedicMahaKalpa(QDate::currentDate().year())));
        ui->textEditVedicDate->append("год от начала Махакальпы");
    }

    ui->textEditVedicDate->append("Кали-юга в Маха-юге");
    ui->textEditVedicDate->append("28-ая Маха-юга в Манвантаре");
    ui->textEditVedicDate->append("7-ая Манвантара в Кальпе, Вайвасвата Ману");
    ui->textEditVedicDate->append("1-ая Кальпа в 51-ом году жизни Брахмы, Вараха");

    // позицию текстового курсора в начало
    QTextCursor textCursorToBegin (ui->textEditVedicDate->textCursor());
    textCursorToBegin.movePosition(QTextCursor::Start);
    ui->textEditVedicDate->setTextCursor(textCursorToBegin);
}
//---------------------------

void MainWindow::showSunTime()
{
    // восстановить данные для вычисления из настроек программы
    QSettings settings;
    bool ok;
    double latitude (settings.value("latitude").toDouble(&ok));
    double longitude (-1*settings.value("longitude").toDouble(&ok));
    double height (settings.value("height").toDouble(&ok));
    double timeZoneOffset (settings.value("timeZoneOffset").toDouble(&ok));

    // рассчёты и вывод информации
    if (true == ok)
    {
        // время восхода Солнца
        QTime sunRise (TComputings::sunTimeRise(longitude,latitude,timeZoneOffset));

        // время захода Солнца
        QTime sunSet (TComputings::sunTimeSet(longitude,latitude,timeZoneOffset));

        // высшая точка Солнца (зенит)
        bool aboveHorizont;
        QTime sunTransit (TComputings::sunTimeTransit(longitude,latitude,aboveHorizont,timeZoneOffset));

        // утренние гражданские сумерки сегодня
        QPair<QTime,QTime> morningCivilTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,height,timeZoneOffset));

        // утренние навигационные сумерки (до восхода)
        QPair<QTime,QTime> morningNavigationTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,height,timeZoneOffset,static_cast<double>(TComputings::navigationTwilight)));

        // утренние астрономические сумерки (до восхода)
        QPair<QTime,QTime> morningAstronomicalTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,height,timeZoneOffset,static_cast<double>(TComputings::astronomicalTwilight)));

        // утренние сумерки (сандхья), как 1/10 часть от половины суток
        QPair<QTime,QTime> morningSandhya(TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,sunRise,timeZoneOffset));

        // утренние сумерки (сандхья), как 1/10 часть от светового дня
        QPair<QTime,QTime> morningSandhya2(TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,sunRise,sunSet,timeZoneOffset));

        // вечерние гражданские сумерки сегодня
        QPair<QTime,QTime> eveningCivilTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,height,timeZoneOffset));

        // вечерние навигационные сумерки (от захода)
        QPair<QTime,QTime> eveningNavigationTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,height,timeZoneOffset,static_cast<double>(TComputings::navigationTwilight)));

        // вечерние астрономические сумерки (от захода)
        QPair<QTime,QTime> eveningAstronomicalTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,height,timeZoneOffset,static_cast<double>(TComputings::astronomicalTwilight)));

        // вечерние сумерки (сандхья), как 1/10 часть от половины суток
        QPair<QTime,QTime> eveningSandhya (TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,sunSet,timeZoneOffset,false));

        // вечерние сумерки (сандхья), как 1/10 часть от светового дня
        QPair<QTime,QTime> eveningSandhya2 (TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,sunRise,sunSet,timeZoneOffset,false));

        // вывести вычисленную информацию по Солнцу (восход, зенит, заход)
        ui->textEditSunTime->clear();
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo(sunRise,sunSet,sunTransit));

        // вывести информацию по Солнцу 2 (утренние сумерки), во избежание расхождения в несколько секунд время восхода/захода берётся ранее вычисленное
        morningCivilTwilight.second = sunRise;
        morningNavigationTwilight.second = sunRise;
        morningAstronomicalTwilight.second = sunRise;
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo2(morningCivilTwilight,morningNavigationTwilight,morningAstronomicalTwilight));

        // вывести информацию по Солнцу 2 (вечерние сумерки), во избежание расхождения в несколько секунд время восхода/захода берётся ранее вычисленное
        eveningCivilTwilight.first = sunSet;
        eveningNavigationTwilight.first = sunSet;
        eveningAstronomicalTwilight.first = sunSet;
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo2(eveningCivilTwilight,eveningNavigationTwilight,eveningAstronomicalTwilight,false));

        // вывести информацию по Солнцу 3 (Сандхьи)
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo3(morningSandhya,morningSandhya2));
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo3(eveningSandhya,eveningSandhya2,false));

        // позицию текстового курсора в начало
        QTextCursor textCursorToBegin (ui->textEditSunTime->textCursor());
        textCursorToBegin.movePosition(QTextCursor::Start);
        ui->textEditSunTime->setTextCursor(textCursorToBegin);
    }
    else
    {
        // не получилось загрузить данные из настроек программы
        qWarning() << "MainWindow::showSunTime" << "settings empty";
        ui->textEditSunTime->clear();
        ui->textEditSunTime->append("Не заданы необходимые настройки для рассчётов.");
        ui->textEditSunTime->append("Опции -> Настройки (Ctrl+o)");
    }
}
//---------------------------

void MainWindow::showMoonTime()
{
    // восстановить данные для вычисления из настроек программы
    QSettings settings;
    bool ok;
    double latitude (settings.value("latitude").toDouble(&ok));
    double longitude (-1*settings.value("longitude").toDouble(&ok));
    double timeZoneOffset (settings.value("timeZoneOffset").toDouble(&ok));

    // рассчёты и вывод информации
    if (true == ok)
    {
        // время захода Луны
        QTime moonSet (TComputings::moonTimeSet(longitude,latitude,timeZoneOffset));

        // время восхода Луны
        QTime moonRise (TComputings::moonTimeRise(longitude,latitude,timeZoneOffset));

        // время высшей точки Луны (зенит)
        bool aboveHorizont;
        QTime moonTransit (TComputings::moonTimeTransit(longitude,latitude,aboveHorizont,timeZoneOffset));

        // предыдущее новолуние
        QDateTime previousNewMoon (TComputings::moonTimeFindPreviousNewMoon(timeZoneOffset));
        if (false == previousNewMoon.isValid())
            qWarning() << "MainWindow::showMoonTime" << "invalid previousNewMoon";

        // вывести информацию о Луне
        ui->textEditMoonDate->clear();
        ui->textEditMoonDate->append(TComputings::toStringMoonTimeInfo(moonSet,moonRise,moonTransit));

        ui->textEditMoonDate->append("Предыдущее новолуние: " + previousNewMoon.toString("dd.MM.yyyy hh:mm"));

        // позицию текстового курсора в начало
        QTextCursor textCursorToBegin (ui->textEditMoonDate->textCursor());
        textCursorToBegin.movePosition(QTextCursor::Start);
        ui->textEditMoonDate->setTextCursor(textCursorToBegin);
    }
    else
    {
        // не получилось загрузить данные из настроек программы
        qWarning() << "MainWindow::showMoonTime" << "settings empty";
        ui->textEditMoonDate->clear();
        ui->textEditMoonDate->append("Не заданы необходимые настройки для рассчётов.");
        ui->textEditMoonDate->append("Опции -> Настройки (Ctrl+o)");
    }
}
//---------------------------

void MainWindow::showTithi()
{
    // восстановить данные для вычисления из настроек программы
    QSettings settings;
    bool ok;
    double timeZoneOffset (settings.value("timeZoneOffset").toDouble(&ok));

    if (true == ok)
    {
        // рассчитать и вывести информацию о титхах
        TTitha curTitha (TTitha::findCurrentTitha(timeZoneOffset));
        TTitha nearestEkadash (TTitha::findNearestEkadash(timeZoneOffset));

        ui->textEditTithi->clear();
        ui->textEditTithi->append(curTitha.asCurTithiStr());
        ui->textEditTithi->append("");
        ui->textEditTithi->append("Следующий экадаш");
        ui->textEditTithi->append(nearestEkadash.asEkadashStr());

        // позицию текстового курсора в начало
        QTextCursor textCursorToBegin (ui->textEditTithi->textCursor());
        textCursorToBegin.movePosition(QTextCursor::Start);
        ui->textEditTithi->setTextCursor(textCursorToBegin);
    }
    else
    {
        // не получилось загрузить данные из настроек программы
        qWarning() << "MainWindow::showSunTime" << "settings empty";
        ui->textEditTithi->clear();
        ui->textEditTithi->append("Не заданы необходимые настройки для рассчётов.");
        ui->textEditTithi->append("Опции -> Настройки (Ctrl+o)");
    }
}
//---------------------------
// КОНЕЦ: MainWindow - private
//---------------------------------------------------------------------------------




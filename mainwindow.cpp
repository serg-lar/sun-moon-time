// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogsettings.h"
#include "dialogtithi.h"
#include "dialogtableview.h"
#include "tithi.h"
#include "calendar.h"
#include "computings.h"
#include <QtMath>
#include <QtDebug>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSound>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------

// НАЧАЛО: MainWindow - private slots
void MainWindow::updateTime()
{
    bool needUpdateSvaras (false);  // нужно обновить информацию о сварах
    bool needCheckWarns (false);    // нужно проверить наличие событий
    if (0 == m_cTimer)
    {
        needUpdateSvaras = true;
        needCheckWarns = true;
    }

    // счётчик срабатывания таймера
    ++m_cTimer;

    // обновить текущее гражданское время
    ui->timeEditCivil->setTime(QTime::currentTime());

    if (0 == (timerInterval*m_cTimer % calendarUpdateInterval))
    {
        // обновить грегорианский календарь
        ui->calendarWidgetGregorian->showToday();
        ui->calendarWidgetGregorian->setSelectedDate(QDate::currentDate());

        // проверить более длительный период времени
        if (0 == (timerInterval*m_cTimer % sunMoonInfoUpdateInterval))
        {
            // нужно обновить информацию по сварам
            needUpdateSvaras = true;

            // проверить ещё более длительный период времени
            if (0 == (timerInterval*m_cTimer % warnInfoInterval))
            {
                // нужно проверить, не пора ли оповестить о событии
                needCheckWarns = true;

                // обнулить счётчик срабатываний
                m_cTimer = 0;
            }
        }
    }

    if (QDate::currentDate() != m_Date)
    {
        // обновить информацию о Солнце и Луне привязанную к дате
        showSunTime();
        showMoonTime();
        showSvara();
        needUpdateSvaras = false;
    }
    if (QDateTime::currentDateTime() >= m_currentTitha.endDateTime())
    {
        // обновить информацию по титхам
        showTithi();
    }

    // текущая дата
    m_Date = QDate::currentDate();

    // загрузить настройки
    QSettings settings;
    bool ok;
    double longitude (settings.value(DialogSettings::longitudeSettingName()).toDouble(&ok));
//    double latitude (settings.value("latitude").toDouble(&ok));
//    double timeZoneOffset (settings.value("timeZoneOffset").toDouble(&ok));
    if (true == ok)
    {
        // обновить текущее солнечное время
        ui->timeEditSunAverage->setTime(TComputings::sunTimeAverage(longitude));
        ui->timeEditSunTrue->setTime(TComputings::sunTimeTrue(longitude));

        if (true == needUpdateSvaras)
            showSvara();                // обновить инфрмацию о сварах
        if (true == needCheckWarns)
        {
            // проверить необходимость оповещения о экадаше

            // загрузить ещё необходимые настройки
            bool ekadashWarn (settings.value(DialogSettings::ekadashWarnSettingName()).toBool());
            bool ekadashWarnAfter (settings.value(DialogSettings::ekadashWarnAfterSettingName()).toBool());
            quint32 ekadashWarnTimeBefore (settings.value(DialogSettings::ekadashWarnTimeBeforeSettingName()).toUInt(&ok));
            bool ekadashWarnRequireConfirmation (settings.value(DialogSettings::ekadashWarnRequireConfirmationSettingName()).toBool());
            if (true == ok)
            {
                if ((true == ekadashWarn) && (false == mf_ekadashWarned))
                {
                    if ((10 == m_currentTitha.num()) &&
                            (QDateTime::currentDateTime().secsTo(m_currentTitha.endDateTime()) <= static_cast<qint64>(ekadashWarnTimeBefore*60*60)))
                    {
                        // необходимо оповестить о начале экадаша
                        m_TrayIcon.showMessage("Экадаш","Начало: "+m_currentTitha.endDateTime().toString("dd MMMM yyyy hh:mm"));
                        QApplication::alert(this);
                        QSound::play(":/sounds/OM_NAMO_NARAYANA.wav");
                        if (true == ekadashWarnRequireConfirmation)
                        {
                            QMessageBox msgBox;
                            msgBox.setText("Экадаш начинается");
                            msgBox.exec();
                        }

                        mf_ekadashWarned = true;
                    }
                    else if ((true == ekadashWarnAfter) && (12 == m_currentTitha.num()) &&
                             (m_currentTitha.beginDateTime().secsTo(QDateTime::currentDateTime()) >= static_cast<qint64>(ekadashWarnTimeBefore*60*60)))
                    {
                        // необходимо оповестить о завершении экадаша
                        m_TrayIcon.showMessage("Экадаш","Конец: "+m_currentTitha.beginDateTime().toString("dd MMMM yyyy hh:mm"));
                        QApplication::alert(this);
                        QSound::play(":/sounds/OM_NAMO_NARAYANA.wav");
                        if (true == ekadashWarnRequireConfirmation)
                        {
                            QMessageBox msgBox;
                            msgBox.setText("Экадаш завершается");
                            msgBox.exec();
                        }

                        mf_ekadashWarned = true;
                    }
                }
            }
            else
                qWarning() << "MainWindow::updateTime" << "ekadashWarnTimeBefore setting load error";
        }
    }
    else
        qWarning() << "MainWindow::updateTime" << "longitude not set";
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
        showSvara();

        // открыть вкладку с информацией о Солнце
        ui->tabWidget->setCurrentWidget(ui->tabSun);
    }    
}
//---------------------------

void MainWindow::realClose()
{
    // закрыть окно по-настоящему насовсем
    mf_realClose = true;
    close();
}
//---------------------------

void MainWindow::computeAndShowAll()
{
    // пересчитать все данные и отобразить в соответствующих вкладках

    // блокировка работы пользователя с интерфейсом
    QCursor currentCursor (cursor());
    setDisabled(true);
    setCursor(QCursor(Qt::WaitCursor));

    // пересчёт и отображение
    showSunTime();
    showMoonTime();
    showTithi();
    showSvara();

    // разблокировка работы пользователя с интерфейсом
    setEnabled(true);
    setCursor(currentCursor);    
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

void MainWindow::on_pushButtonSunTimePeriod_clicked()
{
    // показать диалог рассчёта информации по Солнцу за период времени
    DialogTableView sunTimePeriodDialog(DialogTableView::sunInfo,this);

    int result (sunTimePeriodDialog.exec());
}
//---------------------------

void MainWindow::on_pushButtonMoonDatePeriod_clicked()
{
    // показать диалог рассчёта информации по Луне за период времени
    DialogTableView moontimePeriodDialog(DialogTableView::moonInfo,this);

    int result (moontimePeriodDialog.exec());
}
//---------------------------

void MainWindow::on_systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // "активирована" иконка в трее
    if (QSystemTrayIcon::DoubleClick == reason)
    {
        // отобразить окно программы
        showNormal();
        activateWindow();
    }
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
    ui->textEditVedicDate->append("1-ая Кальпа в 51-ом году жизни Брахмы, Швета-вараха");

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
    double latitude (settings.value(DialogSettings::latitudeSettingName()).toDouble(&ok));
    double longitude (-1*settings.value(DialogSettings::longitudeSettingName()).toDouble(&ok));
    double height (settings.value(DialogSettings::heightSettingName()).toDouble(&ok));
    double timeZoneOffset (settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok));

    // рассчёты и вывод информации
    if (true == ok)
    {
        // время восхода Солнца
        QTime sunRise (TComputings::sunTimeRise(longitude,latitude,timeZoneOffset));
        m_currentSunRise = TComputings::roundToMinTime(sunRise);

        // время захода Солнца
        QTime sunSet (TComputings::sunTimeSet(longitude,latitude,timeZoneOffset));
        m_currentSunSet = TComputings::roundToMinTime(sunSet);

        // высшая точка Солнца (зенит)
        bool aboveHorizont;
        QTime sunTransit (TComputings::sunTimeTransit(longitude,latitude,aboveHorizont,timeZoneOffset));

        // утренние гражданские сумерки сегодня
        QPair<QTime,QTime> morningCivilTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,sunRise,height,timeZoneOffset));

        // утренние навигационные сумерки (до восхода)
        QPair<QTime,QTime> morningNavigationTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,sunRise,height,timeZoneOffset,static_cast<double>(TComputings::navigationTwilight)));

        // утренние астрономические сумерки (до восхода)
        QPair<QTime,QTime> morningAstronomicalTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,sunRise,height,timeZoneOffset,static_cast<double>(TComputings::astronomicalTwilight)));

        // утренние сумерки (сандхья), как 1/10 часть от половины суток
        QPair<QTime,QTime> morningSandhya(TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,sunRise,timeZoneOffset));

        // утренние сумерки (сандхья), как 1/10 часть от светового дня
        QPair<QTime,QTime> morningSandhya2(TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,sunRise,sunSet,timeZoneOffset));

        // вечерние гражданские сумерки сегодня
        QPair<QTime,QTime> eveningCivilTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,sunSet,height,timeZoneOffset));

        // вечерние навигационные сумерки (от захода)
        QPair<QTime,QTime> eveningNavigationTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,sunSet,height,timeZoneOffset,static_cast<double>(TComputings::navigationTwilight)));

        // вечерние астрономические сумерки (от захода)
        QPair<QTime,QTime> eveningAstronomicalTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,sunSet,height,timeZoneOffset,static_cast<double>(TComputings::astronomicalTwilight)));

        // вечерние сумерки (сандхья), как 1/10 часть от половины суток
        QPair<QTime,QTime> eveningSandhya (TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,sunSet,timeZoneOffset,false));

        // вечерние сумерки (сандхья), как 1/10 часть от светового дня
        QPair<QTime,QTime> eveningSandhya2 (TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,sunRise,sunSet,timeZoneOffset,false));

        // вывести вычисленную информацию по Солнцу (восход, зенит, заход)
        ui->textEditSunTime->clear();
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo(sunRise,sunSet,sunTransit));

        // вывести информацию по Солнцу 2 (утренние сумерки), во избежание расхождения в несколько секунд время восхода/захода берётся ранее вычисленное
        ui->textEditSunTime->append(TComputings::toStringSunTimeInfo2(morningCivilTwilight,morningNavigationTwilight,morningAstronomicalTwilight));

        // вывести информацию по Солнцу 2 (вечерние сумерки), во избежание расхождения в несколько секунд время восхода/захода берётся ранее вычисленное
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
    double latitude (settings.value(DialogSettings::latitudeSettingName()).toDouble(&ok));
    double longitude (-1*settings.value(DialogSettings::longitudeSettingName()).toDouble(&ok));
    double timeZoneOffset (settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok));

    // рассчёты и вывод информации
    if (true == ok)
    {
        // простые лунные дни вокруг текущей даты
        QList<TComputings::TMoonDay> currentMoonDays (TComputings::moonTimeRiseTransitSet(longitude,latitude,timeZoneOffset));

        // время захода Луны
//        QTime moonSet (TComputings::moonTimeSet(longitude,latitude,timeZoneOffset));

        // время восхода Луны
//        QTime moonRise (TComputings::moonTimeRise(longitude,latitude,timeZoneOffset));

        // время высшей точки Луны (зенит)
//        bool aboveHorizont;
//        QTime moonTransit (TComputings::moonTimeTransit(longitude,latitude,aboveHorizont,timeZoneOffset));

        // предыдущее новолуние
//        QDateTime previousNewMoon (TComputings::moonTimeFindPreviousNewMoon(timeZoneOffset));
//        if (false == previousNewMoon.isValid())
//            qWarning() << "MainWindow::showMoonTime" << "invalid previousNewMoon";

//        // следующее новолуние
//        QDateTime nextNewMoon (TComputings::moonTimeFindNextNewMoon(timeZoneOffset));
//        if (false == nextNewMoon.isValid())
//            qWarning() << "MainWindow::showMoonTime" << "invalid nextNewMoon";

        // лунные дни
//        QList<QPair<QDateTime,QDateTime> > moonDays(TComputings::moonTimeMoonDays(longitude,latitude,timeZoneOffset, TComputings::moonTimeFindPreviousNewMoon(),
//                                                                                  QDateTime::currentDateTimeUtc().addDays(1)));

        // ближайший лунный день
//        QPair<quint8, QPair<QDateTime,QDateTime> > nearestMoonDay (TComputings::moonTimeNearestMoonDay(longitude,latitude,timeZoneOffset));
//        if (false == nearestMoonDay.second.second.isValid())
//            qWarning() << "MainWindow::showMoonTime" << "invalid nearestMoonDay";

        // новолуния на год вперёд
//        QList<QDateTime> newMoonForYear (TComputings::moonTimeFindNewMoonForYear(timeZoneOffset));

        // простые лунные дни
//        QList<TComputings::TMoonDay> moonDaysSimple (TComputings::moonTimeMoonDaysFast(longitude,latitude,timeZoneOffset));

        // вывести информацию о Луне
        ui->textEditMoonDate->clear();
//        ui->textEditMoonDate->append(QDate::currentDate().toString("dd.MM.yyyy"));
//        ui->textEditMoonDate->append("");
//        ui->textEditMoonDate->append(TComputings::toStringMoonTimeInfo(moonSet,moonRise,moonTransit));

//        ui->textEditMoonDate->append("Предыдущее новолуние: " + previousNewMoon.toString("dd.MM.yyyy hh:mm"));
//        ui->textEditMoonDate->append("Следующее новолуние: " + nextNewMoon.toString("dd.MM.yyyy hh:mm"));
//        ui->textEditMoonDate->append("Ближайший лунный день: "+QString::number(nearestMoonDay.first));
//        ui->textEditMoonDate->append("Начало: "+nearestMoonDay.second.first.toString("dd.MM.yyyy hh:mm"));
//        ui->textEditMoonDate->append("Конец: "+nearestMoonDay.second.second.toString("dd.MM.yyyy hh:mm"));
//        ui->textEditMoonDate->append("");

//        ui->textEditMoonDate->append("Лунные дни:");
//        for (qint32 i = 0; i < moonDays.size(); ++i)
//            ui->textEditMoonDate->append(moonDays.at(i).first.toString("dd.MM.yyyy hh:mm")+" - "+moonDays.at(i).second.toString("dd.MM.yyyy hh:mm"));
//        ui->textEditMoonDate->append("Новолуния на год");
//        ui->textEditMoonDate->append("");
//        foreach (const QDateTime& newMoon, newMoonForYear)
//            ui->textEditMoonDate->append(newMoon.toString("dd.MM.yyyy hh:mm"));
//        ui->textEditMoonDate->append("Лунные дни (простые)");
//        ui->textEditMoonDate->append("");
//        foreach (const TComputings::TMoonDay& moonDay, moonDaysSimple)
//        {
//            ui->textEditMoonDate->append(moonDay.date.toString("dd.MM.yyyy"));
//            ui->textEditMoonDate->append("Восход: "+moonDay.rise.toString("hh:mm"));
//            ui->textEditMoonDate->append("Заход: "+moonDay.set.toString("hh:mm"));
//            ui->textEditMoonDate->append("Зенит: "+moonDay.transit.toString("hh:mm"));
//            if (false == moonDay.transitAboveHorizont)
//                ui->textEditMoonDate->append("Зенит под горизонтом");
//            ui->textEditMoonDate->append("");
//        }
        ui->textEditMoonDate->append(currentMoonDays.first().date.toString("dd.MM.yyyy")+","+currentMoonDays.at(1).date.toString("dd.MM.yyyy")+\
                                     ",\n"+currentMoonDays.last().date.toString("dd.MM.yyyy")+"*");
        ui->textEditMoonDate->append("");
        ui->textEditMoonDate->append("Восход: "+currentMoonDays.first().rise.toString("hh:mm"));
        ui->textEditMoonDate->append("Заход: "+currentMoonDays.first().set.toString("hh:mm"));
        ui->textEditMoonDate->append("Зенит: "+currentMoonDays.first().transit.toString("hh:mm"));
        if (false == currentMoonDays.first().transitAboveHorizont)
            ui->textEditMoonDate->append("зенит под горизонтом");
        ui->textEditMoonDate->append("");
        ui->textEditMoonDate->append("Восход: "+currentMoonDays.at(1).rise.toString("hh:mm"));
        ui->textEditMoonDate->append("Заход: "+currentMoonDays.at(1).set.toString("hh:mm"));
        ui->textEditMoonDate->append("Зенит: "+currentMoonDays.at(1).transit.toString("hh:mm"));
        if (false == currentMoonDays.at(1).transitAboveHorizont)
            ui->textEditMoonDate->append("зенит под горизонтом");
        // номер лунного дня
        TComputings::moonTimeMoonDayNum(longitude,latitude,timeZoneOffset,QDate::currentDate().addDays(-1));
        quint32 moonDayNum (TComputings::moonTimeMoonDayNum(longitude,latitude,timeZoneOffset));
        QString moonDayNumStr (QString::number(moonDayNum));
        if (0 == moonDayNum)
            moonDayNumStr = "-";
        else if ((2 == moonDayNum) && (TComputings::prevMoonDayNum() > 1))
            moonDayNumStr = "1-2";
        else if ((1 == moonDayNum) && (29 == TComputings::prevMoonDayNum()))
            moonDayNumStr = "30-1";
        else if ((1 == moonDayNum) && (30 == TComputings::prevMoonDayNum()) && (currentMoonDays.at(1).rise < TComputings::nextNewMoon().time()))
            moonDayNumStr = "31-1";
        ui->textEditMoonDate->append("Лунный день номер: "+moonDayNumStr);
        ui->textEditMoonDate->append("Фаза: "+QString::number(TComputings::moonTimePhase())+"%");
        ui->textEditMoonDate->append("");
        ui->textEditMoonDate->append("Восход: "+currentMoonDays.last().rise.toString("hh:mm"));
        ui->textEditMoonDate->append("Заход: "+currentMoonDays.last().set.toString("hh:mm"));
        ui->textEditMoonDate->append("Зенит: "+currentMoonDays.last().transit.toString("hh:mm"));
        if (false == currentMoonDays.at(1).transitAboveHorizont)
            ui->textEditMoonDate->append("зенит под горизонтом");
        ui->textEditMoonDate->append("");

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
    double timeZoneOffset (settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok));

    if (true == ok)
    {
        // рассчитать и вывести информацию о титхах
        TTitha curTitha (TTitha::findCurrentTitha(timeZoneOffset));
        TTitha nearestEkadash (TTitha::findNearestEkadash(timeZoneOffset));

        // сохранить данные текущей титхи
        m_currentTitha = curTitha;

        // сбросить флаг предупрежедния о экадаше
        mf_ekadashWarned = false;

        ui->textEditTithi->clear();
        if (true == ui->checkBoxTithiPrintUtc->isChecked())
            ui->textEditTithi->append(curTitha.asCurTithiStr(timeZoneOffset));
        else
            ui->textEditTithi->append(curTitha.asCurTithiStr());
        ui->textEditTithi->append("");
        ui->textEditTithi->append("Следующий экадаш");
        if (true == ui->checkBoxTithiPrintUtc->isChecked())
            ui->textEditTithi->append(nearestEkadash.asEkadashStr(timeZoneOffset));
        else
            ui->textEditTithi->append(nearestEkadash.asEkadashStr());

        // позицию текстового курсора в начало
        QTextCursor textCursorToBegin (ui->textEditTithi->textCursor());
        textCursorToBegin.movePosition(QTextCursor::Start);
        ui->textEditTithi->setTextCursor(textCursorToBegin);
    }
    else
    {
        // не получилось загрузить данные из настроек программы
        qWarning() << "MainWindow::showTithi" << "settings empty";
        ui->textEditTithi->clear();
        ui->textEditTithi->append("Не заданы необходимые настройки для рассчётов.");
        ui->textEditTithi->append("Опции -> Настройки (Ctrl+o)");
    }
}
//---------------------------

void MainWindow::showSvara()
{
    // восстановить данные для вычисления из настроек программы
    QSettings settings;
    bool ok;
    double latitude (settings.value(DialogSettings::latitudeSettingName()).toDouble(&ok));
    double longitude (-1*settings.value(DialogSettings::longitudeSettingName()).toDouble(&ok));
    double timeZoneOffset (settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok));

    // рассчёты и вывод информации
    if (true == ok)
    {
        // вычисление свар
        QList<TComputings::TSvara> svaras;
        if ((true == m_currentSunRise.isValid()) && (true == m_currentSunSet.isValid()))
            svaras = TComputings::sunMoonTimeSvaraList(m_currentSunRise,m_currentSunSet);
        else
            svaras = TComputings::sunMoonTimeSvaraList(longitude,latitude,timeZoneOffset);

        // вывод в таблицу
        ui->tableWidgetSvaras->setColumnCount(2);
        ui->tableWidgetSvaras->setRowCount(svaras.size());
        ui->tableWidgetSvaras->setHorizontalHeaderLabels(QString("Тип,Время").split(","));

        for (qint32 i = 0; i < svaras.size(); ++i)
        {
            // округление до минуты
            svaras[i].begin = TComputings::roundToMinTime(svaras.at(i).begin);
            svaras[i].end = TComputings::roundToMinTime(svaras.at(i).end);

            svaras.at(i).moonSvara ? ui->tableWidgetSvaras->setItem(svaras.at(i).num-1,0,new QTableWidgetItem("Лунная")) : ui->tableWidgetSvaras->setItem(svaras.at(i).num-1,0,new QTableWidgetItem("Солнечная"));
            ui->tableWidgetSvaras->setItem(svaras.at(i).num-1,1,new QTableWidgetItem(svaras.at(i).begin.toString("hh:mm")+" - "+svaras.at(i).end.toString("hh:mm")));
            QColor itemTextColor("purple");
            if (true == svaras.at(i).moonSvara)
                itemTextColor = QColor("navy");
            ui->tableWidgetSvaras->item(svaras.at(i).num-1,0)->setTextColor(itemTextColor);
            ui->tableWidgetSvaras->item(svaras.at(i).num-1,1)->setTextColor(itemTextColor);
            if ((QTime::currentTime() >= svaras.at(i).begin) && (QTime::currentTime() < svaras.at(i).end))
            {
                ui->tableWidgetSvaras->item(svaras.at(i).num-1,0)->setBackgroundColor("gray");
                ui->tableWidgetSvaras->item(svaras.at(i).num-1,1)->setBackgroundColor("gray");

                // вывод в заголовок вкладки
                if (true == svaras.at(i).moonSvara)
                {
                    ui->tabWidget->setTabText(2,"Лунная свара до "+svaras.at(i).end.toString("hh:mm"));
                    ui->tabWidget->tabBar()->setTabTextColor(2,"navy");
                }
                else
                {
                    ui->tabWidget->setTabText(2,"Солнечная свара до "+svaras.at(i).end.toString("hh:mm"));
                    ui->tabWidget->tabBar()->setTabTextColor(2,"purple");
                }
            }
        }

        // вся таблица только для чтения, выравнивание текста по центру
        for (qint32 i = 0; i < ui->tableWidgetSvaras->rowCount(); ++i)
            for (qint32 j = 0; j < ui->tableWidgetSvaras->columnCount(); ++j)
            {
                (ui->tableWidgetSvaras->item(i,j))->setFlags((ui->tableWidgetSvaras->item(i,j))->flags() & ~Qt::ItemIsEditable);
                ui->tableWidgetSvaras->item(i,j)->setTextAlignment(Qt::AlignCenter);
            }

        // расширение столбцов под содержимое
        ui->tableWidgetSvaras->resizeColumnsToContents();
    }
    else
    {
        // не получилось загрузить данные из настроек программы
        qWarning() << "MainWindow::showSvara" << "settings empty";

//        ui->textEditTithi->append("Не заданы необходимые настройки для рассчётов.");
//        ui->textEditTithi->append("Опции -> Настройки (Ctrl+o)");
    }
}
//---------------------------
// КОНЕЦ: MainWindow - private slots
//---------------------------------------------------------------------------------


// НАЧАЛО: MainWindow - public
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::MainWindow)
{
    // основной интерфейс
    ui->setupUi(this);

    // инициализация переменных
    m_cTimer = 0;
    m_Date = QDate::currentDate();
    mf_realClose = false;
    mf_ekadashWarned = false;

    // значок в трее и меню к нему
    m_TrayIcon.setIcon(QIcon(":/icons/sun_moon.ico"));
    m_TrayIcon.setToolTip("Солнечно-Лунное время");
    m_TrayIcon.setIcon(QIcon(":/icons/sun_moon.ico"));
    m_TrayIcon.setToolTip("Солнечно-Лунное время");

    mp_TrayIconMenuActionQuit = new QAction("Выход",&m_TrayIconMenu);
    connect(mp_TrayIconMenuActionQuit, SIGNAL(triggered()), this, SLOT(realClose()));
    m_TrayIconMenu.addAction(mp_TrayIconMenuActionQuit);
    m_TrayIcon.setContextMenu(&m_TrayIconMenu);

    // соединения
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    connect(ui->actionExitApp, SIGNAL(triggered()), this, SLOT(realClose()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(ui->actionReCompAll, SIGNAL(triggered()), this, SLOT(computeAndShowAll()));
    connect(&m_TrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
            SLOT(on_systemTrayIconActivated(QSystemTrayIcon::ActivationReason)));

    // объекты
    m_TrayIcon.show();

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

    // вывести информацию по сварам
    showSvara();

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

    // освобождение ресурсов
    if (mp_TrayIconMenuActionQuit)
        delete mp_TrayIconMenuActionQuit;

    delete ui;
}
//---------------------------
// КОНЕЦ: MainWindow - public
//---------------------------------------------------------------------------------


// НАЧАЛО: MainWindow - protected
void MainWindow::closeEvent(QCloseEvent* e)
{
    // при нажатии на крестик вместо закрытия прятать окно
    // закрывать на совсем только в случае нажатия "выход" в меню в трее    
    if (false == mf_realClose)
    {
        e->ignore();
        hide();
    }
    else
        e->accept();
}
//---------------------------
// КОНЕЦ: MainWindow - protected
//---------------------------------------------------------------------------------

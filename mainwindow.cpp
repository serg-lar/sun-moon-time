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
    // флаги частых вычислений
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

    QDateTime dt (QDateTime::currentDateTimeUtc());  // необходимая уловка, чтобы не происходило внутренних конвертаций времени
    dt.setDate(QDateTime::currentDateTime().date()); // в соответствии с установленным часовым поясом
    dt.setTime(QDateTime::currentDateTime().time());
    if (QDate::currentDate() != m_currentDate)
    {
        // обновить информацию о Солнце и Сварах привязанную к дате
        showSunTime();
        showSvara();
        needUpdateSvaras = false;

        // возможно необходимо попробовать обновить информацию о Лунном дне
        if (dt > m_currentMoonDays.last().rise)
            showMoonTime();
    }
    if (dt >= m_currentTitha.endDateTime())
    {
        // обновить информацию по титхам
        showTithi();
    }
    if (false == m_currentMoonDays.isEmpty())
    {
        // если текущая дата-время перевалило за рассвет последнего лунного дня, то пересчитать новые лунные дни
        // второе условие необходимо, чтобы избежать зависания программы в бесконечном перерасчёте одного и того же результата
        if ((dt > m_currentMoonDays.last().rise) && (m_currentMoonDays != m_PrevMoonDays))
            showMoonTime();
    }

    // текущая дата
    m_currentDate = QDate::currentDate();

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
                    QDateTime dt (QDateTime::currentDateTimeUtc()); // необходимая уловка, чтобы не происходило внутренних конвертаций времени в соответствии с установленным часовым поясом
                    dt.setDate(QDateTime::currentDateTime().date());
                    dt.setTime(QDateTime::currentDateTime().time());
                    if (((10 == m_currentTitha.num()) &&
                            (dt.secsTo(m_currentTitha.endDateTime()) <= static_cast<qint64>(ekadashWarnTimeBefore*60*60))) ||
                            (11 == m_currentTitha.num()))
                    {
                        // необходимо оповестить о начале экадаша
                        m_TrayIcon.showMessage("Экадаш","Начало: "+m_currentTitha.endDateTime().toString("dd MMMM yyyy hh:mm"));
                        QApplication::alert(this);
                        QSound::play(":/sounds/OM_NAMO_NARAYANA.wav");

                        if (true == ekadashWarnRequireConfirmation)
                        {
                            if (nullptr == mp_ekadashWarnMsgBox)
                            {
                                mp_ekadashWarnMsgBox = new QMessageBox;
                                mp_ekadashWarnMsgBox->setWindowTitle("Солнечно-Лунное время");
                            }
                            mp_ekadashWarnMsgBox->setText("Экадаш начинается");                            
                            mp_ekadashWarnMsgBox->show();
                        }

                        mf_ekadashWarned = true;
                    }
                    else if ((true == ekadashWarnAfter) && (12 == m_currentTitha.num()) &&
                             (m_currentTitha.beginDateTime().secsTo(dt) >= static_cast<qint64>(ekadashWarnTimeBefore*60*60)))
                    {
                        // необходимо оповестить о завершении экадаша
                        m_TrayIcon.showMessage("Экадаш","Конец: "+m_currentTitha.beginDateTime().toString("dd MMMM yyyy hh:mm"));
                        QApplication::alert(this);
                        QSound::play(":/sounds/OM_NAMO_NARAYANA.wav");

                        if (true == ekadashWarnRequireConfirmation)
                        {
                            if (nullptr == mp_ekadashWarnMsgBox)
                            {
                                mp_ekadashWarnMsgBox = new QMessageBox;
                                mp_ekadashWarnMsgBox->setWindowTitle("Солнечно-Лунное время");
                            }
                            mp_ekadashWarnMsgBox->setText("Экадаш завершился");
                            mp_ekadashWarnMsgBox->show();
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
        // заблокировать интерфейс
        setDisabled(true);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        repaint();

        // пересчитать все данные согласно новым настройкам
        showSunTime();
        showMoonTime();
        showTithi();
        showSvara();

        // разблокировать интерфейс
        setEnabled(true);
        QApplication::restoreOverrideCursor();

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
    setDisabled(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    repaint();

    // пересчёт и отображение
    showSunTime();
    showMoonTime();
    showTithi();
    showSvara();

    // разблокировка работы пользователя с интерфейсом
    setEnabled(true);
    QApplication::restoreOverrideCursor();
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
        if (true == sunRise.isValid())
            m_currentSunRise = TComputings::roundToMinTime(sunRise);

        // время захода Солнца
        QTime sunSet (TComputings::sunTimeSet(longitude,latitude,timeZoneOffset));
        if (true == sunSet.isValid())
            m_currentSunSet = TComputings::roundToMinTime(sunSet);

        if ((true == sunRise.isValid()) && (true == sunSet.isValid()) && (sunSet > sunRise))
        {
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
        }
        else
        {
            ui->textEditSunTime->clear();
            ui->textEditSunTime->append("Полярный(ая) день(ночь)");
        }


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
    double height (settings.value(DialogSettings::heightSettingName()).toDouble(&ok));

    // рассчёты и вывод информации
    if (true == ok)
    {
        /*

        // заблокировать интерфейс
        setDisabled(true);
        repaint();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        // лунные дни
        QList<TComputings::TMoonDay2> moonDays(TComputings::moonTimeMoonDays(longitude,latitude,timeZoneOffset,QDateTime::currentDateTimeUtc().addDays(-1),
                                                                             QDateTime::currentDateTimeUtc().addDays(2),height));
        QList<TComputings::TMoonDay2> moonDaysExt (TComputings::moonTimeMoonDaysExt(longitude,latitude,timeZoneOffset,QDateTime::currentDateTimeUtc().addDays(-1),
                                                                                    QDateTime::currentDateTimeUtc().addDays(2),height));
        // сохранение текущих лунных дней
        m_PrevMoonDays = m_currentMoonDays;
        m_currentMoonDays = moonDays;

        ui->textEditMoonDate->clear();
        for (qint32 i = 0; i < moonDays.size(); ++i)
        {
            // округление до минуты
            moonDays[i].rise.setTime(TComputings::roundToMinTime(moonDays.at(i).rise.time()));
            moonDays[i].set.setTime(TComputings::roundToMinTime(moonDays.at(i).set.time()));
            moonDays[i].transit.setTime(TComputings::roundToMinTime(moonDays.at(i).transit.time()));


            ui->textEditMoonDate->append("Номер: "+moonDays.at(i).num+"\nВосход: "+moonDays.at(i).rise.toString("dd.MM.yyyy  hh:mm")+\
                                         "\nЗаход:  "+moonDays.at(i).set.toString("dd.MM.yyyy  hh:mm")+"\nЗенит:  "+moonDays.at(i).transit.toString("dd.MM.yyyy  hh:mm"));
            ui->textEditMoonDate->append("Фаза: "+QString::number(TComputings::moonTimePhase(moonDays.at(i).rise.date()))+"%");
            if (true == moonDays.at(i).newMoon.isValid())
            {
                moonDays[i].newMoon.setTime(TComputings::roundToMinTime(moonDays.at(i).newMoon.time()));
                ui->textEditMoonDate->append("Новолуние: "+moonDays.at(i).newMoon.toString("dd.MM.yyyy  hh:mm"));
            }
            ui->textEditMoonDate->append("");            
        }
        if (0 == moonDays.size())
        {
            ui->textEditMoonDate->append("Отсутствует");
            ui->textEditMoonDate->append("");
        }


        ui->textEditMoonDate->append("Расширенные");
        for (qint32 i = 0; i < moonDaysExt.size(); ++i)
        {
            // округление до минуты
            moonDaysExt[i].rise.setTime(TComputings::roundToMinTime(moonDaysExt.at(i).rise.time()));
            moonDaysExt[i].set.setTime(TComputings::roundToMinTime(moonDaysExt.at(i).set.time()));
            moonDaysExt[i].transit.setTime(TComputings::roundToMinTime(moonDaysExt.at(i).transit.time()));

            ui->textEditMoonDate->append("Номер: "+moonDaysExt.at(i).num+"\nВосход: "+moonDaysExt.at(i).rise.toString("dd.MM.yyyy  hh:mm")+\
                                         "\nЗаход:  "+moonDaysExt.at(i).set.toString("dd.MM.yyyy  hh:mm")+"\nЗенит:  "+moonDaysExt.at(i).transit.toString("dd.MM.yyyy  hh:mm"));
            ui->textEditMoonDate->append("Фаза: "+QString::number(TComputings::moonTimePhase(moonDaysExt.at(i).rise.date()))+"%");
            if (true == moonDaysExt.at(i).newMoon.isValid())
            {
                moonDaysExt[i].newMoon.setTime(TComputings::roundToMinTime(moonDaysExt.at(i).newMoon.time()));
                ui->textEditMoonDate->append("Новолуние: "+moonDaysExt.at(i).newMoon.toString("dd.MM.yyyy  hh:mm"));
            }
        }
        if (0 == moonDaysExt.size())
        {
            ui->textEditMoonDate->append("Отсутствует");
        }

        // разблокировать интерфейс
        setEnabled(true);
        QApplication::restoreOverrideCursor();

        // позицию текстового курсора в начало
        QTextCursor textCursorToBegin (ui->textEditMoonDate->textCursor());
        textCursorToBegin.movePosition(QTextCursor::Start);
        ui->textEditMoonDate->setTextCursor(textCursorToBegin);

        */
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
        if (11 != m_currentTitha.num())
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
        ui->tableWidgetSvaras->clear();
        ui->tableWidgetSvaras->setColumnCount(0);
        ui->tableWidgetSvaras->setRowCount(0);

        if (svaras.size() > 0)
        {
            ui->tableWidgetSvaras->setColumnCount(2);
            ui->tableWidgetSvaras->setHorizontalHeaderLabels(QString("Тип,Время").split(","));
            ui->tableWidgetSvaras->setRowCount(svaras.size());
        }

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
                // текущая свара
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

        // Время от захода и до рассвета свара не определена
        if ((true == m_currentSunRise.isValid()) && (true == m_currentSunSet.isValid()) && ((QTime::currentTime() < m_currentSunRise) || (QTime::currentTime() > m_currentSunSet)))
        {
            ui->tabWidget->setTabText(2,"Свара не определена");
            ui->tabWidget->tabBar()->setTabTextColor(2,ui->tabWidget->tabBar()->tabTextColor(0));
        }
        else if ((QTime::currentTime() < TComputings::sunTimeRise(longitude,latitude,timeZoneOffset)) ||
                 (QTime::currentTime() > TComputings::sunTimeSet(longitude,latitude,timeZoneOffset)))
        {
            ui->tabWidget->setTabText(2,"Свара не определена");
            ui->tabWidget->tabBar()->setTabTextColor(2,ui->tabWidget->tabBar()->tabTextColor(0));
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
    m_currentDate = QDate::currentDate();
    mf_realClose = false;
    mf_ekadashWarned = false;
    mp_ekadashWarnMsgBox = nullptr;

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


#ifndef QT_NO_DEBUG
    // ---отладочная---
    bool ok;
    double longitude (-1*settings.value(DialogSettings::longitudeSettingName()).toDouble(&ok));
    double latitude (settings.value(DialogSettings::latitudeSettingName()).toDouble(&ok));
    double timeZoneOffset (settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok));
    double height (settings.value(DialogSettings::heightSettingName()).toDouble(&ok));

    if (true == ok)
    {
        QFile debugFile("debug.txt");
        debugFile.open(QIODevice::Append);
        QTextStream debug (&debugFile);

          // список новолуний
//        QList<QDateTime> newMoonList (TComputings::moonTimeFindNewMoonForPeriod(QDateTime::currentDateTimeUtc(),
//                                                                                QDateTime::currentDateTimeUtc().addYears(2),timeZoneOffset));

//        qDebug() << "newMoonList";
//        debug << "newMoonList";
//        for (qint32 i = 0; i < newMoonList.size(); ++i)
//        {
//            qDebug() << newMoonList.at(i).toString("dd MMMM yyyy hh:mm");
//            debug << newMoonList.at(i).toString("dd MMMM yyyy hh:mm") << endl;
//        }

        // список лунных дней
//        QList<TComputings::TMoonDay2> moonDays (TComputings::moonTimeMoonDays(longitude,latitude,timeZoneOffset,QDateTime::currentDateTimeUtc().addDays(-1),
//                                                                              QDateTime::currentDateTimeUtc().addDays(1), height));

//        for (qint32 i = 0; i < moonDays.size(); ++i)
//        {
//            moonDays[i].rise.setTime(TComputings::roundToMinTime(moonDays[i].rise.time()));
//            moonDays[i].set.setTime(TComputings::roundToMinTime(moonDays[i].set.time()));
//            moonDays[i].transit.setTime(TComputings::roundToMinTime(moonDays[i].transit.time()));
//        }

//        qDebug() << "moonDays";
//        debug << "moonDays";
//        for (qint32 i = 0; i < moonDays.size(); ++i)
//        {
//            qDebug() << "num" << moonDays.at(i).num;
//            qDebug() << "rise" << moonDays.at(i).rise.toString("dd MMMM yyyy hh:mm");
//            qDebug() << "set" << moonDays.at(i).set.toString("dd MMMM yyyy hh:mm");
//            qDebug() << "transit" << moonDays.at(i).transit.toString("dd MMMM yyyy hh:mm");
//            qDebug() << "";

//            debug << "num" << moonDays.at(i).num << endl;
//            debug << "rise" << moonDays.at(i).rise.toString("dd MMMM yyyy hh:mm") << endl;
//            debug << "set" << moonDays.at(i).set.toString("dd MMMM yyyy hh:mm") << endl;
//            debug << "transit" << moonDays.at(i).transit.toString("dd MMMM yyyy hh:mm") << endl;
//            debug << endl;
//        }

        debugFile.close();
    }
#endif
}
//---------------------------

MainWindow::~MainWindow()
{
    // остановить таймер обновления виджетов отображающих время
    m_Timer.stop();

    // освобождение ресурсов
    if (mp_TrayIconMenuActionQuit)
        delete mp_TrayIconMenuActionQuit;

    if (mp_ekadashWarnMsgBox)
        delete mp_ekadashWarnMsgBox;

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

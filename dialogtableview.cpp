// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "dialogtableview.h"
#include "ui_dialogtableview.h"
#include <QtDebug>
#include <QSettings>
#include <QFileDialog>
#include "computings.h"
#include "dialogsettings.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: DialogTableView - public
DialogTableView::DialogTableView(QWidget *parent) :
    QDialog(parent,Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogTableView)
{
    // значения по умолчанию
    m_Type = sunInfo;

    // инициализация интерфейса
    init();
}
//---------------------------

DialogTableView::DialogTableView(const qint32 type, QWidget* parent) :
    QDialog(parent,Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogTableView)
{
    // тип диалога (для инфы по Солнцу или Луне)
    if ((sunInfo == type) || (moonInfo == type))
        m_Type = static_cast<DialogType>(type);
    else
        m_Type = sunInfo;

    // инициализация интерфейса
    init();
}
//---------------------------

DialogTableView::~DialogTableView()
{
    delete ui;
}
//---------------------------
// КОНЕЦ: DialogTableView - public
//---------------------------------------------------------------------------------

// НАЧАЛО: DialogTableView - private slots
void DialogTableView::on_radioButtonCalendar_toggled(bool checked)
{
    // интерфейс: использовать календарь или период
    if (true == checked)
    {
        // календарь
        ui->dateEditBegin->setEnabled(true);
        ui->dateEditEnd->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->comboBoxPeriod->setDisabled(true);
        ui->comboBoxPeriod->setCurrentIndex(0);
        ui->label_3->setDisabled(true);
    }
    else
    {
        // период
        ui->dateEditBegin->setDisabled(true);
        ui->dateEditEnd->setDisabled(true);
        ui->label->setDisabled(true);
        ui->label_2->setDisabled(true);
        ui->comboBoxPeriod->setEnabled(true);
        ui->comboBoxPeriod->setCurrentIndex(1);
        ui->label_3->setEnabled(true);
    }
}
//---------------------------

void DialogTableView::on_comboBoxPeriod_currentIndexChanged(const QString& arg1)
{
    // выбран "календарь"
    if ("календарь" == arg1)
        ui->radioButtonCalendar->setChecked(true);
}
//---------------------------

void DialogTableView::on_dateEditBegin_dateChanged(const QDate& date)
{
    // если дата окончания меньше даты начала, то установить дату окончания значением по умолчанию
    if (ui->dateEditEnd->date() < date)
        ui->dateEditEnd->setDate(date);
    ui->dateEditEnd->setMinimumDate(ui->dateEditBegin->date());
}
//---------------------------

void DialogTableView::on_pushButtonCalculate_clicked()
{
    // Вычислить информацию по Солнцу или Луне
    QSettings settings;
    double longitude, latitude, height, timeZoneOffset;
    bool ok;    

    // загрузить настройки
    longitude = -1*(settings.value(DialogSettings::longitudeSettingName()).toDouble(&ok));
    latitude = settings.value(DialogSettings::latitudeSettingName()).toDouble(&ok);
    height = settings.value(DialogSettings::heightSettingName()).toDouble(&ok);
    timeZoneOffset = settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok);

    if (true == ok)
    {
        // --определение временных рамок--
        QDate d1 (QDate::currentDate());
        QDate d2 (QDate::currentDate());

        if (true == ui->radioButtonCalendar->isChecked())
        {
            // календарь
            d1 = (ui->dateEditBegin->date());
            d2 = (ui->dateEditEnd->date());
        }
        else if (true == ui->radioButtonPeriod->isChecked())
        {
            // период (с... по...)
            switch (ui->comboBoxPeriod->currentIndex())
            {
            case 1:
                d2 = d1.addMonths(1);
                break;
            case 2:
                d2 = d1.addMonths(2);
                break;
            case 3:
                d2 = d1.addMonths(3);
                break;
            case 4:
                d2 = d1.addMonths(6);
                break;
            case 5:
                d2 = d1.addYears(1);
                break;
            case 6:
                d2 = d1.addYears(2);
                break;
            case 7:
                d2 = d1.addYears(3);
                break;
            case 8:
                d2 = d1.addYears(10);
                break;
            default:
                d2 = d1.addMonths(1);
                break;
            }
        }

        // блокирование интерфейса
        QCursor mouseCursor (cursor());
        setCursor(QCursor(Qt::WaitCursor));
        setDisabled(true);

        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setColumnCount(0);
        ui->tableWidget->repaint();

        if (sunInfo == m_Type)
        {
            //--------- рассчёт для Солнца ---------
            // ----таблица----
            // столбцы
            if (true == ui->checkBoxTwilight->isChecked())
            {
                // --с сумерками--
                ui->tableWidget->setColumnCount(17);
                // заголовки столбцов
                ui->tableWidget->setHorizontalHeaderLabels(QString("Дата,Восход,Заход,Зенит,Долгота дня,Утренние гражданские\nсумерки,Утренние навигационные\nсумерки,"\
                                                                   "Утренние астрономические\nсумерки,Утренняя сандхья\nкак 1/10 суток,"\
                                                                   "Утренняя сандхья\nкак 1/10 от половины суток,Утренняя сандхья\nкак 1/10 светового дня,"\
                                                                   "Вечерние гражданские\nсумерки,Вечерние навигационные\nсумерки,Вечерние астрономические\nсумерки,"\
                                                                   "Вечерняя сандхья\nкак 1/10 суток,Вечерняя сандхья\nкак 1/10 от половины суток,"\
                                                                   "Вечерняя сандхья\nкак 1/10 светового дня").split(","));
            }
            else
            {
                // --без сумерек--
                ui->tableWidget->setColumnCount(5);
                // заголовки столбцов
                ui->tableWidget->setHorizontalHeaderLabels(QString("Дата,Восход,Заход,Зенит,Долгота дня,").split(","));
            }

            // строки
            ui->tableWidget->setRowCount(d1.daysTo(d2)+1);

            // ----рассчёт для периода времени----
            quint32 c (0);
            while (d1 <= d2)
            {
                // время восхода Солнца
                QTime sunRise (TComputings::sunTimeRise(longitude,latitude,timeZoneOffset,d1));
                sunRise = TComputings::roundToMinTime(sunRise);

                // время захода Солнца
                QTime sunSet (TComputings::sunTimeSet(longitude,latitude,timeZoneOffset,d1));
                sunSet = TComputings::roundToMinTime(sunSet);

                // высшая точка Солнца (зенит)
                bool aboveHorizont;
                QTime sunTransit (TComputings::sunTimeTransit(longitude,latitude,aboveHorizont,timeZoneOffset,d1));
                sunTransit = TComputings::roundToMinTime(sunTransit);

                // долгота дня
                QTime lengthOfDay (QTime::fromMSecsSinceStartOfDay(sunRise.msecsTo(sunSet)));

                if (true == ui->checkBoxTwilight->isChecked())
                {
                    // --с сумерками--
                    // утренние гражданские сумерки сегодня
                    QPair<QTime,QTime> morningCivilTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,sunRise,height,timeZoneOffset,
                                                                                                 static_cast<double>(TComputings::civilTwilight),d1));
                    morningCivilTwilight = TComputings::roundToMinTime(morningCivilTwilight);

                    // утренние навигационные сумерки (до восхода)
                    QPair<QTime,QTime> morningNavigationTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,sunRise,height,timeZoneOffset,
                                                                                                      static_cast<double>(TComputings::navigationTwilight),d1));
                    if ((true == morningNavigationTwilight.first.isValid()) && (true == morningNavigationTwilight.second.isValid()))
                    {
                        morningNavigationTwilight = TComputings::roundToMinTime(morningNavigationTwilight);
                        morningNavigationTwilight.second = morningCivilTwilight.first;
                    }

                    // утренние астрономические сумерки (до восхода)
                    QPair<QTime,QTime> morningAstronomicalTwilight (TComputings::sunTimeMorningTwilight(longitude,latitude,sunRise,height,timeZoneOffset,
                                                                                                        static_cast<double>(TComputings::astronomicalTwilight),d1));
                    if ((true == morningAstronomicalTwilight.first.isValid()) && (true == morningAstronomicalTwilight.second.isValid()))
                    {
                        morningAstronomicalTwilight = TComputings::roundToMinTime(morningAstronomicalTwilight);
                        morningAstronomicalTwilight.second = morningNavigationTwilight.first;
                    }

                    // утренние сумерки (сандхья), как 1/10 часть от половины суток
                    QPair<QTime,QTime> morningSandhya(TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,sunRise,timeZoneOffset,true,d1));
                    morningSandhya = TComputings::roundToMinTime(morningSandhya);

                    // утренние сумерки (сандхья), как 1/10 часть от суток
                    qint32 offset ((morningSandhya.second.msecsSinceStartOfDay() - morningSandhya.first.msecsSinceStartOfDay())*2);
                    QPair <QTime, QTime> morningSandhya1 (QTime::fromMSecsSinceStartOfDay(morningSandhya.second.msecsSinceStartOfDay() - offset),morningSandhya.second);

                    // утренние сумерки (сандхья), как 1/10 часть от светового дня
                    QPair<QTime,QTime> morningSandhya2(TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,sunRise,sunSet,timeZoneOffset,true,d1));
                    morningSandhya2 = TComputings::roundToMinTime(morningSandhya2);

                    // вечерние гражданские сумерки сегодня
                    QPair<QTime,QTime> eveningCivilTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,sunSet,height,timeZoneOffset,
                                                                                                 static_cast<double>(TComputings::civilTwilight),d1));
                    eveningCivilTwilight = TComputings::roundToMinTime(eveningCivilTwilight);

                    // вечерние навигационные сумерки (от захода)
                    QPair<QTime,QTime> eveningNavigationTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,sunSet,height,timeZoneOffset,
                                                                                                      static_cast<double>(TComputings::navigationTwilight),d1));
                    if ((true == eveningNavigationTwilight.first.isValid()) && (true == eveningNavigationTwilight.second.isValid()))
                    {
                        eveningNavigationTwilight = TComputings::roundToMinTime(eveningNavigationTwilight);
                        eveningNavigationTwilight.first = eveningCivilTwilight.second;
                    }

                    // вечерние астрономические сумерки (от захода)
                    QPair<QTime,QTime> eveningAstronomicalTwilight (TComputings::sunTimeEveningTwilight(longitude,latitude,sunSet,height,timeZoneOffset,
                                                                                                        static_cast<double>(TComputings::astronomicalTwilight),d1));
                    if ((true == eveningAstronomicalTwilight.first.isValid() && true == eveningAstronomicalTwilight.second.isValid()))
                    {
                        eveningAstronomicalTwilight = TComputings::roundToMinTime(eveningAstronomicalTwilight);
                        eveningAstronomicalTwilight.first = eveningNavigationTwilight.second;
                    }

                    // вечерние сумерки (сандхья), как 1/10 часть от половины суток
                    QPair<QTime,QTime> eveningSandhya (TComputings::sunTimeSandhyaAsDayPart(longitude,latitude,sunSet,timeZoneOffset,false,d1));
                    eveningSandhya = TComputings::roundToMinTime(eveningSandhya);

                    // вечерние сумерки (сандхья), как 1/10 часть от суток
                    offset = (eveningSandhya.second.msecsSinceStartOfDay() - eveningSandhya.first.msecsSinceStartOfDay())*2;
                    QPair<QTime, QTime> eveningSandhya1 (eveningSandhya.first, eveningSandhya.first.addMSecs(offset));

                    // вечерние сумерки (сандхья), как 1/10 часть от светового дня
                    QPair<QTime,QTime> eveningSandhya2 (TComputings::sunTimeSandhyaAsLightDayPart(longitude,latitude,sunRise,sunSet,timeZoneOffset,false,d1));
                    eveningSandhya2 = TComputings::roundToMinTime(eveningSandhya2);

                    //добавление в таблицу
                    ui->tableWidget->setItem(c,0,new QTableWidgetItem(d1.toString("dd.MM.yyyy")));
                    ui->tableWidget->setItem(c,1,new QTableWidgetItem(sunRise.toString("hh:mm")));
                    ui->tableWidget->setItem(c,2,new QTableWidgetItem(sunSet.toString("hh:mm")));
                    if (true == aboveHorizont)
                        ui->tableWidget->setItem(c,3,new QTableWidgetItem(sunTransit.toString("hh:mm")));
                    else
                        ui->tableWidget->setItem(c,3,new QTableWidgetItem("под горизонтом"));
                    ui->tableWidget->setItem(c,4,new QTableWidgetItem(lengthOfDay.toString("hh:mm")));
                    ui->tableWidget->setItem(c,5,new QTableWidgetItem(morningCivilTwilight.first.toString("hh:mm") + " - " + morningCivilTwilight.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,6,new QTableWidgetItem(morningNavigationTwilight.first.toString("hh:mm") + " - " + morningNavigationTwilight.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,7,new QTableWidgetItem(morningAstronomicalTwilight.first.toString("hh:mm") + " - " + morningAstronomicalTwilight.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,8,new QTableWidgetItem(morningSandhya1.first.toString("hh:mm") + " - " + morningSandhya1.second.toString("hh:mm")));;
                    ui->tableWidget->setItem(c,9,new QTableWidgetItem(morningSandhya.first.toString("hh:mm") + " - " + morningSandhya.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,10,new QTableWidgetItem(morningSandhya2.first.toString("hh:mm") + " - " + morningSandhya2.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,11,new QTableWidgetItem(eveningCivilTwilight.first.toString("hh:mm") + " - " + eveningCivilTwilight.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,12,new QTableWidgetItem(eveningNavigationTwilight.first.toString("hh:mm") + " - " + eveningNavigationTwilight.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,13,new QTableWidgetItem(eveningAstronomicalTwilight.first.toString("hh:mm") + " - " + eveningAstronomicalTwilight.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,14,new QTableWidgetItem(eveningSandhya1.first.toString("hh:mm") + " - " + eveningSandhya1.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,15,new QTableWidgetItem(eveningSandhya.first.toString("hh:mm") + " - " + eveningSandhya.second.toString("hh:mm")));
                    ui->tableWidget->setItem(c,16,new QTableWidgetItem(eveningSandhya2.first.toString("hh:mm") + " - " + eveningSandhya2.second.toString("hh:mm")));

                }
                else
                {
                    // --без сумерек--
                    ui->tableWidget->setItem(c,0,new QTableWidgetItem(d1.toString("dd.MM.yyyy")));
                    ui->tableWidget->setItem(c,1,new QTableWidgetItem(sunRise.toString("hh:mm")));
                    ui->tableWidget->setItem(c,2,new QTableWidgetItem(sunSet.toString("hh:mm")));
                    if (true == aboveHorizont)
                        ui->tableWidget->setItem(c,3,new QTableWidgetItem(sunTransit.toString("hh:mm")));
                    else
                        ui->tableWidget->setItem(c,3,new QTableWidgetItem("под горизонтом"));
                    ui->tableWidget->setItem(c,4,new QTableWidgetItem(lengthOfDay.toString("hh:mm")));
                }

                // следующая дата
                d1 = d1.addDays(1);
                ++c;
            }
        }
        else if (moonInfo == m_Type)
        {
            //--------- рассчёт для Луны ---------
            // ----таблица----
            // количество столбцов
            ui->tableWidget->setColumnCount(6);
            // заголовки столбцов
            ui->tableWidget->setHorizontalHeaderLabels(QString("День №,Восход,Заход,Зенит,Фаза,Новолуние").split(","));

            // основные рассчёты
            QList<TComputings::TMoonDay2> moonDays (TComputings::moonTimeMoonDays(longitude,latitude,timeZoneOffset,
                                                                                  QDateTime(d1,QTime(0,0)),QDateTime(d2,QTime(23,59,59,999)),height));

            // количество строк таблицы
            ui->tableWidget->setRowCount(moonDays.size());

            for (qint32 i = 0; i < moonDays.size(); ++i)
            {
                // довычисления
                qint32 moonPhase (TComputings::moonTimePhase(moonDays.at(i).rise.date()));
                QString moonPhaseStr ("-");
                QString newMoonStr ("-");
                if (moonPhase >= 0)
                    moonPhaseStr = QString::number(moonPhase)+"%";

                //добавление в таблицу
                ui->tableWidget->setItem(i,0,new QTableWidgetItem(moonDays.at(i).num));
                ui->tableWidget->setItem(i,1,new QTableWidgetItem(moonDays.at(i).rise.toString("dd.MM.yyyy hh:mm")));
                ui->tableWidget->setItem(i,2,new QTableWidgetItem(moonDays.at(i).set.toString("dd.MM.yyyy hh:mm")));
                ui->tableWidget->setItem(i,3,new QTableWidgetItem(moonDays.at(i).transit.toString("dd.MM.yyyy hh:mm")));
                ui->tableWidget->setItem(i,4,new QTableWidgetItem(moonPhaseStr));
                if (true == moonDays.at(i).newMoon.isValid())
                    newMoonStr = moonDays.at(i).newMoon.toString("dd.MM.yyyy hh:mm");
                    ui->tableWidget->setItem(i,5,new QTableWidgetItem(newMoonStr));
            }
        }

        // вся таблица только для чтения, выравнивание текста по центру
        for (qint32 i = 0; i < ui->tableWidget->rowCount(); ++i)
            for (qint32 j = 0; j < ui->tableWidget->columnCount(); ++j)
            {
                (ui->tableWidget->item(i,j))->setFlags((ui->tableWidget->item(i,j))->flags() & ~Qt::ItemIsEditable);
                ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
            }

        // расширение столбцов под содержимое
        ui->tableWidget->resizeColumnsToContents();

        // разблокирование интерфейса
        setCursor(mouseCursor);
        setEnabled(true);
    }
    else
        qWarning() << "DialogTableView::on_pushButtonCalculate_clicked" << "load settings error";
}
//---------------------------

void DialogTableView::on_pushButtonSaveAs_clicked()
{
    //TODO: Сохранить как...
    // ---сохранить как---
    QString fileName (QFileDialog::getSaveFileName(this, "Сохранить как...",QString(),"HyperText (*.html)"));
    if (false == fileName.isEmpty())
    {
        QFile outFile (fileName);
        if (true == outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // вывод в файл
            QTextStream out (&outFile);

            // формат файла - простой html
            out << "<html>" << endl;
            out << "<head>" << endl;

            // заголовок html файла для солнечного времени
            out << "<h1 align='center'>" << endl;
            if (sunInfo == m_Type)
                out << QString("Солнечное время") << endl;
            else if (moonInfo == m_Type)
                out << QString("Лунное время") << endl;

            if (true == ui->radioButtonCalendar->isChecked())
                out << QString("с ") << ui->dateEditBegin->date().toString("dd.MM.yyyy")
                    << QString(" по ") << ui->dateEditEnd->date().toString("dd.MM.yyyy") << endl << endl;
            else if (true == ui->radioButtonPeriod->isChecked())
                out << QString("с ") << QDate::currentDate().toString("dd.MM.yyyy") << QString(" на ") << ui->comboBoxPeriod->currentText() << endl << endl;
            out << "</h1>";
            out << "</head>" << endl;

            // содержимое html файла - таблица
            out << "<body>" << endl;
            out << "<table cols='18'' align='center' border='1' cellpadding='5' cellspacing='0'>" << endl;
            out << "<thead style='font-weight: bold'>" << endl;
            out << "<tr align='center' valign='center'>" << endl;
            // заголовки столбцов в текстовый файл
            for (qint32 i = 0; i < ui->tableWidget->columnCount(); ++i)
                out << "\t" << "<td>" << (ui->tableWidget->horizontalHeaderItem(i)->text()).replace("\n"," ") << "</td>" << endl;
            out << "</tr>" << endl;
            out << "</thead>" << endl;

            // содержимое таблицы в текстовый файл
            for (qint32 i = 0; i < ui->tableWidget->rowCount(); ++i)
            {
                out << "<tr align='center' valign='center'>" << endl;
                for (qint32 j = 0; j < ui->tableWidget->columnCount(); ++j)
                    out << "<td>" << (ui->tableWidget->item(i,j))->text() << "</td>" << endl;
                out << "</tr>" << endl;
            }

            out << "</table>" << endl;
            out << "</body>" << endl;
            out << "</html>" << endl;

            outFile.close();
        }
        else
            qWarning() << "DialogTableView::on_pushButtonSaveAs_clicked" << "open file error";
    }
    else
        qWarning() << "DialogTableView::on_pushButtonSaveAs_clicked" << "getSaveFileName error";
}
//---------------------------
// КОНЕЦ: DialogTableView - private slots
//---------------------------------------------------------------------------------

// НАЧАЛО: DialogTableView - private
void DialogTableView::init()
{
    // основная установка интерфейса
    ui->setupUi(this);

    // объединение "радио" кнопок в одну группу
    m_RadioButtons.addButton(ui->radioButtonCalendar);
    m_RadioButtons.addButton(ui->radioButtonPeriod);

    // заголовок окна
    if (sunInfo == m_Type)
        setWindowTitle("Солнечное время");
    else if (moonInfo == m_Type)
        setWindowTitle("Лунное время");

    // значения по умолчанию для периода расчётов
    ui->dateEditBegin->setDate(QDate::currentDate());
    ui->dateEditEnd->setDate(QDate::currentDate().addDays(1));
    ui->dateEditEnd->setMinimumDate(ui->dateEditBegin->date());

    // чекбокс считать сумерки только для Солнечного времени
    if (moonInfo == m_Type)
        ui->checkBoxTwilight->setVisible(false);
}
//---------------------------
// КОНЕЦ: DialogTableView - private
//---------------------------------------------------------------------------------




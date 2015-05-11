// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "dialogtithi.h"
#include "ui_dialogtithi.h"
#include "tithi.h"
#include <QtDebug>
#include <QSettings>
#include <QFileDialog>
#include "dialogsettings.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: DialogTithi - public
DialogTithi::DialogTithi(QWidget *parent) :
    QDialog(parent,  Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogTithi)
{
    // инициализация
    init();
}
//---------------------------

DialogTithi::DialogTithi(const bool ekadashi, QWidget* parent) :
    QDialog(parent,  Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogTithi)
{
    // задание функциональности диалога
    if (true == ekadashi)
        m_Type = ekadashiType;
    else
        m_Type = tithiType;

    // инициализация
    init();
}
//---------------------------

DialogTithi::~DialogTithi()
{
    delete ui;
}
//---------------------------
// КОНЕЦ: DialogTithi - public
//---------------------------------------------------------------------------------

// НАЧАЛО: DialogTithi - private slots
void DialogTithi::on_dateEditBegin_dateChanged(const QDate& date)
{
    // если дата окончания меньше даты начала, то установить дату окончания значением по умолчанию
    if (ui->dateEditEnd->date() < date)
        ui->dateEditEnd->setDate(date);
    ui->dateEditEnd->setMinimumDate(ui->dateEditBegin->date());
}
//---------------------------

void DialogTithi::on_pushButtonCalculate_clicked()
{
    // рассчитать титхи или экадаши
    QSettings settings;
    double timeZoneOffset;
    bool ok;
    QDateTime dt1 (QDateTime::currentDateTimeUtc());
    QDateTime dt2 (QDateTime::currentDateTimeUtc());

    // прочитать часовой пояс из настроек программы
    timeZoneOffset = settings.value(DialogSettings::timeZoneOffsetSettingName()).toDouble(&ok);
    if (true == ok)
    {
        // рассчитать титхи или экадаши
        if (true == ui->radioButtonCalendar->isChecked())
        {
            // календарь
            dt1.setTime(QTime::currentTime());
            dt1.setDate(ui->dateEditBegin->date());
            dt2.setTime(QTime(0,0));
            dt2.setDate(ui->dateEditEnd->date().addDays(2));
        }
        else if (true == ui->radioButtonPeriod->isChecked())
        {
            // период
            dt1 = QDateTime::currentDateTimeUtc();

            switch (ui->comboBoxPeriod->currentIndex())
            {
            case 1:
                dt2 = dt1.addMonths(1);
                break;
            case 2:
                dt2 = dt1.addMonths(2);
                break;
            case 3:
                dt2 = dt1.addMonths(3);
                break;
            case 4:
                dt2 = dt1.addMonths(6);
                break;
            case 5:
                dt2 = dt1.addYears(1);
                break;
            case 6:
                dt2 = dt1.addYears(2);
                break;
            case 7:
                dt2 = dt1.addYears(3);
                break;
            case 8:
                dt2 = dt1.addYears(10);
                break;
            default:
                dt2 = dt1.addMonths(1);
                break;
            }

            // запасик, чтобы было вычислено не меньше титх, чем задано солнечных дней
            dt2 = dt2.addDays(2);
        }

        // заблокировать интерфейс
        QCursor mouseCursor (cursor());
        setCursor(QCursor(Qt::WaitCursor));
//        ui->pushButtonCalculate->setDisabled(true);
//        ui->pushButtonSaveAs->setDisabled(true);
        setDisabled(true);
        ui->textEdit->clear();
        ui->textEdit->repaint();

        if (true == ui->checkBoxTithiPrintUtc->isChecked())
        {
            QString timeZoneOffsetStr ("Часовой пояс: ");
            if (timeZoneOffset >= 0)
                timeZoneOffsetStr += "+";
            timeZoneOffsetStr += QString::number(timeZoneOffset);
            ui->textEdit->append(timeZoneOffsetStr);
            ui->textEdit->append("");
        }

        // рассчёт
        if (tithiType == m_Type)
        {
            // титхи
            QList<TTitha> tithi (TTitha::findTithi(dt1,dt2,timeZoneOffset));
            foreach (const TTitha& titha, tithi)
            {
                // вывод титх
                ui->textEdit->append(titha.asTithiStrExt());
                ui->textEdit->append("");
            }
        }
        else if (ekadashiType == m_Type)
        {
            // экадаши
            QList<TTitha> ekadashi (TTitha::findEkadashi(dt1,dt2,timeZoneOffset));
            foreach (const TTitha& titha, ekadashi)
            {
                // вывод экадашей
                ui->textEdit->append(titha.asEkadashStr());
                ui->textEdit->append("");
            }
        }

        // разблокировать интерфейс
        setCursor(mouseCursor);
//        ui->pushButtonCalculate->setEnabled(true);
//        ui->pushButtonSaveAs->setEnabled(true);
        setEnabled(true);

        // позицию текстового курсора в начало
        QTextCursor textCursorToBegin (ui->textEdit->textCursor());
        textCursorToBegin.movePosition(QTextCursor::Start);
        ui->textEdit->setTextCursor(textCursorToBegin);
    }
    else // произошла ошибка
    {
        qWarning() << "DialogTithi::on_pushButtonCalculate_clicked" << settings.status();
        ui->textEdit->clear();
        ui->textEdit->append("Не задан часовой пояс, необходимый для рассчётов.");
        ui->textEdit->append("Опции -> Настройки (Ctrl+o)");
    }
}
//---------------------------

void DialogTithi::on_radioButtonCalendar_toggled(bool checked)
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

void DialogTithi::on_comboBoxPeriod_currentIndexChanged(const QString &arg1)
{
    // выбран "календарь"
    if ("календарь" == arg1)
        ui->radioButtonCalendar->setChecked(true);
}
//---------------------------

void DialogTithi::on_pushButtonSaveAs_clicked()
{
    // сохранить как
    QString filter ("Text (*.txt);;HyperText (*.html)");
    QString selectedFilter;
    QString fileName (QFileDialog::getSaveFileName(this, "Сохранить как...",QString(),filter,&selectedFilter));
    if (false == fileName.isEmpty())
    {
//        if ((true == selectedFilter.contains("*.txt")) && (false == fileName.contains(".txt")))
//            fileName += ".txt";
//        else if (false == fileName.contains(".html"))
//            fileName += ".html";

        QFile outFile (fileName);
        if (true == outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // вывод в файл
            QTextStream out (&outFile);
            if (tithiType == m_Type)
            {
                if (true == selectedFilter.contains("*.txt"))
                    out << QString("Титхи") << endl;
                else
                    out << QString("<h1>Титхи</h1>") << endl;
            }
            else if (ekadashiType == m_Type)
            {
                if (true == selectedFilter.contains("*.txt"))
                    out << QString("Экадаши") << endl;
                else
                    out << QString("<h1>Экадаши</h1>") << endl;
            }
            if (true == ui->radioButtonCalendar->isChecked())
            {
                if (true == selectedFilter.contains("*.txt"))
                    out << QString("С ") << ui->dateEditBegin->date().toString("dd.MM.yyyy") <<
                           QString(" по ") << ui->dateEditEnd->date().toString("dd.MM.yyyy") << endl << endl;
                else
                    out << "<h2>" << QString("С ") << ui->dateEditBegin->date().toString("dd.MM.yyyy") <<
                           QString(" по ") << ui->dateEditEnd->date().toString("dd.MM.yyyy") << "</h2>" << endl << endl;
            }
            else if (true == ui->radioButtonPeriod->isChecked())
            {
                if (true == selectedFilter.contains("*.txt"))
                    out << QString("С ") << QDate::currentDate().toString("dd.MM.yyyy") << QString(" на ") << ui->comboBoxPeriod->currentText() << endl << endl;
                else
                    out << "<h2>" << QString("С ") << QDate::currentDate().toString("dd.MM.yyyy") << QString(" на ") << ui->comboBoxPeriod->currentText() << "</h2>" << endl << endl;
            }
            if (true == selectedFilter.contains("*.txt"))
                out << ui->textEdit->toPlainText();
            else
                out << ui->textEdit->toHtml();
            outFile.close();
        }
        else
            qWarning() << "DialogTithi::on_pushButtonSaveAs_clicked" << "open file error";
    }
    else
        qWarning() << "DialogTithi::on_pushButtonSaveAs_clicked" << "getSaveFileName error";
}
//---------------------------
// КОНЕЦ: DialogTithi - private slots
//---------------------------------------------------------------------------------

// НАЧАЛО: DialogTithi - private
void DialogTithi::init()
{
    // установка интерфейса
    ui->setupUi(this);

    // объединение "радио" кнопок в одну группу
    m_RadioButtons.addButton(ui->radioButtonCalendar);
    m_RadioButtons.addButton(ui->radioButtonPeriod);

    // по умолчанию диалог выводит титхи
    if (undefinedType == m_Type)
        m_Type = tithiType;

    // заголовок окна
    if (ekadashiType == m_Type)
        setWindowTitle("Экадаши");

    // значения по умолчанию для периода расчётов
    ui->dateEditBegin->setDate(QDate::currentDate());
    ui->dateEditEnd->setDate(QDate::currentDate().addDays(1));
    ui->dateEditEnd->setMinimumDate(ui->dateEditBegin->date());
}
//---------------------------
// КОНЕЦ: DialogTithi - private
//---------------------------------------------------------------------------------







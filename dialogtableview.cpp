// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "dialogtableview.h"
#include "ui_dialogtableview.h"
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
    // Вычислить...

}
//---------------------------

void DialogTableView::on_pushButtonSaveAs_clicked()
{
    //TODO: Сохранить как...
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
}
//---------------------------
// КОНЕЦ: DialogTableView - private
//---------------------------------------------------------------------------------




#ifndef DIALOGTABLEVIEW_H
#define DIALOGTABLEVIEW_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QDialog>
#include <QButtonGroup>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: классы
namespace Ui {
class DialogTableView;
}

/// \brief Класс диалогового окна для отображения информации за временной период в виде таблицы
class DialogTableView : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTableView(QWidget* parent = 0);
    DialogTableView(const qint32 type, QWidget* parent = 0);
    ~DialogTableView();

    /// \enum перечисление возможной функциональности диалога (рассчёт данных для Солнца или для Луны)
    enum DialogType
    {
        sunInfo,
        moonInfo,
        undefined
    };
    //---------------------------

private slots:

    void on_radioButtonCalendar_toggled(bool checked);
    void on_comboBoxPeriod_currentIndexChanged(const QString &arg1);
    void on_dateEditBegin_dateChanged(const QDate &date);

    void on_pushButtonCalculate_clicked();

    void on_pushButtonSaveAs_clicked();

private:
    Ui::DialogTableView *ui;                    //!< интерфейс
    DialogType          m_Type;                 //!< тип диалога (для рассчёта по Солнцу или Луне)
    QButtonGroup        m_RadioButtons;         //!< группа "радио" кнопок

    /// \brief инициализация (вызывается в конструкторах)
    void init();
};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------

#endif // DIALOGTABLEVIEW_H

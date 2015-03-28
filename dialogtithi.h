#ifndef DIALOGTITHI_H
#define DIALOGTITHI_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QDialog>
#include <QButtonGroup>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: классы
namespace Ui {
class DialogTithi;
}

/// \brief Класс диалогового окна вывода титх и экадашей
class DialogTithi : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTithi(QWidget* parent = 0);

    /// \brief альтернативный конструктор
    /// \param ekadashi - диалог будет использован для рассчёта экадашей или титх?
    /// (true - для экадашей, false - для титх)
    DialogTithi(const bool ekadashi = false, QWidget* parent = 0);

    ~DialogTithi();

private slots:
    void on_dateEditBegin_dateChanged(const QDate& date);

    void on_pushButtonCalculate_clicked();

    void on_radioButtonCalendar_toggled(bool checked);

    void on_comboBoxPeriod_currentIndexChanged(const QString &arg1);

    void on_pushButtonSaveAs_clicked();

private:
    /// \brief перечисление возможной функциональности диалога (рассчёт титх или экадашей)
    enum TithiDialogType
    {
        tithiType,
        ekadashiType,
        undefinedType
    };

    /// \brief инициализация (вызывается в конструкторах)
    void init();

    Ui::DialogTithi*    ui;                    ///< интерфейс
    TithiDialogType     m_Type;                ///< флаг определяющий функционал диалога
    QButtonGroup        m_RadioButtons;        ///< группа "радио" кнопок
};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------

#endif // DIALOGTITHI_H

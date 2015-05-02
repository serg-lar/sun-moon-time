#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QMainWindow>
#include <QTimer>
#include <QDate>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: классы
namespace Ui {
class MainWindow;
}

/// \brief класс главного окна программы
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /// \brief Обновить виджеты, отображающие время
    void updateTime();

    /// \brief Показать диалог настроек программы
    void showSettingsDialog();

    void on_radioButtonKaliYuga_clicked();
    void on_radioButtonMahaYuga_clicked();
    void on_radioButtonManvantara_clicked();
    void on_radioButtonKalpa_clicked();
    void on_radioButtonMahaKalpa_clicked();
    void on_pushButtonTithiPeriod_clicked();
    void on_pushButtonEkadashi_clicked();

    void on_pushButtonSunTimePeriod_clicked();

    void on_pushButtonMoonDatePeriod_clicked();

private:
    /// \enum Перечисление констант
    enum Consts
    {
        timerInterval =             50,             // интервал срабатывания таймера для обновления времени (мс)
        calendarUpdateInterval =    1000,           // интервал для обновления грегорианского календаря (мс)
        sunMoonInfoUpdateInterval = 10*1000         // интервал для обновления информации по Солнцу и Луне
    };

    /// \brief Вывести год по ведическому летоисчислению
    void showVedicDate();

    /// \brief Вывести инфо по времени Солнца
    void showSunTime();

    /// \brief Вывести инфо по времени Луны
    void showMoonTime();

    /// \brief Вывести титхи
    void showTithi();

    /// \brief Вывести информацию по сварам
    void showSvara();

    Ui::MainWindow *ui;                                 ///< интерфейс
    QTimer          m_Timer;                            ///< таймер для обновления времени
    quint32         m_cTimer;                           ///< счётчик срабатываний таймера
    QDate           m_Date;                             ///< дата на момент срабатывания таймера
};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------

#endif // MAINWINDOW_H

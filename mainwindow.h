#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QMainWindow>
#include <QTimer>
#include <QDate>
#include <QSystemTrayIcon>
#include <QMenu>
#include "tithi.h"
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

private slots:
    /// \brief Обновить виджеты, отображающие время
    void updateTime();

    /// \brief Показать диалог настроек программы
    void showSettingsDialog();

    /// \brief Закрыть окно насовсем
    void realClose();

    /// \brief Пересчитать и отобразить всю информацию во всех вкладках
    void computeAndShowAll();

    void on_radioButtonKaliYuga_clicked();
    void on_radioButtonMahaYuga_clicked();
    void on_radioButtonManvantara_clicked();
    void on_radioButtonKalpa_clicked();
    void on_radioButtonMahaKalpa_clicked();
    void on_pushButtonTithiPeriod_clicked();
    void on_pushButtonEkadashi_clicked();

    void on_pushButtonSunTimePeriod_clicked();

    void on_pushButtonMoonDatePeriod_clicked();
    void on_systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    /// \brief переопределение метода для перехвата событий закрытия окна
    virtual void closeEvent(QCloseEvent* e);

private:
    /// \enum Перечисление констант
    enum Consts
    {
        // все интервалы приняты кратными для простоты отработки!
        timerInterval             = 50,             // интервал срабатывания таймера для обновления времени (мс)
        calendarUpdateInterval    = 1000,           // интервал для обновления грегорианского календаря (мс)
        sunMoonInfoUpdateInterval = 10*1000,        // интервал для обновления информации по Солнцу и Луне
        warnInfoInterval          = 50*1000         // интервал для возможного вывода оповещений
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

    Ui::MainWindow*     ui;                          ///< интерфейс
    QTimer              m_Timer;                     ///< таймер для обновления времени
    quint32             m_cTimer;                    ///< счётчик срабатываний таймера
    QDate               m_Date;                      ///< дата на момент срабатывания таймера
    QTime               m_currentSunRise;            ///< время текущего восхода Солнца
    QTime               m_currentSunSet;             ///< время текущего захода Солнца
    TTitha              m_currentTitha;              ///< текущая титха
    bool                mf_ekadashWarned;            ///< оповещение о экадаше произведено
    QSystemTrayIcon     m_TrayIcon;                  ///< значок приложения на панели (в трее)
    QMenu               m_TrayIconMenu;              ///< контекстное меню для значка в трее
    QAction*            mp_TrayIconMenuActionQuit;   ///< пункт "выход" контекстного меню для иконки в трее
    bool                mf_realClose;                ///< флаг закрытия окна на совсем

};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------

#endif // MAINWINDOW_H

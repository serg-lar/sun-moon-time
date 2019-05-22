#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// НАЧАЛО: директивы, глобальные переменные и константы
// Qt
#include <QMainWindow>
#include <QTimer>
#include <QDate>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
// sun-moon-time
#include "tithi.h"
#include "computings.h"
#include "dialogvideosviewer.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: классы
namespace Ui {
class MainWindow;
}

/// \brief Свой класс QGraphicsView для показа постера.
class QPosterGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    QPosterGraphicsView(QWidget *parent = nullptr);
    ~QPosterGraphicsView() override;

protected:
    // Родительский виджет.
    QWidget *mpParent;
    // Геометрия в родительском виджете.
    QRect mInParenGeometry;
    // Флаг состояния полноэкранного отображения.
    bool mbFullScreenState = false;

    /// \brief Двойной клик по постеру открывает его показ во весь экран
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    /// \brief Нажатие клавиши на клавиатуре
    void keyPressEvent(QKeyEvent *event) override;
};
//---------------------------


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

    /// \brief Показать диалог о программе
    void showAboutDialog();

    /// Контроль за настройкой использования google карт
    void useGoogleMapsSettingControl();

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

    void on_tabWidget_currentChanged(int index);

    void on_pushButtonEkadashiVideosLocalCopy_clicked();

    void on_pushButtonSvaraVideosLocalCopy_clicked();

    void on_labelEkadashiHtmlLocalCopy_linkActivated(const QString &link);

    void on_pushButtonBoycottVideoLocalCopy_clicked();

protected slots:
    /// \brief Выровнять виджет по центру эрана
    /// \param w - виджет для выравнивания
    static void moveToScreenCenter(QWidget* w);

public slots:
    /// \brief Действия после отображения главного окна
    void afterShow();

protected:
    // Переменные для хранения постера про настоящий праздник.
    QPixmap mBoycotHolidayPoster;
    // Этот объект нужно создавать с помощью оператора new, иначе ошибка при освобождении ресурсов.
    QGraphicsPixmapItem *mpBoycottHolidayPosterItem = nullptr;
    QGraphicsScene mBoycottHolidayPosterScene;
    // Виджеты-окошки для отображения html со списком локальных копий видео
    QDialogVideosViewer mEkadashiVideosViewer;
    bool mfFirstCalc = true;    //! флаг первых расчётов при показе окна

    /// \brief Событие перед отображением окна (переопределенный метод)
    void showEvent(QShowEvent *event) override;

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

    Ui::MainWindow*               ui;                          ///< интерфейс
    QTimer                        m_Timer;                     ///< таймер для обновления времени
    quint32                       m_cTimer;                    ///< счётчик срабатываний таймера
    QDate                         m_currentDate;               ///< дата на момент срабатывания таймера
    QTime                         m_currentSunRise;            ///< время текущего восхода Солнца
    QTime                         m_currentSunSet;             ///< время текущего захода Солнца
    TTitha                        m_currentTitha;              ///< текущая титха
    TTitha                        m_prevTitha;                 ///< предыдущая титха
    QList<TComputings::TMoonDay> m_currentMoonDays;            ///< список текущих лунных дней
    QList<TComputings::TMoonDay> m_PrevMoonDays;               ///< предыдущий список лунных дней
    bool                          mf_ekadashWarned;            ///< оповещение о экадаше произведено
    QSystemTrayIcon               m_TrayIcon;                  ///< значок приложения на панели (в трее)
    QMenu                         m_TrayIconMenu;              ///< контекстное меню для значка в трее
    QAction*                      mp_TrayIconMenuActionShow;   ///< пункт "показать" контекстного меню для иконки в трее
    QAction*                      mp_TrayIconMenuActionQuit;   ///< пункт "выход" контекстного меню для иконки в трее
    bool                          mf_realClose;                ///< флаг закрытия окна на совсем
    QMessageBox*                  mp_ekadashWarnMsgBox;        ///< диалог с предупрежеднием о экадаше и кнопкой OK

};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------

#endif // MAINWINDOW_H

#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QDialog>
#include <QNetworkAccessManager>

//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: классы
namespace Ui {
class DialogSettings;
}

/// \brief Класс диалога настроек программы
class DialogSettings : public QDialog
{
    Q_OBJECT

public slots:
    /// \brief Установить координаты с гугл карт
    void setCoord(const double latitude, const double longitude);

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();

private slots:
    /// \brief "Обновить javaScript"
    void updateJavaScriptWindow();

    /// \brief обработать полученный по сети ответ на запрос
    /// \param reply - ответ на запрос
    void processNetworkReply(QNetworkReply* reply);

    /// \brief загрузить настройки программы
    /// \retval true - успех, false - неудача
    bool loadSettings();

    /// \brief сохранить настройки программы
    /// \retval true - успех, false - неудача
    bool saveSettings() const;

    void on_buttonBox_accepted();
    void on_webView_loadFinished(bool arg1);
    void on_pushButtonAutoTimeZoneOffset_clicked();

private:
    Ui::DialogSettings *ui;                 ///< интерфейс
    QNetworkAccessManager m_Nas;            ///< менеджер запросов по сети
};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------


#endif // DIALOGSETTINGS_H

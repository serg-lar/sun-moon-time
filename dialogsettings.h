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
//------------------

class SunMoonTimeSettingsMisc {
public:
    // Имена настроек в файле настроек.
    static QString longitudeSettingName() {return "longitude";}
    static QString latitudeSettingName() {return "latitude";}
    static QString timeZoneOffsetSettingName() {return "timeZoneOffset";}
    static QString heightSettingName() {return "height";}
    static QString ekadashWarnSettingName() {return "ekadashWarn";}
    static QString ekadashWarnAfterSettingName() {return "ekadashWarnAfter";}
    static QString ekadashWarnTimeBeforeSettingName() {return "ekadashWarnTimeBefore";}
    static QString ekadashWarnRequireConfirmationSettingName() {return "ekadashWarnRequireConfirmation";}
    static QString useGoogleMapsSettingName() {return "useGoogleMaps";}

    class errors {
    public:
        /// \brief Ошибка сохранения настроек
        static QString saveSettingsError() {return "Settings save error";}
        /// \brief Ошибка загрузки настроек
        static QString loadSettingsError() {return "Settings load error";}
    };
};
//------------------


/// \brief Класс диалога настроек программы
class DialogSettings : public QDialog
{
    Q_OBJECT

public slots:
    /// \brief Установить координаты с гугл карт
    void setCoord(const double latitude, const double longitude);

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

    /// \brief обновить карту в соответствии с новыми значениями геогр. координат
    void updateMap(const QString& value);

    void on_webView_loadFinished(bool arg1);
    void on_pushButtonAutoTimeZoneOffset_clicked();

    void on_checkBoxEkadashWarn_toggled(bool checked);

    void on_checkBoxEkadashWarnAfter_toggled(bool checked);

    void on_checkBoxAutoStartUp_toggled(bool checked);

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();

protected:
    bool mfMapWebPageLoadInProgress = false;   //! флаг: загрузка web-страницы с гугл картой завершена (при открытии диалога "настройки")

    /// \brief Событие при показе диалогового окна (переопределение виртуального метода)
    void showEvent(QShowEvent * event) override;

private:
    Ui::DialogSettings *ui;                 ///< интерфейс
    QNetworkAccessManager m_Nas;            ///< менеджер запросов по сети
};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------


#endif // DIALOGSETTINGS_H

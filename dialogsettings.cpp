// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include "dialogsettings.h"
#include "ui_dialogsettings.h"
#include <QWebFrame>
#include <QSettings>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimeZone>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: DialogSettings - public slots
void DialogSettings::setCoord(const double latitude, const double longitude)
{
    qDebug() << "latitude" << latitude;
    qDebug() << "longitude" << longitude;

    // новые значение широты и долготы
    ui->doubleSpinBoxLatitude->setValue(latitude);
    ui->doubleSpinBoxLongitude->setValue(longitude);

    // узнать ли высоту места над уровнем моря
    if (true == ui->checkBoxRequestHeight->isChecked())
    {
        // отправить запрос в высотную службу гугл карт
        QUrl requestUrl ("https://maps.googleapis.com/maps/api/elevation/json?locations="+QString::number(latitude,'f',7)+","+
                         QString::number(longitude,'f',7)+"&sensor=false");
        m_Nas.get(QNetworkRequest(requestUrl));
    }
    else
    {
        // высота не определяется, условно равно 0
        ui->doubleSpinBoxHeight->setValue(0);
    }
}
//---------------------------
// КОНЕЦ: DialogSettings - public slots
//---------------------------------------------------------------------------------

// НАЧАЛО: DialogSettings - private slots
void DialogSettings::updateJavaScriptWindow()
{
    // добавление java script объекта для связи с основной программой
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("qDialogSettings",this);
}
//---------------------------

void DialogSettings::processNetworkReply(QNetworkReply* reply)
{
    if (QNetworkReply::NoError == reply->error())
    {
        // ответ на запрос получен без ошибок
        QJsonParseError jsonParseError;
        QJsonDocument jsonDocument (QJsonDocument::fromJson(reply->readAll(),&jsonParseError));

        // результаты разбора полученного JSON документа
        if (QJsonParseError::NoError == jsonParseError.error)
        {
            // JSON документ прочитан удачно
            if (reply->url().toString().contains("maps.googleapis.com/maps/api/elevation/json"))
            {
                // выяснение результата обращения в службу высот гугл карт
                QString status (jsonDocument.object().value("status").toString());
                if ("OK" == status)
                {
                    // получен корректный ответ на запрос

                    // извлечение данных
                    double elevation (jsonDocument.object().value("results").toArray().first().toObject().value("elevation").toDouble());
                    double resolution (jsonDocument.object().value("results").toArray().first().toObject().value("resolution").toDouble());

                    qDebug() << "elevation" << elevation;
                    qDebug() << "resolution" << resolution;

                    // отображение данных в интерфейсе
                    ui->doubleSpinBoxHeight->setValue(elevation);
                    ui->lineEditHeightResolution->setText(QString::number(resolution,'f',2));
                }
                else
                    // ошибка выполнения запроса обращения в службу высот гугл карт
                    qWarning() << "DialogSettings::processNetworkReply" << "google maps elevation API" << status;
            }
            else
                // ответ на неизвестный запрос
                qWarning() << "DialogSettings::processNetworkReply" << "reply for unknown request";
        }
        else
        {
            // ошибка разбора JSON документа
            qWarning() << "DialogSettings::processNetworkReply" << jsonParseError.errorString();
        }
    }
    else
    {
        // ошибка отправки-получения запроса
        qWarning() << "DialogSettings::processNetworkReply" << reply->errorString();
    }

    // очистка памяти
    reply->deleteLater();
}
//---------------------------

bool DialogSettings::loadSettings()
{
    bool result (true);

    // загрузить и отобразить настройки программы
//    QSettings settings(QSettings::IniFormat,QSettings::UserScope, "DharmaSoft", "SunMoonTime", const_cast<DialogSettings*>(this));
    QSettings settings;

    // загрузить настройки
    bool ok;
    double latitude (settings.value(latitudeSettingName()).toDouble(&ok));
    double longitude (settings.value(longitudeSettingName()).toDouble(&ok));
    double height (settings.value(heightSettingName()).toDouble(&ok));
    double timeZoneOffset (settings.value(timeZoneOffsetSettingName()).toDouble(&ok));
    bool ekadashWarn (settings.value(ekadashWarnSettingName()).toBool());
    bool ekadashWarnAfter (settings.value(ekadashWarnAfterSettingName()).toBool());
    quint32 ekadashWarnTimeBefore (settings.value(ekadashWarnTimeBeforeSettingName()).toUInt(&ok));
    bool ekadashWarnRequireConfirmation (settings.value(ekadashWarnRequireConfirmationSettingName()).toBool());

    // состояние автозапуска при загрузке ОС
#ifdef Q_OS_WIN32
        QSettings autoStartSetting ("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                    QSettings::NativeFormat);
        if (true == autoStartSetting.contains(QApplication::applicationName()))
            ui->checkBoxAutoStartUp->setChecked(true);
        else
            ui->checkBoxAutoStartUp->setChecked(false);
#endif

    if (true == ok)
    {
        // отобразить настройки в интерфейсе
        ui->doubleSpinBoxLatitude->setValue(latitude);
        ui->doubleSpinBoxLongitude->setValue(longitude);
        ui->doubleSpinBoxHeight->setValue(height);
        ui->doubleSpinBoxHeight->setValue(height);
        ui->doubleSpinBoxTimeZoneOffset->setValue(timeZoneOffset);
        ui->checkBoxEkadashWarn->setChecked(ekadashWarn);
        ui->checkBoxEkadashWarnAfter->setChecked(ekadashWarnAfter);
        switch (ekadashWarnTimeBefore)
        {
        case 0 :
            ui->comboBoxEkadashWarnHours->setCurrentIndex(4);
            break;
        case 1 :
            ui->comboBoxEkadashWarnHours->setCurrentIndex(2);
            break;
        case 2 :
            ui->comboBoxEkadashWarnHours->setCurrentIndex(1);
            break;
        case 3 :
            ui->comboBoxEkadashWarnHours->setCurrentIndex(0);;
            break;
        case 6 :
            ui->comboBoxEkadashWarnHours->setCurrentIndex(3);
            break;
        }
        ui->checkBoxEkadashWarnRequireConfirmation->setChecked(ekadashWarnRequireConfirmation);

        if (false == ui->checkBoxEkadashWarn->isChecked())
        {
            ui->checkBoxEkadashWarnBefore->setChecked(false);
            ui->checkBoxEkadashWarnAfter->setDisabled(true);
            ui->checkBoxEkadashWarnRequireConfirmation->setDisabled(true);
            ui->comboBoxEkadashWarnHours->setDisabled(true);
            ui->label_5->setDisabled(true);
        }
    }
    else
    {
        result = ok; // произошла ошибка
        qWarning() << "DialogSettings::loadSettings" << settings.status();
    }

    return result;
}
//---------------------------

bool DialogSettings::saveSettings() const
{
    bool result (true);

    // сохранить настройки программы
//    QSettings settings(QSettings::IniFormat,QSettings::UserScope, const_cast<DialogSettings*>(this));
    QSettings settings;

    settings.setValue(latitudeSettingName(),ui->doubleSpinBoxLatitude->value());
    settings.setValue(longitudeSettingName(),ui->doubleSpinBoxLongitude->value());
    settings.setValue(heightSettingName(),ui->doubleSpinBoxHeight->value());
    settings.setValue(timeZoneOffsetSettingName(),ui->doubleSpinBoxTimeZoneOffset->value());
    settings.setValue(ekadashWarnSettingName(),ui->checkBoxEkadashWarn->isChecked());
    settings.setValue(ekadashWarnAfterSettingName(),ui->checkBoxEkadashWarnAfter->isChecked());
    quint32 ekadashWarnTimeBefore (0);
    switch (ui->comboBoxEkadashWarnHours->currentIndex())
    {
    case 0 :
        ekadashWarnTimeBefore = 3;
        break;
    case 1 :
        ekadashWarnTimeBefore = 2;
        break;
    case 2 :
        ekadashWarnTimeBefore = 1;
        break;
    case 3 :
        ekadashWarnTimeBefore = 6;
        break;
    case 4:
        ekadashWarnTimeBefore = 0;
        break;
    }
    settings.setValue(ekadashWarnTimeBeforeSettingName(),ekadashWarnTimeBefore);
    settings.setValue(ekadashWarnRequireConfirmationSettingName(),ui->checkBoxEkadashWarnRequireConfirmation->isChecked());
    settings.sync();
    if (QSettings::NoError != settings.status())
    {
        result = false; // произошла ошибка
        qWarning() << "DialogSettings::saveSettings" << settings.status();
    }

    return result;
}
//---------------------------

void DialogSettings::on_buttonBox_accepted()
{
    if (false == saveSettings())
        qWarning() << "DialogSettings::on_buttonBox_accepted" << "error saveSettings";
}
//---------------------------

void DialogSettings::on_webView_loadFinished(bool arg1)
{
    // центрировать карту на заданных геогр. координатах при завершении её загрузки
    if (true == arg1)
    {
        // выполнить соответствующий java script
        QString str;
        str = "map.panTo({lat: " + QString::number(ui->doubleSpinBoxLatitude->value()) +
                ", lng: " + QString::number(ui->doubleSpinBoxLongitude->value()) + "});";
        ui->webView->page()->mainFrame()->evaluateJavaScript(str);
    }
}
//---------------------------

void DialogSettings::on_pushButtonAutoTimeZoneOffset_clicked()
{
    // установить значение смещения от универсального мирового времени автоматически из системных параметров
    ui->doubleSpinBoxTimeZoneOffset->setValue(QDateTime::currentDateTime().timeZone().offsetFromUtc(QDateTime::currentDateTime()) / 3600.);    
}
//---------------------------

void DialogSettings::on_checkBoxEkadashWarn_toggled(bool checked)
{
    if (true == checked)
    {
        ui->checkBoxEkadashWarnBefore->setChecked(true);
        ui->checkBoxEkadashWarnAfter->setEnabled(true);
        ui->comboBoxEkadashWarnHours->setEnabled(true);
        ui->checkBoxEkadashWarnRequireConfirmation->setEnabled(true);
        ui->label_5->setEnabled(true);
    }
    else
    {
        ui->checkBoxEkadashWarnBefore->setChecked(false);
        ui->checkBoxEkadashWarnAfter->setChecked(false);
        ui->checkBoxEkadashWarnAfter->setEnabled(false);
        ui->comboBoxEkadashWarnHours->setEnabled(false);
        ui->checkBoxEkadashWarnRequireConfirmation->setChecked(false);
        ui->checkBoxEkadashWarnRequireConfirmation->setEnabled(false);
        ui->label_5->setEnabled(false);
    }
}
//---------------------------

void DialogSettings::on_checkBoxEkadashWarnAfter_toggled(bool checked)
{
    if (true == checked)
    {
        ui->label_5->setText("За/По истечении");
    }
    else
    {
        ui->label_5->setText("За");
    }
}
//---------------------------

void DialogSettings::on_checkBoxAutoStartUp_toggled(bool checked)
{
    // кликнут чекбокс "автозапуск при загрузке ОС"
    if (true == checked)
    {
        // установить авто запуск при загрузке ОС (для windows)
#ifdef Q_OS_WIN
        QSettings autoStartSetting ("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                    QSettings::NativeFormat);
        autoStartSetting.setValue(QApplication::applicationName(),
                                  QApplication::applicationFilePath().replace("/","\\")+" --minimized");
#endif
    }
    else
    {
        // отключить авто запуск при загрузке (windows)
#ifdef Q_OS_WIN
        QSettings autoStartSetting ("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                    QSettings::NativeFormat);
        autoStartSetting.remove(QApplication::applicationName());
#endif
    }
}
//---------------------------
// КОНЕЦ: DialogSettings - private slots
//---------------------------------------------------------------------------------

// НАЧАЛО: DialogSettings - public
DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogSettings)
{
    // загрузить интерфейс
    ui->setupUi(this);

    // автозапуск при загрузке ОС доступен пока только в windows
#ifdef Q_OS_WIN
    ui->checkBoxAutoStartUp->setEnabled(true);
#endif

    // соединения
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(updateJavaScriptWindow()));
    connect(&m_Nas, SIGNAL(finished(QNetworkReply*)), this, SLOT(processNetworkReply(QNetworkReply*)));

    // загрузить настройки программы
    if (false == loadSettings())
        qWarning() << "DialogSettings::DialogSettings" << "error loadSettings";

    // загрузить карты гугл
    ui->webView->load(QUrl("qrc:/google_maps.html"));
}
//---------------------------

DialogSettings::~DialogSettings()
{
    delete ui;
}
//---------------------------

QString DialogSettings::longitudeSettingName()
{
    return "longitude";
}
//---------------------------

QString DialogSettings::latitudeSettingName()
{
    return "latitude";
}
//---------------------------

QString DialogSettings::timeZoneOffsetSettingName()
{
    return "timeZoneOffset";
}
//---------------------------

QString DialogSettings::heightSettingName()
{
    return "height";
}
//---------------------------

QString DialogSettings::ekadashWarnSettingName()
{
    return "ekadashWarn";
}
//---------------------------

QString DialogSettings::ekadashWarnAfterSettingName()
{
    return "ekadashWarnAfter";
}
//---------------------------

QString DialogSettings::ekadashWarnTimeBeforeSettingName()
{
    return "ekadashWarnTimeBefore";
}
//---------------------------

QString DialogSettings::ekadashWarnRequireConfirmationSettingName()
{
    return "ekadashWarnRequireConfirmation";
}
//---------------------------
// КОНЕЦ: DialogSettings - public
//---------------------------------------------------------------------------------

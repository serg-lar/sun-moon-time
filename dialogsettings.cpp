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

// НАЧАЛО: DialogSettings - public
DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogSettings)
{
    // загрузить интерфейс
    ui->setupUi(this);

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
// КОНЕЦ: DialogSettings - public
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
    double latitude (settings.value("latitude").toDouble(&ok));
    double longitude (settings.value("longitude").toDouble(&ok));
    double height (settings.value("height").toDouble(&ok));
    double timeZoneOffset (settings.value("timeZoneOffset").toDouble(&ok));    

    if (true == ok)
    {
        // отобразить настройки в интерфейсе
        ui->doubleSpinBoxLatitude->setValue(latitude);
        ui->doubleSpinBoxLongitude->setValue(longitude);
        ui->doubleSpinBoxHeight->setValue(height);
        ui->doubleSpinBoxHeight->setValue(height);
        ui->doubleSpinBoxTimeZoneOffset->setValue(timeZoneOffset);        
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

    settings.setValue("latitude",ui->doubleSpinBoxLatitude->value());
    settings.setValue("longitude",ui->doubleSpinBoxLongitude->value());
    settings.setValue("height",ui->doubleSpinBoxHeight->value());
    settings.setValue("timeZoneOffset",ui->doubleSpinBoxTimeZoneOffset->value());
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
// КОНЕЦ: DialogSettings - private slots
//---------------------------------------------------------------------------------


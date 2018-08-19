// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif
// Qt
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
// sun-moon-time
#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "generalmisc.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: DialogAbout - public
DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogAbout)
{
    // Установить интерфейс.
    ui->setupUi(this);

    // Картинка программы.
    QPixmap appIcon(":/icons/sun_moon.ico");
    appIcon = appIcon.scaled(64,64,Qt::KeepAspectRatio);
    ui->labelAppIcon->setPixmap(appIcon);

    // Картинка "КрасныйГлазКаулы"
    QPixmap redEyeIcon(":/images/redeye1");
    redEyeIcon = redEyeIcon.scaled(64,64,Qt::KeepAspectRatio);
    ui->labelRedEyeIcon->setPixmap(redEyeIcon);
    // Вывести информацию о программе в текстовом поле.
    QFile readMe (":/html/readme");
    if (true == readMe.open(QIODevice::ReadOnly)) {
        QTextStream in (&readMe);
        ui->textBrowser->setHtml(in.readAll());
    }
    else { // Ошибка открытия файла.
        qWarning() << Q_FUNC_INFO << SunMoonTimeGeneralMisc::errors::fileOpenError();
    }
}
//---------------------------

DialogAbout::~DialogAbout()
{
    delete ui;
}
//---------------------------

void DialogAbout::on_labelRedEyeLocalCopy_linkActivated(const QString &link)
{
    // Открыть папку с локальной копией видео КрасныйГлазКаулы.
    if (false == QDesktopServices::openUrl(QUrl(QDir::currentPath()+"/videos/redeye/"))) {
        qWarning() << Q_FUNC_INFO << "Could not open videos directory!";
    }
}
//---------------------------
// КОНЕЦ: DialogAbout - public
//---------------------------------------------------------------------------------

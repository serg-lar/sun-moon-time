// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include <QFile>
#include <QTextStream>
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
    ui->labelAppIcon->setPixmap(QIcon(":/icons/sun_moon.ico").pixmap(QSize(ui->labelAppIcon->width(),ui->labelAppIcon->height())));
    ui->labelAppVersion->setText("sun-moon-time v0.65 beta");

    // Картинка "КрасныйГлазКаулы"
    ui->labelRedEyeIcon->setPixmap(QIcon(":/images/redeye1").pixmap(QSize(ui->labelRedEyeIcon->width(),ui->labelRedEyeIcon->height())));
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
// КОНЕЦ: DialogAbout - public
//---------------------------------------------------------------------------------

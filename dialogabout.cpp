// НАЧАЛО: директивы, глобальные переменные и константы
#ifdef QT_NO_DEBUG
    #define QT_NO_DEBUG_OUTPUT
#endif

#include <QFile>
#include <QTextStream>
#include "dialogabout.h"
#include "ui_dialogabout.h"
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: DialogAbout - public
DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    // картинка программы
    ui->label->setPixmap(QIcon(":/icons/sun_moon.ico").pixmap(QSize(ui->label->width(),ui->label->height())));
    ui->label_2->setText("sun-moon-time v0.65 beta");

    // вывести информацию о программе в текстовом поле
    QFile readMe (":/txt/readme.txt");
    if (true == readMe.open(QIODevice::ReadOnly))
    {
        QTextStream in (&readMe);
        ui->textEdit->setText(in.readAll());
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
